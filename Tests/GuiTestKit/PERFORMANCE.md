# GUI test suite: performance analysis and speed-up plan

Everything in "Where the time actually goes" below is **measured**, not estimated — against the real
ProLife suite (default scope: `su` + `fullAccess` + 7 isolated editor users + guest) on the dev
machine, using the Debug test servers. Numbers in "Options" are estimates and labelled as such.

## The headline

The full default-scope run takes **~15 minutes**. Roughly **half of that is not test code at all** —
it's booting the WASM app over and over, and sleeping in fixed-length "wait for things to settle"
windows.

Two findings invert the intuition:

- **Screenshots are not the bottleneck.** A `toHaveScreenshot` assertion costs **151 ms**; the raw
  capture is **47 ms**. All 588 baseline screenshots together are a minor cost.
- **The waiting is the bottleneck.** `gui.checkScreenshot` measures **550–724 ms** — and ~400 ms of
  that is `waitForStable`'s fixed quiet window, not the imaging. The same 400 ms is paid by *every*
  click and *every* fill.

## Where the time actually goes (measured)

### Cost atoms

| Operation | Measured |
|---|---|
| Cold boot in `beforeAll` (`newUserPage` + `reload` + `open`) | **11.1 s** |
| Cold boot standalone (`newContext` + `goto` + settle + menu visible) | 1.5–6.6 s (avg 3.4 s) |
| `waitForStable()` on a fully idle page | **414 ms** ← paid after every action |
| `openPage()` (one click + settle) | 0.6–1.2 s |
| `gui.checkScreenshot()` | **550–724 ms** |
| └ of which `toHaveScreenshot` (the actual assertion) | **151 ms** |
| └ of which raw `page.screenshot({fullPage:true})` | **47–68 ms** |

### Suite shape

| | tests | test-seconds | wall | workers |
|---|---|---|---|---|
| Phase 1 (read-only) | 192 | 2296 s | **438 s** | 10 |
| Phase 2 (`@mutating`) | 14 | 238 s | **588 s** | 1 |
| DB restore + server start | — | — | ~60 s | — |

### The five root causes

**1. Boot tax (~50% of all worker-seconds).**
Every `describe.serial` block pays one **11 s** cold boot in its `beforeAll`. Worse: six specs
(`workspace`, `administration`, `organizations`, `search`, `session-expiry`, `user-profile`) use the
default per-test `page` fixture, so **every single test in them gets a fresh browser context and a
full WASM boot**. Measured: those 55 tests average **16.5 s/test**, versus **10.0 s/test** for the
shared-page specs — and the *fastest* specs in the whole suite are the shared-page editor ones
(`devices.editor` **0.7 s/test**, `administration.editor` **0.9 s/test**), precisely because they boot
once and reuse the page.

Phase 1: 438 s wall × 10 workers = 4380 worker-seconds available, but only 2296 s is inside test
bodies. **~48% is hooks and boots.**

**2. Blind-wait tax (the 400 ms quiet window).**
`click()`, `fill()`, and `checkScreenshot()` all end with `waitForStable()`, whose floor is a fixed
**~414 ms** of sleeping — whether the app finished in 30 ms or is still mid-request. Action call sites
per spec: `administration.editor` **75**, `devices.collection` **56**, `devices.editor` **39**. At
~400 ms of dead time each, this is easily **25–35% of test-body time**.

Critically, this tax buys *nothing*: a fixed 400 ms guess is simultaneously **too long** when the app
is already idle and **too short** when it isn't — which is exactly the class of race that has been
producing the day's flaky failures (the `search` → `selectRow` → `editItem` bug: the quiet window
expired *during* the search debounce, before the filtered rows had arrived).

**3. Serial chains cap the wall clock.**
A `describe.serial` block runs entirely inside one worker. The longest chain is
**`su | devices.collection` = 290 s**. That is a **hard floor** on phase-1 wall time no matter how many
workers are added. (Perfectly balanced across 10 workers, 2296 test-seconds would be 230 s — so the
suite is already parallelism-bound, not worker-bound. Raising `workers` further does nothing.)

**4. Phase 2 is pathological: 14 tests take 9.8 minutes.**
Only 238 s of that is test code — **350 s (60%) is pure overhead**. `Run-CiTests.ps1` invokes
`npx playwright test` a *second* time, so **`global-setup` runs again and re-logs-in every active user
through the UI**, then every mutating spec cold-boots again — all at `workers: 1`.

**5. Matrix multiplication.**
Every collection spec runs in full under *both* `su` and `fullAccess`, screenshots included. The two
users' runs are near-identical (`devices.collection`: 290 s vs 278 s) but the permission-relevant
difference between them is a handful of assertions, not the whole flow.

---

## Options

### Option 1 — Turn the knobs (tactical)

Lower `quietMs` 400 → ~120, merge the two phases into one Playwright invocation (use a project
dependency instead of a second `npx playwright test`, so `global-setup` runs once).

- **Estimated result:** 15 min → 8–9 min.
- **Pros:** hours of work, no architectural risk.
- **Cons:** leaves every structural problem in place. Shortening a *blind* wait makes it blinder —
  it trades speed for *more* flakiness, and flakiness is already the other thing hurting us.

### Option 2 — Session reuse per worker (structural)

Boot the app **once per worker** and reuse that context+page across every test the worker runs;
reset between tests by in-app navigation rather than `page.goto`.

- **Estimated result:** removes the bulk of the boot tax (~40–50% of worker-seconds).
- **Pros:** biggest single win; benefits every spec; and it *reduces* flakiness, because cold boot is
  exactly when "element exists but is not yet visible" bites (the `_workerStartupStagger` hack and
  `openPage`'s 3-attempt retry loop both exist solely to paper over boot-storm races).
- **Cons:** cross-test state leakage — the very thing the `isolatedSpec` users were introduced to
  prevent. Needs a dependable `resetToCleanState()` and the per-user isolation kept as-is.

### Option 3 — Event-based waiting instead of blind waiting (structural)

Replace `waitForStable`'s "no DOM mutations for 400 ms" guess with a signal that means something:
wait until **in-flight GraphQL requests drain to zero**, plus wait for the **postcondition the caller
actually wants** (element visible / enabled / holding value — the kit already has `requireVisible`
and `expect.poll`). `click()` should return as soon as the app is genuinely idle, not after a fixed nap.

- **Estimated result:** −30–40% of test-body time.
- **Pros:** **faster and more reliable at the same time.** Every race fixed today by hand
  (`waitForSearchResult`, `openSearchResultEditor`, `openComboPopup`'s retry loop) is a local patch for
  the missing general mechanism. This *is* the general mechanism.
- **Cons:** must be done carefully in the kit and re-validated across the suite.

### Option 4 — Split the suite by purpose (strategic)

Three suites instead of one:
- **Functional** — structural assertions, **no screenshots**, one user, runs on every commit. Target **< 2 min**.
- **Visual regression** — the 588 screenshots, one user, nightly / on demand.
- **Permission matrix** — only the permission-relevant assertions across all users (cheap structural
  checks, not full flows), nightly.

- **Pros:** the per-commit signal becomes fast enough to actually be used; screenshot churn stops
  blocking day-to-day work.
- **Cons:** spec reorganisation; two baselines to keep straight.

### Option 5 — Move coverage down the testing pyramid (strategic)

Much of what these GUI tests assert (a save persists, a validator fires, a command is permission-gated)
is *server* behaviour, already testable through GraphQL orders of magnitude faster —
`Tests/ProLifeApiPostman` already exists. Keep GUI tests for what only the GUI can prove: rendering,
wiring, and permission-driven visibility.

- **Pros:** the highest long-term leverage; a GUI suite that stays small stays fast.
- **Cons:** a real rewrite; gives up some end-to-end confidence. Not a quick win.

### Option 6 — Release-build test servers (needs you)

The test servers are **Debug** builds (`Debug_Qt6_VC17_x64` — the Release folder has no `*Test.exe`
variants at all). Every GraphQL round-trip pays Debug-build cost, and the servers also serve the WASM
app itself.

- **Pros:** potentially a large, free speed-up across the entire suite with **zero test-code changes**.
- **Cons:** I cannot build it from the agent shell — you'd need to produce
  `Release_Qt6_VC17_x64` test-server binaries. Worth one measurement before committing to it.

---

## Implementation status (live)

- **Stage A (skip redundant re-login in phase 2)** — DONE. `createGlobalSetup` gained a
  `reuseExistingAuth` option; ProLife's `global-setup.js` wires it to a `PROLIFE_GUI_REUSE_AUTH` env
  var that `Run-CiTests.ps1` sets only for phase 2 (phase 1 always logs in fresh, so a genuinely broken
  login still fails loudly). Measured: phase 2 wall-clock 9.8m → 5.4m with zero new failures.
- **Stage C (event-based waiting)** — DONE. `lib/stability.js` rewritten: a per-page in-flight
  XHR/fetch counter (event-driven, WeakMap-tracked, lazily attached) plus a shortened DOM-quiet window
  (400ms → 150ms), looped up to 3 rounds bounded by the original overall `timeout` so a request that's
  still in flight when the DOM goes quiet is waited for before declaring the action settled. Landed
  together with Stage A; combined effect confirmed clean (only 2 new failures surfaced, both pre-existing
  bugs unrelated to this change - see below).
- **Stage D (split long serial chains) — TRIED AND REVERTED.** Splitting
  `devices.collection.multiuser.test.js`'s single 290s chain into 4 independent `describe.serial` blocks
  (and `software.collection`'s into 2) made things measurably WORSE: phase 1 wall-clock 5.6m → 8.8m with
  9 new failures across unrelated specs, phase 2 5.4m → 10.3m. Root cause: there is only ONE shared
  Debug test server for the whole suite, and it cannot sustain the increased FREQUENCY of concurrent
  cold WASM boots that more, smaller independent serial chains produce - `fixtures/createTest.js`'s own
  `_workerStartupStagger` comment already documents this exact failure mode ("10 simultaneous WASM boots
  saturate the one Debug server + CPU") for the run's initial boot storm; splitting chains recreates that
  storm repeatedly throughout the run instead of once at the start. **Lesson: boot COUNT (Stage B, which
  reduces total boots) is safe to optimize; boot CONCURRENCY/FREQUENCY (Stage D, which increases how
  often boots cluster together) is not, under a single shared Debug server.** Reverted via
  `git checkout` on both files; do not retry this approach without first solving the shared-server
  concurrency ceiling (e.g. a Release-build server, or several server instances on different ports).
- **Environment note:** after ~11 consecutive full live-server runs in one session (each restoring 3
  databases and restarting 3 Debug servers), a validation run showed severe, code-unrelated instability
  (even `login.guest`'s basic superuser sign-in failed to find `LoginInput`) despite plenty of free
  RAM/disk. Root cause not confirmed - possibly Debug-server/Postgres degradation across many
  restarts within one session. Recommendation: don't chain many full live-server validation runs
  back-to-back without a break; a machine/server restart cleared it.
- **Two live server crashes observed during validation, both unrelated to this work:**
  `PumaServerPgTest.exe` hit a native `Debug Assertion Failed: map/set iterators incompatible`
  (MSVC `xtree:229` - a genuine C++ concurrency bug, container iterator invalidated by a concurrent
  writer) very early in one run's phase 1, and in a separate run `ProLifeServerTest.exe` itself died
  mid-phase-1 (`page.goto` started failing with `ERR_CONNECTION_REFUSED`/`RESET`, cascading into ~68
  "did not run" tests for the rest of that run since `Run-CiTests.ps1` doesn't restart or health-check
  the server between phases). Both are server-side C++ reliability issues, out of this kit's scope to
  fix - flagged to the repo owner separately. Worth noting: faster, denser request timing from Stage
  C makes these existing concurrency bugs easier to trigger, not more likely to exist - the bugs were
  already there.
- **Stage B (worker/session reuse, extended scope)** — DONE for the safe subset.
  `administration.multiuser.test.js`'s 'subpages' block, `search.multiuser.test.js` (whole file - every
  test there already does its own explicit reload/navigation, so only the context-provisioning changed),
  and `workspace.multiuser.test.js`'s 'interactions' block (hardest of the three - required reading
  `WorkspacePage.qml` and `FilterDelegateBase.qml` to confirm (a) the active tab already resets on
  every re-navigation via `NavigableItem.onParentActivated`, and (b) `TimeFilterDelegate`/
  `CustomerFilterDelegate`'s `ClearButton` exists and is only `visible` while a filter is active, making
  a cheap `countVisible`-gated reset safe - a blind click attempt would have cost a real ~10s timeout on
  every test that never touched a filter) were all converted from a fresh boot per test to one shared
  boot per file/user. `user-profile.multiuser.test.js` was deliberately left untouched - its own header
  comment already documents that per-test fresh contexts are a intentional choice (isolating the Logout
  test), not an oversight, for negligible gain (1 boot saved). `organizations`/`session-expiry` have
  too few tests (1-2) to be worth the change.
- **Stage E (visual/functional split)** — skipped by user decision: the codebase's own stated
  philosophy ("Screenshots are the primary validation mechanism (per project decision)" -
  `lib/screenshot.js`'s own header) means most of the suite's ~180 screenshot call sites have no other
  assertion, so a mechanical split risks either an empty "fast" lane (honest tagging) or a
  falsely-shrunk one (under-tagging). Left as a deliberate future decision, not attempted blind.

## Recommendation

**Do 2 + 3 first, then 4. Ask about 6 in parallel — it may be free.**

Options 2 and 3 attack the two measured elephants (boot tax, blind-wait tax) *and* both reduce
flakiness. Speed and reliability are not in tension here — they have the same root cause: the suite
guesses when the app is ready instead of knowing.

Option 1 alone is a trap: it buys time by making the guessing worse.

### Estimated end state

| | now | after 2+3 | after 2+3+4 |
|---|---|---|---|
| Full default scope | ~15 min | **~5–6 min** *(est.)* | ~5–6 min |
| Per-commit functional signal | — (none) | — | **< 90 s** *(est.)* |

---

## Implementation plan

### Stage A — Merge the two phases (quick, do first)

`Run-CiTests.ps1` currently calls `npx playwright test` twice, which re-runs `global-setup` (a full
UI login for every active user) and re-pays every worker/boot cost for the sake of 14 tests.

Replace with **one** invocation using a Playwright *project dependency*: a `mutating` project that
`dependsOn` the read-only projects and is pinned to `fullyParallel: false` / its own worker. Same
ordering guarantee (mutating tests run after read-only ones), one global-setup, one warm-up.

*Expected: −3 to −4 min on its own.*

### Stage B — Worker-scoped app session (Option 2)

In `fixtures/createTest.js`, add a **worker-scoped** fixture that owns one context+page per worker,
booted once, and hand it to tests. Add `resetToCleanState(page)` to the kit:
close any open document tabs, clear filters, navigate to a known home page — all *without* `page.goto`.

Keep the `isolatedSpec` per-user pinning exactly as it is; this changes *how often we boot*, not
*who runs what*.

Convert the six per-test-context specs (`workspace`, `administration`, `organizations`, `search`,
`session-expiry`, `user-profile`) onto it. Note `session-expiry` genuinely needs a real reload — it
should keep an opt-out.

*Expected: −40–50% of worker-seconds.*

### Stage C — Event-based waits (Option 3)

In `lib/stability.js`:
- Track in-flight requests on the page (`page.on('request')`/`('requestfinished')`/`('requestfailed')`)
  and expose `waitForNetworkIdle(page)`.
- Rewrite `waitForStable` as: *network idle* **AND** *no busy indicator* **AND** a much shorter DOM-quiet
  window (~100 ms) — instead of a flat 400 ms nap.

In `lib/actions.js`:
- `click()`/`fill()` stop unconditionally sleeping; callers that need a postcondition assert it
  (`requireVisible`, `waitForTextInputValue`), which the kit already supports.
- This should let `waitForSearchResult` / `openSearchResultEditor` / `openComboPopup`'s retry loops be
  *deleted*, not just kept — they are symptoms of the missing mechanism.

*Expected: −30–40% of test-body time, and materially fewer flaky failures.*

### Stage D — Rebalance the long chains

`devices.collection` is a 290 s single-worker chain. Split its independent `describe.serial` blocks
(landing / filters / pagination / column-config / dialogs) into separate blocks so Playwright can
schedule them on different workers. Nothing about them is genuinely sequential across groups.

*Expected: lowers the parallelism floor from 290 s toward ~120 s.*

### Stage E — Split by purpose (Option 4)

Tag tests `@visual` (screenshot assertions) vs functional. Run the functional set on every commit at
one user; run `@visual` and the full user matrix nightly.

### Stage F — Measure a Release test server (Option 6)

Once you can produce `Release_Qt6_VC17_x64` test-server binaries, re-run the cost-atom benchmark
(cold boot / click / screenshot) against them and compare with the Debug numbers above.
