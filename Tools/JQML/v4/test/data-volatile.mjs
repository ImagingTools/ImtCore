// A binding that reads the rows of a plain array or of a nested model (made anew on every read) cannot be told that they
// changed, so it was run on every pass of every frame: a hidden grid of 200 cards cost 15 ms of each frame of IotPlatform, and
// the same for the cells of a table with 400 rows. It runs again when a model was changed by a call of its own, as such a call
// moves `engine.dataEpoch`.
// `node test/data-volatile.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick
import com.imtcore.imtqml 1.0

Item {
	id: root
	width: 200
	height: 100
	property alias tree: treeModel
	property alias probe: label
	property alias rowsView: view
	property var plain: [1, 2, 3]

	TreeItemModel {
		id: treeModel
		Component.onCompleted: setData("items", [{ a: 1 }], 0)
	}

	Text {
		id: label
		objectName: "probe"
		text: treeModel.getTreeItemModel("items").getData("a", 0)
	}

	ListView {
		id: view
		width: 100
		height: 100
		model: root.plain
		delegate: Text {
			objectName: "cell"
			property int rowCount: view.count
			text: modelData + "/" + rowCount
		}
	}

	Text {
		id: layoutText
		objectName: "layoutProbe"
		text: "x" + label.contentWidth
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
const frame = () => {
	scene.root.$updateRequested = true;
	scene.drawFrame(t++ * 0.016);
};
for (let i = 0; i < 6; i++) frame();
const root = viewer.content.$children[0];

const bindingOf = (name, key) => [...engine.bindings].find((b) => b.owner.objectName === name && b.key === key);
const probe = bindingOf('probe', 'text');
const cell = bindingOf('cell', 'rowCount');
const layoutProbe = bindingOf('layoutProbe', 'text');

check('the probe reads a row of a nested model', root.probe.text, '1');
check('the probe is a binding that cannot follow its rows', [probe.volatile, probe.dataOnly], [true, true]);
check('the count of a view is one of them', [cell.volatile, cell.dataOnly], [true, true]);
check('a content size goes with the layout', [layoutProbe.volatile, layoutProbe.dataOnly], [true, false]);

const before = [probe.runs, cell.runs, layoutProbe.runs];
for (let i = 0; i < 20; i++) frame();
check('frames without a change do not run the bindings of the rows again', [probe.runs, cell.runs], [before[0], before[1]]);
check('the bindings of the layout run on every frame', layoutProbe.runs > before[2], true);

// A nested model changed through another object that stands for it.
root.tree.getTreeItemModel('items').setData('a', 5, 0);
for (let i = 0; i < 3; i++) frame();
check('a model that was changed by a call of its own moves the bindings of its rows', root.probe.text, '5');

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
