// The header of a table is a horizontal ListView with `reuseItems: true`, its delegates take their width from the model of the
// widths of the columns when they are built (and TableHeaderDelegate when its column index changes). A list that scrolls must
// give a reused delegate the new index, and its scroll range is the content width that the table assigned: the length measured
// from the delegates keeps the sizes of delegates that are not built, out of date after a column was resized.
// `node test/list-reuse-width.mjs`
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
	height: 100
	property alias header: headerList
	property alias rows: rowList
	property var widths: [100, 140, 60, 200, 100, 140, 60, 200, 100, 140, 60, 200]
	property int total: 1480

	ListView {
		id: headerList
		width: 300
		height: 40
		orientation: ListView.Horizontal
		reuseItems: true
		model: 12
		// The table assigns the sum of the widths of its columns.
		contentWidth: root.total
		delegate: Item {
			id: cell
			property int columnIndex: index
			height: 40
			function applyWidth() {
				cell.width = root.widths[cell.columnIndex]
			}
			Component.onCompleted: applyWidth()
			onColumnIndexChanged: applyWidth()
		}
	}

	// The header of a table: the width is set once, but the cache buffer keeps every column built, so no delegate changes its column.
	ListView {
		id: bufferedList
		y: 100
		width: 300
		height: 40
		orientation: ListView.Horizontal
		reuseItems: true
		cacheBuffer: root.total
		model: 12
		delegate: Item {
			id: bufferedCell
			property int columnIndex: index
			height: 40
			Component.onCompleted: bufferedCell.width = root.widths[bufferedCell.columnIndex]
		}
	}
	property alias buffered: bufferedList
	ListView {
		id: tall
		y: 150
		width: 100
		height: 60
		cacheBuffer: 100
		model: 100
		delegate: Item { width: 100; height: 20 }
	}
	property alias tallList: tall
	ListView {
		id: fixedHeight
		y: 220
		width: 100
		height: 60
		contentHeight: 500
		model: 3
		delegate: Item { width: 100; height: 20 }
	}
	property alias fixedList: fixedHeight

	// A vertical list that scrolls sideways by the content width that was assigned.
	ListView {
		id: rowList
		y: 50
		width: 300
		height: 50
		model: 5
		contentWidth: 900
		delegate: Item {
			width: 300
			height: 20
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
scene.resize(400, 100, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
let t = 0;
const frames = (n = 4) => {
	for (let i = 0; i < n; i++) scene.drawFrame(t++ * 0.016);
};
frames();
const root = viewer.content.$children[0];
const header = root.header;

const wrong = () => {
	const bad = [];
	for (const [index, delegate] of header.$delegates) {
		if (Math.abs(delegate.$aw - root.widths[index]) > 0.01) bad.push([index, delegate.$aw, root.widths[index]]);
	}
	return bad;
};

check('the delegates are built with the width of their column', wrong(), []);

// Scroll along the columns and back: the pool hands its delegates to other columns.
for (const x of [150, 420, 700, 900, 1100, 300, 0, 1180, 500]) {
	header.contentX = x;
	frames();
	check(`after contentX ${x} every delegate has the width of its own column`, wrong(), []);
}

check('the scroll range of the header is the content width that was assigned', header.$maxContentX, root.total - header.width);
root.total = 1600;
frames();
check('and follows it', header.$maxContentX, 1600 - header.width);
header.contentX = 99999;
frames();
check('the header can be scrolled to the end', header.contentX, 1600 - header.width);

const buffered = root.buffered;
const staleBuffered = () => {
	const bad = [];
	for (const [index, delegate] of buffered.$delegates) {
		if (Math.abs(delegate.$aw - root.widths[index]) > 0.01) bad.push([index, delegate.$aw, root.widths[index]]);
	}
	return bad;
};
check('a cache buffer keeps all the columns built', buffered.$delegates.size, 12);
for (const x of [150, 700, 1100, 0, 1180, 300]) {
	buffered.contentX = x;
	frames();
	check(`with a cache buffer the widths stay right at contentX ${x}`, [staleBuffered(), buffered.$delegates.size], [[], 12]);
}

const tall = root.tallList;
check('a vertical cache buffer builds rows 0..8 (60 + 100 px)', [...tall.$delegates.keys()].sort((a, b) => a - b), [0, 1, 2, 3, 4, 5, 6, 7, 8]);
tall.contentY = 400;
frames();
check('and rows on both sides of the view', [...tall.$delegates.keys()].sort((a, b) => a - b), [15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28]);

check('the scroll range of a vertical list is its assigned content height', root.fixedList.$maxContentY, 500 - 60);
root.fixedList.contentY = 99999;
frames();
check('and it can be scrolled to the end of it', root.fixedList.contentY, 440);

check('a vertical list is scrolled sideways by its assigned content width', root.rows.$maxContentX, 900 - 300);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
