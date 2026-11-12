// The header of a table is a horizontal ListView whose cells are 0 wide until the table has measured its columns (TableHeaderDelegate
// sets `width = headerWidth / columnCount`, which is 0 at first, and sets it again on `widthRecalc`). A cell of no size in front of the
// view must keep its delegate: it was released, its size was remembered as 0 and never asked again, so the header of the roles table drew only
// its last column. In Qt the cell is there and moves the next one when it gets its width.
// `node test/list-zero-width.mjs`
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
	height: 100
	property alias list: list
	property bool measured: false
	signal widthRecalc()

	ListView {
		id: list
		width: 600
		height: 40
		orientation: ListView.Horizontal
		reuseItems: true
		cacheBuffer: 600
		model: 3
		delegate: Item {
			id: cell
			height: 40
			property int columnIndex: index
			function setCellWidth() {
				cell.width = root.measured ? 200 : 0
				cell.visible = cell.width > 0
			}
			Component.onCompleted: {
				root.widthRecalc.connect(cell.setCellWidth)
				cell.setCellWidth()
			}
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
scene.resize(600, 100, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
let t = 0;
const frames = (n) => {
	for (let i = 0; i < n; i++) {
		scene.root.$updateRequested = true;
		scene.drawFrame((t += 0.016));
	}
};
frames(6);
const root = viewer.content.$children[0];
const cells = () => [...root.list.$delegates.entries()].sort((a, b) => a[0] - b[0]).map(([index, item]) => [index, Math.round(item.x), Math.round(item.width)]);

// The columns are measured after the cells were built.
root.measured = true;
root.widthRecalc();
frames(8);
check('every column has its cell once the widths are known', cells(), [[0, 0, 200], [1, 200, 200], [2, 400, 200]]);
check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
