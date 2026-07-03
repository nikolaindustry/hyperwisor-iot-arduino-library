#include "hyperwisor-hsc.h"
#include <Preferences.h>
#include <memory>
#include <string.h>

// mbedTLS 3.x (ESP32 Arduino core 3.x) hides struct members behind MBEDTLS_PRIVATE.
// Allow direct access so the same code compiles on both mbedTLS 2.x and 3.x.
#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "mbedtls/ecp.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/bignum.h"
#include "mbedtls/sha256.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/base64.h"

// NVS namespace + key for the private scalar. Kept separate from "wifi-creds".
static const char *HSC_NS = "hsc-v1";
static const char *HSC_KEY = "priv";

// HSC canonical framing (must match relay utils/hsc.js).
static const char *HSC_DOMAIN = "HYPERWISOR-HSC-v1";
static const char HSC_US = '\x1f'; // unit separator

// --- small helpers ---------------------------------------------------------

static String b64(const uint8_t *data, size_t len)
{
  size_t olen = 0;
  // First call sizes the output.
  mbedtls_base64_encode(nullptr, 0, &olen, data, len);
  std::unique_ptr<unsigned char[]> buf(new unsigned char[olen + 1]);
  if (mbedtls_base64_encode(buf.get(), olen + 1, &olen, data, len) != 0) return String("");
  buf[olen] = 0;
  return String((char *)buf.get());
}

// Seed a CTR-DRBG from the ESP32 hardware entropy source. Returns 0 on success.
static int seedRng(mbedtls_ctr_drbg_context *ctr, mbedtls_entropy_context *ent)
{
  mbedtls_entropy_init(ent);
  mbedtls_ctr_drbg_init(ctr);
  const char *pers = "hyperwisor-hsc-v1";
  return mbedtls_ctr_drbg_seed(ctr, mbedtls_entropy_func, ent,
                               (const unsigned char *)pers, strlen(pers));
}

// ---------------------------------------------------------------------------

HyperwisorHSC::HyperwisorHSC() {}

String HyperwisorHSC::buildMessage(const String &role, const String &deviceId,
                                   const String &channelId, const String &nonce,
                                   const String &ts)
{
  String m;
  m.reserve(strlen(HSC_DOMAIN) + role.length() + deviceId.length() +
            channelId.length() + nonce.length() + ts.length() + 8);
  m += HSC_DOMAIN; m += HSC_US;
  m += role;       m += HSC_US;
  m += deviceId;   m += HSC_US;
  m += channelId;  m += HSC_US;
  m += nonce;      m += HSC_US;
  m += ts;
  return m;
}

bool HyperwisorHSC::begin()
{
  if (loadFromNVS()) {
    _ready = true;
    Serial.println("🔐 HSC: device key loaded from NVS");
    return true;
  }
  Serial.println("🔐 HSC: no key found — generating on-chip keypair...");
  if (generateAndStore()) {
    _ready = true;
    Serial.println("🔐 HSC: keypair generated & stored (private key stays on-device)");
    return true;
  }
  Serial.println("❌ HSC: keypair generation failed");
  return false;
}

bool HyperwisorHSC::loadFromNVS()
{
  Preferences p;
  if (!p.begin(HSC_NS, true)) return false;
  size_t len = p.getBytesLength(HSC_KEY);
  if (len != 32) { p.end(); return false; }
  p.getBytes(HSC_KEY, _priv, 32);
  p.end();

  // Derive the public key Q = d*G so we always have it available.
  mbedtls_ecp_group grp;
  mbedtls_mpi d;
  mbedtls_ecp_point Q;
  mbedtls_ecp_group_init(&grp);
  mbedtls_mpi_init(&d);
  mbedtls_ecp_point_init(&Q);

  mbedtls_ctr_drbg_context ctr; mbedtls_entropy_context ent;
  bool ok = false;
  if (seedRng(&ctr, &ent) == 0 &&
      mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1) == 0 &&
      mbedtls_mpi_read_binary(&d, _priv, 32) == 0 &&
      mbedtls_ecp_mul(&grp, &Q, &d, &grp.G, mbedtls_ctr_drbg_random, &ctr) == 0) {
    size_t olen = 0;
    if (mbedtls_ecp_point_write_binary(&grp, &Q, MBEDTLS_ECP_PF_UNCOMPRESSED,
                                       &olen, _pub, sizeof(_pub)) == 0 && olen == 65) {
      ok = true;
    }
  }

  mbedtls_ctr_drbg_free(&ctr); mbedtls_entropy_free(&ent);
  mbedtls_ecp_point_free(&Q); mbedtls_mpi_free(&d); mbedtls_ecp_group_free(&grp);
  return ok;
}

bool HyperwisorHSC::generateAndStore()
{
  mbedtls_ctr_drbg_context ctr; mbedtls_entropy_context ent;
  if (seedRng(&ctr, &ent) != 0) { mbedtls_ctr_drbg_free(&ctr); mbedtls_entropy_free(&ent); return false; }

  mbedtls_ecp_keypair key;
  mbedtls_ecp_keypair_init(&key);
  bool ok = false;

  if (mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, &key,
                          mbedtls_ctr_drbg_random, &ctr) == 0) {
    size_t olen = 0;
    if (mbedtls_mpi_write_binary(&key.d, _priv, 32) == 0 &&
        mbedtls_ecp_point_write_binary(&key.grp, &key.Q, MBEDTLS_ECP_PF_UNCOMPRESSED,
                                       &olen, _pub, sizeof(_pub)) == 0 && olen == 65) {
      Preferences p;
      if (p.begin(HSC_NS, false)) {
        ok = (p.putBytes(HSC_KEY, _priv, 32) == 32);
        p.end();
      }
    }
  }

  mbedtls_ecp_keypair_free(&key);
  mbedtls_ctr_drbg_free(&ctr); mbedtls_entropy_free(&ent);
  return ok;
}

String HyperwisorHSC::getPublicKeyBase64()
{
  if (!_ready) return String("");
  return b64(_pub, 65);
}

String HyperwisorHSC::signChallenge(const String &deviceId, const String &nonce, const String &ts)
{
  if (!_ready) return String("");

  String msg = buildMessage("device", deviceId, deviceId, nonce, ts);

  // hash = SHA-256(msg)
  uint8_t hash[32];
  {
    mbedtls_sha256_context sha;
    mbedtls_sha256_init(&sha);
    mbedtls_sha256_starts(&sha, 0); // 0 => SHA-256
    mbedtls_sha256_update(&sha, (const unsigned char *)msg.c_str(), msg.length());
    mbedtls_sha256_finish(&sha, hash);
    mbedtls_sha256_free(&sha);
  }

  mbedtls_ctr_drbg_context ctr; mbedtls_entropy_context ent;
  if (seedRng(&ctr, &ent) != 0) { mbedtls_ctr_drbg_free(&ctr); mbedtls_entropy_free(&ent); return String(""); }

  mbedtls_ecp_group grp; mbedtls_mpi d, r, s;
  mbedtls_ecp_group_init(&grp);
  mbedtls_mpi_init(&d); mbedtls_mpi_init(&r); mbedtls_mpi_init(&s);

  String out = "";
  if (mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1) == 0 &&
      mbedtls_mpi_read_binary(&d, _priv, 32) == 0 &&
      mbedtls_ecdsa_sign(&grp, &r, &s, &d, hash, 32,
                         mbedtls_ctr_drbg_random, &ctr) == 0) {
    // raw signature = r(32) || s(32)  (IEEE P1363)
    uint8_t sig[64];
    if (mbedtls_mpi_write_binary(&r, sig, 32) == 0 &&
        mbedtls_mpi_write_binary(&s, sig + 32, 32) == 0) {
      out = b64(sig, 64);
    }
  }

  mbedtls_mpi_free(&d); mbedtls_mpi_free(&r); mbedtls_mpi_free(&s);
  mbedtls_ecp_group_free(&grp);
  mbedtls_ctr_drbg_free(&ctr); mbedtls_entropy_free(&ent);
  return out;
}

void HyperwisorHSC::erase()
{
  Preferences p;
  if (p.begin(HSC_NS, false)) {
    p.remove(HSC_KEY);
    p.end();
  }
  _ready = false;
  Serial.println("🔐 HSC: keypair erased (a new identity will be generated next boot)");
}
