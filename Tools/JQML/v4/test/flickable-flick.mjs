// A Flickable throws the content after the release, as desktop Qt 6.11 does (measured with QtTest, test/qt-measure/flickable):
// the drag of an axis starts at the first move after the one that went farther than 10 px from the press; below 75 px/s the movement
// ends with the release; above it the content runs on with the deceleration of 1500 px/s^2 (it stops at its end exactly) and the
// signals come in the order movementStarted, dragStarted, dragEnded, flickStarted, flickEnded, movementEnded. `node test/flickable-flick.mjs`
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
	property alias flick: flick
	Flickable {
		id: flick
		width: 300
		height: 300
		contentWidth: 1500
		contentHeight: 300
		onMovementStarted: root.log += "MS(" + Math.round(flick.contentX) + ") "
		onMovementEnded: root.log += "ME(" + Math.round(flick.contentX) + ") "
		onFlickStarted: root.log += "FS(" + Math.round(flick.contentX) + ") "
		onFlickEnded: root.log += "FE "
		onDragStarted: root.log += "DS(" + Math.round(flick.contentX) + ") "
		onDragEnded: root.log += "DE(" + Math.round(flick.contentX) + ") "
	}
}`,
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

// the clock of the pointer events and of the frames, in ms
let clock = 1000;
performance.now = () => clock;

const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(300, 300, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
const frame = () => scene.drawFrame(clock / 1000);
frame();
const root = viewer.content.$children[0];
const flick = root.flick;
const ox = root.$sx;
const oy = root.$sy;
const at = (x) => [ox + x, oy + 150];

function drag(start, steps, delay, idle = 0) {
	flick.contentX = 0;
	root.log = '';
	scene.pointerMove(...at(start));
	scene.pointerDown(...at(start), 0);
	let x = start;
	for (const dx of steps) {
		clock += delay;
		x -= dx;
		scene.pointerMove(...at(x));
		frame();
	}
	clock += idle;
	scene.pointerUp(...at(x));
}

function settle() {
	for (let i = 0; i < 400 && (flick.flicking || engine.flicks.size > 0); i++) {
		clock += 16;
		frame();
	}
}

// 14 moves of 1 px: the drag starts at the 12th (Qt), nothing is thrown, the movement ends at the release
drag(250, new Array(14).fill(1), 20);
check('1 px moves: the content moved by 2', flick.contentX, 2);
check('1 px moves: order of the signals', root.log, 'MS(0) DS(0) DE(2) ME(2) ');
check('1 px moves: nothing runs on', [flick.moving, flick.flicking, flick.dragging], [false, false, false]);

// 20 px every 100 ms is 200 px/s: thrown by 13 px (Qt 13)
drag(250, [20, 20, 20, 20, 20], 100);
check('slow drag: flicking after the release', [flick.flicking, flick.moving, flick.dragging], [true, true, false]);
settle();
check('slow drag: comes to rest at 73 (Qt 73)', Math.round(flick.contentX), 73);
check('slow drag: signals', root.log, 'MS(0) DS(0) DE(60) FS(60) FE ME(73) ');

// 20 px every 10 ms is 2000 px/s: thrown by about 1075 px (Qt 1075)
drag(250, [20, 20, 20, 20, 20], 10);
settle();
check('fast drag: thrown far, about 1075 px (Qt 1075)', Math.abs(flick.contentX - 60 - 1075) < 150, true);
check('fast drag: the movement ends with the flick', [flick.moving, flick.flicking, flick.horizontalVelocity], [false, false, 0]);
check('fast drag: signals', root.log.replace(/\((\d+)\)/g, '').trim(), 'MS DS DE FS FE ME');

// a pause of 100 ms before the release: not thrown (Qt)
drag(250, [20, 20, 20, 20, 20], 10, 120);
check('rest before the release: not thrown', [flick.flicking, flick.moving, flick.contentX], [false, false, 60]);

// stops at the end exactly
flick.contentX = 0;
drag(250, [50, 50, 50, 50, 50, 50, 50, 50], 10);
settle();
check('the content stops at its end only after its range is used up or comes to rest', flick.contentX <= flick.$maxContentX, true);
flick.contentX = 1100;
drag(250, [50, 50, 50, 50, 50, 50, 50, 50], 10);
settle();
check('thrown at the end of the range: stops at the end exactly', flick.contentX, flick.$maxContentX);

// a press stops a flick that runs
drag(250, [20, 20, 20, 20, 20], 10);
clock += 100;
frame();
const during = flick.contentX;
scene.pointerDown(...at(100), 0);
check('a press stops the flick', [flick.flicking, flick.moving], [false, false]);
scene.pointerUp(...at(100));
clock += 200;
frame();
check('the content stays where the press stopped it', flick.contentX, during);

process.exit(failures === 0 ? 0 : 1);
