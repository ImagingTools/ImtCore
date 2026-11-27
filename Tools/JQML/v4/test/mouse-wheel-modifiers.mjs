// `wheel` of a MouseArea and `modifiers` of the mouse events, as desktop Qt gives them:
//   the wheel goes to the area that listens to it, also when items that take no input lie over it
//   an area without a handler of `wheel` does not take it
//   `wheel.accepted = false` hands the event to what is below the area, a sibling underneath as well
//   pressed, positionChanged, released and clicked carry the modifiers held at the pointer event
// `node test/mouse-wheel-modifiers.mjs`
import { Engine } from '../src/engine.js';
import { domModifiers, pointerState } from '../src/keys.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick

Item {
	id: root
	width: 400
	height: 400
	property string log: ""
	property alias flick: flick
	property alias passArea: passArea

	MouseArea {
		width: 100; height: 100
		onWheel: root.log += "W(" + wheel.x + "," + wheel.y + "," + wheel.angleDelta.y + "," + ((wheel.modifiers & Qt.ControlModifier) !== 0) + ") "
		onPressed: root.log += "P" + ((mouse.modifiers & Qt.ControlModifier) !== 0) + " "
		onPositionChanged: root.log += "M" + ((mouse.modifiers & Qt.ControlModifier) !== 0) + " "
		onReleased: root.log += "R" + ((mouse.modifiers & Qt.ControlModifier) !== 0) + " "
		onClicked: root.log += "C" + ((mouse.modifiers & Qt.ControlModifier) !== 0) + " "
	}

	Rectangle {
		width: 100; height: 100
		color: "transparent"
	}

	Flickable {
		id: flick
		x: 200; width: 100; height: 100
		contentWidth: 100; contentHeight: 1000
	}

	MouseArea {
		id: passArea
		x: 200; width: 100; height: 100
		onWheel: {
			root.log += "pass "
			wheel.accepted = false
		}
	}

	Flickable {
		id: quietFlick
		objectName: "quiet"
		y: 200; width: 100; height: 100
		contentWidth: 100; contentHeight: 1000

		MouseArea {
			width: 100; height: 1000
		}
	}
	property alias quietFlick: quietFlick
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
scene.resize(400, 400, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
let time = 0;
const frame = (n = 3) => {
	for (let i = 0; i < n; i++) scene.drawFrame((time += 0.016));
};
frame(5);
const root = viewer.content.$children[0];
const origin = [root.$sx, root.$sy];
const at = (x, y) => [origin[0] + x, origin[1] + y];
const take = () => {
	const text = root.log.trim();
	root.log = '';
	return text;
};

scene.wheel(...at(10, 20), 1);
check('the wheel reaches the area below an item that takes no input', take(), 'W(10,20,120,false)');

pointerState.modifiers = domModifiers({ ctrlKey: true });
scene.wheel(...at(10, 20), -1);
check('the wheel carries the modifiers', take(), 'W(10,20,-120,true)');

scene.pointerDown(...at(10, 10));
scene.pointerMove(...at(20, 20));
scene.pointerUp(...at(20, 20));
check('the mouse events carry the modifiers', take(), 'Ptrue Mtrue Rtrue Ctrue');

pointerState.modifiers = domModifiers({});
scene.pointerDown(...at(10, 10));
scene.pointerUp(...at(10, 10));
check('and none of them without a key held', take(), 'Pfalse Rfalse Cfalse');

scene.wheel(...at(250, 50), -1);
frame(30);
check('a rejected wheel is seen by the area', take(), 'pass');
check('and goes on to the view below it', root.flick.contentY > 0, true);
check('the area takes the wheel again afterwards', root.passArea.$acceptsWheel, true);

scene.wheel(...at(50, 250), -1);
frame(30);
check('an area without onWheel leaves the wheel to the view around it', root.quietFlick.contentY > 0, true);

check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
