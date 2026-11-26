import assert from 'node:assert/strict';
import path from 'node:path';
import { test } from 'node:test';
import { fileURLToPath } from 'node:url';
import { LanguageService } from '../src/service.mjs';

const here = path.dirname(fileURLToPath(import.meta.url));
const fixtures = path.join(here, 'fixtures');
const mainFile = path.join(fixtures, 'app', 'main.qml');

function createService() {
	const service = new LanguageService({ engineIndexPath: path.join(here, '..', 'lib', 'engine-index.json') });
	service.indexQmldir(path.join(fixtures, 'mymod', 'qmldir'));
	return service;
}

// `text` with `|` marking the cursor: returns the text without it and the offset.
function cursor(text) {
	const offset = text.indexOf('|');
	return { text: text.slice(0, offset) + text.slice(offset + 1), offset };
}

const labels = (items) => items.map((item) => item.label);
const normal = (file) => file.replace(/\\/g, '/');

test('a valid file has no diagnostics', () => {
	const service = createService();
	assert.deepEqual(service.getDiagnostics(mainFile), []);
});

test('a syntax error is reported with its position', () => {
	const service = createService();
	const diagnostics = service.getDiagnostics(mainFile, 'import QtQuick 2.12\n\nItem {\n    width: 1 +\n}\n');
	assert.equal(diagnostics.length, 1);
	assert.equal(diagnostics[0].severity, 'error');
	assert.equal(diagnostics[0].range.start.line, 3);
});

test('an unclosed object is a syntax error', () => {
	const service = createService();
	const diagnostics = service.getDiagnostics(mainFile, 'Item {\n    width: 1\n');
	assert.equal(diagnostics[0].severity, 'error');
});

test('an unknown type and a duplicate id are reported', () => {
	const service = createService();
	const text = 'import mymod 1.0\nItem {\n    id: a\n    Nonexistent {}\n    Item { id: a }\n}\n';
	const messages = service.getDiagnostics(mainFile, text).map((item) => item.message);
	assert.ok(messages.some((message) => message.includes("Unknown type 'Nonexistent'")));
	assert.ok(messages.some((message) => message.includes("Duplicate id 'a'")));
});

test('an id may repeat in separate delegates and components', () => {
	const service = createService();
	const text = 'Item {\n    id: a\n    ListView { delegate: Item { id: a } }\n    Component { Item { id: a } }\n}\n';
	assert.deepEqual(service.getDiagnostics(mainFile, text).filter((item) => item.message.includes('Duplicate')), []);
});

test('a mistyped property is a warning with a suggestion, an unrelated one is only information', () => {
	const service = createService();
	const text = 'Rectangle {\n    colr: "red"\n    unrelatedThing: 1\n    Layout.fillWidth: true\n    anchors.fill: parent\n}\n';
	const diagnostics = service.getDiagnostics(mainFile, text);
	const typo = diagnostics.find((item) => item.message.includes("'colr'"));
	assert.equal(typo.severity, 'warning');
	assert.ok(typo.message.includes("Did you mean 'color'"));
	assert.equal(diagnostics.find((item) => item.message.includes('unrelatedThing')).severity, 'information');
	assert.equal(diagnostics.length, 2);
});

test('members of types from qmldir modules are known, handlers included', () => {
	const service = createService();
	const text = 'import mymod 1.0\nCard {\n    title: "a"\n    level: 2\n    accent: "blue"\n    onActivated: refresh(false)\n    onLevelChanged: {}\n}\n';
	assert.deepEqual(service.getDiagnostics(mainFile, text), []);
});

test('completion of members inside an object', () => {
	const service = createService();
	const { text, offset } = cursor('import mymod 1.0\nCard {\n    |\n}\n');
	const items = labels(service.getCompletions(mainFile, text, offset));
	for (const name of ['title', 'level', 'accent', 'width', 'onActivated', 'onWidthChanged', 'Rectangle', 'Card', 'property']) assert.ok(items.includes(name), name);
});

test('completion after a colon offers values, not members', () => {
	const service = createService();
	const { text, offset } = cursor('Item {\n    id: a\n    width: |\n}\n');
	const items = labels(service.getCompletions(mainFile, text, offset));
	assert.ok(items.includes('a'));
	assert.ok(items.includes('parent'));
	assert.ok(!items.includes('Rectangle'));
});

test('completion after id. gives the members of that type', () => {
	const service = createService();
	const { text, offset } = cursor('import mymod 1.0\nItem {\n    Card { id: card }\n    Component.onCompleted: card.|\n}\n');
	const items = labels(service.getCompletions(mainFile, text, offset));
	assert.ok(items.includes('refresh'));
	assert.ok(items.includes('title'));
	assert.ok(items.includes('width'));
});

test('completion after a singleton name and after anchors.', () => {
	const service = createService();
	let probe = cursor('import mymod 1.0\nItem {\n    width: Theme.|\n}\n');
	const theme = labels(service.getCompletions(mainFile, probe.text, probe.offset));
	assert.ok(theme.includes('background') && theme.includes('radius'));
	probe = cursor('Item {\n    onWidthChanged: { anchors.| }\n}\n');
	assert.ok(labels(service.getCompletions(mainFile, probe.text, probe.offset)).includes('fill'));
});

test('completion works while the file does not parse', () => {
	const service = createService();
	const { text, offset } = cursor('import mymod 1.0\nCard {\n    id: card\n    width: card.|\n    height: "unterminated\n}\n');
	assert.ok(labels(service.getCompletions(mainFile, text, offset)).includes('refresh'));
});

test('nothing is offered in a comment or a string', () => {
	const service = createService();
	let probe = cursor('Item {\n    // wid|\n}\n');
	assert.deepEqual(service.getCompletions(mainFile, probe.text, probe.offset), []);
	probe = cursor('Item {\n    objectName: "wi|"\n}\n');
	assert.deepEqual(service.getCompletions(mainFile, probe.text, probe.offset), []);
});

test('import completion lists the known modules', () => {
	const service = createService();
	const { text, offset } = cursor('import |\n\nItem {}\n');
	assert.ok(labels(service.getCompletions(mainFile, text, offset)).includes('mymod'));
});

test('definition of a type opens its file', () => {
	const service = createService();
	const text = 'import mymod 1.0\nCard {}\n';
	const locations = service.getDefinition(mainFile, text, text.indexOf('Card') + 1);
	assert.equal(path.basename(locations[0].filePath), 'Card.qml');
});

test('definition of an inherited property goes to the declaring file', () => {
	const service = createService();
	const text = 'import mymod 1.0\nCard {\n    title: "a"\n}\n';
	const locations = service.getDefinition(mainFile, text, text.indexOf('title') + 1);
	assert.equal(path.basename(locations[0].filePath), 'Base.qml');
	assert.equal(locations[0].range.start.line, 5);
});

test('definition of an id, a member through an id and a handler', () => {
	const service = createService();
	const text = 'import mymod 1.0\nItem {\n    Card { id: card; onActivated: card.refresh(1) }\n    width: card.level\n}\n';
	let locations = service.getDefinition(mainFile, text, text.lastIndexOf('card') + 1);
	assert.equal(normal(locations[0].filePath), normal(mainFile));
	locations = service.getDefinition(mainFile, text, text.indexOf('refresh') + 1);
	assert.equal(path.basename(locations[0].filePath), 'Base.qml');
	locations = service.getDefinition(mainFile, text, text.indexOf('onActivated') + 1);
	assert.equal(path.basename(locations[0].filePath), 'Base.qml');
	assert.equal(locations[0].range.start.line, 7);
});

test('definition of a singleton and of a member of a singleton', () => {
	const service = createService();
	const text = 'import mymod 1.0\nItem { width: Theme.radius }\n';
	assert.equal(path.basename(service.getDefinition(mainFile, text, text.indexOf('Theme') + 1)[0].filePath), 'Theme.qml');
	assert.equal(service.getDefinition(mainFile, text, text.indexOf('radius') + 1)[0].range.start.line, 5);
});

test('definition of an import opens the qmldir', () => {
	const service = createService();
	const text = 'import mymod 1.0\nItem {}\n';
	assert.equal(path.basename(service.getDefinition(mainFile, text, text.indexOf('mymod') + 1)[0].filePath), 'qmldir');
});

test('hover shows the declaration', () => {
	const service = createService();
	const text = 'import mymod 1.0\nCard {\n    level: 1\n}\n';
	const hover = service.getHover(mainFile, text, text.indexOf('level') + 1);
	assert.ok(hover.detail.includes('property int level'));
	const builtin = service.getHover(mainFile, 'Rectangle { width: 1 }', 14);
	assert.ok(builtin.detail.includes('width'));
});

test('references of an id', () => {
	const service = createService();
	const text = 'Item {\n    id: a\n    width: a.height\n    height: a.width\n}\n';
	assert.equal(service.getReferences(mainFile, text, text.indexOf('a.height')).length, 3);
});

test('the outline lists objects and declarations', () => {
	const service = createService();
	const symbols = service.getSymbols(mainFile, 'Item {\n    id: root\n    property int a\n    signal s()\n    Rectangle { id: r }\n}\n');
	assert.equal(symbols[0].name, 'Item #root');
	assert.deepEqual(symbols[0].children.map((child) => child.name), ['a', 's', 'Rectangle #r']);
});
