// Low-level objectName -> DOM locator engine.
//
// ProLife runs as Qt/QML compiled to WASM. Each QML element with an `objectName` is exposed in the
// DOM as an element carrying an `objectName="..."` attribute, and (for anything clickable) contains
// an inner `[objectName="MouseArea"]`. Visible elements also carry a `visible` attribute.
//
// A "path" is an array of objectNames from an outer container to the target; the descendant combinator
// means intermediate nesting is allowed, so you only list the meaningful anchors.

const OBJ = (name) => `[objectName="${cssEscape(name)}"]`;

// Escape a value for use inside an attribute-selector string.
function cssEscape(value) {
  return String(value).replace(/["\\]/g, '\\$&');
}

/**
 * Build the CSS selector for an objectName path (no visibility filter).
 * @param {string[]} path
 * @returns {string}
 */
function selectorForPath(path) {
  if (!Array.isArray(path) || path.length === 0) {
    throw new Error('dom.selectorForPath: path must be a non-empty array of objectNames');
  }
  return path.map(OBJ).join(' ');
}

/**
 * Locator for the (first visible) element addressed by an objectName path.
 * Visibility is filtered on the DOM attribute the QML bridge sets.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 * @returns {import('@playwright/test').Locator}
 */
function byPath(page, path) {
  return page.locator(`${selectorForPath(path)}[visible]`).first();
}

/**
 * Locator for an element by path WITHOUT the visibility filter (use when asserting hidden/absent).
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 */
function byPathAnyVisibility(page, path) {
  return page.locator(selectorForPath(path)).first();
}

/**
 * The inner clickable MouseArea of the element addressed by `path`. This is what actually receives
 * QML mouse events, so clicks target it rather than the wrapper.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 */
function mouseAreaOf(page, path) {
  return page.locator(`${selectorForPath(path)} [objectName="MouseArea"][visible]`).first();
}

/**
 * A text item inside an open PopupMenuDialog (combo-box dropdown / context menu).
 * Text-keyed delegates expose it as [objectName="<text>"] with spaces stripped from the name.
 * @param {import('@playwright/test').Page} page
 * @param {string} text
 */
function popupItem(page, text) {
  return mouseAreaOf(page, ['PopupMenuDialog', text.replace(/\s/g, '')]);
}

/**
 * The Nth selectable item in an open PopupMenuDialog, addressed by position. Separator rows have their
 * MouseArea hidden, so the [visible] filter already skips them.
 * @param {import('@playwright/test').Page} page
 * @param {number} index  zero-based position among the popup's selectable (non-separator) rows
 */
function popupItemByIndex(page, index) {
  return page.locator('[objectName="PopupMenuDialog"] [objectName="MouseArea"][visible]').nth(index);
}

/**
 * The LAST selectable item in an open PopupMenuDialog - for a menu whose trailing item sits at a
 * variable position because earlier rows are data-driven (see clickPopupItemLast in actions.js).
 * @param {import('@playwright/test').Page} page
 */
function popupItemLast(page) {
  return page.locator('[objectName="PopupMenuDialog"] [objectName="MouseArea"][visible]').last();
}

/**
 * How many (visible) elements match a path. Used by the assertion helpers to hard-fail instead of
 * silently proceeding on a missing target.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 */
async function countVisible(page, path) {
  return page.locator(`${selectorForPath(path)}[visible]`).count();
}

/**
 * How many elements match a path REGARDLESS of visibility (no [visible] filter). Distinguishes
 * "present in the DOM but hidden" from "absent".
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 */
async function countAny(page, path) {
  return page.locator(selectorForPath(path)).count();
}

/**
 * Walk a path prefix by prefix and report where it stops resolving, so an error can say whether a
 * container, the target, or its clickability was the missing piece. Only called on a failure path.
 *
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 * @param {string[]} [extra]  segments to append and probe as well (e.g. ['MouseArea'], ['TextInput'])
 * @returns {Promise<string>} one indented line per segment, ready to append to an error message
 */
async function describePath(page, path, extra = []) {
  const segments = [...path, ...extra];
  const lines = [];
  const walked = [];
  for (const segment of segments) {
    walked.push(segment);
    // eslint-disable-next-line no-await-in-loop
    const [total, visible] = await Promise.all([countAny(page, walked), countVisible(page, walked)]);
    const mark = total === 0 ? 'MISSING' : visible === 0 ? 'present but not visible' : `${visible} visible`;
    lines.push(`    ${walked.join(' > ')}: ${mark}${total > 0 ? ` (${total} in DOM)` : ''}`);
    if (total === 0) {
      // Everything deeper is missing too; only worth saying when there WAS anything deeper.
      const remaining = segments.length - walked.length;
      if (remaining > 0) {
        lines.push(`    -> the remaining ${remaining} segment(s) cannot resolve under a missing parent`);
      }
      break;
    }
  }
  return lines.join('\n');
}

/**
 * Whether `path` becomes visible within `timeout` - a probe, not an assertion. Waits rather than
 * sampling once because parts of a view (notably the command bar) populate after an async round-trip.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 * @param {number} [timeout]
 */
async function isVisible(page, path, timeout = 2000) {
  return byPath(page, path)
    .waitFor({ state: 'visible', timeout })
    .then(() => true)
    .catch(() => false);
}

// Generous timeout for a cold view to paint before "is this offered?" stops being answerable.
const OFFER_ANCHOR_TIMEOUT = 30000;
// Once the view has rendered, the element either exists or not - a short look is enough.
const OFFER_PROBE_TIMEOUT = 2000;
// Used only when the anchor never appeared: ask the slow way before answering "no".
const OFFER_FALLBACK_TIMEOUT = 10000;

/** How many VISIBLE elements match a raw attribute selector, optionally inside an objectName path. */
async function countVisibleMatching(page, selector, scope = []) {
  const prefix = scope.length ? `${selectorForPath(scope)} ` : '';
  return page.locator(`${prefix}${selector}[visible]`).count();
}

/**
 * Whether the client OFFERS `path` to the logged-in user - the permission question, asked of the
 * running client rather than of a table kept in the suite.
 *
 * `anchorSelector` names the SIBLINGS of what is being looked for; while none is on screen the view
 * hasn't rendered and "no" would be a guess. Once any sibling is visible, probe the target quickly.
 * If no sibling ever appears the view rendered nothing, so ask the slow way before returning "no".
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 * @param {{anchorSelector: string, anchorScope?: string[], anchorTimeout?: number}} options
 */
async function isOffered(page, path, options) {
  const {
    anchorSelector,
    anchorScope = [],
    anchorTimeout = OFFER_ANCHOR_TIMEOUT,
  } = options;

  const deadline = Date.now() + anchorTimeout;
  let anchored = false;
  for (;;) {
    if ((await countVisibleMatching(page, anchorSelector, anchorScope)) > 0) {
      anchored = true;
      break;
    }
    if (Date.now() >= deadline) break;
    await page.waitForTimeout(150);
  }

  return isVisible(page, path, anchored ? OFFER_PROBE_TIMEOUT : OFFER_FALLBACK_TIMEOUT);
}

/**
 * ONE bounding rect per requested column PER TABLE INSTANCE currently on screen (viewport-relative,
 * {x,y,width,height}), spanning the full visible row area. Used to mask non-deterministic columns
 * (e.g. Added/Last Modified timestamps) in screenshots.
 *
 * Rows are grouped by their nearest enclosing `[objectName="Dialog"]` (or the page root) so multiple
 * tables on screen don't merge, each group's span is clamped to its table/dialog/viewport bounds, and
 * every other open Dialog's rect is subtracted so a background column can't paint over a foreground Dialog.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} headerIds
 * @returns {Promise<{x:number,y:number,width:number,height:number}[]>}
 */
async function columnRects(page, headerIds) {
  return page.evaluate((ids) => {
    const rows = Array.from(document.querySelectorAll('[objectName^="TableRow_"][visible]'));
    if (!rows.length) return [];

    const groups = new Map();
    for (const row of rows) {
      const scope = row.closest('[objectName="Dialog"]') || document.body;
      if (!groups.has(scope)) groups.set(scope, []);
      groups.get(scope).push(row);
    }

    // [visible] filter: a dismissed Dialog can linger in the DOM and subtracting it would punch a hole.
    const dialogEls = Array.from(document.querySelectorAll('[objectName="Dialog"][visible]'));

    // Subtract `occluder` from `rect` (same x-range only) - returns 0, 1, or 2 (split above/below) rects.
    function subtractOccluder(rect, occluder) {
      const overlapsX = rect.x < occluder.right && rect.x + rect.width > occluder.left;
      if (!overlapsX) return [rect];
      const pieces = [];
      if (rect.y < occluder.top) {
        pieces.push({ ...rect, height: Math.min(rect.y + rect.height, occluder.top) - rect.y });
      }
      const bottom = rect.y + rect.height;
      if (bottom > occluder.bottom) {
        const y = Math.max(rect.y, occluder.bottom);
        pieces.push({ ...rect, y, height: bottom - y });
      }
      return pieces.filter((p) => p.height > 0);
    }

    const rects = [];
    for (const [scope, groupRows] of groups.entries()) {
      const scopeRect = scope.getBoundingClientRect();
      // Clamp to the nearest enclosing Table (the clipped row viewport) when there is one, so the mask
      // stops at the table's real bottom rather than bleeding through the dialog's title bar/footer.
      const tableEl = groupRows[0].closest('[objectName="Table"]');
      const containerRect = tableEl ? tableEl.getBoundingClientRect() : scopeRect;
      const rowRects = groupRows.map((r) => r.getBoundingClientRect());
      const rawTop = Math.min(...rowRects.map((r) => r.top));
      const rawBottom = Math.max(...rowRects.map((r) => r.bottom));
      // Never past the scope's or the table container's own visible bounds, nor the viewport.
      const groupTop = Math.max(rawTop, scopeRect.top, containerRect.top, 0);
      const groupBottom = Math.min(rawBottom, scopeRect.bottom, containerRect.bottom, window.innerHeight);
      if (groupBottom <= groupTop) continue;

      const occluders = dialogEls.filter((d) => d !== scope).map((d) => d.getBoundingClientRect());

      for (const id of ids) {
        // The WIDEST cell in the column, not the first found: a cell can render slightly past its
        // neighbours, and a mask cut to one row would leave a few pixels of another timestamp showing.
        let left = null;
        let right = null;
        for (const row of groupRows) {
          const cell = row.querySelector(`[objectName="${id}"][visible]`);
          if (!cell) continue;
          const r = cell.getBoundingClientRect();
          if (r.width <= 0) continue;
          left = left === null ? r.x : Math.min(left, r.x);
          right = right === null ? r.x + r.width : Math.max(right, r.x + r.width);
        }
        if (left === null || right - left <= 0) continue;

        let pieces = [{ x: left, y: groupTop, width: right - left, height: groupBottom - groupTop }];
        for (const occluder of occluders) {
          pieces = pieces.flatMap((p) => subtractOccluder(p, occluder));
        }
        rects.push(...pieces);
      }
    }
    return rects;
  }, headerIds);
}

module.exports = {
  OBJ,
  OFFER_ANCHOR_TIMEOUT,
  OFFER_PROBE_TIMEOUT,
  OFFER_FALLBACK_TIMEOUT,
  countVisibleMatching,
  isOffered,
  cssEscape,
  selectorForPath,
  byPath,
  byPathAnyVisibility,
  mouseAreaOf,
  popupItem,
  popupItemByIndex,
  popupItemLast,
  countVisible,
  describePath,
  countAny,
  isVisible,
  columnRects,
};
