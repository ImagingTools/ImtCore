// Screenshot assertion + masking, and a few structural assertions.
//
// Screenshots are the primary validation mechanism. The structural helpers (expectVisible /
// expectHidden / expectCount) guard a screenshot so it is only taken once the intended state is present.

const { expect } = require('@playwright/test');
const dom = require('./dom');
const { waitForStable } = require('./stability');

// A screenshot is the only assertion that compares EVERY pixel, so it settles harder than an action
// does. A cold view arrives in chunks (shell, icons, command bar, first page of rows) and is DOM-quiet
// BETWEEN them - with nothing in flight yet, because the next request has not been sent - so the short
// quiet window an action is happy with can land inside such a gap and capture a half-drawn view. As a
// baseline that is permanent: every later run then compares against a page that never finished loading.
const SCREENSHOT_SETTLE = { quietMs: 600, timeout: 20000 };

// Text glyphs rasterise a few pixels differently from one machine to the next, so a baseline made on a
// developer box fails on the build agent over antialiasing alone - measured across a full Agentino run
// on both: 2 differing pixels on a page holding no machine-specific content, 4-21 raw pixels elsewhere.
// This budget sits well above that, which is a deliberate trade: a real change of the same order passes
// unnoticed (a drifting build number in a table cell measured 62px). Anything larger still fails - a
// machine name in a column was 1161px, a mask sized to its content ~5300px. Non-deterministic CONTENT
// belongs in a mask, never in this budget.
const MAX_DIFF_PIXELS = 100;

// Per-pixel colour tolerance, and the right knob for cross-machine rasterisation: antialiasing is
// MANY pixels differing by almost nothing, so raising this drops them out of the count entirely,
// whereas MAX_DIFF_PIXELS would let a few pixels differ by any amount at all. Measured on one
// icon-dense page across two machines: 18224 pixels differ at all, but the median difference is 1 of
// 255 and only a single pixel exceeds 95. At 0.05 that still counted 105 and failed the shot; 0.2 -
// Playwright's own default, which this kit had tightened - leaves a handful. Real changes are not
// affected: dark text on white differs by 200+, well beyond any threshold.
const THRESHOLD = 0.2;

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

    await waitForStable(page, SCREENSHOT_SETTLE);
    // MAX_DIFF_PIXELS absorbs cross-machine font antialiasing and nothing else; non-deterministic
    // CONTENT still gets a mask, never this budget. THRESHOLD handles the rasterisation noise.
    await expect(page).toHaveScreenshot(`${name}.png`, { fullPage: true, threshold: THRESHOLD, maxDiffPixels: MAX_DIFF_PIXELS });
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
  // Sampled ACROSS the settle window rather than once at the end of it. A single late sample turns a
  // slow arrival into a PASS, which is the one failure mode a negative assertion must not have: under
  // load the element this is meant to catch is exactly the one that arrives late.
  const deadline = Date.now() + settleMs;
  const fail = message || `expected [${path.join(' > ')}] to be hidden/absent`;
  for (;;) {
    const count = await dom.countVisible(page, path);
    if (count > 0) expect(count, fail).toBe(0);
    if (Date.now() >= deadline) return;
    await page.waitForTimeout(100);
  }
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

    await waitForStable(page, SCREENSHOT_SETTLE);
    const locator = dom.byPath(page, path);
    await locator.waitFor({ state: 'visible' });
    await expect(locator).toHaveScreenshot(`${name}.png`, { threshold: THRESHOLD, maxDiffPixels: MAX_DIFF_PIXELS });
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
  // fixedWidth/fixedHeight pin the mask to a size that is the same on every run, unlike rect.width/
  // height which is only as wide as the masked element's CURRENT content (e.g. a text label with no
  // explicit width). A tight mask bakes in whatever size happened to be true when the baseline was
  // captured, so a later run whose content renders a different width leaves a boundary sliver exposed.
  const width = maskParams.fixedWidth !== undefined ? maskParams.fixedWidth : rect.width + pad * 2;
  const height = maskParams.fixedHeight !== undefined ? maskParams.fixedHeight : rect.height + pad * 2;
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
    { id, x: rect.x - pad, y: rect.y - pad, width, height }
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
 * @param {number} [fixedWidth]  pin the mask to this width instead of the element's own (variable)
 *   rendered width - needed for content whose text width isn't stable run to run (e.g. an unpadded
 *   day-of-month), so the mask doesn't leave a different-sized sliver exposed each time.
 */
async function masksForPrefix(page, objectNamePrefix, padding = 3, fixedWidth) {
  const rects = await page.evaluate((prefix) => {
    return Array.from(document.querySelectorAll(`[objectName^="${prefix}"][visible]`))
      .map((el) => el.getBoundingClientRect())
      .filter((r) => r.width > 0 && r.height > 0)
      .map((r) => ({ x: r.x, y: r.y, width: r.width, height: r.height }));
  }, objectNamePrefix);
  return rects.map((r) => ({ ...r, padding, ...(fixedWidth !== undefined ? { fixedWidth } : {}) }));
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
