// A GridView builds the delegates of the rows of the grid that the view and the cache buffer reach, not one for every row of the model:
// a page of 500 cards builds as many as the view shows. Delegates that left the range are pooled with `reuseItems`.
// `node test/grid-virtualize.mjs`
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
	property alias grid: view
	property int created: 0
	property int model: 500

	GridView {
		id: view
		width: 200
		height: 300
		cellWidth: 100
		cellHeight: 50
		cacheBuffer: 0
		reuseItems: true
		model: root.model
		delegate: Rectangle {
			width: 100
			height: 50
			property int shown: index
			Component.onCompleted: root.created++
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
frames(8);
const root = viewer.content.$children[0];
const grid = root.grid;
const built = () => [...grid.$delegates.keys()].sort((a, b) => a - b);

check('the model has 500 rows', grid.count, 500);
check('the view shows 6 rows of 2 cells and the row under it touches the view: 14 delegates, not 500', built().length, 14);
check('from the first cell', [built()[0], built()[13]], [0, 13]);
check('a cell sits in its row and column', [grid.itemAtIndex(3).x, grid.itemAtIndex(3).y], [100, 50]);

const first = root.created;
grid.contentY = 5000;
frames(8);
check('scrolled, the delegates follow the rows in the view and the rows that touch it', [built()[0], built()[built().length - 1], built().length], [198, 213, 16]);
check('and a cell is placed relative to the view', [grid.itemAtIndex(200).x, grid.itemAtIndex(200).y], [0, 0]);
check('and they are the delegates of before, rebound, but the two that were short', root.created - first, 2);

const created = root.created;
grid.contentY = 0;
frames(8);
check('back at the top it is the same again', root.created, created);
check('the first row shows its own cells again', [built()[0], grid.itemAtIndex(1).shown], [0, 1]);

grid.cacheBuffer = 100;
frames(30);
check('a cache buffer adds the rows around the view, a few in a frame', built().length, 18);

root.model = 40;
frames(8);
check('a smaller model builds what it has', built().length, 18);
root.model = 6;
frames(8);
check('a model that fits the view builds all its cells', built().length, 6);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
