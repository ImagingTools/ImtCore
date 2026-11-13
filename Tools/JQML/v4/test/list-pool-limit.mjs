// The pool of a list or a grid holds no more delegates than the view and its cache buffer can build: a jump still reuses all of them, and
// a model that got smaller, or a view that shrank, lets the others go.
// `node test/list-pool-limit.mjs`
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
	property alias list: view
	property alias grid: cells
	property int made: 0
	property int cellsMade: 0
	property int rows: 500

	ListView {
		id: view
		width: 200
		height: 300
		cacheBuffer: 100
		reuseItems: true
		model: root.rows
		delegate: Rectangle {
			width: 200
			height: 50
			Component.onCompleted: root.made++
		}
	}

	GridView {
		id: cells
		x: 200
		width: 200
		height: 300
		cellWidth: 100
		cellHeight: 50
		cacheBuffer: 100
		reuseItems: true
		model: root.rows
		delegate: Rectangle {
			width: 100
			height: 50
			Component.onCompleted: root.cellsMade++
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
let t = 0;
const frames = (n = 6) => {
	for (let i = 0; i < n; i++) {
		scene.root.$updateRequested = true;
		scene.drawFrame(t++ * 0.016);
	}
};
frames(40);
const root = viewer.content.$children[0];
const { list, grid } = root;

// 300 px of view and 100 px of buffer on both sides: 500 / 50 = 10 rows, and 4 more for the rows that are half in
check('the list can build 14 delegates', list.$poolLimit(), 14);
check('the grid can build (500 / 50 + 2) rows of 2 cells', grid.$poolLimit(), 24);

const made = [root.made, root.cellsMade];
for (const y of [5000, 12000, 0, 8000, 20000, 3000, 0]) {
	list.contentY = y;
	grid.contentY = Math.min(y, grid.$maxContentY);
	frames(40);
}
check('jumps are served by the pool: the delegates are not made again', [root.made - made[0] <= 14, root.cellsMade - made[1] <= 24], [true, true]);
check('and the pool stays within the limit', [list.$pool.length <= 14, grid.$pool.length <= 24], [true, true]);

list.contentY = 5000;
frames(40);
const built = list.$delegates.size;
root.rows = 3;
frames(10);
check('a model that got smaller lets the pooled delegates go', list.$pool.length + list.$delegates.size <= 14 && list.$delegates.size === 3, true);
check('the delegates that it needs are kept', built > 3 && list.$delegates.size === 3, true);

list.height = 100;
frames(10);
check('a view that shrank holds a smaller pool', list.$pool.length <= list.$poolLimit(), true);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
