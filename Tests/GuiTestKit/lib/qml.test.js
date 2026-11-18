const assert = require('node:assert/strict');
const test = require('node:test');

const { compoundToQml, qmlSelectors } = require('./qml');
const { maskColumns } = require('./dom');

test('a name is quoted, so a state after it and a name that is not ASCII are read right', () => {
  assert.deepEqual(qmlSelectors('[objectName="Page_Users"][visible]'), ['"Page_Users":visible']);
  assert.deepEqual(qmlSelectors('[objectName="ПользовательElementView"]'), ['"ПользовательElementView"']);
});

test('prefix, suffix and substring tests become globs', () => {
  assert.deepEqual(compoundToQml('', '[objectName^="TableRow_"]'), ['"TableRow_*"']);
  assert.deepEqual(compoundToQml('', '[objectName$="Button"]'), ['"*Button"']);
  assert.deepEqual(compoundToQml('', '[objectName*="Row"]'), ['"*Row*"']);
});

test('a type name stands for the unnamed items of the type and those named after it, as on v3', () => {
  assert.deepEqual(compoundToQml('', '[objectName="MouseArea"][visible]'), ['""@MouseArea:visible', '"MouseArea"@MouseArea:visible']);
  assert.deepEqual(compoundToQml('', '[objectName="TextInput"]'), ['""@TextInput', '"TextInput"@TextInput']);
});

test('the tags of the editors become type tests', () => {
  assert.deepEqual(compoundToQml('', 'input'), ['@TextInput']);
  assert.deepEqual(compoundToQml('', 'textarea[visible]'), ['@TextEdit:visible']);
});

test('[visible] alone and an empty compound find any item', () => {
  assert.deepEqual(compoundToQml('', '[visible]'), [':visible']);
  assert.deepEqual(compoundToQml('', ''), ['*']);
});

test('descendants are joined by a space and the variants of a type name multiply out', () => {
  assert.deepEqual(qmlSelectors('[objectName="PopupMenuDialog"] [objectName="MouseArea"][visible]'), [
    '"PopupMenuDialog" ""@MouseArea:visible',
    '"PopupMenuDialog" "MouseArea"@MouseArea:visible',
  ]);
});

test('the alternatives of a comma list are all kept, for their items together', () => {
  assert.deepEqual(qmlSelectors('[objectName="TextInput"] input, input[objectName="TextInput"]'), [
    '""@TextInput @TextInput',
    '"TextInput"@TextInput @TextInput',
    '""@TextInput',
    '"TextInput"@TextInput',
  ]);
});

test('an attribute value with a space stays one compound', () => {
  assert.deepEqual(qmlSelectors('[objectName="Save as"] [visible]'), ['"Save as" :visible']);
});

test('what the kit does not translate fails with the selector in the message', () => {
  assert.throws(() => qmlSelectors('div[objectName="X"]'), /not supported on JQML v4: div/);
  assert.throws(() => qmlSelectors('[text="OK"]'), /not supported on JQML v4/);
  assert.throws(() => qmlSelectors('[objectName]'), /not supported on JQML v4/);
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
