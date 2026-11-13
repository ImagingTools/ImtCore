// The rows of the view are built at once, the rows of the cache buffer a few in a frame (Qt builds them in the background): a jump of the
// scroll bar of a table with a cache buffer cost all the rows of the buffer in one frame, and IotPlatform stalled for most of a second.
// `node test/list-cache-buffer.mjs`
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
	height: 400
	property alias list: view

	ListView {
		id: view
		width: 300
		height: 400
		cacheBuffer: 4000
		reuseItems: true
		model: 300
		delegate: Item {
			width: 300
			height: 40
			Repeater {
				model: 20
				Rectangle {
					x: index % 300
					width: 4
					height: 4
					color: index % 2 === 0 ? "red" : "blue"
				}
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
scene.resize(300, 400, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
let t = 0;
const frame = () => scene.drawFrame(t++ * 0.016);
const root = viewer.content.$children[0];
const list = root.list;
const FULL_TOP = 111;
const FULL_JUMP = 211;
const keys = () => [...list.$delegates.keys()].sort((a, b) => a - b);
const contiguous = () => {
	const k = keys();
	return k.length > 0 && k[k.length - 1] - k[0] + 1 === k.length;
};

frame();
const firstFrame = list.$delegates.size;
check('the rows of the view are built at once', keys().slice(0, 10), [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]);
check('the rows of the buffer are not all built in the first frame', firstFrame < FULL_TOP, true);

let frames = 1;
while (list.$delegates.size < FULL_TOP && frames < 400) {
	frame();
	frames++;
	if (!contiguous()) break;
}
check('the buffer is filled in the next frames', [list.$delegates.size, contiguous()], [FULL_TOP, true]);
check('and it took more than one frame', frames > 1, true);

// A jump of the scroll bar: the rows that are seen are there in the same frame, the rest follow.
list.contentY = 6000;
frame();
const seen = keys();
const lo = Math.floor(6000 / 40);
check('after a jump the rows of the view are built in the same frame', [seen[0] <= lo, seen[seen.length - 1] >= lo + 9], [true, true]);
check('and the rows are one run, without gaps', contiguous(), true);
check('and the buffer of the new place is not built yet', list.$delegates.size < FULL_JUMP, true);
for (let i = 0; i < 400 && list.$delegates.size < FULL_JUMP; i++) frame();
check('the buffer of the new place is filled later', [list.$delegates.size, contiguous()], [FULL_JUMP, true]);
check('the first row is above the view by the buffer', keys()[0], lo - 100);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
