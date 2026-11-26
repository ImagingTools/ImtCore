// What the Shortcut items do with the keys of a page, seen from the DOM events the browser sends: the keys that a shortcut takes are
// reported as taken (the page then keeps the browser from using them), autoRepeat, a shortcut that goes away with its item, StandardKey,
// the Enter of the number pad, Shift+Tab, a key the focused text editor keeps, and a key sequence that is not valid.
// The comparison with desktop Qt is in test/desktop/Shortcut_*; `node test/shortcut.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick

Item {
	id: root
	width: 300
	height: 200
	property string entries: ""
	function note(name) { entries = entries + name + ";" }
	property bool loaded: true

	Shortcut { sequence: "Escape"; onActivated: root.note("Escape") }
	Shortcut { sequence: "Ctrl+S"; onActivated: root.note("Ctrl+S") }
	Shortcut { sequence: "Return"; onActivated: root.note("Return") }
	Shortcut { sequence: "Enter"; onActivated: root.note("Enter") }
	Shortcut { sequence: "Shift+Tab"; onActivated: root.note("Shift+Tab") }
	Shortcut { sequence: "Tab"; onActivated: root.note("Tab") }
	Shortcut { sequence: "A"; onActivated: root.note("A") }
	Shortcut { sequence: "Ctrl+Z"; onActivated: root.note("Ctrl+Z") }
	Shortcut { sequence: "F3"; autoRepeat: false; onActivated: root.note("F3") }
	Shortcut { sequence: "F4"; onActivated: root.note("F4") }
	Shortcut { sequence: StandardKey.Copy; onActivated: root.note("Copy") }
	Shortcut { sequence: StandardKey.Find; onActivated: root.note("Find") }
	Shortcut { sequence: "nonsense"; onActivated: root.note("nonsense") }
	Shortcut { sequence: "Ctrl+B"; enabled: root.loaded; onActivated: root.note("Ctrl+B") }
	Loader {
		id: loader
		active: true
		sourceComponent: Component {
			Item {
				Shortcut { sequence: "F9"; onActivated: root.note("F9") }
			}
		}
	}
	TextInput { id: input; y: 50; width: 100; text: "x" }
	Item { id: plain; y: 100; width: 10; height: 10 }
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

const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(300, 200, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
let t = 0;
const frames = (n) => {
	for (let i = 0; i < n; i++) {
		scene.root.$updateRequested = true;
		scene.drawFrame((t += 0.016));
	}
};
frames(4);
const root = viewer.content.$children[0];

// What the browser sends for a key, pressed and taken (true) or left (false).
const dom = (key, extra = {}) => ({ key, code: '', ctrlKey: false, shiftKey: false, altKey: false, metaKey: false, repeat: false, getModifierState: () => false, ...extra });
const press = (event) => {
	root.entries = '';
	const taken = scene.keyDown(event);
	return [taken, root.entries.split(';').filter((name) => name !== '')];
};

const plainItem = root.$children.find((child) => child.$typeName === 'Item' && child.height === 10);
plainItem.forceActiveFocus();
const input = root.$children.find((child) => child.$typeName === 'TextInput');
const loader = root.$children.find((child) => child.$typeName === 'Loader');

check('Escape is taken and activates', press(dom('Escape')), [true, ['Escape']]);
check('a key that no shortcut has is left', press(dom('x')), [false, []]);
check('Ctrl+S is taken (the browser must not save the page)', press(dom('s', { code: 'KeyS', ctrlKey: true })), [true, ['Ctrl+S']]);
check('Ctrl+S with a Shift is another key', press(dom('S', { code: 'KeyS', ctrlKey: true, shiftKey: true })), [false, []]);
check('Return of the main block', press(dom('Enter', { code: 'Enter' })), [true, ['Return']]);
check('Enter of the number pad', press(dom('Enter', { code: 'NumpadEnter' })), [true, ['Enter']]);
check('Shift+Tab is a shortcut of its own', press(dom('Tab', { code: 'Tab', shiftKey: true })), [true, ['Shift+Tab']]);
check('Tab', press(dom('Tab', { code: 'Tab' })), [true, ['Tab']]);
check('a letter of a shortcut "A" is the key a, without modifiers', press(dom('a', { code: 'KeyA' })), [true, ['A']]);
check('Shift+a is not', press(dom('A', { code: 'KeyA', shiftKey: true })), [false, []]);
check('on another layout the key of Ctrl+Z is the physical one', press(dom('я', { code: 'KeyZ', ctrlKey: true })), [true, ['Ctrl+Z']]);
check('autoRepeat: the first press', press(dom('F3')), [true, ['F3']]);
check('autoRepeat false: the repeated press is taken and does not activate', press(dom('F3', { repeat: true })), [true, []]);
check('autoRepeat true: the repeated press activates', press(dom('F4', { repeat: true })), [true, ['F4']]);
check('StandardKey.Copy is Ctrl+C', press(dom('c', { code: 'KeyC', ctrlKey: true })), [true, ['Copy']]);
check('StandardKey.Find is Ctrl+F', press(dom('f', { code: 'KeyF', ctrlKey: true })), [true, ['Find']]);
check('a sequence that is not valid never activates', press(dom('n')), [false, []]);
check('the release of a key does nothing', (() => {
	root.entries = '';
	scene.keyUp(dom('Escape'));
	return root.entries;
})(), '');

// enabled follows a binding
root.loaded = false;
frames(2);
check('a shortcut that is not enabled', press(dom('b', { code: 'KeyB', ctrlKey: true })), [false, []]);
root.loaded = true;
frames(2);
check('and is again', press(dom('b', { code: 'KeyB', ctrlKey: true })), [true, ['Ctrl+B']]);

// a shortcut goes away with the item that has it
check('the shortcut in the loader', press(dom('F9')), [true, ['F9']]);
loader.active = false;
frames(4);
check('does not activate when the loader has dropped it', [press(dom('F9')), engine.shortcuts.size], [[false, []], 14]);

// the text editor that has the focus keeps the keys of editing
input.forceActiveFocus();
check('Escape with a TextInput: the shortcut', press(dom('Escape')), [true, ['Escape']]);
check('a letter with a TextInput: the editor takes it, the shortcut does not', [press(dom('a', { code: 'KeyA' })), input.text], [[true, []], 'xa']);
check('Ctrl+Z with a TextInput: the editor', press(dom('z', { code: 'KeyZ', ctrlKey: true })), [true, []]);
check('Return with a TextInput: the shortcut', press(dom('Enter', { code: 'Enter' })), [true, ['Return']]);
input.readOnly = true;
check('a read-only TextInput keeps no key', press(dom('a', { code: 'KeyA' })), [true, ['A']]);
check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
