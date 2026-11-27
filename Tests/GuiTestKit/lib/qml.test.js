const assert = require('node:assert/strict');
const test = require('node:test');

const { KitLocator, compoundToQml, playwrightLocator, qmlSelector, qmlSelectors } = require('./qml');
const { columnRects, maskColumns } = require('./dom');

function createQmlPage(items) {
  const children = new Map([[null, []], ...items.map((item) => [item.id, []])]);
  const byId = new Map(items.map((item) => [item.id, item]));
  for (const item of items) {
    children.get(item.parent || null).push(item);
  }
  const descendants = (scope) => {
    const result = [];
    const visit = (parent) => {
      for (const child of children.get(parent) || []) {
        result.push(child);
        visit(child.id);
      }
    };
    visit(scope);
    return result;
  };
  const qmlTest = {
    started: true,
    find(scope, selector) {
      const visible = selector.endsWith(':visible');
      const query = visible ? selector.slice(0, -8) : selector;
      return descendants(scope).filter((item) => {
        if (visible && !item.visible) return false;
        if (query === '*' || query === '') return true;
        return query[0] === '@' && item.type === query.slice(1);
      });
    },
    get(id, name) {
      return byId.get(id)[name];
    },
    children(id) {
      return children.get(id);
    },
  };
  const page = {
    async evaluate(fn, argument) {
      const hadWindow = Object.prototype.hasOwnProperty.call(global, 'window');
      const previousWindow = global.window;
      global.window = { qmlTest };
      try {
        return await fn(argument);
      } finally {
        if (hadWindow) global.window = previousWindow;
        else delete global.window;
      }
    },
  };
  return page;
}

function locatorForName(page, name, operator = '=') {
  const escaped = name.replace(/["\\]/g, '\\$&');
  return new KitLocator(page, [{ css: `[objectName${operator}"${escaped}"]`, pick: null }]);
}

test('objectName matching treats wildcards, punctuation, quotes, and backslashes literally', async () => {
  const values = [
    'Unit?',
    'Unit1',
    'Unit*',
    'Read,write',
    'Group[0]',
    'Rate [kWh]',
    'say "hi"\\there',
    'ButtonPrimary',
    'PrimaryButton',
    'DataRow',
  ];
  const items = values.map((objectName, index) => ({
    id: `item-${index}`,
    objectName,
    type: 'Item',
    visible: true,
  }));
  const page = createQmlPage(items);

  for (const name of values) {
    assert.equal(await locatorForName(page, name).run('count'), 1, name);
  }
  assert.equal(await locatorForName(page, 'Unit?').run('count'), 1);
  assert.equal(await locatorForName(page, 'Unit?', '^=').run('count'), 1);
  assert.equal(await locatorForName(page, 'Button', '^=').run('count'), 1);
  assert.equal(await locatorForName(page, 'Button', '$=').run('count'), 1);
  assert.equal(await locatorForName(page, 'Row', '*=').run('count'), 1);
});

test('selector lists, descendant compounds, and spaced punctuation are parsed outside quotes only', async () => {
  const page = createQmlPage([
    { id: 'root', objectName: '', type: 'Item', visible: true },
    { id: 'comma', objectName: 'Read,write', type: 'Item', visible: true, parent: 'root' },
    { id: 'rate', objectName: 'Rate [kWh]', type: 'Item', visible: true, parent: 'comma' },
    { id: 'group', objectName: 'Group[0]', type: 'Item', visible: true },
    { id: 'other', objectName: 'Other', type: 'Item', visible: true },
  ]);
  const path = new KitLocator(page, [
    { css: '[objectName="Read,write"] [objectName="Rate [kWh]"]', pick: null },
  ]);
  const list = new KitLocator(page, [{ css: '[objectName="Group[0]"], [objectName="Other"]', pick: null }]);

  assert.equal(await path.run('count'), 1);
  assert.equal(await list.run('count'), 2);
  assert.equal(qmlSelectors('[objectName="Save as"] [visible]').length, 1);
});

test('a type-name fallback applies to unnamed items, not explicitly named items', async () => {
  const page = createQmlPage([
    { id: 'root', objectName: '', type: 'Item', visible: true },
    { id: 'named-editor', objectName: 'TextInput', type: 'TextEdit', visible: true, parent: 'root' },
    { id: 'named-input', objectName: 'TextInput', type: 'TextInput', visible: true, parent: 'root' },
    { id: 'unnamed-editor', objectName: '', type: 'TextInput', visible: true, parent: 'root' },
    { id: 'other-named-editor', objectName: 'Other', type: 'TextInput', visible: true, parent: 'root' },
  ]);

  assert.equal(await locatorForName(page, 'TextInput').run('count'), 3);
  const input = new KitLocator(page, [{ css: 'input[objectName="TextInput"]', pick: null }]);
  assert.equal(await input.run('count'), 1);
});

test('the editor tags become qmlTest type searches', () => {
  assert.deepEqual(compoundToQml('', 'input'), [{ selector: '@TextInput', names: [] }]);
  assert.deepEqual(compoundToQml('', 'textarea[visible]'), [{ selector: '@TextEdit:visible', names: [] }]);
  assert.deepEqual(compoundToQml('', ''), [{ selector: '*', names: [] }]);
});

test('unsupported selectors fail with the original selector in the message', () => {
  assert.throws(() => qmlSelectors('div[objectName="X"]'), /not supported on JQML v4: div/);
  assert.throws(() => qmlSelectors('[text="OK"]'), /not supported on JQML v4/);
  assert.throws(() => qmlSelectors('[objectName]'), /not supported on JQML v4/);
});

test('a DOM screenshot can unwrap a locator created before backend detection', () => {
  const native = {};
  const wrapper = new KitLocator({ locator: () => native }, [{ css: '[objectName="Dialog"]', pick: null }]);

  assert.equal(playwrightLocator(wrapper), native);
  assert.equal(playwrightLocator(native), native);
});

test('waitFor passes the DOM fallback only the caller timeout remaining after backend detection', async () => {
  let nativeTimeout;
  const native = { waitFor: async ({ timeout }) => (nativeTimeout = timeout) };
  const page = {
    url: () => 'http://example.test',
    evaluate: async () => {
      await new Promise((resolve) => setTimeout(resolve, 80));
      return 'dom';
    },
    locator: () => native,
  };
  const started = Date.now();

  await new KitLocator(page, [{ css: '[objectName="Dialog"]', pick: null }]).waitFor({ timeout: 300 });

  assert.ok(nativeTimeout > 0 && nativeTimeout < 240);
  assert.ok(Date.now() - started < 300);
});

test('backend detection cannot extend a short waitFor timeout', async () => {
  let nativeWaitCalled = false;
  const page = {
    url: () => 'http://not-ready.test',
    evaluate: async () => null,
    waitForTimeout: async (timeout) => new Promise((resolve) => setTimeout(resolve, timeout)),
    locator: () => ({
      waitFor: async () => {
        nativeWaitCalled = true;
      },
    }),
  };
  const started = Date.now();

  await assert.rejects(
    new KitLocator(page, [{ css: '[objectName="Dialog"]', pick: null }]).waitFor({ timeout: 150 }),
    /Timeout 150ms/
  );

  assert.ok(Date.now() - started < 220);
  assert.equal(nativeWaitCalled, false);
});

test('maskColumns: the span of the rows, clamped to the table, cut to the widest cell, minus another dialog', () => {
  const edges = (left, top, right, bottom) => ({ left, top, right, bottom });
  const layout = {
    viewportHeight: 1000,
    groups: [
      {
        scope: edges(0, 0, 1920, 1000),
        container: edges(0, 100, 1920, 500),
        rows: [edges(0, 80, 1920, 120), edges(0, 480, 1920, 520)],
        cells: { Added: [edges(300, 80, 400, 120), edges(295, 480, 402, 520), edges(500, 0, 500, 10)], Missing: [] },
        occluders: [edges(350, 200, 900, 300)],
      },
    ],
  };
  assert.deepEqual(maskColumns(layout, ['Added', 'Missing']), [
    { x: 295, y: 100, width: 107, height: 100 },
    { x: 295, y: 300, width: 107, height: 200 },
  ]);
});

test('maskColumns: a group whose rows are all outside its table gives nothing', () => {
  const edges = (left, top, right, bottom) => ({ left, top, right, bottom });
  const layout = {
    viewportHeight: 1000,
    groups: [
      {
        scope: edges(0, 0, 1920, 1000),
        container: edges(0, 100, 1920, 200),
        rows: [edges(0, 300, 1920, 340)],
        cells: { Added: [edges(300, 300, 400, 340)] },
        occluders: [],
      },
    ],
  };
  assert.deepEqual(maskColumns(layout, ['Added']), []);
});

test('direct selectors preserve literal names and reject unsupported names', async () => {
  const { readFile } = require('node:fs/promises');
  const { resolve } = require('node:path');
  const source = await readFile(resolve(__dirname, '../../../Tools/JQML/v4/src/testapi.js'), 'utf8');
  const { parseSelector } = await import(`data:text/javascript;base64,${Buffer.from(source).toString('base64')}`);
  const steps = parseSelector(qmlSelector(['Пользователь', 'say "hi"', 'a\\b'], { visible: true }));
  assert.deepEqual(steps.map((step) => step.compound.name), ['Пользователь', 'say "hi"', 'a\\b']);
  assert.equal(steps[2].compound.visible, true);
  assert.throws(() => qmlSelector(['Unit?']), /cannot be represented/);
  assert.throws(() => qmlSelector([`both " and '`]), /cannot be represented/);
  assert.equal(require('./dom').qmlSelector, qmlSelector);
});

test('cell texts select literal names among descendants', async () => {
  const page = createQmlPage([
    { id: 'row', objectName: 'TableRow_1', type: 'Item', visible: true },
    { id: 'wrong', objectName: 'Unit1', type: 'Item', visible: true, parent: 'row', text: 'wrong' },
    { id: 'right', objectName: 'Unit?', type: 'Item', visible: true, parent: 'row', text: 'right' },
    { id: 'quoted', objectName: 'say "hi"\\there', type: 'Item', visible: true, parent: 'row', text: 'quoted' },
  ]);
  const rows = new KitLocator(page, [{ css: '[objectName^="TableRow_"]', pick: null }]);
  assert.deepEqual(await rows.run('cellTexts', { cell: 'Unit?' }), ['right']);
  assert.deepEqual(await rows.run('cellTexts', { cell: 'say "hi"\\there' }), ['quoted']);
});

test('column masks select visible cells by exact objectName', async () => {
  const cells = [{ id: 'wrong', objectName: 'Unit1' }, { id: 'right', objectName: 'Unit?' }, { id: 'quote', objectName: '"x"\\y' }];
  const qmlTest = {
    find(scope, selector) {
      if (scope === null && selector === '"TableRow_*":visible') return [{ id: 'row' }];
      if (scope === null && selector === '"Dialog":visible') return [];
      if (scope === 'row' && selector === '*:visible') return cells;
      throw new Error(`unexpected ${scope} ${selector}`);
    },
    closest() { return null; },
    info(id) { return { box: id === 'row' ? { x: 0, y: 0, width: 100, height: 20 } : { x: id === 'wrong' ? 5 : id === 'right' ? 30 : 60, y: 0, width: 10, height: 20 } }; },
  };
  const page = {
    url: () => 'test:mask',
    async evaluate(fn, arg) {
      const previous = global.window;
      global.window = { qmlTest, innerWidth: 100, innerHeight: 100 };
      try { return await fn(arg); } finally { global.window = previous; }
    },
  };
  assert.deepEqual(await columnRects(page, ['Unit?', '"x"\\y']), [
    { x: 30, y: 0, width: 10, height: 20 },
    { x: 60, y: 0, width: 10, height: 20 },
  ]);
});

test('zero timeout waits through backend detection and until the QML item appears', async () => {
  let detections = 0;
  let states = 0;
  let polls = 0;
  const page = {
    url: () => 'test:unlimited',
    async evaluate(fn, arg) {
      if (!arg) return ++detections >= 3 ? 'qml' : null;
      return ++states >= 3 ? { attached: true, visible: true } : { attached: false, visible: false };
    },
    async waitForTimeout(ms) { assert.ok(ms > 0 && ms <= 100); polls++; },
  };
  await new KitLocator(page, [{ css: '[objectName="item"]', pick: null }]).waitFor({ timeout: 0 });
  assert.equal(polls, 4);
});
