// `copyItemDataToModel` of TreeItemModel, as in the C++ model: the next batch of a ComboBoxGql (the address search of IotPlatform) adds a row
// with insertNewItem() and fills it with copyItemDataToModel(). Without the method the row stayed empty, the popup took it for a separator
// and gave it the height 5, so the end of the list showed a gap.
// `node test/tree-copy-item-data.mjs`
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
	property alias view: list
	property alias target: target
	property alias batch: batch
	property var results: []

	TreeItemModel { id: batch }
	TreeItemModel { id: target }
	TreeItemModel { id: nested }

	ListView {
		id: list
		width: 300
		height: 200
		model: target
		delegate: Rectangle {
			width: 300
			height: model.id === "" ? 5 : 40
			Text { text: model.name }
		}
	}

	function load() {
		for (var i = 0; i < batch.getItemsCount(); i++) {
			var index = target.insertNewItem()
			results.push(batch.copyItemDataToModel(i, target, index))
		}
	}

	Component.onCompleted: {
		nested.setData("x", 1, 0)
		batch.setData("id", "a", 0)
		batch.setData("name", "Alpha", 0)
		batch.setData("children", nested, 0)
		batch.setData("id", "b", 1)
		batch.setData("name", "Beta", 1)
		batch.insertNewItem()
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
frames(4);
const root = viewer.content.$children[0];

root.load();
frames(4);
check('every row is copied (the last one has no keys: false, as in C++)', root.results, [true, true, false]);
check('the rows of the target', [root.target.getItemsCount(), root.target.getData('id', 0), root.target.getData('name', 1)], [3, 'a', 'Beta']);
check('a nested model is shared', root.target.getData('children', 0) === root.target.getData('children', 0) && root.target.isTreeModel('children', 0), true);
check('the rows of the list have the height of a row with an id', [...root.view.$delegates.entries()].sort((a, b) => a[0] - b[0]).slice(0, 2).map(([, d]) => d.height), [40, 40]);
check('a row of another model is not touched', root.batch.getData('name', 0), 'Alpha');
check('a row that does not exist', root.batch.copyItemDataToModel(9, root.target, 0), false);
check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
