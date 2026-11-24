// A TextEdit with textFormat: Text.RichText is laid out and painted as the rich text of a Text: the colors and the backgrounds of the
// spans show (the terminal of Agentino writes its output as such a text and draws the block cursor as a span with a background that
// holds one space at the end), the lines of <br/> are lines, and the size is that of the marked up text.
// `node test/text-edit-rich-text.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const HTML = '<span style="color:#ff0000">AB</span><br/><span style="color:#3b3b3b">CD&gt;</span><span style="background-color:#005fb8;color:#ffffff"> </span>';
const FILES = {
	'main.qml': `
import QtQuick

Item {
	width: 300
	height: 300
	TextEdit { id: edit; width: 200; readOnly: true; textFormat: Text.RichText; font.pixelSize: 16; text: ${JSON.stringify(HTML)} }
	TextEdit { id: plain; width: 200; readOnly: true; font.pixelSize: 16; text: "AB\\nCD>" }
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
scene.resize(300, 300, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
for (let i = 0; i < 6; i++) scene.drawFrame(i * 0.016);
const [edit, plain] = viewer.content.$children[0].$children.filter((item) => item.$typeName === 'TextEdit');

// the font of the test is the box font: every letter is as wide as the pixel size
const LETTER = 16;
const quads = [];
const glyphs = [];
const renderer = {
	pack: (color) => (typeof color === 'string' ? color : JSON.stringify(color)),
	fillQuad: (x, y, w, h, color) => quads.push([Math.round(x / LETTER), Math.round(y / LETTER), Math.round(w / LETTER), color]),
	glyph: (glyph, x, y, color) => glyphs.push([Math.round(x / LETTER), color]),
	translate() {},
};
edit.$paint(renderer, 1);

check('the cursor span paints its background after the text of its line', quads.map((q) => q.slice(0, 3)), [[3, 1, 1]]);
check('the width of the second line counts the space of the cursor', Math.round(edit.contentWidth / LETTER), 4);
check('the height is two lines', Math.round(edit.contentHeight / LETTER) >= 2, true);
check('the background has the color of the span', quads.length === 1 && /0.*95.*184|005fb8/i.test(String(quads[0][3])), true);
check('a TextEdit of plain text is as before', Math.round(plain.contentWidth / LETTER), 3);

// a change of the text lays it out again
edit.text = '<span style="background-color:#005fb8"> </span>';
for (let i = 0; i < 3; i++) scene.drawFrame((10 + i) * 0.016);
check('a new text is laid out again', Math.round(edit.contentWidth / LETTER), 0);

check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
