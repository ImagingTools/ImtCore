// The padding of Text as in Qt: `padding` and the padding of each side are in the implicit size and not in the content size, the text is
// laid out (wrapped, elided) in the width without the padding and painted inside it. A Row places the next item after the padding.
// `node test/text-padding.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick

Item {
	width: 400
	height: 200

	property alias plain: plain
	property alias padded: padded
	property alias sides: sides
	property alias wrapped: wrapped
	property alias trailing: trailing
	property alias after: after
	property real readLeft: sides.leftPadding

	Text { id: plain; text: "Shared by me" }
	Text { id: padded; text: "Shared by me"; padding: 4 }
	Text { id: sides; text: "Shared by me"; padding: 4; leftPadding: 10; font.underline: true }
	Text { id: wrapped; width: 100; leftPadding: 20; rightPadding: 20; wrapMode: Text.WordWrap; text: "one two three four five six" }

	Row {
		spacing: -8
		Rectangle { width: 50; height: 10 }
		Text { id: trailing; leftPadding: 12; text: "Shared by me" }
		Rectangle { id: after; width: 10; height: 10 }
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
scene.resize(400, 200, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
const frame = () => scene.drawFrame(0.016);
frame();
const root = viewer.content.$children[0];
const { plain, padded, sides, wrapped, trailing, after } = root;

const textWidth = plain.implicitWidth;
const textHeight = plain.implicitHeight;
check('no padding by default', [plain.padding, plain.leftPadding, plain.topPadding], [0, 0, 0]);
check('padding is in the implicit size', [padded.implicitWidth, padded.implicitHeight], [textWidth + 8, textHeight + 8]);
check('and not in the content size', [padded.contentWidth, padded.contentHeight], [textWidth, textHeight]);
check('a side without its own padding has padding', [padded.leftPadding, padded.bottomPadding], [4, 4]);
check('the padding of a side', [sides.leftPadding, sides.rightPadding, sides.implicitWidth], [10, 4, textWidth + 14]);

check('the text wraps in the width without the padding', wrapped.contentWidth <= 60 && wrapped.lineCount > 1, true);

check('a Row places the next item after the padding', [trailing.x, after.x], [42, 42 + textWidth + 12 - 8]);

// The underline of the text is painted at the left and top padding
const quads = [];
let tx = 0;
let ty = 0;
const recorder = {
	pack: () => 0,
	translate(x, y) {
		tx += x;
		ty += y;
	},
	glyph() {},
	fillQuad(x, y, w) {
		quads.push([x + tx, w]);
	},
};
sides.$paint(recorder, 1);
check('the text is painted inside the padding', quads.length > 0 && quads[0][0] === 10 && quads[0][1] === textWidth, true);
check('the painter is given back unmoved', [tx, ty], [0, 0]);

sides.padding = 6;
frame();
check('a change of padding moves the sides without their own', [root.readLeft, sides.rightPadding], [10, 6]);
sides.leftPadding = undefined;
frame();
check('a reset side has padding again', [sides.leftPadding, root.readLeft], [6, 6]);
check('no errors', engine.errors, []);

if (failures > 0) {
	console.log(`${failures} check(s) failed`);
	process.exit(1);
}
