// Screenshot assertion + masking, and a few structural assertions.
//
// Screenshots are the primary validation mechanism (per project decision). The structural helpers
// (expectVisible / expectHidden / expectCount) exist to guard a screenshot - so it is only taken once
// the intended state is actually present. They never replace the screenshot; they make it trustworthy.

const { expect } = require('@playwright/test');
const dom = require('./dom');
const { waitForStable } = require('./stability');

/**
 * Compare the current page against a stored baseline. `name` is the logical snapshot name; the
 * per-user directory and platform suffix are applied by snapshotPathTemplate in playwright.config.js.
 * @param {import('@playwright/test').Page} page
 * @param {string} name
 * @param {Object|Object[]} [mask]  {path:[...]} or {x,y,width,height} (+ optional padding); or array
 */
async function checkScreenshot(page, name, mask) {
  const masks = mask ? (Array.isArray(mask) ? mask : [mask]) : [];
  const handles = [];
  // Inside the try from the first mask onwards: addMask can throw partway through the loop (a target
  // that never resolves), and the masks already injected would then stay in the DOM, black-barring
  // every later screenshot in the same shared-page block.
  try {
    for (const m of masks) handles.push(await addMask(page, m));

    await waitForStable(page);
    // snapshotPathTemplate derives {ext} from this argument's own extension, so toHaveScreenshot needs
    // it spelled out here - a bare name is rejected before any pixels are compared.
    //
    // maxDiffPixels: 0 - every pixel matches, or the shot fails. A budget cannot tell "render noise"
    // from "a date nobody masked": both are a handful of pixels, and the second is a screenshot that
    // quietly stopped checking what it was written to check. Anything genuinely non-deterministic
    // (timestamps, generated ids, secrets) gets a mask instead; `threshold` still absorbs sub-pixel
    // colour variation inside a pixel.
    await expect(page).toHaveScreenshot(`${name}.png`, { fullPage: true, threshold: 0.05, maxDiffPixels: 0 });
  } finally {
    // Masks are injected DOM nodes, so they outlive a failure and would sit over every later screenshot
    // in the same shared page - turning one real failure into a run of unrelated-looking ones.
    for (const h of handles) await removeMask(page, h).catch(() => {});
  }
}

// How long the polling structural assertions wait for the target to reach the expected state. The
// WASM UI populates parts of a view asynchronously (e.g. the command bar fills in after a GraphQL
// GetCommands round-trip that can land AFTER waitForStable's quiet window), so a single-shot count is
// inherently racy - poll instead. 10000ms matches lib/actions' DEFAULT_TIMEOUT: under the mandated 10
// concurrent workers that GetCommands round-trip (and other async view population) is slow enough that
// the old 5000ms poll could give up before a legitimately-present command/element appeared.
const ASSERT_TIMEOUT = 10000;

/**
 * Assert an objectName path becomes visible. Polls until at least one visible match exists or the
 * timeout elapses - so a target that renders a moment after the page "settles" is waited for, not
 * failed on instantly (the old single-shot countVisible was the top source of flaky failures).
 */
async function expectVisible(page, path, message) {
  await expect
    .poll(() => dom.countVisible(page, path), {
      message: message || `expected [${path.join(' > ')}] to be visible`,
      timeout: ASSERT_TIMEOUT,
    })
    .toBeGreaterThan(0);
}

/**
 * Assert an objectName path is NOT visible (used for permission negatives). Waits briefly and requires
 * the count to be (and stay) 0 - a plain single-shot check could pass simply because the element had
 * not rendered yet. `settleMs` gives an async-appearing element a chance to show before we conclude
 * it is genuinely absent.
 */
async function expectHidden(page, path, message, settleMs = 800) {
  await waitForStable(page);
  await page.waitForTimeout(settleMs);
  const count = await dom.countVisible(page, path);
  expect(count, message || `expected [${path.join(' > ')}] to be hidden/absent`).toBe(0);
}

/** Assert the number of visible matches for a path (polls until it reaches `expected` or times out). */
async function expectCount(page, path, expected, message) {
  await expect
    .poll(() => dom.countVisible(page, path), {
      message: message || `expected ${expected} visible [${path.join(' > ')}]`,
      timeout: ASSERT_TIMEOUT,
    })
    .toBe(expected);
}

/**
 * The same comparison as checkScreenshot, but of ONE element instead of the whole page.
 *
 * For a modal: what is behind it is not what the test is about, and it is not under the test's
 * control either - every spec signed in as the same user shares the server-side column layout and
 * last-open page, so the backdrop changes when an unrelated test runs beside this one (measured as a
 * 21524-pixel diff behind an identical Profile dialog). Shooting the dialog says what the test means.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path  objectName path of the element to compare
 * @param {string} name
 * @param {Object|Object[]} [mask]
 */
async function checkElementScreenshot(page, path, name, mask) {
  const masks = mask ? (Array.isArray(mask) ? mask : [mask]) : [];
  const handles = [];
  try {
    for (const m of masks) handles.push(await addMask(page, m));

    await waitForStable(page);
    const locator = dom.byPath(page, path);
    await locator.waitFor({ state: 'visible' });
    await expect(locator).toHaveScreenshot(`${name}.png`, { threshold: 0.05, maxDiffPixels: 0 });
  } finally {
    for (const h of handles) await removeMask(page, h).catch(() => {});
  }
}

// --- masking (ported and de-duplicated from the old utils.addMask/removeMask) -------------------

let maskSeq = 0;

async function addMask(page, maskParams) {
  let rect = null;

  if (maskParams.path) {
    const locator = dom.byPathAnyVisibility(page, maskParams.path);
    await locator.waitFor({ timeout: 3000 });
    rect = await locator.boundingBox();
    if (!rect) throw new Error('mask: boundingBox is null for path ' + maskParams.path.join(' > '));
  } else if (maskParams.x !== undefined) {
    rect = { x: maskParams.x, y: maskParams.y, width: maskParams.width, height: maskParams.height };
  }

  if (!rect) throw new Error('mask must specify either { path } or { x, y, width, height }');

  const pad = maskParams.padding || 0;
  const id = `pw-mask-${++maskSeq}`;
  await page.evaluate(
    ({ id, x, y, width, height }) => {
      const mask = document.createElement('div');
      Object.assign(mask.style, {
        position: 'fixed',
        top: y + 'px',
        left: x + 'px',
        width: width + 'px',
        height: height + 'px',
        backgroundColor: '#000',
        zIndex: '99999',
        pointerEvents: 'none',
      });
      mask.setAttribute('data-mask-id', id);
      document.body.appendChild(mask);
    },
    { id, x: rect.x - pad, y: rect.y - pad, width: rect.width + pad * 2, height: rect.height + pad * 2 }
  );
  return id;
}

async function removeMask(page, id) {
  await page.evaluate((id) => {
    const el = document.querySelector(`div[data-mask-id="${id}"]`);
    if (el) el.remove();
  }, id);
}

/**
 * Masks for every VISIBLE element matching an objectName prefix, as plain rects.
 *
 * Rects, not paths: a mask given a path waits for that exact element and fails the screenshot if it
 * is not there - which is the wrong outcome for "mask the comment timestamps" or "mask the product
 * rows", where how many there are is part of what the test is exercising. Nothing to mask is a valid
 * answer, and a caller that needs the element to exist should assert that separately.
 * @param {import('@playwright/test').Page} page
 * @param {string} objectNamePrefix
 * @param {number} [padding]
 */
async function masksForPrefix(page, objectNamePrefix, padding = 3) {
  const rects = await page.evaluate((prefix) => {
    return Array.from(document.querySelectorAll(`[objectName^="${prefix}"][visible]`))
      .map((el) => el.getBoundingClientRect())
      .filter((r) => r.width > 0 && r.height > 0)
      .map((r) => ({ x: r.x, y: r.y, width: r.width, height: r.height }));
  }, objectNamePrefix);
  return rects.map((r) => ({ ...r, padding }));
}

module.exports = {
  checkScreenshot,
  checkElementScreenshot,
  masksForPrefix,
  expectVisible,
  expectHidden,
  expectCount,
  addMask,
  removeMask,
};
