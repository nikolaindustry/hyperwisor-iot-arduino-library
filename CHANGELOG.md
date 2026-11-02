# Changelog

Arduino Library Manager indexes **git tags**, not the version in
`library.properties`. Only `v3.0.0` was ever tagged, so 3.0.1 through 3.2.0
exist in this repository but never reached anyone installing from Library
Manager. Updating from 3.0.0 therefore brings four releases at once, including
a security fix.

## 3.3.2

Packaging. The library people download is now a third of the size.

Arduino Library Manager packages the whole repository, so anything tracked
here is downloaded by everyone who installs Hyperwisor-IOT. Two directories
were riding along:

- `.qoder/repowiki/` — 916 KB of generated API documentation across 28 files.
  Besides the weight, it was a second and unmaintained description of this
  library, certain to drift from the README and contradict it.
- `.vscode/` — editor configuration, including a hardcoded `/Users/user`
  include path. Machine-specific, and useless to anyone else.

Both are untracked and ignored now. Nothing in `src/` changed.

## 3.3.1

Documentation only; no code change.

- The Library Manager description named **nikolaindustry-realtime** as the
  transport. That service was retired on 2026-07-30 and has since been
  repurposed for unrelated work, so the listing was pointing users at a system
  that is no longer part of this platform. The library itself has always
  connected to the correct secure relay.
- The README linked twice to `github.com/your-org/nikolaindustry-realtime`, a
  placeholder that was never a real address.
- Rewrote the README around what a developer actually needs: how a device gets
  onto an account, how commands arrive, what reconnection now does, and the
  API in one table. The full licence text is linked rather than inlined, and
  the invitation to send pull requests is gone — it contradicted a licence that
  does not permit derivative works.

## 3.3.0

**Reconnects back off, and spread across a fleet.**

`setReconnectInterval(2000)` was fixed, forever, with no jitter. On one board
that is a fast reconnect. Across many devices it is the worst failure mode
there is: anything that drops everyone at once — a server restart, an upstream
blip — has every device retrying every 2 seconds in lockstep, and the storm
never thins out because the interval never grows.

- Reconnects now back off 2s, 4s, 8s … capped at 60s.
- Each retry lands on a random point in a window as wide as the wait itself,
  so a fleet spreads out instead of arriving together. The spread widens with
  the delay, which is where it is needed: a long backoff means a long outage
  means the biggest crowd.
- A **rejected identity** is paced differently from a dead host. `auth_fail`
  previously only logged; it now records that the server answered and said it
  does not know this device, which almost always means onboarding is still in
  flight and the credential row is seconds away. That gets a flat 5s retry and
  no escalation, rather than being pushed out to a minute at exactly the moment
  it was about to succeed.
- The attempt counter resets on `auth_ok`, not on the socket opening. The
  transport coming up is not success when the handshake is what decides.

**What you will notice on a single device:** after a long outage, reconnection
can now take up to about 90 seconds rather than retrying every 2. That is the
deliberate cost of not having a fleet stampede a server. A device recovering
from an ordinary blip is on attempt 0, which is 1–3 seconds — no slower than
before.

## 3.2.0 — never released until now

**SECURITY: a provisioning access point per device, not one for all.**

## 3.1.0 — never released until now

A device reports which firmware it is running.

## 3.0.2 — never released until now

The connection-status callback is exposed.

## 3.0.1 — never released until now

Nothing is sent before the server says `auth_ok`, and recovery is faster.

A sketch publishes on its own timer and knows nothing about the handshake, so a
sensor reading could land in the window between "sent auth" and
"authenticated". That stray frame was treated as a failed handshake and the
connection was closed, so the device reconnected into the same race — a loop
that looked like a valid device being rejected.

The handshake's own frames bypass that gate, which they must: the auth reply is
by definition sent while unauthenticated, so routing it through the gate would
drop the one frame that ends the unauthenticated state.

## 3.0.0

Last version available through Library Manager before 3.3.0.
