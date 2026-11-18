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
// is refused), and an item without an objectName has an empty name on v4, where v3 named it after its type, so the
// type names the kit uses (`MouseArea`, `TextInput`, `TextEdit`) become a type test (`@MouseArea`).
//
// Clicks stay real mouse clicks at the middle of the item, as on v3, so the item on top at that point takes them
// just as it does for a user. Text goes in through qmlTest.fill, which focuses the editor, selects its text and types.

const backendByPage = new WeakMap();

const TYPE_NAMES = new Set(['MouseArea', 'TextInput', 'TextEdit']);
const TAG_TYPES = { input: '@TextInput', textarea: '@TextEdit' };

// What a Playwright locator waits by default.
const DEFAULT_WAIT_TIMEOUT = 30000;
const POLL_MS = 100;

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

/** The backend of the page, waiting until the application has loaded far enough to tell; 'dom' when it never does. */
async function backendOf(page, timeout = DEFAULT_WAIT_TIMEOUT) {
  const deadline = Date.now() + timeout;
  for (;;) {
    const kind = await detectBackend(page);
    if (kind) return kind;
    if (Date.now() >= deadline) return 'dom';
    await page.waitForTimeout(POLL_MS);
  }
}

async function isQml(page) {
  return (await backendOf(page)) === 'qml';
}

function unsupported(selector, part) {
  return new Error(`GUI selector is not supported on JQML v4: ${selector} (at ${part})`);
}

// One compound of the CSS subset (no spaces) -> one compound of a qmlTest selector.
function compoundToQml(selector, text) {
  const match = /^([A-Za-z]*)((?:\[[^\]]*\])*)$/.exec(text);
  if (!match) throw unsupported(selector, text);
  let type = '';
  if (match[1]) {
    type = TAG_TYPES[match[1].toLowerCase()];
    if (!type) throw unsupported(selector, text);
  }
  let name = '';
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
      continue;
    }
    if (op === '=') name = value;
    else if (op === '^=') name = `${value}*`;
    else if (op === '$=') name = `*${value}`;
    else name = `*${value}*`;
  }
  const out = `${name ? JSON.stringify(name) : ''}${type}${visible ? ':visible' : ''}`;
  return out || '*';
}

/**
 * The qmlTest selectors for a selector of the kit's CSS subset: one per alternative of a comma list, tried in turn.
 * @param {string} css
 * @returns {string[]}
 */
function qmlSelectors(css) {
  return css.split(',').map((alternative) =>
    alternative
      .trim()
      .split(/\s+(?![^[]*\])/)
      .map((compound) => compoundToQml(css, compound))
      .join(' ')
  );
}

// ----- in the page -------------------------------------------------------------------------------------------------

/**
 * Runs in the page: resolves the steps of a KitLocator with window.qmlTest and does one operation on what they find.
 * A step is { sel: [qmlTest selectors, the first that finds anything wins], pick: null | index (-1 is the last) };
 * each step searches below every item the previous one found.
 */
async function inPage({ op, steps, args }) {
  const t = window.qmlTest;
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

  let list = [];
  let scopes = [null];
  for (const step of steps) {
    list = [];
    const seen = new Set();
    for (const scope of scopes) {
      for (const selector of step.sel) {
        const found = t.find(scope, selector);
        if (found.length === 0) continue;
        for (const handle of found) {
          if (!seen.has(handle.id)) {
            seen.add(handle.id);
            list.push(handle);
          }
        }
        break;
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
      return first ? t.info(first.id).box : null;
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
    case 'describe':
      return first ? t.dump(first.id, { depth: 3 }) : '';
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
 * of the same selectors, on v4 to window.qmlTest. It has the part of the Playwright Locator API the kit uses.
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

  run(op, args = {}) {
    const steps = this.steps.map((step) => ({
      sel: typeof step.css === 'string' ? qmlSelectors(step.css) : step.css.qml,
      pick: step.pick,
    }));
    return this.page.evaluate(inPage, { op, steps, args });
  }

  async count() {
    return (await this.isQml()) ? this.run('count') : this.playwright().count();
  }

  async waitFor(options = {}) {
    if (!(await this.isQml())) return this.playwright().waitFor(options);
    const state = options.state || 'visible';
    const timeout = options.timeout === undefined ? DEFAULT_WAIT_TIMEOUT : options.timeout;
    const deadline = Date.now() + timeout;
    for (;;) {
      const now = await this.run('state');
      const reached =
        state === 'attached' ? now.attached : state === 'detached' ? !now.attached : state === 'hidden' ? !now.visible : now.visible;
      if (reached) return;
      if (Date.now() >= deadline) throw timeoutError(`locator.waitFor (${state}) ${this}`, timeout);
      await this.page.waitForTimeout(POLL_MS);
    }
  }

  async isVisible() {
    return (await this.isQml()) ? (await this.run('state')).visible : this.playwright().isVisible();
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

/** Runs one in-page operation for the items a CSS selector of the kit finds (v4 only). */
function query(page, css, op, args) {
  return new KitLocator(page, [{ css, pick: null }]).run(op, args);
}

/** Calls a method of window.qmlTest in the page (v4 only). */
function callQmlTest(page, method, ...args) {
  return page.evaluate(([name, list]) => window.qmlTest[name](...list), [method, args]);
}

module.exports = {
  KitLocator,
  knownBackend,
  detectBackend,
  backendOf,
  isQml,
  qmlSelectors,
  locate,
  query,
  callQmlTest,
};
