// JQML v4 support.
//
// JQML v3 builds a DOM tree, so the kit finds the items of an application with CSS on `[objectName="..."]`. JQML v4
// draws the application on a canvas and has no such tree; instead every v4 build publishes `window.qmlTest` (see
// ImtCore Docs/JQML/TestApi.md), which finds the items by their objectName and reads them. This module picks the
// backend of a page and gives the rest of the kit a locator that speaks to either.
//
// The kit addresses items with a small CSS subset: `[objectName="X"]`, `[objectName^="X"]`, `[objectName$="X"]`,
// `[objectName*="X"]`, `[visible]`, the tags `input`/`textarea`, the descendant combinator and a list of selectors
// separated by commas. objectName conditions are checked against qmlTest handles so punctuation is treated literally
// rather than as selector syntax or a glob. An item without an objectName has an empty name on v4, where v3 named it
// after its type; the kit therefore also checks unnamed items by type for those legacy names.
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
    await page.waitForTimeout(Math.min(POLL_MS, Math.max(0, deadline - Date.now())));
  }
}

async function isQml(page) {
  return (await backendOf(page)) === 'qml';
}

function unsupported(selector, part) {
  return new Error(`GUI selector is not supported on JQML v4: ${selector} (at ${part})`);
}

function splitSelectorList(selector) {
  if (!selector.trim()) return [''];
  const alternatives = [];
  let start = 0;
  let quote = '';
  let brackets = 0;
  let escaped = false;
  for (let i = 0; i < selector.length; i++) {
    const character = selector[i];
    if (escaped) {
      escaped = false;
      continue;
    }
    if (character === '\\') {
      escaped = true;
      continue;
    }
    if (quote) {
      if (character === quote) quote = '';
      continue;
    }
    if (character === '"' || character === "'") quote = character;
    else if (character === '[') brackets++;
    else if (character === ']') {
      brackets--;
      if (brackets < 0) throw unsupported(selector, character);
    } else if (character === ',' && brackets === 0) {
      alternatives.push(selector.slice(start, i).trim());
      start = i + 1;
    }
  }
  if (quote || brackets !== 0 || escaped) throw unsupported(selector, selector.slice(start));
  alternatives.push(selector.slice(start).trim());
  if (alternatives.some((alternative) => alternative === '')) throw unsupported(selector, selector);

  return alternatives;
}

function splitCompounds(selector, alternative) {
  if (!alternative.trim()) return [''];
  const compounds = [];
  let start = -1;
  let quote = '';
  let brackets = 0;
  let escaped = false;
  for (let i = 0; i < alternative.length; i++) {
    const character = alternative[i];
    if (escaped) {
      escaped = false;
      continue;
    }
    if (start < 0 && !/\s/.test(character)) start = i;
    if (character === '\\') {
      escaped = true;
      continue;
    }
    if (quote) {
      if (character === quote) quote = '';
      continue;
    }
    if (character === '"' || character === "'") quote = character;
    else if (character === '[') brackets++;
    else if (character === ']') {
      brackets--;
      if (brackets < 0) throw unsupported(selector, alternative.slice(start < 0 ? 0 : start, i + 1));
    } else if (/\s/.test(character) && brackets === 0) {
      if (start >= 0) {
        compounds.push(alternative.slice(start, i));
        start = -1;
      }
    }
  }
  if (quote || brackets !== 0 || escaped) throw unsupported(selector, alternative.slice(start < 0 ? 0 : start));
  if (start >= 0) compounds.push(alternative.slice(start));
  if (compounds.length === 0) throw unsupported(selector, alternative);

  return compounds;
}

function parseAttribute(selector, attribute) {
  const match = /^\[\s*([\w-]+)\s*(.*)\]$/.exec(attribute);
  if (!match) throw unsupported(selector, attribute);
  const [, key, remainder] = match;
  if (!remainder.trim()) {
    if (key === 'visible') return { key };
    throw unsupported(selector, attribute);
  }

  const operatorMatch = /^\s*(\^=|\$=|\*=|=)\s*/.exec(remainder);
  if (key !== 'objectName' || !operatorMatch) throw unsupported(selector, attribute);
  const operator = operatorMatch[1];
  const valueStart = operatorMatch[0].length;
  const quote = remainder[valueStart];
  if (quote !== '"' && quote !== "'") throw unsupported(selector, attribute);

  let escaped = false;
  let end = valueStart + 1;
  for (; end < remainder.length; end++) {
    const character = remainder[end];
    if (escaped) {
      escaped = false;
      continue;
    }
    if (character === '\\') {
      escaped = true;
      continue;
    }
    if (character === quote) break;
  }
  if (end === remainder.length || remainder.slice(end + 1).trim() !== '') throw unsupported(selector, attribute);

  return {
    key,
    operator,
    value: remainder.slice(valueStart + 1, end).replace(/\\(.)/gs, '$1'),
  };
}

/**
 * One CSS compound -> qmlTest type/visibility searches plus literal objectName predicates.
 * @returns {{ selector: string, names: Array<{ operator: string, value: string }> }[]}
 */
function compoundToQml(selector, text) {
  const tagMatch = /^([A-Za-z]*)/.exec(text);
  let type = '';
  let rest = text.slice(tagMatch[0].length);
  if (tagMatch[1]) {
    type = TAG_TYPES[tagMatch[1].toLowerCase()];
    if (!type) throw unsupported(selector, text);
  }

  const attributes = [];
  while (rest) {
    if (!rest.startsWith('[')) throw unsupported(selector, text);
    let quote = '';
    let escaped = false;
    let end = 1;
    for (; end < rest.length; end++) {
      const character = rest[end];
      if (escaped) {
        escaped = false;
        continue;
      }
      if (character === '\\') {
        escaped = true;
        continue;
      }
      if (quote) {
        if (character === quote) quote = '';
      } else if (character === '"' || character === "'") quote = character;
      else if (character === ']') break;
    }
    if (end === rest.length || quote) throw unsupported(selector, rest);
    attributes.push(parseAttribute(selector, rest.slice(0, end + 1)));
    rest = rest.slice(end + 1);
  }

  let visible = false;
  const names = [];
  for (const attribute of attributes) {
    if (attribute.key === 'visible') {
      visible = true;
      continue;
    }
    names.push({ operator: attribute.operator, value: attribute.value });
  }
  const state = visible ? ':visible' : '';
  const variant = (searchType, nameFilters) => ({ selector: `${searchType || '*'}${state}`, names: nameFilters });
  if (names.length === 0) return [variant(type, names)];

  const variants = [variant(type, names)];
  const typeName = names.find(({ operator }) => operator === '=');
  if (!type && names.length === 1 && typeName && TYPE_NAMES.has(typeName.value)) {
    variants.push(variant(`@${typeName.value}`, [{ operator: '=', value: '' }]));
  }

  return variants;
}

/**
 * The parsed selector list. Each alternative is a descendant path of compounds; each compound has
 * qmlTest searches and objectName predicates applied to the returned handles.
 * @param {string} css
 */
function qmlSelectors(css) {
  return splitSelectorList(css).map((alternative) =>
    splitCompounds(css, alternative).map((compound) => compoundToQml(css, compound))
  );
}

/** Build a direct qmlTest selector for a path of literal objectNames. */
function qmlSelector(path, options = {}) {
  if (!Array.isArray(path) || path.length === 0 || path.some((name) => typeof name !== 'string')) {
    throw new Error('qmlSelector: path must be a non-empty array of strings');
  }
  const compounds = path.map((name) => {
    if (/[*?]/.test(name) || (name.includes('"') && name.includes("'"))) {
      throw new Error('qmlSelector: this objectName cannot be represented literally by qmlTest');
    }
    const quote = name.includes('"') ? "'" : '"';
    return `${quote}${name}${quote}`;
  });
  if (options.visible) compounds[compounds.length - 1] += ':visible';
  return compounds.join(' ');
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
  const matchesNames = (handle, names) =>
    names.every(({ operator, value }) => {
      const name = handle.objectName || '';
      if (operator === '=') return name === value;
      if (operator === '^=') return name.startsWith(value);
      if (operator === '$=') return name.endsWith(value);
      return name.includes(value);
    });
  const findAll = (scope, searches) => {
    const found = new Map();
    for (const search of searches) {
      for (const handle of t.find(scope, search.selector)) {
        if (matchesNames(handle, search.names)) found.set(handle.id, handle);
      }
    }
    if (found.size < 2) return [...found.values()];
    return t.find(scope, '*').filter((handle) => found.has(handle.id));
  };
  const findSelector = (scope, compounds) => {
    let found = [];
    let scopes = [scope];
    for (const compound of compounds) {
      found = [];
      const seen = new Set();
      for (const parent of scopes) {
        for (const handle of findAll(parent, compound)) {
          if (!seen.has(handle.id)) {
            seen.add(handle.id);
            found.push(handle);
          }
        }
      }
      scopes = found.map((handle) => handle.id);
      if (scopes.length === 0) break;
    }
    return found;
  };
  const findSelectors = (scope, alternatives) => {
    const found = new Map();
    for (const compounds of alternatives) {
      for (const handle of findSelector(scope, compounds)) found.set(handle.id, handle);
    }
    if (found.size < 2) return [...found.values()];
    return t.find(scope, '*').filter((handle) => found.has(handle.id));
  };

  let list = [];
  let scopes = [null];
  for (const step of steps) {
    list = [];
    const seen = new Set();
    for (const scope of scopes) {
      for (const handle of findSelectors(scope, step.sel)) {
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
        .map((row) => t.find(row.id, '*').find((handle) => handle.objectName === args.cell))
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
      sel:
        typeof step.css === 'string'
          ? qmlSelectors(step.css)
          : [[...step.css.qml.map((selector) => ({ selector, names: [] }))]],
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
    const timeout = options.timeout === undefined ? DEFAULT_WAIT_TIMEOUT : options.timeout;
    const deadline = timeout === 0 ? null : Date.now() + timeout;
    const backend = await backendOf(this.page, timeout === 0 ? DEFAULT_WAIT_TIMEOUT : timeout);
    if (backend !== 'qml') {
      if (deadline === null) return this.playwright().waitFor(options);
      const remaining = Math.max(0, deadline - Date.now());
      if (remaining === 0) throw timeoutError(`locator.waitFor ${this}`, timeout);
      return this.playwright().waitFor({ ...options, timeout: remaining });
    }
    const state = options.state || 'visible';
    for (;;) {
      const now = await this.runOrNothing({ attached: false, visible: false }, 'state');
      const reached =
        state === 'attached' ? now.attached : state === 'detached' ? !now.attached : state === 'hidden' ? !now.visible : now.visible;
      if (reached) return;
      if (deadline !== null && Date.now() >= deadline) throw timeoutError(`locator.waitFor (${state}) ${this}`, timeout);
      await this.page.waitForTimeout(deadline === null ? POLL_MS : Math.min(POLL_MS, Math.max(0, deadline - Date.now())));
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
  qmlSelector,
  playwrightLocator: (locator) => (locator instanceof KitLocator ? locator.playwright() : locator),
  locate,
  callQmlTest,
};
