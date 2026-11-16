// The API for the tests of applications (src/testapi.js): the selectors, the search from any found item, the information about an item,
// actions through the scene (click, text, keys, scrolling), what stands in the way of a click, and waiting for the application.
// `node test/testapi.mjs`
import { Engine } from '../src/engine.js';
import { Flickable, ListView } from '../src/items.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { createTestApi, keyEventFor, parseSelector } from '../src/testapi.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick

Item {
	id: root
	width: 400
	height: 300
	property string clicked: ""

	Rectangle {
		objectName: "panel"
		x: 0; y: 0; width: 200; height: 150
		Text { objectName: "title"; text: "Left"; width: 50; height: 20 }
		MouseArea {
			objectName: "ok"
			y: 30; width: 80; height: 30
			onClicked: root.clicked = root.clicked + "left-ok;"
		}
	}
	Rectangle {
		objectName: "panel"
		x: 200; y: 0; width: 200; height: 150
		Text { objectName: "title"; text: "Right"; width: 50; height: 20 }
		MouseArea {
			objectName: "ok"
			y: 30; width: 80; height: 30
			onClicked: root.clicked = root.clicked + "right-ok;"
		}
		Item {
			objectName: "inner"
			Text { objectName: "title"; text: "Deep" }
		}
	}
	MouseArea {
		objectName: "disabledArea"
		y: 160; width: 50; height: 30
		enabled: false
	}
	Item {
		objectName: "hidden"
		visible: false
		width: 50; height: 50
	}
	MouseArea {
		objectName: "covered"
		x: 100; y: 200; width: 60; height: 40
		onClicked: root.clicked = root.clicked + "covered;"
	}
	MouseArea { objectName: "cover"; x: 100; y: 200; width: 60; height: 40 }
	TextInput { objectName: "edit"; x: 200; y: 160; width: 150; height: 24; text: "abc" }
	Flickable {
		objectName: "scroller"
		x: 0; y: 220; width: 90; height: 60
		contentWidth: 90; contentHeight: 600
		clip: true
		Item {
			objectName: "far"
			y: 400; width: 90; height: 30
			MouseArea { objectName: "farButton"; anchors.fill: parent; onClicked: root.clicked = root.clicked + "far;" }
		}
	}
	Loader {
		id: loader
		objectName: "loader"
		active: false
		sourceComponent: Component { Text { objectName: "late"; text: "late" } }
	}
}
`,
};

class MemoryFS {
	async readText(path) {
		return path in FILES ? FILES[path] : null;
	}

	async readBytes() {
		return null;
	}
}

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}
async function rejects(name, promiseOrFn, part) {
	let message = '';
	try {
		await (typeof promiseOrFn === 'function' ? promiseOrFn() : promiseOrFn);
	} catch (error) {
		message = String(error.message);
	}
	const ok = message.includes(part);
	if (!ok) failures++;
	console.log(`${ok ? '[+]' : '[-]'} ${name}${ok ? '' : `: got '${message}', expected a message with '${part}'`}`);
}

const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(400, 400, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
let t = 0;
const tick = () => {
	scene.root.$updateRequested = true;
	scene.drawFrame((t += 0.016));
};
for (let i = 0; i < 4; i++) tick();
const api = createTestApi({ engine, scene, nextFrame: async () => tick(), classes: { Flickable, ListView } });
const root = viewer.content.$children[0];
const names = (list) => list.map((h) => h.objectName);

// ----- selectors -----
check('a name', parseSelector('ok').length, 1);
check('descendants and children', parseSelector('a > b c').map((s) => s.combinator), ['descendant', 'child', 'descendant']);
check('a leading >', parseSelector('> ok')[0].combinator, 'child');
for (const bad of ['', '>', 'a >', '[x', '@', 'a >> b', ':nothing']) {
	let message = '';
	try {
		parseSelector(bad);
	} catch (error) {
		message = error.message;
	}
	check(`bad selector '${bad}'`, message.startsWith('Invalid selector'), true);
}

// ----- search -----
check('all titles of the page', api.find(null, 'title').length, 3);
check('the same by #', api.find(null, '#title').length, 3);
const panels = api.find(null, 'panel');
check('two panels with one name', panels.length, 2);
check('the order of the tree', api.find(null, 'title').map((h) => api.get(h.id, 'text')), ['Left', 'Right', 'Deep']);
check('from the left panel only its title', api.find(panels[0].id, 'title').map((h) => api.get(h.id, 'text')), ['Left']);
check('from the right panel: the title and the deep one', api.find(panels[1].id, 'title').map((h) => api.get(h.id, 'text')), ['Right', 'Deep']);
check('a child of the panel only', api.find(panels[1].id, '> title').map((h) => api.get(h.id, 'text')), ['Right']);
check('a chain of names', api.find(null, 'panel inner title').map((h) => api.get(h.id, 'text')), ['Deep']);
check('a chain with a direct child', api.find(null, 'panel > inner > title').length, 1);
check('a chain with a skipped level is not a child', api.find(null, 'panel > title').length, 2);
check('a search from an item does not see what is above it', api.find(panels[0].id, 'panel').length, 0);
check('and not the scope itself', api.find(panels[1].id, 'inner panel').length, 0);
check('a wildcard', names(api.find(null, 'far*')), ['far', 'farButton']);
check('a type (the page of the viewer has two more)', api.find(null, '@Rectangle').length, 4);
check('a type with a name', api.find(null, '@MouseArea#cover').length, 1);
check('a text property', api.find(null, '[text="Right"]').length, 1);
check('contains', api.find(null, '@Text[text*="e"]').length, 4);
check('a number', api.find(null, '@Rectangle[width>=200]').length, 4);
check('visible', names(api.find(null, 'hidden:visible')), []);
check('not visible is found without :visible', names(api.find(null, 'hidden')), ['hidden']);
check('disabled', names(api.find(null, '@MouseArea:disabled')), ['disabledArea']);
check('a quoted name', api.find(null, '"title"').length, 3);
check('count', api.count(null, 'ok'), 2);
check('limit', api.find(null, 'ok', { limit: 1 }).length, 1);
check('parent of a title', api.parent(api.find(null, 'title')[0].id).objectName, 'panel');
check('closest', api.closest(api.find(null, 'inner title')[0].id, 'panel').id, panels[1].id);
check('a name that nothing has', api.find(null, 'nothing'), []);
check('a structured selector', api.find(null, { objectName: 'ok', props: { enabled: true } }).length, 2);
check('an item that is not built yet is not found', api.find(null, 'late'), []);
root.$children.find((c) => c.$typeName === 'Loader').active = true;
tick();
tick();
check('and is found once it is', api.find(null, 'late').length, 1);

// ----- information -----
const rightOk = api.find(panels[1].id, 'ok')[0];
const info = api.info(rightOk.id);
check('the box of the right ok', [info.box.x, info.box.y - api.info(panels[1].id).box.y, info.box.width, info.box.height], [200, 30, 80, 30]);
check('visible and enabled', [info.visible, info.enabled], [true, true]);
check('the hidden one is not visible', api.info(api.find(null, 'hidden')[0].id).visible, false);
check('the disabled one', api.info(api.find(null, 'disabledArea')[0].id).enabled, false);
check('get and set', (api.set(api.find(null, 'edit')[0].id, 'text', 'xyz'), api.get(api.find(null, 'edit')[0].id, 'text')), 'xyz');
check('the tree of a panel', api.tree(panels[1].id, { depth: 1 }).children.map((c) => c.objectName), ['title', 'ok', 'inner']);
check('the dump has the names', api.dump(panels[1].id).includes('#inner'), true);

// ----- actionability -----
check('a free item can be clicked', api.actionability(rightOk.id).ok, true);
check('a hidden item cannot', api.actionability(api.find(null, 'hidden')[0].id).reasons, ['not visible']);
check('a disabled item cannot', api.actionability(api.find(null, 'disabledArea')[0].id).reasons.includes('is disabled'), true);
const covered = api.find(null, 'covered')[0];
check('a covered item says by what', api.actionability(covered.id).reasons, ["is covered by MouseArea 'cover'"]);

// ----- actions -----
await api.click(panels[0].id === '' ? null : api.find(panels[0].id, 'ok')[0].id);
check('a click in the left panel', root.clicked, 'left-ok;');
await api.click(rightOk.id);
check('a click in the right panel', root.clicked, 'left-ok;right-ok;');
root.clicked = '';
await rejects('a click on a covered item fails', () => api.click(covered.id), 'NOT_ACTIONABLE');
await api.click(covered.id, { force: true });
check('a forced click goes to what is on top', root.clicked, '');
await rejects('a click on a hidden item fails', () => api.click(api.find(null, 'hidden')[0].id), 'not visible');
const far = api.find(null, 'farButton')[0];
check('an item of a scroller is outside before the scroll', api.info(far.id).visibleBox, null);
await api.click(far.id);
check('a click scrolls the item into view', root.clicked, 'far;');
const scroller = root.$children.find((c) => c.$objectName === 'scroller');
check('the scroller moved', scroller.contentY > 0, true);

// ----- text and keys -----
const edit = api.find(null, 'edit')[0];
await api.focus(edit.id);
await api.fill(edit.id, 'hello');
check('fill replaces the text', api.get(edit.id, 'text'), 'hello');
await api.type(' there');
check('type appends', api.get(edit.id, 'text'), 'hello there');
await api.press('Backspace');
check('press Backspace', api.get(edit.id, 'text'), 'hello ther');
await api.fill(edit.id, '');
check('an empty text clears', api.get(edit.id, 'text'), '');
check('key Control+a', [keyEventFor('Control+a').ctrlKey, keyEventFor('Control+a').key], [true, 'a']);
check('key Shift+Tab', [keyEventFor('Shift+Tab').shiftKey, keyEventFor('Shift+Tab').key], [true, 'Tab']);
check('a capital letter has the shift', keyEventFor('A').shiftKey, true);

// ----- stale handles and waiting -----
const late = api.find(null, 'late')[0];
root.$children.find((c) => c.$typeName === 'Loader').active = false;
tick();
tick();
check('a handle of a destroyed item does not exist', api.exists(late.id), false);
await rejects('and an action fails with STALE', () => api.info(late.id), 'STALE');
check('whenIdle', await api.whenIdle({ timeout: 2000 }), true);
check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
