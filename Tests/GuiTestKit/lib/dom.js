// Low-level objectName -> DOM locator engine.
//
// ProLife runs as Qt/QML compiled to WASM. Each QML element with an `objectName` is exposed in the
// DOM as an element carrying an `objectName="..."` attribute, and (for anything clickable) contains
// an inner `[objectName="MouseArea"]`. Visible elements also carry a `visible` attribute.
//
// A "path" is an array of objectNames from an outer container to the target, e.g.
//   ['FilterPanel', 'SearchTextInput']  ->  [objectName="FilterPanel"] [objectName="SearchTextInput"]
// The descendant combinator means intermediate nesting is allowed - you only list the meaningful
// anchors, not every wrapper.

const OBJ = (name) => `[objectName="${cssEscape(name)}"]`;

// Escape a value for use inside an attribute-selector string. objectNames are simple identifiers in
// practice, but ids can contain generated segments, so be safe.
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
 * QML controls that render a text-keyed delegate expose it as [objectName="<text>"]; spaces are
 * stripped in the auto-generated names, matching imtcontrols/Buttons/Button.qml behaviour.
 * @param {import('@playwright/test').Page} page
 * @param {string} text
 */
function popupItem(page, text) {
  return mouseAreaOf(page, ['PopupMenuDialog', text.replace(/\s/g, '')]);
}

/**
 * The Nth selectable item in an open PopupMenuDialog, addressed by position instead of its display
 * text. Every delegate row is a Button-derived ItemDelegate (PopupMenuDelegate.qml), so it always
 * carries an inner [objectName="MouseArea"]; that MouseArea is explicitly hidden for separator rows
 * (PopupMenuDelegate.qml's isSeparator handling), so filtering on [visible] here already skips them
 * without needing to know the model's real item text/id - just its position in the list.
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
 * Walk a path prefix by prefix and report where it stops resolving.
 *
 * "[FilterPanel > CreationDateFilter] has no visible MouseArea" does not say whether the panel was
 * missing, the filter was missing, or the filter was there but not clickable - three different bugs
 * with one message, and telling them apart meant opening the failure screenshot by hand. This answers
 * it in the error itself. Only ever called on a failure path, so it costs nothing on a passing run.
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
 * Whether `path` becomes visible within `timeout` - a probe, not an assertion.
 *
 * This is how a test asks whether a flow is available to the CURRENT user at all. The running client
 * already holds that user's permissions and has rendered accordingly, so it is the only source that
 * cannot drift; a table of "who may do what" kept in the test suite is a copy of the product's
 * configuration and goes stale silently. Waits rather than sampling once because parts of a view
 * (notably the command bar) populate after an async round-trip.
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

/**
 * ONE bounding rect per requested column PER TABLE INSTANCE currently on screen (viewport-relative,
 * {x,y,width,height}) - spanning the full visible row area rather than one rect per row-cell. Used to
 * mask non-deterministic columns (e.g. Added/Last Modified timestamps) in screenshots:
 * TableCellDelegateBase's objectName is bound to its own column's headerId, so a cell for headerId
 * "added" is `[objectName="TableRow_i"] [objectName="added"]`.
 *
 * Masking per-ROW (one rect per cell) was tried first and had two problems: table rows have no gap
 * between them, so the per-row rects visually merge into one solid bar anyway - and the last row's
 * rect could extend past the table's actual bottom edge into the pagination bar below it (a small
 * per-row rounding/virtualization difference compounds over ~25 rows). Building one rect per column
 * instead - x/width from any single matching cell, y/height clamped to the union of all currently
 * rendered rows' own top/bottom - covers exactly the table's real extent and nothing past it.
 *
 * "TableRow_<i>" numbering also isn't globally unique (every Table instance counts its own rows from
 * 0 - see the Table class comment in controls/index.js), so a naive single min/max span across EVERY
 * matching row on the page mixes unrelated tables together whenever more than one is on screen at once
 * (e.g. a "Select device" dialog that embeds its own full collection table over the page's own table
 * behind it) - the combined span can overshoot past either table's real bottom edge into blank space.
 * Group rows by their nearest enclosing `[objectName="Dialog"]` (or the page root if none) and compute
 * each group's span independently.
 *
 * That grouping alone still isn't enough: a full page of rows (e.g. a 25-row page inside a Dialog whose
 * own scrollable content area is shorter than 25*rowHeight) reports `getBoundingClientRect()` positions
 * for its last few rows that run past the Dialog's own clipped/visible bottom edge - the QML `clip:
 * true` on the Table's Flickable stops those rows from actually being *drawn* past the boundary, but
 * doesn't stop the WASM-to-DOM bridge from reporting their real, unclipped scene position for the
 * `[visible]` attribute and bounding rect (confirmed by dumping row rects live: a Dialog whose own
 * bounding box bottom was 1030 still had rows whose bottom exceeded it). A span built from those raw
 * rects can run well past the Dialog's real bottom edge into whatever sliver of the page is still
 * visible below/around it - which is exactly the "mask covers the whole page height" bug this was
 * built to avoid. Clamp each group's span to its own scope's bounding rect (the Dialog's box, or the
 * viewport for the un-dialogued page group) so it can never extend past what's actually visible there.
 *
 * One more source of the same visual bug: the mask itself is a `position: fixed`, high-z-index DOM div
 * (see screenshot.js's addMask) painted in viewport coordinates - it has no idea about QML stacking
 * order, so a background-page group's column rect paints straight over a foreground Dialog sitting on
 * top of it wherever their x-ranges overlap (a Dialog rarely covers the full viewport - e.g. a 1820x980
 * Dialog centered in a 1920x1080 page leaves ~50px of the real page visible on every edge - but the
 * background table's OWN "Added"/"Last Modified" columns commonly line up in x with the Dialog's, since
 * both are the rightmost columns of a similarly-proportioned table). Subtract every open Dialog's rect
 * from every OTHER group's column rects before returning (splitting a rect that straddles a Dialog
 * vertically into the strip above it and the strip below, when the Dialog actually covers its x-range) -
 * a group scoped to a Dialog never has its own Dialog subtracted from itself.
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

    // Same [visible] filter as the rows above: a dismissed Dialog can linger in the DOM with a real
    // bounding rect, and subtracting it would punch a hole through the mask it is no longer covering.
    const dialogEls = Array.from(document.querySelectorAll('[objectName="Dialog"][visible]'));

    // Subtract `occluder` from `rect` (same x-range only, since these are vertical column strips) -
    // returns 0, 1 (untouched or trimmed), or 2 (split above/below) rects.
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
      // Clamping to the enclosing Dialog's own outer rect is still too generous: a Dialog's outer box
      // includes its title bar and footer (pagination controls, OK/Cancel buttons) below the table, so
      // a mask clamped only to the Dialog can bleed down through those - genuinely non-table content -
      // instead of stopping at the table's own real bottom edge. Clamp to the nearest enclosing
      // `[objectName="Table"]` (the actual clipped row viewport) when there is one; it's always at
      // least as tight as the Dialog/page bound, often tighter.
      const tableEl = groupRows[0].closest('[objectName="Table"]');
      const containerRect = tableEl ? tableEl.getBoundingClientRect() : scopeRect;
      const rowRects = groupRows.map((r) => r.getBoundingClientRect());
      const rawTop = Math.min(...rowRects.map((r) => r.top));
      const rawBottom = Math.max(...rowRects.map((r) => r.bottom));
      // Never past the scope's or the table container's own visible bounds, and never above/below the
      // viewport itself.
      const groupTop = Math.max(rawTop, scopeRect.top, containerRect.top, 0);
      const groupBottom = Math.min(rawBottom, scopeRect.bottom, containerRect.bottom, window.innerHeight);
      if (groupBottom <= groupTop) continue;

      const occluders = dialogEls.filter((d) => d !== scope).map((d) => d.getBoundingClientRect());

      for (const id of ids) {
        // The WIDEST cell in the column, not the first one found: a cell can render slightly past its
        // neighbours (longer text, a different row height), and a mask cut to one row left those few
        // pixels of somebody else's timestamp showing - invisible under a pixel budget, a failure
        // without one.
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
