// A Binding with `delayed: true` publishes once per frame (Qt: at the end of the event loop). The combo box decorator of ImtCore
// centers its text with `leftMargin: width / 2 - text.width / 2` while the text, anchored left and right, gets narrower with
// the margin: each step is half of the one before, and it took ~50 passes of one frame (a second and a half in IotPlatform).
// `node test/binding-delayed.mjs`
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
	width: 200
	height: 40
	property alias centered: title
	property alias spare: other
	property int moves: 0

	Rectangle {
		id: box
		width: 80
		height: 30

		Text {
			id: title
			anchors.left: box.left
			anchors.right: arrow.left
			anchors.rightMargin: 10
			text: "2026"
		}

		Item {
			id: arrow
			width: 12
			height: 12
			anchors.right: box.right
		}

		Binding {
			target: title.anchors
			property: "leftMargin"
			value: box.width / 2 - title.width / 2
			delayed: true
		}
	}

	Item {
		id: other
		width: 50
		property int size: 10

		Binding {
			target: other
			property: "size"
			value: 20
		}
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
scene.resize(200, 100, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);

let frames = 0;
let passes = 0;
const runBindingPass = engine.runBindingPass.bind(engine);
engine.runBindingPass = () => {
	passes++;
	return runBindingPass();
};
const frameStart = engine.frameId;
while (frames < 200) {
	const drawn = scene.drawFrame(frames * 0.016);
	frames++;
	if (!drawn) break;
}
const root = viewer.content.$children[0];

const width = root.centered.width;
const margin = root.centered.anchors.leftMargin;
check('the text is centered: leftMargin = width / 2 - text width / 2', Math.abs(margin - (40 - width / 2)) < 0.05, true);
check('the chain ends within a few frames, not hundreds of passes of one', frames < 30, true);
check('a frame has a few passes', passes / frames <= 8, true);
check('the frames are counted by the scene', engine.frameId - frameStart, frames - 1);
check('a Binding without delayed publishes at once', root.spare.size, 20);

console.log(`frames ${frames}, passes ${passes}, leftMargin ${margin}, width ${width}`);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
