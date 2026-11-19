// The way ReorderableListDelegate of ImtCore drags a row: a MouseArea whose `drag.target` is chosen by a binding (nothing until the button is down)
// and whose `drag.axis` is Drag.YAxis, an item with a bound `Drag.active`, `Drag.source` and a hot spot bound to its size, and a DropArea that
// is told when the row comes over it and leaves it. The row moves along the axis only, the area hears of it, and the press is no click.
// `node test/drag-reorder.mjs`
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
	height: 200
	property bool held: false
	property string log: ""
	property alias content: content
	property alias handle: da
	Rectangle {
		id: content
		x: 0; y: 0; width: 40; height: 40
		Drag.active: root.held
		Drag.source: da
		Drag.hotSpot.x: width / 2
		Drag.hotSpot.y: height / 2
	}
	MouseArea {
		id: da
		x: 0; y: 0; width: 40; height: 40
		drag.target: !root.held ? undefined : content
		drag.axis: Drag.YAxis
		drag.minimumY: 0
		drag.maximumY: 160
		onPressed: root.held = true
		onReleased: root.held = false
		onClicked: root.log += "click "
	}
	DropArea {
		x: 0; y: 100; width: 100; height: 50
		onEntered: root.log += "in "
		onExited: root.log += "out "
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
scene.resize(200, 200, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
let time = 0;
const frame = (n = 3) => {
	for (let i = 0; i < n; i++) scene.drawFrame((time += 0.016));
};
frame(5);
const root = viewer.content.$children[0];
const { content, handle } = root;
const take = () => {
	const text = root.log.trim();
	root.log = '';
	return text;
};

check('Drag.YAxis is the number of Qt', handle.drag.axis, 2);
check('the target is nothing before the press', handle.drag.target, null);
const from = [handle.$sx + 20, handle.$sy + 20];
scene.pointerDown(...from);
frame();
check('the press makes the row held and the target the row', [root.held, handle.drag.target === content], [true, true]);
check('Drag.active follows the binding', content.Drag.active, true);
check('Drag.source is the area', content.Drag.source === handle, true);
check('the drag has not started before the threshold', handle.drag.active, false);
scene.pointerMove(from[0] + 30, from[1] + 5);
scene.pointerMove(from[0] + 30, from[1] + 90);
frame();
check('the drag is active', handle.drag.active, true);
check('the row moved along the axis and not across it', [content.x, content.y], [0, 90]);
check('the DropArea heard of the row coming over it', take(), 'in');
scene.pointerMove(from[0] + 30, from[1] + 400);
frame();
check('the maximum of the axis holds', content.y, 160);
check('the row left the area', take(), 'out');
scene.pointerMove(from[0] + 30, from[1] + 100);
frame();
take();
scene.pointerUp(from[0] + 30, from[1] + 100);
frame();
check('released: the drag is over', [root.held, handle.drag.active, content.Drag.active], [false, false, false]);
check('and the DropArea is told that the row left', take(), 'out');
check('a press that dragged is no click', root.log.includes('click'), false);

// a press without a drag is a click, and the target stays nothing
scene.pointerDown(...from);
scene.pointerUp(...from);
frame();
check('a press with no move is a click', root.log.includes('click'), true);
check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
