// The bridge between an input element of the page and the editing model (src/textproxy.js): what the element reports (its value and
// selection after typing, a virtual keyboard, a composition, a paste, the autofill) becomes a change of the model like a typed key, and a
// key that the element edits does not edit the model a second time.
// `node test/text-input-bridge.mjs`
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
	property string keys: ""
	property int edited: 0
	TextInput {
		id: input
		width: 200
		height: 30
		maximumLength: 8
		inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
		onTextEdited: root.edited = root.edited + 1
		Keys.onPressed: function(event) { root.keys = root.keys + event.key + ";" }
	}
	TextEdit {
		id: area
		y: 40
		width: 200
		height: 60
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
const input = root.$children.find((c) => c.$typeName === 'TextInput');
const area = root.$children.find((c) => c.$typeName === 'TextEdit');
input.forceActiveFocus();
frames(1);

const state = (item) => [item.text, item.cursorPosition, item.selectionStart, item.selectionEnd];

check('Qt.InputMethodHints', [input.inputMethodHints, root.$scope === undefined ? 0 : 0], [0x40 | 0x10000, 0]);
check('the editor reports what its element mirrors', input.$domState(), { text: '', start: 0, end: 0, backward: false });

// typing in the element
check('a letter typed in the element', [input.$fromDom('a', 1, 1, false), ...state(input)], [true, 'a', 1, 1, 1]);
check('textEdited came', root.edited, 1);
check('more letters, one step of undo', [input.$fromDom('abc', 3, 3, false), ...state(input)], [true, 'abc', 3, 3, 3]);
input.undo();
check('undo takes the typed letters together', input.text, '');
input.redo();
check('and redo brings them back', input.text, 'abc');

// edits inside the text
check('a letter inserted in the middle', [input.$fromDom('abXc', 3, 3, false), ...state(input)], [true, 'abXc', 3, 3, 3]);
check('a delete inside', [input.$fromDom('abc', 2, 2, false), ...state(input)], [true, 'abc', 2, 2, 2]);
check('a selection replaced', [input.$fromDom('aZc', 2, 2, false), ...state(input)], [true, 'aZc', 2, 2, 2]);
check('only the selection changed', [input.$fromDom('aZc', 0, 3, false), ...state(input)], [false, 'aZc', 3, 0, 3]);
check('a backward selection', [input.$fromDom('aZc', 0, 2, true), input.cursorPosition, input.selectionStart, input.selectionEnd], [false, 0, 0, 2]);

// the model keeps its own limits
input.text = '';
check('maximumLength: more than it takes', [input.$fromDom('123456789', 9, 9, false), input.text], [true, '12345678']);
input.text = '';
check('a line break in a TextInput is a space', [input.$fromDom('ab\ncd', 5, 5, false), input.text], [true, 'ab cd']);
input.readOnly = true;
check('a read-only editor does not follow', [input.$fromDom('x', 1, 1, false), input.text], [false, 'ab cd']);
input.readOnly = false;

// a TextEdit keeps its line breaks, and the pairs of the surrogates stay whole
check('a TextEdit takes a line break', [area.$fromDom('ab\ncd', 5, 5, false), area.text], [true, 'ab\ncd']);
area.$fromDom('', 0, 0, false);
check('an emoji typed (a surrogate pair)', [area.$fromDom('\u{1F600}', 2, 2, false), area.text.length, area.cursorPosition], [true, 2, 2]);
check('another emoji after it', [area.$fromDom('\u{1F600}\u{1F601}', 4, 4, false), area.text.length], [true, 4]);
check('the second one deleted', [area.$fromDom('\u{1F600}', 2, 2, false), area.text === '\u{1F600}'], [true, true]);

// autofill: the whole value at once
input.text = '';
check('autofill sets a value', [input.$fromDom('user@example.com', 16, 16, false), input.text], [true, 'user@exa']);

// a key that the element edits: the Keys handlers get it, the model is not edited a second time
input.text = '';
root.keys = '';
const dom = (key, extra = {}) => ({ key, code: '', ctrlKey: false, shiftKey: false, altKey: false, metaKey: false, repeat: false, getModifierState: () => false, ...extra });
check('a key edited by the element: not typed again', [scene.keyDown(dom('a', { code: 'KeyA' }), true), input.text, root.keys], [false, '', '65;']);
check('without native the engine types it', [scene.keyDown(dom('b', { code: 'KeyB' }), false), input.text], [true, 'b']);
check('keys of the editor that the element keeps for itself', [input.$shortcutOverride({ key: 0x41, modifiers: 0 }), input.$shortcutOverride({ key: 0x01000004, modifiers: 0 })], [true, false]);
check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
