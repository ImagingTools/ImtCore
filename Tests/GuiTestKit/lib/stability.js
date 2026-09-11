// Wait for the QML/WASM canvas DOM to settle after an interaction.
//
// The old suite polled `document.documentElement.outerHTML` every 100 ms and diffed the whole string
// - O(document size) per tick and easily fooled by tiny animated attributes. This version installs a
// single in-page MutationObserver and resolves once there have been no DOM mutations for `quietMs`,
// or bails out after `timeout`. It is cheap (the browser pushes mutations to us) and robust across
// navigations (the injected function is self-contained and re-injected on each call).
//
// A flat DOM-quiet window alone is a guess, not a fact: too long when the app was already idle (dead
// time on every click/fill in the suite - measured live at ~400ms each, the single biggest line item
// in a test's own wall-clock), too short when a GraphQL round-trip is still in flight (the DOM can go
// quiet WHILE waiting for the network response, then mutate again once it arrives - exactly the shape
// of bug that produced the "search debounce" race fixed by hand in administration.editor.multiuser
// .test.js). Network tracking below turns "is a request still in flight" from a guess into a fact,
// so quietMs only has to cover the tail-end render flush after mutations genuinely stop, not an
// entire round-trip - hence it can be much shorter than before.

const DEFAULTS = { quietMs: 150, timeout: 6000 };
const NETWORK_IDLE_TIMEOUT = 5000;
// A settled action is typically: DOM mutates (click feedback) -> quiet while a request is in flight ->
// response arrives -> DOM mutates again (view updates) -> quiet for real. That's one extra round after
// the first network wait; a couple more are allowed for a rarer chained-request flow (e.g. a save
// followed by a GetCommands refresh), bounded overall by `timeout` below regardless of round count.
const MAX_SETTLE_ROUNDS = 3;

// ImtCore's two spinner components (imtcontrols/Views/BusyIndicator.qml, Loading.qml) both animate
// via a Timer mutating a `rotation`/transform property every 10ms - never touching objectName,
// visible, or childList, i.e. none of the attributes the MutationObserver above watches. DOM can go
// quiet while one is still visibly spinning (caught live: a screenshot mid-redo captured the spinner
// itself). Both were given the same objectName as test instrumentation - poll for it directly instead
// of assuming "DOM quiet" implies "no pending async work".
const BUSY_SELECTOR = '[objectName="BusyIndicator"][visible]';
const BUSY_INDICATOR_TIMEOUT = 5000;

/**
 * Wait for any visible busy/loading indicator to disappear. Near-zero cost when none is showing (a
 * single in-page check, no polling loop needed); polls efficiently in-browser (Playwright's own
 * `waitForFunction` polling, not repeated Node<->browser round trips) when one is. Swallows its own
 * timeout rather than throwing - this is a screenshot-noise reducer, not a structural assertion, so a
 * spinner that's still up after `timeout` shouldn't fail the caller's real action/assertion; whatever
 * it's waiting on taking unusually long will surface via the caller's own outcome instead.
 * @param {import('@playwright/test').Page} page
 * @param {{timeout?: number}} [options]
 */
async function waitForBusyIndicatorGone(page, options = {}) {
  const { timeout = BUSY_INDICATOR_TIMEOUT } = options;
  try {
    await page.waitForFunction((selector) => !document.querySelector(selector), BUSY_SELECTOR, {
      timeout,
      polling: 100,
    });
  } catch (err) {
    if (!/Timeout/.test(String(err && err.message))) throw err;
  }
}

// --- network tracking ---------------------------------------------------------------------------
//
// Per-page in-flight XHR/fetch counter, attached lazily (once) and kept for the page's whole life -
// event-driven (Playwright pushes request/requestfinished/requestfailed to us), not polled, so
// checking it is free when nothing is in flight. Deliberately narrowed to xhr/fetch (GraphQL calls go
// through fetch/XHR): a WASM app's own asset requests (the .wasm/.js bundle itself, images) fire once
// at boot and would otherwise pad every later wait, and this must never count long-lived connections
// (websocket/eventsource, if the app ever adds one) as "in flight" - those never finish, which would
// make waitForNetworkIdle time out on every call instead of resolving instantly.
const networkStateByPage = new WeakMap();

function isTrackedRequest(req) {
  const type = req.resourceType();
  return type === 'xhr' || type === 'fetch';
}

function trackNetwork(page) {
  let state = networkStateByPage.get(page);
  if (state) return state;
  state = { pending: 0, waiters: [] };
  const settle = () => {
    if (state.pending === 0 && state.waiters.length) {
      const waiters = state.waiters;
      state.waiters = [];
      waiters.forEach((resolve) => resolve());
    }
  };
  page.on('request', (req) => {
    if (isTrackedRequest(req)) state.pending++;
  });
  const onDone = (req) => {
    if (isTrackedRequest(req)) {
      state.pending = Math.max(0, state.pending - 1);
      settle();
    }
  };
  page.on('requestfinished', onDone);
  page.on('requestfailed', onDone);
  networkStateByPage.set(page, state);
  return state;
}

/**
 * Wait for currently in-flight XHR/fetch requests to drain to zero. Resolves immediately if nothing
 * is in flight (the common case - most UI actions are purely client-side). Swallows its own timeout
 * like waitForBusyIndicatorGone - a stuck request should surface via the caller's own action/assertion,
 * not from this internal settle helper.
 * @param {import('@playwright/test').Page} page
 * @param {{timeout?: number}} [options]
 */
async function waitForNetworkIdle(page, options = {}) {
  const { timeout = NETWORK_IDLE_TIMEOUT } = options;
  const state = trackNetwork(page);
  if (state.pending === 0) return;
  await new Promise((resolve) => {
    const timer = setTimeout(resolve, Math.max(0, timeout));
    state.waiters.push(() => {
      clearTimeout(timer);
      resolve();
    });
  });
}

/** One DOM-quiet pass: resolves after `quietMs` with no mutations, or after `timeout` regardless. */
function domQuietOnce(page, quietMs, timeout) {
  return page.evaluate(
    ({ quietMs, timeout }) =>
      new Promise((resolve) => {
        const target = document.body || document.documentElement;
        if (!target) {
          resolve();
          return;
        }

        let quietTimer = null;
        const hardStop = setTimeout(finish, Math.max(0, timeout));

        const observer = new MutationObserver(() => {
          if (quietTimer) clearTimeout(quietTimer);
          quietTimer = setTimeout(finish, quietMs);
        });

        function finish() {
          if (quietTimer) clearTimeout(quietTimer);
          clearTimeout(hardStop);
          observer.disconnect();
          resolve();
        }

        observer.observe(target, {
          subtree: true,
          childList: true,
          attributes: true,
          characterData: true,
        });

        // Arm the initial quiet window in case nothing mutates at all.
        quietTimer = setTimeout(finish, quietMs);
      }),
    { quietMs, timeout }
  );
}

/**
 * @param {import('@playwright/test').Page} page
 * @param {{quietMs?: number, timeout?: number}} [options]
 */
async function waitForStable(page, options = {}) {
  const { quietMs, timeout } = { ...DEFAULTS, ...options };
  const deadline = Date.now() + timeout;
  trackNetwork(page);

  try {
    for (let round = 0; round < MAX_SETTLE_ROUNDS; round++) {
      await domQuietOnce(page, quietMs, Math.max(0, deadline - Date.now()));

      const state = networkStateByPage.get(page);
      if (!state || state.pending === 0 || Date.now() >= deadline) break;
      // A request was still in flight when the DOM went quiet - its response is very likely about to
      // mutate the DOM again (a view update), so wait for it and loop back for one more quiet pass
      // instead of declaring victory on a merely-temporary lull.
      await waitForNetworkIdle(page, { timeout: Math.max(0, deadline - Date.now()) });
    }
  } catch (err) {
    // A navigation can destroy the execution context mid-wait. Fall back to the DOM-ready signal and
    // a short settle; never let stability-waiting itself throw the test.
    if (/Execution context was destroyed|Target closed|Navigation/.test(String(err && err.message))) {
      await page.waitForLoadState('domcontentloaded').catch(() => {});
      await page.waitForTimeout(quietMs);
      await waitForBusyIndicatorGone(page);
      return;
    }
    throw err;
  }

  // DOM-quiet doesn't guarantee a busy/loading spinner has finished - see BUSY_SELECTOR's comment.
  // Near-zero added cost on the common path (nothing showing).
  await waitForBusyIndicatorGone(page);
}

module.exports = { waitForStable, waitForBusyIndicatorGone, waitForNetworkIdle, DEFAULTS };
