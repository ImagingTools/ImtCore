// Keyboard and mouse editing of TextInput and TextEdit, driven through the scene without a browser.
// `node test/editing.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const SOURCE = `
import QtQuick 2.15

Item {
	width: 400
	height: 300
	property var log: []

	Item {
		id: first
		KeyNavigation.tab: edit
		KeyNavigation.right: edit
		width: 200
		height: 20

		TextInput {
			id: input
			objectName: "input"
			anchors.fill: parent
			focus: true
			onAccepted: log.push("accepted")
			onTextEdited: log.push("edited")
			onEditingFinished: log.push("finished")
		}
	}

	Item {
		id: second
		y: 40
		width: 200
		height: 80

		TextEdit {
			id: edit
			objectName: "edit"
			anchors.fill: parent
			wrapMode: TextEdit.WordWrap
		}
	}

	TextInput {
		id: narrow
		objectName: "narrow"
		y: 140
		width: 40
		height: 20
		text: "abcdefghijklmnopqrstuvwxyz"
	}

	MouseArea {
		id: area
		objectName: "area"
		y: 180
		width: 100
		height: 20
		acceptedButtons: Qt.RightButton
		onClicked: log.push("right " + mouse.button)
	}

	MouseArea {
		id: plain
		objectName: "plain"
		y: 210
		width: 100
		height: 20
		onClicked: log.push("left " + mouse.button)
	}

	Rectangle {
		id: veil
		objectName: "veil"
		visible: false
		anchors.fill: parent
		z: 100
		MouseArea { anchors.fill: parent }
	}

	TextInput {
		id: above
		objectName: "above"
		z: 200
		x: 150
		y: 140
		width: 100
		height: 20
	}

	Item {
		id: button
		objectName: "button"
		property int focusPolicy: Qt.WheelFocus
		x: 150
		y: 180
		width: 100
		height: 20
	}
}
`;

class MemoryFS {
	async readText(path) {
		return path === 'main.qml' ? SOURCE : null;
	}

	async readBytes() {
		return null;
	}
}

const engine = new Engine();
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(400, 300, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
const frame = () => {
	for (let i = 0; i < 4; i++) scene.drawFrame(frame.time += 0.016);
};
frame.time = 0;
frame();

const find = (name, item = viewer.content) => {
	if (item.$objectName === name) return item;
	for (const child of item.$children) {
		const hit = find(name, child);
		if (hit !== null) return hit;
	}
	return null;
};
const input = find('input');
const edit = find('edit');
const narrow = find('narrow');
const area = find('area');
const plain = find('plain');
const root = viewer.content.$children[0];

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}

const key = (k, extra = {}) => {
	const down = scene.keyDown({ key: k, code: k.length === 1 ? `Key${k.toUpperCase()}` : k, ...extra });
	scene.keyUp({ key: k, ...extra });
	frame();
	return down;
};
const type = (text) => {
	for (const ch of text) key(ch);
};
const ctrl = (k) => key(k, { ctrlKey: true });
const click = (x, y, button = 0) => {
	scene.pointerDown(x, y, button);
	scene.pointerUp(x, y);
	frame();
};
const state = (item) => [item.text, item.cursorPosition, item.selectionStart, item.selectionEnd];

check('engine errors', engine.errors, []);
check('the first editor has the focus', input.$active, true);

type('Hello');
check('typing', state(input), ['Hello', 5, 5, 5]);
check('textEdited per key', root.log.filter((e) => e === 'edited').length, 5);

key('Backspace');
check('backspace', state(input), ['Hell', 4, 4, 4]);
key('ArrowLeft');
key('ArrowLeft');
check('left', input.cursorPosition, 2);
key('Delete');
check('delete', state(input), ['Hel', 2, 2, 2]);
key('Home');
check('home', input.cursorPosition, 0);
key('End', { shiftKey: true });
check('shift+end selects', state(input), ['Hel', 3, 0, 3]);
key('ArrowLeft');
check('left collapses the selection to its start', state(input), ['Hel', 0, 0, 0]);
key('ArrowRight', { ctrlKey: true });
check('ctrl+right moves by a word', input.cursorPosition, 3);

ctrl('a');
check('ctrl+a', state(input), ['Hel', 3, 0, 3]);
type('X');
check('typing replaces the selection', state(input), ['X', 1, 1, 1]);
ctrl('z');
check('undo restores the text and the selection', state(input), ['Hel', 3, 0, 3]);
ctrl('y');
check('redo', input.text, 'X');

type('yz');
ctrl('z');
check('typing is one undo step', input.text, 'X');

input.text = 'one two';
ctrl('a');
ctrl('c');
input.text = '';
ctrl('v');
check('copy and paste', input.text, 'one two');
ctrl('a');
ctrl('x');
check('cut', input.text, '');
ctrl('v');
check('paste after cut', input.text, 'one two');

key('Enter');
check('return emits accepted and editingFinished', root.log.filter((e) => e === 'accepted' || e === 'finished'), ['accepted', 'finished']);

input.maximumLength = 5;
check('maximumLength cuts the text', input.text, 'one t');
type('abc');
check('maximumLength refuses more', input.text, 'one t');
input.maximumLength = 32767;

engine.clipboard = { text: 'sentinel', html: '' };
input.echoMode = 2;
check('password shows bullets', input.displayText, '\u25cf'.repeat(5));
ctrl('a');
ctrl('c');
input.text = 'kept';
check('password cannot be copied', engine.clipboard.text, 'sentinel');
input.echoMode = 0;

input.readOnly = true;
type('q');
key('Backspace');
check('readOnly refuses typing', input.text, 'kept');
input.readOnly = false;

// the mouse
const at = (item, index) => {
	const p = item.positionToRectangle(index);
	return [item.$sx + p.x + 0.5, item.$sy + p.y + 3];
};
input.text = 'hello world';
frame();
click(...at(input, 4));
check('click puts the cursor', state(input), ['hello world', 4, 4, 4]);
scene.pointerDown(...at(input, 2));
scene.pointerMove(...at(input, 7));
scene.pointerUp(...at(input, 7));
frame();
check('drag selects', state(input), ['hello world', 7, 2, 7]);
input.$lastPress = 0;
click(...at(input, 8));
click(...at(input, 8));
check('double click selects a word', input.selectedText, 'world');

// focus and KeyNavigation
click(...at(edit, 0));
check('a click moves the focus', [input.activeFocus, edit.activeFocus], [false, true]);
check('editingFinished when the focus is lost', root.log.filter((e) => e === 'finished').length, 2);
key('Tab');
check('a TextEdit types the tab and keeps the focus (Qt)', [edit.activeFocus, edit.text], [true, '	']);
key('Tab', { shiftKey: true });
check('shift+tab is typed as well (Qt)', [edit.activeFocus, edit.text], [true, '		']);
edit.text = '';
narrow.forceActiveFocus();
frame();
key('Tab', { shiftKey: true });
check('shift+tab of a TextInput goes back to the editor', [edit.activeFocus, narrow.activeFocus], [true, false]);
narrow.forceActiveFocus();
frame();
key('Tab');
key('Tab');
check('an item with a tab focus policy is in the chain', find('button').activeFocus, true);
key('Tab');
check('and the chain wraps', input.activeFocus, true);
edit.forceActiveFocus();
frame();
type('ab');
key('Enter');
type('cd');
check('return in a TextEdit starts a line', edit.text, 'ab\ncd');
check('line count', edit.lineCount, 2);
key('ArrowUp');
check('up moves to the line above', edit.cursorPosition < 3, true);
key('ArrowDown');
check('down moves back', edit.cursorPosition > 2, true);
key('Home', { ctrlKey: true });
key('End', { shiftKey: true, ctrlKey: true });
check('ctrl+shift+end selects everything', edit.selectedText, 'ab\ncd');
ctrl('z');
check('undo in a TextEdit', edit.text !== 'ab\ncd', true);

edit.text = 'a long line of words that has to wrap around';
frame();
check('a TextEdit wraps', edit.lineCount > 1, true);
const wrapped = edit.contentHeight;
edit.wrapMode = 0;
frame();
check('without wrapping it is one line', edit.lineCount, 1);
check('wrapping adds height', wrapped > edit.contentHeight, true);

input.forceActiveFocus();
frame();
key('Tab');
check('KeyNavigation.tab moves the focus', edit.activeFocus, true);
input.forceActiveFocus();
frame();
input.cursorPosition = input.text.length;
key('ArrowRight');
check('KeyNavigation.right takes the key at the end of the text', edit.activeFocus, true);

// a long line scrolls
narrow.forceActiveFocus();
frame();
key('End');
check('a long line scrolls to the cursor', narrow.cursorRectangle.x <= narrow.width && narrow.cursorRectangle.x > 0, true);
key('Home');
check('and back', narrow.cursorRectangle.x, 0);

// buttons
const inside = (item) => [item.$sx + 10, item.$sy + 10];
click(...inside(area), 2);
check('a right-button area gets a right click', root.log.slice(-1), ['right RightButton']);
click(...inside(area), 0);
check('and ignores a left click', root.log.slice(-1), ['right RightButton']);
click(...inside(plain), 2);
check('a left-button area ignores a right click', root.log.slice(-1), ['right RightButton']);
click(...inside(plain), 0);
check('and takes a left click', root.log.slice(-1), ['left LeftButton']);
check('no errors', engine.errors, []);

// painting: a selection quad, a cursor quad and the glyphs go through the renderer
const painted = { quads: [] };
const renderer = scene.renderer;
renderer.fillQuad = (x, y, w, h) => painted.quads.push([x, y, w, h]);
const paint = () => {
	painted.quads.length = 0;
	input.$update();
	scene.drawFrame((frame.time += 0.016));
};
input.forceActiveFocus();
input.text = 'hello';
input.select(1, 3);
paint();
check('the selection and the cursor are painted', painted.quads.filter((q) => q[2] === 1).length, 1);
check('the selection is wider than the cursor', painted.quads.some((q) => q[2] > 1), true);
input.readOnly = true;
paint();
check('a read-only editor shows no cursor', painted.quads.filter((q) => q[2] === 1).length, 0);
input.readOnly = false;
input.$update();
scene.drawFrame((frame.time += 0.016));
scene.drawFrame((frame.time += 0.6));
paint();
check('the cursor blinks', painted.quads.filter((q) => q[2] === 1).length, 0);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
