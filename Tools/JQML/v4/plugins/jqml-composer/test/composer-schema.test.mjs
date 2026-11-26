import assert from 'node:assert/strict';
import { test } from 'node:test';
import { addExport, addNode, basesOf, deleteNode, describeExports, emptySchema, exportsOf, moveNode, normalizeSchema, parseSchema, removeExport, renameExport, setExtends, setId, setProperty, stringifySchema, viewOf } from '../src/composer/schema.mjs';
import { generate, validateSchema } from '../src/composer/generate.mjs';

// a "file system" of schemas: key -> schema
function world(files) {
	const store = new Map(Object.entries(files).map(([key, schema]) => [key, normalizeSchema(schema)]));
	const resolve = (ref, from) => {
		const key = ref.replace(/^\.\//, '');
		return store.has(key) ? { key, schema: store.get(key) } : null;
	};
	return { store, resolve, ctx: (key) => ({ key, resolve }) };
}

const node = (uid, type, id, properties = {}, children = []) => ({ uid, type, source: '', id, properties, children });
const instance = (uid, source, id, properties = {}) => ({ uid, type: '', source, id, properties, children: [] });

// Card: a Rectangle with a Text; the text and the color are exported
const CARD = {
	root: node('r', 'Rectangle', 'card', { width: '200', height: '80', color: '"white"' }, [node('t', 'Text', 'label', { text: '"Title"', 'font.pixelSize': '16' })]),
	exports: [{ name: 'title', uid: 't', property: 'text', type: 'string' }, { name: 'fill', uid: 'r', property: 'color', type: 'color' }],
};

const WORLD = () => world({
	'Card.jqschema': CARD,
	'BigCard.jqschema': { extends: 'Card.jqschema', root: node('br', '', 'big', { title: '"Big"', width: '320' }, [node('x', 'Rectangle', 'badge', { width: '10', height: '10' })]), exports: [{ name: 'badgeColor', uid: 'x', property: 'color' }] },
	'Page.jqschema': { root: node('pr', 'Item', 'page', { width: '600', height: '400' }, [instance('i1', 'Card.jqschema', 'first', { title: '"One"', x: '10' }), instance('i2', 'BigCard.jqschema', 'second', { title: '"Two"', badgeColor: '"red"', y: '100' })]), exports: [] },
});

test('a new schema is an Item with a size', () => {
	const schema = emptySchema();
	assert.equal(schema.root.type, 'Item');
	assert.equal(schema.root.properties.width, '640');
	assert.deepEqual(schema.exports, []);
});

test('a schema is read and written as JSON, a broken text gives an empty schema and the error', () => {
	const text = stringifySchema(normalizeSchema(CARD));
	assert.deepEqual(parseSchema(text).schema.exports, CARD.exports);
	assert.equal(parseSchema('{ nope').error !== '', true);
	assert.equal(parseSchema('').error, '');
});

test('the properties are set, changed and removed', () => {
	const w = world({ 'Card.jqschema': CARD });
	const schema = w.store.get('Card.jqschema');
	const changed = setProperty(schema, w.ctx('Card.jqschema'), 'r', 'color', '"red"');
	assert.equal(changed.root.properties.color, '"red"');
	assert.equal(schema.root.properties.color, '"white"');
	assert.equal('color' in setProperty(changed, w.ctx('Card.jqschema'), 'r', 'color', '').root.properties, false);
});

test('ids are checked and an object gets a unique one', () => {
	const w = world({ 'Card.jqschema': CARD });
	const schema = w.store.get('Card.jqschema');
	assert.throws(() => setId(schema, 't', 'card'), /used by another/);
	assert.throws(() => setId(schema, 't', '1x'), /valid id/);
	const added = addNode(schema, w.ctx('Card.jqschema'), 'r', { type: 'Text' });
	const again = addNode(added.schema, w.ctx('Card.jqschema'), 'r', { type: 'Text' });
	assert.deepEqual(again.schema.root.children.map((child) => child.id), ['label', 'text1', 'text2']);
});

test('objects are added at an index, moved and deleted, and the exports of a deleted object go', () => {
	const w = world({ 'Card.jqschema': CARD });
	const ctx = w.ctx('Card.jqschema');
	let schema = w.store.get('Card.jqschema');
	const first = addNode(schema, ctx, 'r', { type: 'Rectangle' }, { x: '5' }, 0);
	assert.deepEqual(first.schema.root.children.map((child) => child.type), ['Rectangle', 'Text']);
	schema = moveNode(first.schema, 't', first.uid);
	assert.deepEqual(schema.root.children.map((child) => child.id), ['rectangle1']);
	assert.deepEqual(schema.root.children[0].children.map((child) => child.id), ['label']);
	assert.throws(() => moveNode(schema, first.uid, 't'), /itself/);
	const gone = deleteNode(schema, 't');
	assert.deepEqual(gone.exports.map((item) => item.name), ['fill']);
	assert.throws(() => deleteNode(schema, 'r'), /root/);
});

test('exports: the name, the uniqueness, rename and remove', () => {
	const w = world({ 'Card.jqschema': CARD });
	const ctx = w.ctx('Card.jqschema');
	let schema = w.store.get('Card.jqschema');
	schema = addExport(schema, ctx, 't', 'font.pixelSize');
	assert.equal(schema.exports.at(-1).name, 'font_pixelSize');
	schema = addExport(schema, ctx, 'r', 'width');
	schema = addExport(schema, ctx, 't', 'width');
	assert.equal(schema.exports.at(-1).name, 'label_width');
	assert.throws(() => addExport(schema, ctx, 'r', 'width'), /already exported/);
	assert.throws(() => addExport(schema, ctx, 'r', 'height', 'title'), /already exported/);
	schema = renameExport(schema, ctx, 'fill', 'background');
	assert.ok(schema.exports.some((item) => item.name === 'background'));
	assert.equal(removeExport(schema, 'background').exports.some((item) => item.name === 'background'), false);
});

test('the exports of a schema include the ones of its bases', () => {
	const w = WORLD();
	const big = { key: 'BigCard.jqschema', schema: w.store.get('BigCard.jqschema') };
	assert.deepEqual(exportsOf(big, w.resolve).map((item) => [item.name, item.inherited]), [['title', true], ['fill', true], ['badgeColor', false]]);
	assert.deepEqual(basesOf(big, w.resolve).map((base) => base.key), ['Card.jqschema']);
	assert.deepEqual(describeExports('BigCard.jqschema', big.schema, w.resolve).map((item) => item.name), ['title', 'fill', 'badgeColor']);
});

test('an inheriting schema can set only the exported properties of its base', () => {
	const w = WORLD();
	const ctx = w.ctx('BigCard.jqschema');
	const big = w.store.get('BigCard.jqschema');
	assert.equal(setProperty(big, ctx, 'br', 'fill', '"blue"').root.properties.fill, '"blue"');
	assert.equal(setProperty(big, ctx, 'br', 'x', '4').root.properties.x, '4');
	assert.throws(() => setProperty(big, ctx, 'br', 'color', '"blue"'), /not exported/);
	assert.throws(() => setProperty(big, ctx, 'br', 'radius', '3'), /not exported/);
	assert.equal(setProperty(big, ctx, 'x', 'color', '"blue"').root.children[0].properties.color, '"blue"');
	assert.throws(() => addExport(big, ctx, 'br', 'width'), /no properties of its own/);
});

test('an instance of a schema is closed: its exports and its placement can be set, nothing else', () => {
	const w = WORLD();
	const ctx = w.ctx('Page.jqschema');
	const page = w.store.get('Page.jqschema');
	assert.equal(setProperty(page, ctx, 'i1', 'fill', '"gray"').root.children[0].properties.fill, '"gray"');
	assert.equal(setProperty(page, ctx, 'i1', 'anchors.margins', '4').root.children[0].properties['anchors.margins'], '4');
	assert.throws(() => setProperty(page, ctx, 'i1', 'radius', '4'), /not exported/);
	assert.throws(() => addNode(page, ctx, 'i1', { type: 'Item' }), /closed/);
	assert.throws(() => moveNode(page, 'x', 'i1'), /No such/);
});

test('a schema cannot use or extend itself, and the cycles are found', () => {
	const w = WORLD();
	const ctx = w.ctx('Card.jqschema');
	assert.throws(() => addNode(w.store.get('Card.jqschema'), ctx, 'r', { source: 'Card.jqschema' }), /itself/);
	assert.throws(() => setExtends(w.store.get('Card.jqschema'), ctx, 'BigCard.jqschema'), /cycle/);
	assert.throws(() => setExtends(w.store.get('Card.jqschema'), ctx, 'Card.jqschema'), /itself/);
	assert.throws(() => setExtends(w.store.get('Card.jqschema'), ctx, 'Nope.jqschema'), /not found/);
});

test('extending a schema drops the properties of the root that the base does not export', () => {
	const w = WORLD();
	const ctx = w.ctx('Page.jqschema');
	const result = setExtends(w.store.get('Page.jqschema'), ctx, 'Card.jqschema');
	// width and height are placement properties: they stay
	assert.deepEqual(result.dropped, []);
	assert.equal(result.schema.root.type, '');
	const odd = setProperty(w.store.get('Page.jqschema'), ctx, 'pr', 'radius', '3');
	assert.deepEqual(setExtends(odd, ctx, 'Card.jqschema').dropped, ['radius']);
	assert.equal(setExtends(result.schema, ctx, '').schema.root.type, 'Item');
});

test('the view has the objects of the bases and the values set on exports', () => {
	const w = WORLD();
	const big = viewOf('BigCard.jqschema', w.store.get('BigCard.jqschema'), w.resolve);
	assert.equal(big.type, 'Rectangle');
	assert.equal(big.derived, 'Card');
	assert.deepEqual(big.children.map((child) => [child.id, child.editable]), [['label', false], ['badge', true]]);
	assert.equal(big.props.find((prop) => prop.name === 'width').value, '320');
	assert.equal(big.children[0].props.find((prop) => prop.name === 'text').value, '"Big"');
	assert.equal(big.editable, true);
});

test('the view of an instance applies its values inside', () => {
	const w = WORLD();
	const page = viewOf('Page.jqschema', w.store.get('Page.jqschema'), w.resolve);
	const [first, second] = page.children;
	assert.equal(first.instanceOf, 'Card');
	assert.equal(first.editable, true);
	assert.equal(first.children[0].editable, false);
	assert.equal(first.children[0].props.find((prop) => prop.name === 'text').value, '"One"');
	assert.equal(first.props.find((prop) => prop.name === 'x').value, '10');
	assert.equal(second.children.find((child) => child.id === 'badge').props.find((prop) => prop.name === 'color').value, '"red"');
	assert.equal(second.children.find((child) => child.id === 'label').props.find((prop) => prop.name === 'text').value, '"Two"');
	assert.equal(first.uid, 'i1');
});

test('QML is generated: a type per schema, aliases for the exports, the base as the type of the root', () => {
	const w = WORLD();
	const result = generate({ key: 'Page.jqschema', schema: w.store.get('Page.jqschema') }, w.resolve);
	assert.deepEqual([...result.files.keys()].sort(), ['BigCard.qml', 'Card.qml', 'Page.qml']);
	assert.equal(result.entryName, 'Page.qml');
	const card = result.files.get('Card.qml');
	assert.ok(card.includes('import QtQuick 2.12'));
	assert.ok(card.includes('Rectangle {\n    id: card\n    property alias title: label.text\n    property alias fill: card.color\n'));
	const big = result.files.get('BigCard.qml');
	assert.ok(big.includes('Card {\n    id: big\n    property alias badgeColor: badge.color\n    title: "Big"\n    width: 320\n'));
	const page = result.files.get('Page.qml');
	assert.ok(page.includes('    Card {\n        id: first\n        title: "One"\n        x: 10\n    }'));
	assert.ok(page.includes('    BigCard {\n        id: second\n        title: "Two"\n        badgeColor: "red"\n        y: 100\n    }'));
});

test('handlers and multi-line values are written as QML', () => {
	const w = world({ 'A.jqschema': { root: node('r', 'MouseArea', 'area', { onClicked: 'console.log("x")\nfoo()', onEntered: '{ bar() }', width: '10' }) } });
	const qml = generate({ key: 'A.jqschema', schema: w.store.get('A.jqschema') }, w.resolve).files.get('A.qml');
	assert.ok(qml.includes('    width: 10\n    onClicked: {\n        console.log("x")\n        foo()\n    }\n    onEntered: { bar() }'));
});

test('generation reports every problem', () => {
	const bad = world({
		'Card.jqschema': CARD,
		'Bad.jqschema': { extends: 'Card.jqschema', root: node('r', '', 'b', { color: '"red"' }), exports: [{ name: 'gone', uid: 'nope', property: 'x' }] },
		'Missing.jqschema': { root: node('r', 'Item', 'm', {}, [instance('q', 'Nothing.jqschema', 'q')]) },
	});
	assert.equal(validateSchema({ key: 'Bad.jqschema', schema: bad.store.get('Bad.jqschema') }, bad.resolve).length, 2);
	assert.throws(() => generate({ key: 'Bad.jqschema', schema: bad.store.get('Bad.jqschema') }, bad.resolve), /color is not exported[\s\S]*gone/);
	assert.throws(() => generate({ key: 'Missing.jqschema', schema: bad.store.get('Missing.jqschema') }, bad.resolve), /Nothing.jqschema is not found/);
});
