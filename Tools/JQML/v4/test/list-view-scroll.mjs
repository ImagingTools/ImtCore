// What desktop Qt 6.11 does with the scroll position of a ListView and a GridView (measured with QtTest, test/qt-measure/listview-scroll):
// positionViewAtIndex (Beginning, Center, End, Visible, Contain), the drag with the pointer (also over a MouseArea that fills each row: the view
// takes the drag, the area gets `canceled`) and the flick after the release. `node test/list-view-scroll.mjs`
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
	width: 600
	height: 400
	property string log: ""
	property alias list: list
	property alias rows: rows
	property alias grid: grid
	function contain(index) { list.positionViewAtIndex(index, ListView.Contain) }
	ListView {
		id: list
		x: 0; y: 0; width: 200; height: 200
		model: 20
		delegate: Rectangle { width: 200; height: 50; color: "white" }
		onMovementStarted: root.log += "MS "
		onMovementEnded: root.log += "ME "
		onFlickStarted: root.log += "FS "
		onFlickEnded: root.log += "FE "
	}
	ListView {
		id: rows
		x: 220; y: 0; width: 200; height: 200
		model: 20
		delegate: Rectangle {
			width: 200; height: 50; color: "white"
			MouseArea {
				anchors.fill: parent
				onPressed: root.log += "P" + index + " "
				onReleased: root.log += "R" + index + " "
				onClicked: root.log += "C" + index + " "
				onCanceled: root.log += "X" + index + " "
			}
		}
		onMovementStarted: root.log += "MS "
		onMovementEnded: root.log += "ME "
	}
	GridView {
		id: grid
		x: 440; y: 0; width: 150; height: 200
		cellWidth: 50; cellHeight: 50
		model: 60
		delegate: Rectangle { width: 48; height: 48; color: "gray" }
		onMovementStarted: root.log += "MS "
		onMovementEnded: root.log += "ME "
		onFlickStarted: root.log += "FS "
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

let clock = 1000;
performance.now = () => clock;
const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(600, 400, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
const frame = () => scene.drawFrame(clock / 1000);
frame();
frame();
const root = viewer.content.$children[0];
const { list, rows, grid } = root;
const ox = root.$sx;
const oy = root.$sy;

// positionViewAtIndex: rows of 50 in a view of 200, the content is 1000 high
const Beginning = 0;
const Center = 1;
const End = 2;
const Visible = 3;
const Contain = 4;
const place = (index, mode) => {
	list.positionViewAtIndex(index, mode);
	return list.contentY;
};
list.contentY = 0;
check('Contain 15 from the top: the row at the bottom', place(15, Contain), 600);
check('Contain 14: it shows already', place(14, Contain), 600);
check('Contain 2: the row at the top', place(2, Contain), 100);
check('Beginning 3', place(3, Beginning), 150);
check('Center 10', place(10, Center), 425);
check('End 10', place(10, End), 350);
check('Visible 1 (not shown)', place(1, Visible), 50);
check('Visible 19 (not shown), kept inside the content', place(19, Visible), 800);
check('Beginning 19 is kept inside the content', place(19, Beginning), 800);
list.contentY = 0;
root.contain(12);
check('ListView.Contain written in QML (an enum of the engine is a name)', list.contentY, 450);
list.positionViewAtBeginning();
check('positionViewAtBeginning', list.contentY, 0);
list.positionViewAtEnd();
check('positionViewAtEnd', list.contentY, 800);
list.contentY = 0;

function drag(view, x, y, steps, delay, idle = 0) {
	const at = (px, py) => [ox + view.x + px, oy + view.y + py];
	scene.pointerMove(...at(x, y));
	scene.pointerDown(...at(x, y), 0);
	let py = y;
	for (const dy of steps) {
		clock += delay;
		py -= dy;
		scene.pointerMove(...at(x, py));
		frame();
	}
	clock += idle;
	scene.pointerUp(...at(x, py));
}
const settle = () => {
	for (let i = 0; i < 400 && engine.flicks.size > 0; i++) {
		clock += 16;
		frame();
	}
};

// 6 moves of 20 px every 100 ms: the drag starts at the second, the view follows by 80; the release throws it by 13 (Qt 93)
root.log = '';
drag(list, 100, 150, [20, 20, 20, 20, 20, 20], 100);
check('list drag: following the pointer', list.contentY, 80);
settle();
check('list drag: thrown a little after the release (Qt 93)', Math.round(list.contentY), 93);
check('list drag: signals', root.log, 'MS FS FE ME ');

// over a MouseArea per row: the view takes the drag, the area is canceled and gets no release and no click
root.log = '';
drag(rows, 100, 125, [20, 20, 20, 20, 20, 20], 100);
check('rows drag: the area is pressed, then canceled, no release, no click', root.log, 'P2 MS X2 ');
check('rows drag: the view followed', rows.contentY, 80);
settle();
check('rows drag: thrown a little after the release (Qt 93)', Math.round(rows.contentY), 93);

// a click that does not move is the click of the area
root.log = '';
rows.contentY = 0;
frame();
frame();
drag(rows, 100, 25, [], 10);
check('rows click', root.log, 'P0 R0 C0 ');

// a grid
root.log = '';
drag(grid, 75, 150, [20, 20, 20, 20, 20, 20], 100);
check('grid drag: following the pointer', grid.contentY, 80);
settle();
check('grid drag: thrown a little after the release (Qt 93)', Math.round(grid.contentY), 93);
grid.positionViewAtIndex(40, Contain);
check('grid Contain 40: the row 13 (cells of 50, 3 columns) at the bottom', grid.contentY, 500);

// a quick flick on the list runs to the end of the content
list.contentY = 0;
drag(list, 100, 190, [25, 25, 25, 25, 25, 25], 10);
settle();
check('flick: runs to the end of the content', list.contentY, 800);

process.exit(failures === 0 ? 0 : 1);
