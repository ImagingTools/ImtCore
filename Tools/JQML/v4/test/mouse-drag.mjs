// MouseArea.drag as desktop Qt 6.11 gives it, measured with QtTest (test/qt-measure/mouse/Main.qml). The numbers below are what Qt printed:
//   defaults        threshold 10, axis 3 (XAndYAxis), minimum and maximum of every axis +-3.4028234663852886e+38 (FLT_MAX), filterChildren false,
//                   smoothed true; Drag.dragType 2 (Internal), supportedActions 7, proposedAction 2, keys [], hotSpot 0, 0
//   the start       the move that goes farther than the threshold (strictly) from the press only takes the pointer as the reference; the drag
//                   starts with the NEXT move (drag.active true) and the target follows the pointer from the reference: press (15, 15) of the
//                   item, moves to 5, 9, 11 pixels: nothing; the move to 15 pixels (4 beyond the reference): the target is 4 pixels on
//   the end         \`released\` is sent while drag.active (and a bound Drag.active) are still true; there is no \`clicked\`, also when the threshold
//                   was crossed and the button was released before the drag started
//   the axis        \`drag.axis: Drag.YAxis\` moves along y only, the limits clamp
// \`node test/mouse-drag.mjs\`
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
	width: 400
	height: 400
	property string log: ""
	property alias box: box
	property alias grab: grab
	property alias axisBox: axisBox
	property alias axisArea: axisArea
	Rectangle {
		id: box
		x: 250; y: 250; width: 30; height: 30
		Drag.active: grab.drag.active
		MouseArea {
			id: grab
			anchors.fill: parent
			drag.target: box
			onPressed: root.log += "P "
			onReleased: root.log += "R(drag.active=" + drag.active + ",Drag.active=" + box.Drag.active + ") "
			onClicked: root.log += "C "
		}
	}
	Rectangle {
		id: axisBox
		x: 300; y: 250; width: 30; height: 30
		MouseArea {
			id: axisArea
			anchors.fill: parent
			drag.target: axisBox
			drag.axis: Drag.YAxis
			drag.minimumY: 200
			drag.maximumY: 300
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
const { box, grab, axisBox, axisArea } = root;
const origin = [box.$sx - 250, box.$sy - 250]; // the scene position of the point (0, 0) of the root
const at = (x, y) => [origin[0] + x, origin[1] + y];
const take = () => {
	const text = root.log.trim();
	root.log = '';
	return text;
};
const state = () => [box.x, box.y, grab.drag.active];

check('threshold', grab.drag.threshold, 10);
check('axis', grab.drag.axis, 3);
check('minimum and maximum', [grab.drag.minimumX, grab.drag.maximumX], [-3.4028234663852886e38, 3.4028234663852886e38]);
check('filterChildren and smoothed', [grab.drag.filterChildren, grab.drag.smoothed], [false, true]);
check('Drag.dragType, supportedActions, proposedAction', [box.Drag.dragType, box.Drag.supportedActions, box.Drag.proposedAction], [2, 7, 2]);
check('Drag.keys and hotSpot', [box.Drag.keys.length, box.Drag.hotSpot.x, box.Drag.hotSpot.y], [0, 0, 0]);

// the press is at (265, 265): the point (15, 15) of the item; the pointer positions are those of Qt (they follow the item that moves)
scene.pointerDown(...at(265, 265));
frame();
check('pressed', state(), [250, 250, false]);
scene.pointerMove(...at(270, 265));
frame();
check('moved 5 pixels', state(), [250, 250, false]);
scene.pointerMove(...at(274, 265));
frame();
check('moved 9 pixels', state(), [250, 250, false]);
scene.pointerMove(...at(276, 265));
frame();
check('moved 11 pixels: the threshold is crossed, nothing starts', state(), [250, 250, false]);
scene.pointerMove(...at(280, 265));
frame();
check('the next move starts the drag, 4 pixels from the reference', state(), [254, 250, true]);
scene.pointerMove(...at(309, 295));
frame();
check('moved far: the target follows from the reference', state(), [283, 280, true]);
scene.pointerUp(...at(309, 295));
frame();
check('released while the drag is active, no click', take(), 'P R(drag.active=true,Drag.active=true)');
check('and the drag is over, the target stays', state(), [283, 280, false]);

// the threshold is crossed and the button is released: no drag, no click
box.x = 250;
box.y = 250;
frame();
scene.pointerDown(...at(265, 265));
scene.pointerMove(...at(280, 265));
frame();
check('crossed', state(), [250, 250, false]);
scene.pointerUp(...at(280, 265));
frame();
check('released after the crossing only: no click, no drag', take(), 'P R(drag.active=false,Drag.active=false)');
check('the target did not move', state(), [250, 250, false]);

// the axis and the limits: the item starts at (300, 250)
scene.pointerDown(...at(315, 265));
scene.pointerMove(...at(340, 290));
scene.pointerMove(...at(345, 310));
frame();
scene.pointerMove(...at(360, 470));
frame();
check('Drag.YAxis: y only, down to the maximum', [axisBox.x, axisBox.y], [300, 300]);
scene.pointerMove(...at(360, 0));
frame();
check('and up to the minimum', [axisBox.x, axisBox.y], [300, 200]);
scene.pointerUp(...at(360, 0));
frame();

check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
