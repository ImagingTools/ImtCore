// Wait for the QML/WASM canvas DOM to settle after an interaction.
//
// Installs a single in-page MutationObserver and resolves once there have been no DOM mutations for
// `quietMs`, or bails out after `timeout`. Network tracking below turns "is a request still in flight"
// from a guess into a fact, so quietMs only has to cover the render flush after mutations stop.

const DEFAULTS = { quietMs: 150, timeout: 6000 };
const NETWORK_IDLE_TIMEOUT = 5000;
// A settled action is: DOM mutates -> quiet while a request is in flight -> response -> DOM mutates ->
// quiet for real. Allow a couple of rounds for chained requests, bounded overall by `timeout`.
const MAX_SETTLE_ROUNDS = 3;

// The busy/loading spinners animate a style property ~11x/second, so the observer never sees the DOM
// go quiet while one is up. They also legitimately signal a view is still arriving (filtering their
// frames was tried and reverted), so poll for the spinner directly rather than relying on DOM-quiet.
const BUSY_SELECTOR = '[objectName="BusyIndicator"][visible]';
const BUSY_INDICATOR_TIMEOUT = 5000;

/**
 * Wait for any visible busy/loading indicator to disappear. Swallows its own timeout rather than
 * throwing - this is a screenshot-noise reducer, not a structural assertion.
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
// Per-page in-flight XHR/fetch counter, attached lazily and event-driven. Narrowed to xhr/fetch
// (GraphQL calls) so boot-time asset requests and long-lived connections don't pad or stall the wait.
const networkStateByPage = new WeakMap();

function isTrackedRequest(req) {
  const type = req.resourceType();
  return type === 'xhr' || type === 'fetch';
}

function trackNetwork(page) {
  let state = networkStateByPage.get(page);
  if (state) return state;
  state = { pending: 0, inFlight: new Set(), waiters: [] };
  const settle = () => {
    if (state.pending === 0 && state.waiters.length) {
      const waiters = state.waiters;
      state.waiters = [];
      waiters.forEach((resolve) => resolve());
    }
  };
  // Track the request OBJECTS, not just a count: a bare counter also decremented for requests started
  // before these listeners attached, letting `pending` hit 0 while something was still in flight.
  page.on('request', (req) => {
    if (isTrackedRequest(req)) {
      state.inFlight.add(req);
      state.pending = state.inFlight.size;
    }
  });
  const onDone = (req) => {
    if (state.inFlight.delete(req)) {
      state.pending = state.inFlight.size;
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
 * is in flight. Swallows its own timeout like waitForBusyIndicatorGone.
 * @param {import('@playwright/test').Page} page
 * @param {{timeout?: number}} [options]
 */
async function waitForNetworkIdle(page, options = {}) {
  const { timeout = NETWORK_IDLE_TIMEOUT } = options;
  const state = trackNetwork(page);
  if (state.pending === 0) return;
  await new Promise((resolve) => {
    const waiter = () => {
      clearTimeout(timer);
      resolve();
    };
    // Remove the waiter on the timeout path, so it doesn't accumulate and get re-invoked by later settle().
    const timer = setTimeout(() => {
      const index = state.waiters.indexOf(waiter);
      if (index !== -1) state.waiters.splice(index, 1);
      resolve();
    }, Math.max(0, timeout));
    state.waiters.push(waiter);
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
      // A request was still in flight when the DOM went quiet - wait for it and loop for one more pass.
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
  await waitForBusyIndicatorGone(page);
}

module.exports = { waitForStable, waitForBusyIndicatorGone, waitForNetworkIdle, DEFAULTS };
