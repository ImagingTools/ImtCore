// The wrap modes of a plain Text as in Qt: WordWrap breaks at the spaces and lets a word wider than the line out of it, Wrap breaks such
// a word between its characters from a line of its own, WrapAnywhere fills every line up to the width. The lines are those of desktop Qt
// in a monospace font of 10 columns (test/qt-measure/text-wrap), the spaces at the end of a line left out; `node test/text-wrap.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

// Without fonts every character, the space too, is as wide as the pixel size: 10 columns
const FILES = {
	'main.qml': `
import QtQuick

Item {
	width: 400
	height: 300

	property alias label: label
	property alias message: message

	Text { id: label; width: 100; font.pixelSize: 10 }
	Text {
		id: message
		y: 100
		width: 250
		font.pixelSize: 10
		wrapMode: Text.Wrap
		text: "Connecting to http://files.example-server.cloud:7380/Files/Views/html/index.html ..."
	}
}
`,
};

const CASES = [
	{
		text: 'one two abcdefghijklmnopqrstuvwxyz three',
		WordWrap: ['one two', 'abcdefghijklmnopqrstuvwxyz', 'three'],
		WrapAnywhere: ['one two ab', 'cdefghijkl', 'mnopqrstuv', 'wxyz three'],
		Wrap: ['one two', 'abcdefghij', 'klmnopqrst', 'uvwxyz', 'three'],
	},
	{
		text: 'one two three four five',
		WordWrap: ['one two', 'three four', 'five'],
		WrapAnywhere: ['one two th', 'ree four f', 'ive'],
		Wrap: ['one two', 'three four', 'five'],
	},
	{
		text: 'abc defghijklmnop',
		WordWrap: ['abc', 'defghijklmnop'],
		WrapAnywhere: ['abc defghi', 'jklmnop'],
		Wrap: ['abc', 'defghijklm', 'nop'],
	},
	{
		text: 'x  abcdefghijklmnopqrstu  y',
		WordWrap: ['x', 'abcdefghijklmnopqrstu', 'y'],
		WrapAnywhere: ['x  abcdefg', 'hijklmnopq', 'rstu  y'],
		Wrap: ['x', 'abcdefghij', 'klmnopqrst', 'u  y'],
	},
];

const MODES = { WordWrap: 1, WrapAnywhere: 3, Wrap: 4 };

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
scene.resize(400, 300, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
const frame = () => scene.drawFrame(0.016);
frame();
const root = viewer.content.$children[0];
const { label, message } = root;
const lines = (text) => text.$measure().lines.map((line) => line.trimEnd());

for (const item of CASES) {
	label.text = item.text;
	for (const mode of Object.keys(MODES)) {
		label.wrapMode = MODES[mode];
		frame();
		check(`${mode}: "${item.text}"`, lines(label), item[mode]);
		check(`${mode}: the line count of "${item.text}"`, label.lineCount, item[mode].length);
	}
}

label.text = 'abcdefghijklmnopqrstuvwxyz';
label.wrapMode = MODES.WordWrap;
frame();
check('a word wider than the line goes out of it with WordWrap', [label.lineCount, label.contentWidth], [1, 260]);
label.wrapMode = MODES.Wrap;
frame();
check('a change of wrapMode lays the text out again', [label.lineCount, label.contentWidth], [3, 100]);
check('the implicit width is that of the text without wrapping', label.implicitWidth, 260);

// The address in the message has no spaces: it is broken between characters and stays inside the width
const shown = lines(message);
check('the message is in the width', [message.contentWidth <= 250, shown.every((line) => line.length <= 25)], [true, true]);
check('and every character of it is shown', shown.join('').replace(/ /g, ''), message.text.replace(/ /g, ''));
check('the address starts on a line of its own', shown[0], 'Connecting to');
check('no errors', engine.errors, []);

if (failures > 0) {
	console.log(`${failures} check(s) failed`);
	process.exit(1);
}
