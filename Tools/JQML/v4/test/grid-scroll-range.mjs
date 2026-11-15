// A GridView scrolls over the rows of its grid: a calendar of 42 cells in 6 rows that fits its view does not scroll.
// `node test/grid-scroll-range.mjs`
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
	width: 700
	height: 400
	property alias fits: gridFits
	property alias tall: gridTall

	ListModel { id: days; Component.onCompleted: { for (let i = 0; i < 42; i++) days.append({ d: i }) } }

	GridView {
		id: gridFits
		width: 700
		height: 342
		cellWidth: 100
		cellHeight: 57
		model: days
		delegate: Rectangle { width: 100; height: 57 }
	}
	GridView {
		id: gridTall
		y: 0
		width: 700
		height: 150
		cellWidth: 100
		cellHeight: 57
		model: days
		delegate: Rectangle { width: 100; height: 57 }
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
scene.resize(700, 400, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
for (let i = 0; i < 6; i++) {
	scene.root.$updateRequested = true;
	scene.drawFrame(i * 0.016);
}
const root = viewer.content.$children[0];

check('the content height is the rows of the grid', [root.fits.contentHeight, root.tall.contentHeight], [342, 342]);
root.fits.$mouseWheel(-3);
check('a grid that fits is not scrolled by the wheel', root.fits.contentY, 0);
root.tall.$mouseWheel(-30);
check('a taller one stops at the end of its rows', root.tall.contentY, 342 - 150);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
