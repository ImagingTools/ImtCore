// A delegate that is reused for another row runs again only the bindings that read the row (`model`, `modelData`, a role, `index`):
// a delegate of a table has some 900 bindings, and rebinding it for a jump of the scroll bar ran them all.
// `node test/list-rebind.mjs`
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
	height: 100
	property alias list: view
	property string suffix: "!"

	ListModel {
		id: rows
		Component.onCompleted: {
			for (var i = 0; i < 60; i++) append({ title: "row" + i, size: i })
		}
	}

	ListView {
		id: view
		width: 200
		height: 100
		reuseItems: true
		model: rows
		delegate: Item {
			id: cell
			width: 200
			height: 20
			property string fromRole: title + root.suffix
			property string fromModel: "m" + model.size
			property string fromIndex: "i" + index
			property string fromOutside: "o" + root.suffix
			property string viaFunction: describe()
			function describe() { return "f" + size }
			Text {
				objectName: "label"
				text: cell.fromRole + cell.fromModel + cell.fromIndex + cell.fromOutside + cell.viaFunction
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
scene.resize(200, 100, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
let t = 0;
const frames = (n = 4) => {
	for (let i = 0; i < n; i++) scene.drawFrame(t++ * 0.016);
};
frames(6);
const root = viewer.content.$children[0];
const view = root.list;
const labels = () => [...view.$delegates.entries()].sort((a, b) => a[0] - b[0]).map(([i, d]) => [i, d.$children.find((c) => c.objectName === 'label').text]);
const expected = (i) => `row${i}!m${i}i${i}o!f${i}`;
const wrong = () => labels().filter(([i, text]) => text !== expected(i));

check('the rows show their data', wrong(), []);

const bindingOf = (delegate, key) => [...engine.bindings].find((b) => b.owner === delegate && b.key === key);
const probe = view.$delegates.get(0);
const runs = () => ['fromRole', 'fromModel', 'fromIndex', 'fromOutside', 'viaFunction'].map((key) => bindingOf(probe, key).runs);
const before = runs();

// Scroll far enough for the delegates to be reused for other rows.
for (const y of [400, 800, 200, 1000, 0]) {
	view.contentY = y;
	frames(12);
	check(`after contentY ${y} every row shows its own data`, wrong(), []);
}

const after = runs();
check('the bindings that read a role, the model or the index ran again', [after[0] > before[0], after[1] > before[1], after[2] > before[2], after[4] > before[4]], [true, true, true, true]);
check('the binding that does not read the row did not run for the reuse', after[3], before[3]);

// A change of what an unrelated binding reads still moves it.
root.suffix = "?";
frames(4);
check('and it follows what it reads', labels().filter(([i, text]) => text !== `row${i}?m${i}i${i}o?f${i}`), []);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
