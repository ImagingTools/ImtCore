// Screenshot assertion + masking, and a few structural assertions.
//
// Screenshots are the primary validation mechanism. The structural helpers (expectVisible /
// expectHidden / expectCount) guard a screenshot so it is only taken once the intended state is present.

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
  // Injected from the try so a mask that throws mid-loop is still cleaned up in finally.
  try {
    for (const m of masks) handles.push(await addMask(page, m));

    await waitForStable(page);
    // maxDiffPixels: 0 - every pixel matches or the shot fails; non-deterministic content gets a mask
    // instead. threshold still absorbs sub-pixel colour variation.
    await expect(page).toHaveScreenshot(`${name}.png`, { fullPage: true, threshold: 0.05, maxDiffPixels: 0 });
  } finally {
    // Masks are injected DOM nodes; remove them so a failure doesn't leave bars over later screenshots.
    for (const h of handles) await removeMask(page, h).catch(() => {});
  }
}

// Poll timeout for the structural assertions; matches actions' DEFAULT_TIMEOUT, since the WASM UI
// populates parts of a view asynchronously (e.g. the command bar after a GetCommands round-trip).
const ASSERT_TIMEOUT = 10000;

/**
 * Assert an objectName path becomes visible. Polls until at least one visible match exists or timeout.
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
 * Assert an objectName path is NOT visible (used for permission negatives). `settleMs` gives an
 * async-appearing element a chance to show before we conclude it is genuinely absent.
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
 * The same comparison as checkScreenshot, but of ONE element instead of the whole page - for a modal
 * whose backdrop is shared, non-deterministic state the test doesn't control.
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

// --- masking -------------------

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
 * Masks for every VISIBLE element matching an objectName prefix, as plain rects. Rects, not paths:
 * "nothing to mask" is a valid answer, so this never fails when there are no matches.
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
