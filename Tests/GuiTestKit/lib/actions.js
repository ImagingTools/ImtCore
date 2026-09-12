// GUI action layer. Every function here HARD-FAILS if its target objectName is missing, invisible,
// or ambiguous - it never silently no-ops (the bug in the old fillTextInput). This is what keeps a
// screenshot-only suite honest: you can never capture a screenshot of a state reached by an action
// that quietly did nothing.

const dom = require('./dom');
const { waitForStable } = require('./stability');

// Passing interactions resolve their target within ~1-2s solo, but with multiple Playwright workers
// running concurrently (see playwright.config.js's `workers`) the WASM app's initial render + async
// data loads (the customers filter list, the command bar's GetCommands round-trip, table rows) take
// noticeably longer under shared CPU/server contention - confirmed empirically: 3000ms caused spurious
// "DevicesButton not found" failures at 3 workers; at the mandated 10 workers, 5000ms in turn was too
// tight (late-arriving combo items / command buttons / rows failed the locator before they rendered).
// 10000ms gives 10-worker contention enough headroom while still being far under a dead-wait tax on a
// genuinely-missing element. This is a ceiling (time-to-fail), so raising it never slows a passing test.
const DEFAULT_TIMEOUT = 10000;

function fmtPath(path) {
  return Array.isArray(path) ? path.join(' > ') : String(path);
}

/**
 * Resolve a required, visible target for a path or throw a descriptive error.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 * @param {{timeout?: number, what?: string}} [opts]
 * @returns {Promise<import('@playwright/test').Locator>}
 */
async function requireVisible(page, path, opts = {}) {
  const timeout = opts.timeout || DEFAULT_TIMEOUT;
  const locator = dom.byPath(page, path);
  try {
    await locator.waitFor({ state: 'visible', timeout });
  } catch (_) {
    const what = opts.what || 'element';
    throw new Error(
      [`GUI target not found: [${fmtPath(path)}] (${what}) after ${timeout}ms`, await dom.describePath(page, path)].join(
        '\n'
      )
    );
  }
  return locator;
}

/**
 * Click the element addressed by `path` (via its inner MouseArea, at its centre) and wait for the UI
 * to settle. Throws if the target is missing/invisible.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 */
async function click(page, path, opts = {}) {
  const self = await requireVisible(page, path, opts);
  const mouse = dom.mouseAreaOf(page, path);
  let target = mouse;
  try {
    await mouse.waitFor({ state: 'visible', timeout: opts.timeout || DEFAULT_TIMEOUT });
  } catch (_) {
    // An imtcontrols Button wraps its clickable surface in an inner [objectName="MouseArea"], and that
    // convention is what this addresses. Some controls ARE the surface instead, with no such child - a
    // bare QML MouseArea carrying the objectName itself. The filter chips became one of those when
    // ImtCore 9230d4c847 (2026-08-03) reworked FilterDelegateBase's two Buttons into bare MouseAreas,
    // which silently put every filter click in this suite out of reach.
    //
    // So: NO MouseArea child at all means the element itself is the thing to click. One that exists but
    // never became visible is still a failure - that is a control which should have been clickable and
    // was not, and collapsing the two would hide it.
    if ((await dom.countAny(page, [...path, 'MouseArea'])) > 0) {
      const timeout = opts.timeout || DEFAULT_TIMEOUT;
      throw new Error(
        [
          `GUI click target has a MouseArea that never became visible: [${fmtPath(path)}] after ${timeout}ms`,
          await dom.describePath(page, path, ['MouseArea']),
        ].join('\n')
      );
    }
    target = self;
  }
  await target.scrollIntoViewIfNeeded();
  const box = await target.boundingBox();
  if (!box) throw new Error(`GUI click target has no bounding box: [${fmtPath(path)}]`);
  await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
  await waitForStable(page);
}

/**
 * Click an element addressed by `path` at ITS OWN centre, for controls where the addressed node IS the
 * clickable surface (a bare MouseArea carrying its own objectName directly, e.g.
 * AuthorizationPage.qml's "Forgot password?"/"Sign up" links: `MouseArea { objectName: "RegisterUser" }`
 * with no wrapper) - unlike click(), which assumes the Button/ToolButton convention of a wrapper
 * objectName with a NESTED `[objectName="MouseArea"]` child, and throws "has no visible MouseArea" on
 * a bare self-named MouseArea like this (confirmed live). Use click() by default; reach for this only
 * when click() throws that specific error for a control that's genuinely just a MouseArea.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 */
async function clickSelf(page, path, opts = {}) {
  const target = await requireVisible(page, path, opts);
  await target.scrollIntoViewIfNeeded();
  const box = await target.boundingBox();
  if (!box) throw new Error(`GUI click target has no bounding box: [${fmtPath(path)}]`);
  await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
  await waitForStable(page);
}

/**
 * Click by raw coordinates. Kept ONLY for cases with no addressable objectName (e.g. a table cell on
 * a build where rows are not named). Prefer click()/page objects. Documented as a deliberate escape
 * hatch so its use is greppable.
 */
async function clickAt(page, x, y) {
  await page.mouse.click(x, y);
  await waitForStable(page);
}

/**
 * Click a button-like element addressed by objectName WITHIN a given Locator scope, instead of by an
 * absolute path from the page root. For repeated per-row controls whose OWN objectName isn't uniquely
 * addressable page-wide - e.g. OrderProductDelegate's Edit/Remove buttons are identical in every row
 * (the row itself carries the PRODUCT's own name as its objectName, e.g. "WidgetLicenseElementView",
 * colliding whenever two rows share a product - so tests instead locate the row by POSITION, then must
 * reach into that specific row instance for its Edit/Remove button).
 * @param {import('@playwright/test').Page} page
 * @param {import('@playwright/test').Locator} scopeLocator  a locator already narrowed to one row/instance
 * @param {string} objectName
 */
async function clickWithin(page, scopeLocator, objectName) {
  const target = scopeLocator.locator(`[objectName="${cssEscapeLocal(objectName)}"][visible]`).first();
  try {
    await target.waitFor({ state: 'visible', timeout: DEFAULT_TIMEOUT });
  } catch (_) {
    const total = await scopeLocator.locator(`[objectName="${cssEscapeLocal(objectName)}"]`).count();
    throw new Error(
      `GUI click target not found within scope: [objectName="${objectName}"] after ${DEFAULT_TIMEOUT}ms - ` +
        (total === 0 ? 'nothing matches it inside the scope' : `${total} match(es) inside the scope, none visible`)
    );
  }
  const mouse = target.locator('[objectName="MouseArea"][visible]').first();
  try {
    await mouse.waitFor({ state: 'visible', timeout: DEFAULT_TIMEOUT });
  } catch (_) {
    const total = await target.locator('[objectName="MouseArea"]').count();
    throw new Error(
      `GUI click target has no visible MouseArea within scope: [objectName="${objectName}"] - ` +
        (total === 0 ? 'it has no MouseArea child at all' : `${total} MouseArea(s) present, none visible`)
    );
  }
  await mouse.scrollIntoViewIfNeeded();
  const box = await mouse.boundingBox();
  if (!box) throw new Error(`GUI click target has no bounding box within scope: [objectName="${objectName}"]`);
  await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
  await waitForStable(page);
}

// Same escaping rule as dom.js's cssEscape - objectNames are simple identifiers in practice, but be safe.
function cssEscapeLocal(value) {
  return String(value).replace(/["\\]/g, '\\$&');
}

/**
 * Navigate to a page via the left MenuPanel. `pageId` is the QML PageId (e.g. 'Workspace',
 * 'Accounts', 'Devices', 'Administration'); the button objectName is `<PageId>Button`.
 */
async function openPage(page, pageId) {
  // The MenuPanel button is often the FIRST thing a test touches after a cold page load, and under
  // workers:10 the left menu can take a while to reach [visible] on that contended first render -
  // surfacing as "[MenuPanel > XButton] - N element(s) exist but none became visible" (the button node
  // is in the DOM but its visible attribute lags). That is transient, so retry the whole click a few
  // times, letting the DOM settle between tries, before giving up. click() waits for [visible] BEFORE
  // clicking and throws without clicking if it never appears, so a retry never double-navigates.
  const path = ['MenuPanel', `${pageId}Button`];
  let lastErr;
  for (let attempt = 1; attempt <= 3; attempt++) {
    try {
      await click(page, path, { what: `page "${pageId}"` });
      lastErr = null;
      break;
    } catch (err) {
      lastErr = err;
      if (attempt < 3) await waitForStable(page);
    }
  }
  if (lastErr) throw lastErr;
  // A prior test may have left an editor document tab open on this page; the app restores it on
  // navigation, which hides the collection command bar (New/Edit/... become present-but-not-visible)
  // and the table. Close leftover tabs so the page lands on its clean, single collection tab.
  await closeAllDocumentTabs(page);
}

/**
 * Run a command from the CommandsView command bar. `commandId` is the command element id (e.g. 'New',
 * 'Edit', 'Save', 'Undo', 'Bind'); the button objectName is `<commandId>Button`.
 */
async function clickCommand(page, commandId) {
  await click(page, ['CommandsView', `${commandId}Button`], { what: `command "${commandId}"` });
}

/**
 * Click any button by objectName path (single name or nested path).
 */
async function clickButton(page, pathOrName) {
  const path = Array.isArray(pathOrName) ? pathOrName : [pathOrName];
  await click(page, path, { what: 'button' });
}

/**
 * Type into a text input found at `path`. Waits for it, focuses it, clears it, types, and asserts the
 * field actually holds the typed value - so a wrong/missing field fails loudly.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path       Path to the control that contains the [objectName="TextInput"]
 * @param {string} text
 * @param {{clear?: boolean, verify?: boolean}} [opts]
 */
// A QML text control reaches the DOM in one of three shapes depending on the control - a real <input>
// inside the wrapper, an <input> carrying the objectName itself, or the wrapper alone. The comma list
// is resolved in DOCUMENT order, so .first() on it lands on the wrapper whenever one exists, never on
// the inner <input>: readTextValue below has to cope with the wrapper, not assume an <input>.
const TEXT_INPUT_SELECTOR = '[objectName="TextInput"] input, input[objectName="TextInput"], [objectName="TextInput"]';

/**
 * Current value of a text control, or null if this element exposes none.
 *
 * Three readings, in order, because the QML->DOM bridge renders the same logical control differently
 * depending on whether it is editable: an editable field mirrors its value onto a real <input> and onto
 * the wrapper's `text` attribute, while a READ-ONLY one has neither - its rendered string lives as the
 * textContent of an inner `<div class="impl">`. Reading only the first two is what made this return null
 * for exactly the fields expectReadOnly exists to check.
 */
function readTextValue(input) {
  return input
    .evaluate((el) => {
      // A single-line TextInput and a multi-line TextEdit render differently: the bridge gives the
      // first an <input> and the second a <textarea class="impl">, and BOTH carry their text in
      // .value. Reading a textarea's textContent returns its initial markup, which stays empty when
      // the value is set by property - so a description field that visibly held the typed text read
      // back as "" and fill() called it a no-op.
      const valueOf = (node) => (node.tagName === 'INPUT' || node.tagName === 'TEXTAREA' ? node.value : null);
      const read = () => {
        const own = valueOf(el);
        if (own !== null) return own;
        const attr = el.getAttribute('text');
        if (attr !== null) return attr;
        const nested = el.querySelector('input, textarea');
        if (nested) return valueOf(nested);
        const impl = el.classList.contains('impl') ? el : el.querySelector('.impl');
        if (!impl) return null;
        const implValue = valueOf(impl);
        return implValue !== null ? implValue : impl.textContent.trim();
      };
      const value = read();
      // An EMPTY field renders a zero-width space rather than nothing (confirmed live: every
      // "never reached the expected value" failure reported last seen U+200B), so a caller waiting for
      // "" would wait forever on a field that is, in every sense that matters, empty.
      return value === null ? null : value.replace(/[\u200B\uFEFF]/g, '');
    })
    .catch(() => null);
}

// A masked field (password echo) renders one glyph per character, so its value can be read but says
// nothing about what was typed - verifying a fill against it compares the text to a row of bullets.
function isMasked(value, typed) {
  return value.length > 0 && /^[\u2022\u25CF\u00B7*]+$/.test(value) && !/[\u2022\u25CF\u00B7*]/.test(typed);
}

/**
 * The value the control MIRRORS - a real <input>, or the wrapper's `text` attribute - or null when it
 * mirrors none.
 *
 * Deliberately does NOT fall back to the rendered `.impl` text the way readTextValue does. An empty
 * `.impl` means one of two things that cannot be told apart: the field is empty, or the control keeps
 * its text somewhere else. A multi-line editor is the second (confirmed live: the Support ticket's
 * Description shows the typed text on screen while its `.impl` reads empty), so treating "" as proof
 * that a fill did nothing fails a working app.
 */
function readMirroredValue(input) {
  return input
    .evaluate((el) => {
      // textarea as well as input: a multi-line TextEdit mirrors its text there (see readTextValue).
      const valueOf = (node) => (node.tagName === 'INPUT' || node.tagName === 'TEXTAREA' ? node.value : null);
      const own = valueOf(el);
      if (own !== null) return own;
      const attr = el.getAttribute('text');
      if (attr !== null) return attr;
      const nested = el.querySelector('input, textarea');
      return nested ? valueOf(nested) : null;
    })
    .catch(() => null);
}

async function fill(page, path, text, opts = {}) {
  const clear = opts.clear !== false;
  const verify = opts.verify !== false;

  const container = await requireVisible(page, path, { what: 'text input' });
  const input = container.locator(TEXT_INPUT_SELECTOR).first();
  try {
    await input.waitFor({ state: 'visible', timeout: opts.timeout || DEFAULT_TIMEOUT });
  } catch (_) {
    const timeout = opts.timeout || DEFAULT_TIMEOUT;
    throw new Error(
      [
        `GUI fill target has no visible TextInput: [${fmtPath(path)}] after ${timeout}ms`,
        await dom.describePath(page, path, ['TextInput']),
      ].join('\n')
    );
  }

  const box = await input.boundingBox();
  if (!box) throw new Error(`GUI fill target TextInput has no bounding box: [${fmtPath(path)}]`);
  await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);

  if (clear) {
    await page.keyboard.press('Control+A');
    await page.keyboard.press('Delete');
  }
  await page.keyboard.type(text);
  await waitForStable(page);

  if (verify && text.length > 0) {
    // readMirroredValue is deliberately narrow - it reads only what the control MIRRORS back, so a
    // formatted or masked rendering is never mistaken for the typed text. But it returns null for a
    // control with neither an <input> nor a `text` attribute, and that is precisely the shape of a
    // READ-ONLY field: `value != null` then skipped the check, and a fill into a field the user cannot
    // edit passed silently - the exact no-op this layer exists to prevent. Fall back to the full reader
    // (which also sees the .impl rendering) rather than letting the unreadable case through.
    const value = (await readMirroredValue(input)) ?? (await readTextValue(input));
    if (value == null) {
      throw new Error(
        `GUI fill cannot verify [${fmtPath(path)}]: the control exposes no readable value, so whether ` +
          'the text arrived is unknown'
      );
    }
    if (!isMasked(value, text) && !String(value).includes(text)) {
      const why = value === '' ? ' (field is empty - read-only, or the click missed it)' : '';
      throw new Error(`GUI fill did not take effect at [${fmtPath(path)}]: expected to contain "${text}", got "${value}"${why}`);
    }
  }
}

/**
 * Poll a TextInput control's current DOM value until it satisfies `predicate` (default: becomes empty),
 * or throw after `timeout`. Use after an action that's SUPPOSED to change a text field as a side effect
 * (e.g. a "Clear all filters" command clearing the search box) instead of trusting generic DOM-quiet:
 * waitForStable's MutationObserver can report quiet on a brief pause mid-update, before the field's own
 * value has actually settled - confirmed live, a "clear all filters" screenshot occasionally still
 * showed the OLD search text (stably, not a one-off glitch - "captured a stable screenshot" fired in
 * Playwright's own log, i.e. two consecutive frames already agreed with EACH OTHER, just not yet with
 * the field's final value) under concurrent-worker load, where the value-commit lags the click's own
 * settle by more than the generic quiet window. Silently returns without waiting if the path doesn't
 * resolve to a readable TextInput at all (matches fill()'s own "best-effort" treatment of that case).
 * @param {import('@playwright/test').Page} page
 * @param {string[]} path
 * @param {(value: string) => boolean} [predicate]
 * @param {number} [timeout]
 */
async function waitForTextInputValue(page, path, predicate = (v) => v === '', timeout = DEFAULT_TIMEOUT) {
  const container = dom.byPath(page, path);
  const input = container.locator(TEXT_INPUT_SELECTOR).first();

  const deadline = Date.now() + timeout;
  let consecutiveMatches = 0;
  for (;;) {
    const value = await readTextValue(input);
    if (value == null) return; // not a readable TextInput at this path - nothing to wait for
    if (predicate(value)) {
      // Require the match to hold for two consecutive reads (~100ms apart), not just one instant: the
      // DOM attribute this reads can apparently flip to the target value briefly before the WASM
      // canvas's own repaint has actually caught up (confirmed live - a single successful read here
      // still preceded a screenshot stably showing the OLD text), so one read isn't proof the visible
      // canvas has settled. Mirrors Playwright's own toHaveScreenshot stability check (two identical
      // frames in a row) applied to this attribute instead of pixels.
      consecutiveMatches++;
      if (consecutiveMatches >= 2) return;
    } else {
      consecutiveMatches = 0;
    }
    if (Date.now() >= deadline) {
      throw new Error(`GUI text input at [${fmtPath(path)}] never reached the expected value (last seen: "${value}")`);
    }
    await page.waitForTimeout(100);
  }
}

/**
 * Click a combo box and confirm its PopupMenuDialog actually opened, retrying the click once if not.
 * A click that lands right after an unrelated DOM update elsewhere on the page (e.g. a search box's
 * live-filtered table re-render) can occasionally miss/no-op on WASM canvas coordinates without
 * throwing - the combo just silently doesn't open. That leaves nothing for the caller to find (an
 * item lookup then fails misleadingly as "not found"), so verify the actual postcondition here rather
 * than assuming the click worked.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} comboPath
 */
async function openComboPopup(page, comboPath) {
  const ATTEMPTS = 3;
  for (let attempt = 1; attempt <= ATTEMPTS; attempt++) {
    await click(page, comboPath, { what: 'combo box' });
    // WAIT for the popup rather than sampling its presence once: under concurrent-worker load the
    // PopupMenuDialog can open a beat after the click's own settle returns, and an immediate count()
    // then reads 0 and wrongly declares the click a no-op. Give each attempt a short window before
    // re-clicking, so a merely-slow open isn't mistaken for a dropped click.
    try {
      await page
        .locator('[objectName="PopupMenuDialog"][visible]')
        .first()
        .waitFor({ state: 'visible', timeout: 2500 });
      return;
    } catch (_) {
      if (attempt === ATTEMPTS) {
        throw new Error(`GUI combo box did not open its popup after ${attempt} attempts: [${fmtPath(comboPath)}]`);
      }
    }
  }
}

/**
 * Select an item in a ComboBox / dropdown: click the combo (opens PopupMenuDialog), then click the
 * item whose text matches `itemText` (spaces stripped, matching the QML auto-naming).
 * @param {import('@playwright/test').Page} page
 * @param {string[]} comboPath
 * @param {string} itemText
 */
async function select(page, comboPath, itemText) {
  await openComboPopup(page, comboPath);
  const item = dom.popupItem(page, itemText);
  try {
    await item.waitFor({ state: 'visible', timeout: DEFAULT_TIMEOUT });
  } catch (_) {
    throw new Error(`GUI combo item not found in PopupMenuDialog: "${itemText}" (opened from [${fmtPath(comboPath)}])`);
  }
  const box = await item.boundingBox();
  if (!box) throw new Error(`GUI combo item "${itemText}" has no bounding box`);
  await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
  await waitForStable(page);
}

/**
 * Select an item in a ComboBox / dropdown BY POSITION instead of its display text. Catalogue-driven
 * combos (device type, hardware configuration, ...) are populated from seeded/live server data, so a
 * hardcoded caption ("RTV", "Sensorswithalicense", ...) silently drifts out of sync with whatever the
 * test database actually contains - this instead just opens the popup and clicks its Nth row, which
 * works regardless of what that row's real catalogue text is.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} comboPath
 * @param {number} index
 */
async function selectIndex(page, comboPath, index) {
  await openComboPopup(page, comboPath);
  const item = dom.popupItemByIndex(page, index);
  try {
    await item.waitFor({ state: 'visible', timeout: DEFAULT_TIMEOUT });
  } catch (_) {
    throw new Error(`GUI combo item not found in PopupMenuDialog at index ${index} (opened from [${fmtPath(comboPath)}])`);
  }
  const box = await item.boundingBox();
  if (!box) throw new Error(`GUI combo item at index ${index} has no bounding box`);
  await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
  await waitForStable(page);
}

/**
 * Click the Nth item of an ALREADY-OPEN PopupMenuDialog, by position - for popups a command opens
 * directly (not via a ComboBox click), e.g. CreateLicenseFile's Encrypt/Unencrypt choice
 * (DeviceCollectionViewCommandsDelegate.qml's encryptPopupMenuDialog uses PopupMenuDialog's own
 * default delegate, which - unlike ComboBox.qml's inline delegate - does NOT give each item a
 * text-based objectName, so only positional addressing works here).
 * @param {import('@playwright/test').Page} page
 * @param {number} index
 */
async function clickPopupItemByIndex(page, index) {
  const item = dom.popupItemByIndex(page, index);
  try {
    await item.waitFor({ state: 'visible', timeout: DEFAULT_TIMEOUT });
  } catch (_) {
    throw new Error(`GUI popup item not found in PopupMenuDialog at index ${index}`);
  }
  const box = await item.boundingBox();
  if (!box) throw new Error(`GUI popup item at index ${index} has no bounding box`);
  await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
  await waitForStable(page);
}

/**
 * Click the LAST item of an ALREADY-OPEN PopupMenuDialog - for a menu whose trailing item is at a
 * variable position because earlier rows are data-driven (e.g. UserPanel.qml's account menu: Profile,
 * then one row per organization the user belongs to, then "No organization", then Logout always last).
 * Same no-text-objectName reasoning as clickPopupItemByIndex.
 * @param {import('@playwright/test').Page} page
 */
async function clickPopupItemLast(page) {
  const item = dom.popupItemLast(page);
  try {
    await item.waitFor({ state: 'visible', timeout: DEFAULT_TIMEOUT });
  } catch (_) {
    throw new Error('GUI popup item not found in PopupMenuDialog (last)');
  }
  const box = await item.boundingBox();
  if (!box) throw new Error('GUI popup item (last) has no bounding box');
  await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
  await waitForStable(page);
}

/**
 * Dismiss the currently-open modal Dialog (imtcontrols/Dialogs/Dialog.qml, and everything built on
 * it - Revision/Remove/Bind/TransferLicenses/info dialogs) via Escape, which every such dialog wires
 * to `finished(Enums.cancel)` as long as `escapeEnabled` (the default). Use after a test that only
 * screenshots a dialog and doesn't otherwise interact with it, so a chained/shared-page test suite
 * (test.describe.serial with one page for the whole block) doesn't leave a modal blocking the next
 * test's clicks - a bare reload() used to paper over this by wiping the dialog along with everything
 * else, which a shared page no longer does.
 * @param {import('@playwright/test').Page} page
 */
async function dismissDialog(page) {
  await page.keyboard.press('Escape');
  await waitForStable(page);
}

/**
 * Scroll with the mouse wheel (positive = down).
 */
async function scroll(page, deltaY, at) {
  if (at) await page.mouse.move(at.x, at.y);
  await page.mouse.wheel(0, deltaY);
  await waitForStable(page);
}

/**
 * Close every open document tab, so the workspace is a clean single collection tab.
 *
 * ProLife (MultiDocumentCollectionView.qml) persists the user's open document tabs in server-side
 * workspace state, so a fresh page.goto RESTORES whatever tabs a previous test left open - a test that
 * opened a "New" editor and didn't close it strands the next test inside that editor (no collection
 * command bar, so "New" etc. are present-but-not-visible). That is a pure test-isolation defect, not a
 * server/timeout issue. This closes the leftovers: the pinned collection tab has no CloseButton (only
 * document tabs do), and closing a dirty document raises a "Save all changes?" confirm - we answer No
 * (discard) so cleanup never blocks on unsaved edits.
 */
async function closeAllDocumentTabs(page) {
  for (let i = 0; i < 12; i++) {
    // The tab's close "X" carries objectName "CloseButton" and (per TabPanelDecorator.qml) is only
    // visible for closeable document tabs - the pinned collection tab has none. Click its inner
    // MouseArea, which is what QML actually reacts to.
    const closeBtn = page.locator('[objectName="CloseButton"][visible]').first();
    if ((await closeBtn.count()) === 0) {
      return;
    }
    const mouse = closeBtn.locator('[objectName="MouseArea"]').first();
    const target = (await mouse.count()) > 0 ? mouse : closeBtn;
    const box = await target.boundingBox();
    if (!box) return;
    await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
    await waitForStable(page);

    // Dirty document -> "Save all changes?" (Yes/No) confirm. Click No to discard and proceed. Waited
    // for rather than sampled once: under worker contention it can paint just after the click's own
    // settle, and a missed No leaves a modal that swallows every later click in the run.
    const noBtn = page.locator('[objectName="NoButton"][visible]').first();
    const confirmShown = await noBtn
      .waitFor({ state: 'visible', timeout: 1500 })
      .then(() => true)
      .catch(() => false);
    if (confirmShown) {
      const nMouse = noBtn.locator('[objectName="MouseArea"]').first();
      const nTarget = (await nMouse.count()) > 0 ? nMouse : noBtn;
      const nbox = await nTarget.boundingBox();
      if (nbox) {
        await page.mouse.click(nbox.x + nbox.width / 2, nbox.y + nbox.height / 2);
        await waitForStable(page);
      }
    }
  }
}

/**
 * Reload the app root and wait for it to settle.
 *
 * NOTE: a leaked editor tab is NOT closed here - the app opens to the Workspace landing, and a Devices
 * (etc.) editor tab a prior test left open only re-appears once you navigate to that page. Tab cleanup
 * therefore happens in openPage() (right after navigation), not here.
 */
async function reload(page, url) {
  await page.goto(url || '/');
  await waitForStable(page, { timeout: 15000, quietMs: 600 });
}

/**
 * Log in through the authorization page using objectName-addressed fields (LoginInput /
 * PasswordInput / LoginButton from ImtCore's AuthorizationPage.qml). Throws if the form is missing.
 * Used by global-setup to mint each user's storageState.
 * @param {import('@playwright/test').Page} page
 * @param {string} username
 * @param {string} password
 */
async function login(page, username, password) {
  await requireVisible(page, ['LoginInput'], { what: 'login field', timeout: 20000 });
  await fill(page, ['LoginInput'], username, { verify: false });
  await fill(page, ['PasswordInput'], password, { verify: false });
  await click(page, ['LoginButton'], { what: 'Sign in button' });
  await waitForStable(page, { timeout: 15000, quietMs: 600 });
}

module.exports = {
  DEFAULT_TIMEOUT,
  requireVisible,
  click,
  clickSelf,
  clickAt,
  clickWithin,
  clickButton,
  openPage,
  clickCommand,
  fill,
  waitForTextInputValue,
  select,
  selectIndex,
  openComboPopup,
  clickPopupItemByIndex,
  clickPopupItemLast,
  dismissDialog,
  scroll,
  reload,
  closeAllDocumentTabs,
  login,
};
