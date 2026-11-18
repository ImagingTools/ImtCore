// JQML v4 support.
//
// JQML v3 builds a DOM tree, so the kit finds the items of an application with CSS on `[objectName="..."]`. JQML v4
// draws the application on a canvas and has no such tree; instead every v4 build publishes `window.qmlTest` (see
// ImtCore Docs/JQML/TestApi.md), which finds the items by their objectName and reads them. This module picks the
// backend of a page and gives the rest of the kit a locator that speaks to either.
//
// The kit addresses items with a small CSS subset: `[objectName="X"]`, `[objectName^="X"]`, `[objectName$="X"]`,
// `[objectName*="X"]`, `[visible]`, the tags `input`/`textarea`, the descendant combinator and a list of selectors
// separated by commas. qmlSelectors() turns that into the selectors of qmlTest. Two rules of those selectors matter:
// a name is always quoted (`"X":visible`; unquoted, `X:visible` is read as one name, and a name that is not ASCII
// is refused), and an item without an objectName has an empty name on v4, where v3 named it after its type. So the
// type names the kit uses (`MouseArea`, `TextInput`, `TextEdit`) stand for an item of that type that is unnamed or
// named so (`""@MouseArea`, `"MouseArea"@MouseArea`), as on v3, where a MouseArea with a name of its own is skipped.
//
// Clicks stay real mouse clicks at the middle of the item, as on v3, so the item on top at that point takes them
// just as it does for a user. Text goes in through qmlTest.fill, which focuses the editor, selects its text and types.

const backendByPage = new WeakMap();
// The address of a page where the application was not found in time, so the next look at it does not wait again.
const fallbackUrlByPage = new WeakMap();

const TYPE_NAMES = new Set(['MouseArea', 'TextInput', 'TextEdit']);
const TAG_TYPES = { input: '@TextInput', textarea: '@TextEdit' };

// What a Playwright locator waits by default.
const DEFAULT_WAIT_TIMEOUT = 30000;
const POLL_MS = 100;

// The message of a page that has no window.qmlTest at the moment, and the errors of a navigation under way.
const NOT_READY_MESSAGE = 'window.qmlTest is not ready';
const NOT_READY = /window\.qmlTest is not ready|Execution context was destroyed|Cannot find context with specified id/;

/** The backend of the page when it is already known: 'qml', 'dom' or undefined. */
function knownBackend(page) {
  return backendByPage.get(page);
}

/** One look at the page: 'qml' or 'dom' once the application is there, null before it is. */
async function detectBackend(page) {
  const known = backendByPage.get(page);
  if (known) return known;
  const kind = await page
    .evaluate(() => {
      if (window.qmlTest !== undefined) return 'qml';
      return document.querySelector('[objectName]') ? 'dom' : null;
    })
    .catch(() => null);
  if (kind) backendByPage.set(page, kind);
  return kind;
}

/**
 * The backend of the page, waiting until the application has loaded far enough to tell; 'dom' when it never does.
 * That answer is kept for the address of the page, so a page without the application costs the wait once.
 */
async function backendOf(page, timeout = DEFAULT_WAIT_TIMEOUT) {
  if (fallbackUrlByPage.get(page) === page.url()) {
    const kind = await detectBackend(page);
    return kind || 'dom';
  }
  const deadline = Date.now() + timeout;
  for (;;) {
    const kind = await detectBackend(page);
    if (kind) return kind;
    if (Date.now() >= deadline) {
      fallbackUrlByPage.set(page, page.url());
      return 'dom';
    }
    await page.waitForTimeout(POLL_MS);
  }
}

async function isQml(page) {
  return (await backendOf(page)) === 'qml';
}

function unsupported(selector, part) {
  return new Error(`GUI selector is not supported on JQML v4: ${selector} (at ${part})`);
}

/**
 * One compound of the CSS subset (no spaces) -> the compounds of qmlTest that together find the same items; more than
 * one only for a type name, which stands for the unnamed items of the type and those named after it.
 * @returns {string[]}
 */
function compoundToQml(selector, text) {
  const match = /^([A-Za-z]*)((?:\[[^\]]*\])*)$/.exec(text);
  if (!match) throw unsupported(selector, text);
  let type = '';
  if (match[1]) {
    type = TAG_TYPES[match[1].toLowerCase()];
    if (!type) throw unsupported(selector, text);
  }
  let names = [''];
  let visible = false;
  for (const attribute of match[2].match(/\[[^\]]*\]/g) || []) {
    const parts = /^\[\s*(\w+)\s*(?:([\^$*]?=)\s*"((?:[^"\\]|\\.)*)")?\s*\]$/.exec(attribute);
    if (!parts) throw unsupported(selector, attribute);
    const [, key, op, raw] = parts;
    if (key === 'visible' && !op) {
      visible = true;
      continue;
    }
    if (key !== 'objectName' || !op) throw unsupported(selector, attribute);
    const value = raw.replace(/\\(.)/g, '$1');
    if (op === '=' && TYPE_NAMES.has(value)) {
      type = `@${value}`;
      names = ['""', JSON.stringify(value)];
      continue;
    }
    if (op === '=') names = [JSON.stringify(value)];
    else if (op === '^=') names = [JSON.stringify(`${value}*`)];
    else if (op === '$=') names = [JSON.stringify(`*${value}`)];
    else names = [JSON.stringify(`*${value}*`)];
  }
  return names.map((name) => `${name}${type}${visible ? ':visible' : ''}` || '*');
}

/**
 * The qmlTest selectors for a selector of the kit's CSS subset. Their items together are the items of the CSS
 * selector: one or more per alternative of a comma list (a type name gives two, see compoundToQml).
 * @param {string} css
 * @returns {string[]}
 */
function qmlSelectors(css) {
  return css.split(',').flatMap((alternative) =>
    alternative
      .trim()
      .split(/\s+(?![^[]*\])/)
      .map((compound) => compoundToQml(css, compound))
      .reduce((heads, variants) => heads.flatMap((head) => variants.map((variant) => (head ? `${head} ${variant}` : variant))), [''])
  );
}

// ----- in the page -------------------------------------------------------------------------------------------------

/**
 * Runs in the page: resolves the steps of a KitLocator with window.qmlTest and does one operation on what they find.
 * A step is { sel: qmlTest selectors whose items together are the step's items, pick: null | index (-1 is the last) };
 * each step searches below every item the previous one found. The items of a step come in the order of the tree.
 */
async function inPage({ op, steps, args, notReady }) {
  const t = window.qmlTest;
  if (t === undefined || t.started !== true) throw new Error(notReady);
  const property = (id, name) => {
    try {
      return t.get(id, name);
    } catch (_) {
      return undefined;
    }
  };
  // The text of an item as textContent/innerText would give it: its own `text`, or the texts of its children.
  const textOf = (id, visibleOnly) => {
    const own = property(id, 'text');
    if (typeof own === 'string' && own !== '') return own;
    let out = '';
    for (const child of t.children(id)) {
      if (visibleOnly && property(child.id, 'visible') !== true) continue;
      out += textOf(child.id, visibleOnly);
    }
    return out;
  };
  const shown = (id) => {
    const info = t.info(id);
    return property(id, 'visible') === true && info.width > 0 && info.height > 0;
  };
  // The items below `scope` that one of the selectors finds, in the order of the tree, like a CSS selector list.
  const findAll = (scope, selectors) => {
    if (selectors.length === 1) return t.find(scope, selectors[0]);
    const ids = new Set(selectors.flatMap((selector) => t.find(scope, selector).map((handle) => handle.id)));
    return ids.size === 0 ? [] : t.find(scope, '*').filter((handle) => ids.has(handle.id));
  };

  let list = [];
  let scopes = [null];
  for (const step of steps) {
    list = [];
    const seen = new Set();
    for (const scope of scopes) {
      for (const handle of findAll(scope, step.sel)) {
        if (!seen.has(handle.id)) {
          seen.add(handle.id);
          list.push(handle);
        }
      }
    }
    if (step.pick !== null && step.pick !== undefined) {
      const at = step.pick < 0 ? list.length + step.pick : step.pick;
      list = at >= 0 && at < list.length ? [list[at]] : [];
    }
    scopes = list.map((handle) => handle.id);
    if (scopes.length === 0) break;
  }

  const first = list[0];
  switch (op) {
    case 'count':
      return list.length;
    case 'state':
      return first ? { attached: true, visible: shown(first.id) } : { attached: false, visible: false };
    case 'box':
      // As Playwright's boundingBox(): null for an item that is not shown.
      return first && shown(first.id) ? t.info(first.id).box : null;
    case 'boxes':
      return list
        .map((handle) => t.info(handle.id).box)
        .filter((box) => box !== null && box.width > 0 && box.height > 0);
    case 'scroll':
      if (first) await t.scrollIntoView(first.id);
      return Boolean(first);
    case 'text':
      return first ? textOf(first.id, args.visibleOnly === true) : null;
    case 'texts':
      return list.map((handle) => textOf(handle.id, args.visibleOnly === true).trim());
    case 'names':
      return list.map((handle) => handle.objectName);
    case 'value': {
      if (!first) return null;
      const value = property(first.id, 'text');
      return typeof value === 'string' ? value : null;
    }
    case 'fill':
      if (!first) return false;
      if (args.clear) return t.fill(first.id, args.text);
      await t.focus(first.id);
      t.set(first.id, 'cursorPosition', (property(first.id, 'text') || '').length);
      return t.type(args.text);
    case 'cellTexts':
      return list
        .map((row) => t.find(row.id, args.cell)[0])
        .filter(Boolean)
        .map((cell) => textOf(cell.id, false).trim());
    default:
      throw new Error(`unknown operation ${op}`);
  }
}

// ----- the locator -------------------------------------------------------------------------------------------------

function timeoutError(what, timeout) {
  return new Error(`${what}: Timeout ${timeout}ms exceeded.`);
}

/**
 * A locator of the kit's CSS subset that works on both backends: on v3 every method goes to the Playwright locator
 * of the same selectors, on v4 to window.qmlTest. It has the part of the Playwright Locator API the kit and the specs
 * use; a locator made before the backend of the page is known is one of these on v3 too.
 * A step is { css: string | { qml: string[] }, pick }; `{ qml }` gives qmlTest selectors directly (v4 only).
 */
class KitLocator {
  constructor(page, steps) {
    this.page = page;
    this.steps = steps;
  }

  toString() {
    return this.steps
      .map((step) => `${typeof step.css === 'string' ? step.css : step.css.qml.join(', ')}${step.pick === null ? '' : ` [${step.pick}]`}`)
      .join(' >> ');
  }

  locator(css) {
    return new KitLocator(this.page, [...this.steps, { css, pick: null }]);
  }

  first() {
    return this.nth(0);
  }

  last() {
    return this.nth(-1);
  }

  nth(index) {
    const steps = this.steps.slice();
    steps[steps.length - 1] = { ...steps[steps.length - 1], pick: index };
    return new KitLocator(this.page, steps);
  }

  /** The Playwright locator of the same steps (v3). */
  playwright() {
    let locator = null;
    for (const step of this.steps) {
      if (typeof step.css !== 'string') throw new Error(`GUI locator ${this} has qmlTest selectors and works on JQML v4 only`);
      locator = locator === null ? this.page.locator(step.css) : locator.locator(step.css);
      if (step.pick === -1) locator = locator.last();
      else if (step.pick !== null) locator = locator.nth(step.pick);
    }
    return locator;
  }

  isQml() {
    return isQml(this.page);
  }

  /** Runs one in-page operation; fails with a "not ready" error while the page has no window.qmlTest. */
  run(op, args = {}) {
    const steps = this.steps.map((step) => ({
      sel: typeof step.css === 'string' ? qmlSelectors(step.css) : step.css.qml,
      pick: step.pick,
    }));
    return this.page.evaluate(inPage, { op, steps, args, notReady: NOT_READY_MESSAGE });
  }

  /** run(), where a page without window.qmlTest (a navigation under way) finds nothing: that gives `nothing`. */
  async runOrNothing(nothing, op, args) {
    try {
      return await this.run(op, args);
    } catch (err) {
      if (NOT_READY.test(String(err && err.message))) return nothing;
      throw err;
    }
  }

  async count() {
    return (await this.isQml()) ? this.runOrNothing(0, 'count') : this.playwright().count();
  }

  async waitFor(options = {}) {
    if (!(await this.isQml())) return this.playwright().waitFor(options);
    const state = options.state || 'visible';
    const timeout = options.timeout === undefined ? DEFAULT_WAIT_TIMEOUT : options.timeout;
    const deadline = Date.now() + timeout;
    for (;;) {
      const now = await this.runOrNothing({ attached: false, visible: false }, 'state');
      const reached =
        state === 'attached' ? now.attached : state === 'detached' ? !now.attached : state === 'hidden' ? !now.visible : now.visible;
      if (reached) return;
      if (Date.now() >= deadline) throw timeoutError(`locator.waitFor (${state}) ${this}`, timeout);
      await this.page.waitForTimeout(POLL_MS);
    }
  }

  async isVisible() {
    if (!(await this.isQml())) return this.playwright().isVisible();

    return (await this.runOrNothing({ visible: false }, 'state')).visible;
  }

  async boundingBox(options = {}) {
    if (!(await this.isQml())) return this.playwright().boundingBox(options);
    await this.waitFor({ state: 'attached', timeout: options.timeout });
    return this.run('box');
  }

  async scrollIntoViewIfNeeded(options = {}) {
    if (!(await this.isQml())) return this.playwright().scrollIntoViewIfNeeded(options);
    await this.waitFor({ state: 'attached', timeout: options.timeout });
    await this.run('scroll');
  }

  async textContent(options = {}) {
    if (!(await this.isQml())) return this.playwright().textContent(options);
    await this.waitFor({ state: 'attached', timeout: options.timeout });
    return this.run('text', { visibleOnly: false });
  }

  async innerText(options = {}) {
    if (!(await this.isQml())) return this.playwright().innerText(options);
    await this.waitFor({ state: 'attached', timeout: options.timeout });
    return this.run('text', { visibleOnly: true });
  }

  /** A real mouse click at the middle of the item, as the kit clicks on v3. */
  async click(options = {}) {
    if (!(await this.isQml())) return this.playwright().click(options);
    await this.waitFor({ state: 'visible', timeout: options.timeout });
    await this.run('scroll');
    const box = await this.run('box');
    if (!box) throw new Error(`locator.click ${this}: the item is not shown`);
    await this.page.mouse.click(box.x + box.width / 2, box.y + box.height / 2, options);
  }

  async fill(text, options = {}) {
    if (!(await this.isQml())) return this.playwright().fill(text, options);
    await this.waitFor({ state: 'visible', timeout: options.timeout });
    await this.run('fill', { text, clear: true });
  }

  /** v3 only: the kit reads the DOM itself in a few places. */
  async evaluate(fn, arg, options) {
    if (await this.isQml()) throw new Error(`GUI locator ${this}: evaluate() reads the DOM, which JQML v4 does not have`);
    return this.playwright().evaluate(fn, arg, options);
  }
}

/**
 * The locator of a CSS selector of the kit: Playwright's own when the page is known to be v3 (the code path of the
 * kit before v4, unchanged), a KitLocator otherwise.
 * @param {import('@playwright/test').Page} page
 * @param {string} css
 */
function locate(page, css) {
  return knownBackend(page) === 'dom' ? page.locator(css) : new KitLocator(page, [{ css, pick: null }]);
}

/** Calls a method of window.qmlTest in the page (v4 only); fails with a "not ready" error while there is none. */
function callQmlTest(page, method, ...args) {
  return page.evaluate(([name, list, notReady]) => {
    if (window.qmlTest === undefined) throw new Error(notReady);
    return window.qmlTest[name](...list);
  }, [method, args, NOT_READY_MESSAGE]);
}

module.exports = {
  KitLocator,
  NOT_READY_MESSAGE,
  knownBackend,
  detectBackend,
  backendOf,
  isQml,
  compoundToQml,
  qmlSelectors,
  locate,
  callQmlTest,
};
