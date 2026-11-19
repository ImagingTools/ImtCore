// MouseArea.onCanceled (7 uses, Button.qml among them: it resets `down`) and onPressAndHold (6 uses), as desktop Qt 6.11 gives them. The values
// were measured with QtTest (test/qt-measure/mouse/Main.qml sends the events to a window):
//   canceled        the area (or a parent) is hidden while pressed, or a DragHandler takes the drag over: `pressed` is false and neither released nor
//                   clicked follow. A DISABLED area keeps the press: it gets its release and its click. (A Flickable does not take the drag from an area
//                   under it in this engine: it scrolls by a part with no area.)
//   pressAndHold    the button stays down for pressAndHoldInterval (800 ms); clicked is not sent on the release after it. The time runs wherever the
//                   pointer goes, and the signal is sent if the pointer is inside the area when it is over: a move of 15 pixels inside, or out and back,
//                   still gives it; staying outside does not
// `node test/mouse-area-canceled-hold.mjs`
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
	height: 300
	property string log: ""
	property alias plain: plain
	property alias held: held
	property alias dragger: dragger

	MouseArea {
		id: plain
		x: 0; y: 0; width: 100; height: 100
		onPressed: root.log += "P1 "
		onReleased: root.log += "R1 "
		onClicked: root.log += "C1 "
		onCanceled: root.log += "X1 "
	}
	MouseArea {
		id: held
		x: 100; y: 0; width: 100; height: 100
		onPressed: root.log += "P2 "
		onReleased: root.log += "R2 "
		onClicked: root.log += "C2 "
		onPressAndHold: root.log += "H2 "
		onCanceled: root.log += "X2 "
	}
	Item {
		id: dragger
		x: 0; y: 100; width: 200; height: 100
		DragHandler { target: null }
		MouseArea {
			anchors.fill: parent
			onPressed: root.log += "P3 "
			onReleased: root.log += "R3 "
			onClicked: root.log += "C3 "
			onCanceled: root.log += "X3 "
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
scene.resize(300, 300, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
let time = 0;
const advance = (seconds) => {
	const end = time + seconds;
	while (time < end) scene.drawFrame((time += 0.016));
};
advance(0.1);
const root = viewer.content.$children[0];
// the viewer puts the content below its own bar: the coordinates of the scene come from the items
const at = (item, dx = 50, dy = 50) => [item.$sx + dx, item.$sy + dy];
const take = () => {
	const text = root.log.trim();
	root.log = '';
	return text;
};

// the ordinary click is the reference: nothing of the new signals appears
scene.pointerDown(...at(root.plain));
scene.pointerUp(...at(root.plain));
check('a click', take(), 'P1 R1 C1');

// disabled while pressed: no cancel (Qt: [P], pressed, then [R C])
scene.pointerDown(...at(root.plain));
check('pressed', root.plain.pressed, true);
root.plain.enabled = false;
advance(0.05);
check('a disabled area keeps the press', [take(), root.plain.pressed], ['P1', true]);
scene.pointerUp(...at(root.plain));
check('and gets the release and the click', take(), 'R1 C1');
root.plain.enabled = true;

// canceled: hidden while pressed
scene.pointerDown(...at(root.plain));
root.plain.visible = false;
advance(0.05);
check('canceled when hidden while pressed', take(), 'P1 X1');
scene.pointerUp(...at(root.plain));
check('the release after it sends nothing', take(), '');
root.plain.visible = true;

// canceled: a DragHandler takes the drag
scene.pointerDown(...at(root.dragger, 50, 50));
scene.pointerMove(...at(root.dragger, 50, 40));
scene.pointerMove(...at(root.dragger, 50, 20));
scene.pointerMove(...at(root.dragger, 50, 0));
advance(0.05);
scene.pointerUp(...at(root.dragger, 50, 0));
advance(0.05);
const dragLog = take();
check('a DragHandler that takes the drag cancels the area under it', dragLog, 'P3 X3');

// pressAndHold
check('pressAndHoldInterval is 800 by default', root.held.pressAndHoldInterval, 800);
scene.pointerDown(...at(root.held));
advance(0.5);
check('not yet after 500 ms', take(), 'P2');
advance(0.4);
check('after 800 ms', take(), 'H2');
scene.pointerUp(...at(root.held));
check('released, and not clicked after a hold', take(), 'R2');

// released before the time
scene.pointerDown(...at(root.held));
advance(0.3);
scene.pointerUp(...at(root.held));
advance(1);
check('an early release is a click and no hold', take(), 'P2 R2 C2');

// moved inside: the hold is still sent (Qt: [P H] for 4 and for 15 pixels)
scene.pointerDown(...at(root.held, 20, 50));
scene.pointerMove(...at(root.held, 35, 50));
advance(0.9);
check('a move of 15 pixels inside the area keeps the hold', take(), 'P2 H2');
scene.pointerUp(...at(root.held, 35, 50));
check('and the release is no click', take(), 'R2');

// moved out and left there: no hold (Qt: [P], then [R])
scene.pointerDown(...at(root.held));
scene.pointerMove(...at(root.held, 50, 130));
advance(0.9);
check('staying outside the area when the time is over gives no hold', take(), 'P2');
scene.pointerUp(...at(root.held, 50, 130));
check('released outside: no click', take(), 'R2');

// out and back before the time is over: the hold (Qt: [P H])
scene.pointerDown(...at(root.held));
scene.pointerMove(...at(root.held, 50, 130));
scene.pointerMove(...at(root.held, 50, 60));
advance(0.9);
check('out and back gives the hold', take(), 'P2 H2');
scene.pointerUp(...at(root.held, 50, 60));
take();

// the interval is a property
root.held.pressAndHoldInterval = 200;
scene.pointerDown(...at(root.held));
advance(0.3);
check('a shorter interval', take(), 'P2 H2');
scene.pointerUp(...at(root.held));
take();

check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
