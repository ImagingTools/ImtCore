// GUI action layer. Every function here hard-fails if its target objectName is missing, invisible,
// or ambiguous - it never silently no-ops.

const dom = require('./dom');
const { waitForStable } = require('./stability');

// Ceiling (time-to-fail) sized for 10-worker contention; raising it never slows a passing test.
const DEFAULT_TIMEOUT = 10000;
// Short, separate bound for value READS, so an unbounded evaluate auto-wait can't hang the test.
const READ_TIMEOUT = 1000;
// Extra patience given ONCE `timeout` has already run out, but only while the page is demonstrably
// still busy (a GraphQL round-trip queued behind others under worker contention, or a websocket
// frame just landed) - not a flat extension, since a page that has gone genuinely quiet without the
// target ever appearing is not going to change its mind by waiting longer.
const SETTLE_GRACE_TIMEOUT = 20000;
const SETTLE_RETRY_TIMEOUT = 2000;

function fmtPath(path) {
  return Array.isArray(path) ? path.join(' > ') : String(path);
}

/**
 * Wait for the DOM and network to actually go quiet (bounded by SETTLE_GRACE_TIMEOUT) and give
 * `locator` one more, short look. Used once an initial wait has already failed.
 * @param {import('@playwright/test').Page} page
 * @param {import('@playwright/test').Locator} locator
 */
async function settleAndRecheck(page, locator) {
  await waitForStable(page, { timeout: SETTLE_GRACE_TIMEOUT });
  try {
    await locator.waitFor({ state: 'visible', timeout: SETTLE_RETRY_TIMEOUT });
    return true;
  } catch (_) {
    return false;
  }
}

/**
 * Wait for `locator` to become visible within `timeout`; on failure, wait for the DOM and network to
 * actually go quiet and give it one more, short look before giving up. Returns whether it ended up
 * visible.
 * @param {import('@playwright/test').Page} page
 * @param {import('@playwright/test').Locator} locator
 * @param {number} timeout
 */
async function waitVisibleWithGrace(page, locator, timeout) {
  try {
    await locator.waitFor({ state: 'visible', timeout });
    return true;
  } catch (_) {
    return settleAndRecheck(page, locator);
  }
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
  if (!(await waitVisibleWithGrace(page, locator, timeout))) {
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
  const timeout = opts.timeout || DEFAULT_TIMEOUT;
  let target = mouse;
  try {
    await mouse.waitFor({ state: 'visible', timeout });
  } catch (_) {
    // No MouseArea child means the element itself is clickable, not a failure. One that EXISTS but
    // never became visible may simply be arriving late under worker contention - give it the same
    // settle-and-recheck grace as requireVisible before treating it as broken.
    if ((await dom.countAny(page, [...path, 'MouseArea'])) === 0) {
      target = self;
    } else if (!(await settleAndRecheck(page, mouse))) {
      throw new Error(
        [
          `GUI click target has a MouseArea that never became visible: [${fmtPath(path)}] after ${timeout}ms`,
          await dom.describePath(page, path, ['MouseArea']),
        ].join('\n')
      );
    }
  }
  await target.scrollIntoViewIfNeeded();
  const box = await target.boundingBox();
  if (!box) throw new Error(`GUI click target has no bounding box: [${fmtPath(path)}]`);
  await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
  await waitForStable(page);
}

/**
 * Click an element addressed by `path` at ITS OWN centre, for controls where the addressed node IS the
 * clickable surface (a bare MouseArea carrying its own objectName). Use click() by default.
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
 * Click by raw coordinates. Kept ONLY for cases with no addressable objectName. Prefer click().
 */
async function clickAt(page, x, y) {
  await page.mouse.click(x, y);
  await waitForStable(page);
}

/**
 * Click a button-like element addressed by objectName WITHIN a given Locator scope, for repeated
 * per-row controls whose OWN objectName isn't uniquely addressable page-wide.
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

// Same escaping rule as dom.js's cssEscape.
function cssEscapeLocal(value) {
  return String(value).replace(/["\\]/g, '\\$&');
}

/**
 * Navigate to a page via the left MenuPanel. `pageId` is the QML PageId (e.g. 'Workspace',
 * 'Accounts', 'Devices', 'Administration'); the button objectName is `<PageId>Button`.
 */
async function openPage(page, pageId) {
  // Under workers:10 the left menu button can lag reaching [visible] on a cold render; retry the
  // whole click a few times. click() waits for [visible] before clicking, so a retry never double-navigates.
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
  // A prior test may have left an editor document tab open; the app restores it on navigation, hiding
  // the collection command bar and table. Close leftover tabs so the page lands on its clean collection tab.
  await closeAllDocumentTabs(page);
}

/**
 * Run a command from the CommandsView command bar. `commandId` is the command element id (e.g. 'New',
 * 'Edit', 'Save', 'Undo', 'Bind'); the button objectName is `<commandId>Button`.
 */
async function clickCommand(page, commandId) {
  if ((await dom.countVisible(page, ['CommandsView', `${commandId}Button`])) > 0) {
    await click(page, ['CommandsView', `${commandId}Button`], { what: `command "${commandId}"` });
    return;
  }
  // Commands with priority -1, or pushed off a too-narrow bar, live in the "..." overflow menu.
  if ((await dom.countVisible(page, ['MoreCommandsButton'])) === 0) {
    await click(page, ['CommandsView', `${commandId}Button`], { what: `command "${commandId}"` });
    return;
  }
  await click(page, ['MoreCommandsButton'], { what: 'the "..." command overflow menu' });
  await click(page, ['PopupMenuDialog', `PopupItem_${commandId}`], { what: `command "${commandId}" in the overflow menu` });
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
// A QML text control reaches the DOM as a real <input> inside the wrapper, an <input> carrying the
// objectName itself, or the wrapper alone. Resolved in document order, so .first() lands on the wrapper.
const TEXT_INPUT_SELECTOR = '[objectName="TextInput"] input, input[objectName="TextInput"], [objectName="TextInput"]';

/**
 * Current value of a text control, or null if this element exposes none.
 *
 * Reads an <input>/<textarea> value, then the wrapper's `text` attribute, then a read-only field's
 * inner `<div class="impl">` textContent.
 */
function readTextValue(input) {
  return input
    .evaluate((el) => {
      // Single-line TextInput renders an <input>, multi-line TextEdit a <textarea>; both carry text in .value.
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
      // An empty field renders a zero-width space rather than nothing; strip it so "" compares equal.
      return value === null ? null : value.replace(/[\u200B\uFEFF]/g, '');
    }, undefined, { timeout: READ_TIMEOUT })
    .catch(() => null);
}

// A masked field renders one glyph per character, so its value says nothing about what was typed.
function isMasked(value, typed) {
  return value.length > 0 && /^[\u2022\u25CF\u00B7*]+$/.test(value) && !/[\u2022\u25CF\u00B7*]/.test(typed);
}

/**
 * The value the control MIRRORS - a real <input>, or the wrapper's `text` attribute - or null when it
 * mirrors none. Deliberately does NOT fall back to rendered `.impl` text (a multi-line editor mirrors
 * nothing there while still holding the typed text).
 */
function readMirroredValue(input) {
  return input
    .evaluate((el) => {
      // textarea as well as input: a multi-line TextEdit mirrors its text there.
      const valueOf = (node) => (node.tagName === 'INPUT' || node.tagName === 'TEXTAREA' ? node.value : null);
      const own = valueOf(el);
      if (own !== null) return own;
      const attr = el.getAttribute('text');
      if (attr !== null) return attr;
      const nested = el.querySelector('input, textarea');
      return nested ? valueOf(nested) : null;
    }, undefined, { timeout: READ_TIMEOUT })
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
    // readMirroredValue returns null for a read-only field (no <input>, no `text`); fall back to the
    // full reader so a fill into an uneditable field fails rather than passing silently.
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
 * Read a text control's current value, or null when the control at `path` exposes none (it isn't a text
 * input, or it isn't there). Same three readings as fill's own verification - see readTextValue.
 */
async function textInputValue(page, path) {
  const container = dom.byPath(page, path);
  return readTextValue(container.locator(TEXT_INPUT_SELECTOR).first());
}

/**
 * Poll a TextInput control's current DOM value until it satisfies `predicate` (default: becomes empty),
 * or throw after `timeout`. Use after an action SUPPOSED to change a text field as a side effect;
 * waitForStable can report quiet mid-update, before the field's value has actually settled.
 *
 * A path that never resolves to a readable field FAILS, it does not pass.
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
    if (value == null) {
      // Not readable yet is normal right after an action - keep polling until the window elapses.
      consecutiveMatches = 0;
      if (Date.now() >= deadline) {
        throw new Error(
          [
            `GUI text input at [${fmtPath(path)}] never became readable after ${timeout}ms, so its value could not be checked`,
            await dom.describePath(page, path),
          ].join('\n')
        );
      }
      await page.waitForTimeout(100);
      continue;
    }
    if (predicate(value)) {
      // Require the match to hold for two consecutive reads: the DOM attribute can flip to the target
      // value briefly before the WASM canvas repaint catches up.
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
 * A click on WASM canvas coordinates can occasionally no-op without throwing, so verify the postcondition.
 * @param {import('@playwright/test').Page} page
 * @param {string[]} comboPath
 */
async function openComboPopup(page, comboPath) {
  const ATTEMPTS = 3;
  for (let attempt = 1; attempt <= ATTEMPTS; attempt++) {
    await click(page, comboPath, { what: 'combo box' });
    // Wait for the popup rather than sampling once: under load it can open a beat after the click settles.
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
 * Select an item in a ComboBox / dropdown BY POSITION instead of its display text - for catalogue-driven
 * combos whose captions come from live server data and would drift out of sync if hardcoded.
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
 * directly (not via a ComboBox click), whose default delegate gives items no text-based objectName.
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
 * variable position because earlier rows are data-driven (e.g. Logout always last after per-org rows).
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
 * Dismiss the currently-open modal Dialog via Escape (wired to cancel while `escapeEnabled`, the
 * default). Use after a screenshot-only dialog test so it doesn't block the next test in a shared page.
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
 * The app restores server-persisted document tabs on a fresh goto, so a test that left an editor open
 * strands the next test inside it. The pinned collection tab has no CloseButton; closing a dirty
 * document raises a "Save all changes?" confirm which we answer No (discard).
 */
async function closeAllDocumentTabs(page) {
  for (let i = 0; i < 12; i++) {
    // CloseButton is only visible for closeable document tabs; click its inner MouseArea.
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

    // Dirty document -> "Save all changes?" confirm; click No to discard. Waited for rather than
    // sampled once, since it can paint just after the click settles.
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
 * Reload the app root and wait for it to settle. A leaked editor tab is closed in openPage(), not here.
 */
async function reload(page, url) {
  await page.goto(url || '/');
  await waitForStable(page, { timeout: 15000, quietMs: 600 });
}

/**
 * Log in through the authorization page using objectName-addressed fields. Throws if the form is missing.
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

// AuthorizationPage.qml re-asserts focus on the login field via a 500ms PauseAnimation on load.
const LOGIN_FOCUS_SETTLE = 1000;

/**
 * Put the login form's focus ring in one known place, so a screenshot of the form is reproducible.
 *
 * The ring is painted by Qt onto the canvas, so nothing in the DOM signals it. Two things move it:
 * the 500ms decoratorPause re-asserting focus, and window focus (Qt paints the ring only for a focused
 * window, which a click provides). Hence the wait, then an optional click on the given field.
 * @param {import('@playwright/test').Page} page
 * @param {{ field?: string[] }} [options]  field to click afterwards, e.g. ['PasswordInput']
 */
async function settleLoginFocus(page, options = {}) {
  await requireVisible(page, ['LoginInput'], { what: 'login field', timeout: 20000 });
  await page.waitForTimeout(LOGIN_FOCUS_SETTLE);

  if (options.field) {
    await click(page, options.field, { what: `the ${options.field.join(' > ')} field` });
    await page.waitForTimeout(250);
  }
}

module.exports = {
  DEFAULT_TIMEOUT,
  LOGIN_FOCUS_SETTLE,
  settleLoginFocus,
  requireVisible,
  click,
  clickSelf,
  clickAt,
  clickWithin,
  clickButton,
  openPage,
  clickCommand,
  fill,
  textInputValue,
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
