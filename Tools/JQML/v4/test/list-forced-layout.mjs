// A script that changes the model of a list and reads a size after each change (the address tree closes a branch row by row) makes the scene
// lay out for each read. The list is arranged for the changed model in the next frame, not in each of these layouts, and it is right then.
// `node test/list-forced-layout.mjs`
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
	property alias list: view
	property alias rows: model
	property real read: 0

	ListModel { id: model }

	ListView {
		id: view
		width: 300
		height: 300
		reuseItems: true
		model: model
		delegate: Rectangle {
			width: 300
			height: 30
			property int shown: index
			property string label: name
		}
	}

	Rectangle { id: probe; width: 40; height: 20 }

	Component.onCompleted: {
		for (var i = 0; i < 200; i++) model.append({ name: "row " + i })
	}

	// Removes rows one by one and reads a size after each, as a loop of a control does.
	function removeRows(from, count) {
		for (var i = 0; i < count; i++) {
			model.remove(from)
			probe.width = probe.width + 1
			root.read = probe.width
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
let t = 0;
const frames = (n = 6) => {
	for (let i = 0; i < n; i++) {
		scene.root.$updateRequested = true;
		scene.drawFrame(t++ * 0.016);
	}
};
frames(20);
const root = viewer.content.$children[0];
const list = root.list;

let arranged = 0;
const arrange = list.$arrangeBody.bind(list);
list.$arrangeBody = () => {
	arranged++;
	return arrange();
};

root.removeRows(3, 100);
check('the scene was laid out for each read, the list was arranged hardly at all', arranged <= 3, true);

frames(10);
check('the next frame arranged it', arranged >= 1, true);
check('the model has what is left', list.count, 100);
const rows = [...list.$delegates].sort((a, b) => a[0] - b[0]);
check('each delegate shows its own row', rows.every(([index, item]) => item.shown === index), true);
check('and its text', rows.every(([index, item]) => item.label === (index < 3 ? `row ${index}` : `row ${index + 100}`)), true);

root.removeRows(0, 100);
frames(10);
check('a model that was emptied has no delegates', [list.count, list.$delegates.size], [0, 0]);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
