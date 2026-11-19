// Drag and DropArea (ReorderableListDelegate moves the rows of a list with them), as desktop Qt gives them for an item pulled by a MouseArea:
//   Drag.active: mouse.drag.active   the item sends its position to the areas under it
//   DropArea                         onEntered when the item comes over the area, containsDrag while it is over, onExited when it leaves,
//                                    onDropped when the item is released over it (drop.source is the item, Drag.drop() is done by the release)
//   Drag.keys / DropArea.keys        an area takes the items that share a key (an area with no keys takes all)
// `node test/drag-drop.mjs`
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
	height: 200
	property string log: ""
	property alias box: box
	property alias grab: grab
	property alias target: target
	property alias other: other

	Rectangle {
		id: box
		x: 10; y: 10; width: 40; height: 40
		color: "red"
		Drag.active: grab.drag.active
		Drag.keys: ["row"]
		Drag.hotSpot.x: 20
		Drag.hotSpot.y: 20
		MouseArea {
			id: grab
			anchors.fill: parent
			drag.target: box
		}
	}
	DropArea {
		id: target
		x: 150; y: 10; width: 100; height: 100
		keys: ["row"]
		onEntered: (drag) => root.log += "enter(" + (drag.source === box) + ") "
		onExited: root.log += "exit "
		onDropped: (drop) => root.log += "drop(" + (drop.source === box) + ") "
	}
	DropArea {
		id: other
		x: 150; y: 120; width: 100; height: 70
		keys: ["column"]
		onEntered: root.log += "WRONG "
		onDropped: root.log += "WRONG-DROP "
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
scene.resize(300, 200, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
let time = 0;
const frame = (n = 2) => {
	for (let i = 0; i < n; i++) scene.drawFrame((time += 0.016));
};
frame(4);
const root = viewer.content.$children[0];
const at = (item, dx, dy) => [item.$sx + dx, item.$sy + dy];
// a missing property is a failed check, not the end of the test
const safe = (fn) => {
	try {
		return fn();
	} catch {
		return undefined;
	}
};
const take = () => {
	const text = root.log.trim();
	root.log = '';
	return text;
};

check('Drag is attached to the item', root.box.Drag !== undefined, true);
check('inactive at first', safe(() => root.box.Drag.active), false);
check('the area does not contain a drag', root.target.containsDrag, false);

// pull the box over the area, then away, then back and drop it
const from = at(root.box, 20, 20);
scene.pointerDown(...from);
scene.pointerMove(from[0] + 20, from[1]);
scene.pointerMove(from[0] + 60, from[1]);
frame();
check('the drag is active while pulled', [safe(() => root.grab.drag.active), safe(() => root.box.Drag.active)], [true, true]);
check('the area is not reached yet', take(), '');
scene.pointerMove(from[0] + 160, from[1] + 10);
frame();
check('entered, with the source', take(), 'enter(true)');
check('containsDrag while over', root.target.containsDrag, true);
scene.pointerMove(from[0] + 170, from[1] + 20);
frame();
check('moving inside is not a new entry', take(), '');
scene.pointerMove(from[0] + 60, from[1]);
frame();
check('exited when it leaves', take(), 'exit');
check('containsDrag is false again', root.target.containsDrag, false);
scene.pointerMove(from[0] + 160, from[1] + 10);
frame();
take();
scene.pointerUp(from[0] + 160, from[1] + 10);
frame();
check('dropped on the release, with the source', take(), 'drop(true)');
check('the drag ends', [safe(() => root.grab.drag.active), safe(() => root.box.Drag.active)], [false, false]);
check('the item stays where it was dropped', root.box.x > 100, true);

// over the area that has another key: nothing
root.box.x = 10;
root.box.y = 10;
frame();
const again = at(root.box, 20, 20);
scene.pointerDown(...again);
scene.pointerMove(again[0] + 20, again[1] + 20);
scene.pointerMove(again[0] + 160, again[1] + 130);
frame();
scene.pointerUp(again[0] + 160, again[1] + 130);
frame();
check('an area with another key takes nothing', take().includes('WRONG'), false);

check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
