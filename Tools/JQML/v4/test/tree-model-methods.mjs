// The methods of the C++ TreeItemModel (imtbase::CTreeItemModel) that our QML calls and the engine has not got:
//   setUpdateEnabled(bool)  CollectionStructureBase: while it is false the model does not tell the views about changes (m_isUpdateEnabled)
//   getParent()             TreeViewCommandsDelegateBase: the model that holds this one as the value of a key (null for the top one)
//   insertNewItemWithParameters(index, map)  BasicTableView: a new row at `index` with the keys of the object; out of range is ignored
// `node test/tree-model-methods.mjs`
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
	width: 100
	height: 100
	property alias tree: tree
	property alias lonely: lonely
	property int changes: 0

	TreeItemModel {
		id: tree
		onModelChanged: root.changes++
	}
	TreeItemModel { id: lonely }
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
function attempt(name, fn) {
	try {
		return fn();
	} catch (error) {
		failures++;
		console.log(`[-] ${name}: threw ${error.message}`);
		return undefined;
	}
}

const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(100, 100, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
const frame = (n = 4) => {
	for (let i = 0; i < n; i++) scene.drawFrame(i * 0.016);
};
frame();
const root = viewer.content.$children[0];
const tree = root.tree;

// getParent
check('getParent exists', typeof tree.getParent, 'function');
check('the top model has no parent', attempt('getParent top', () => tree.getParent()), null);
const child = tree.addTreeModel('Child', 0);
check('a model made with addTreeModel knows its parent', attempt('getParent child', () => child.getParent()) === tree, true);
const grandChild = child.addTreeModel('Leaf', 0);
check('and the parent of the parent', attempt('getParent grandchild', () => grandChild.getParent().getParent()) === tree, true);
const extern = root.lonely;
tree.setExternTreeModel('Extern', extern, 0);
check('a model set as a value takes the model as its parent', attempt('getParent extern', () => extern.getParent()) === tree, true);

// setUpdateEnabled
check('setUpdateEnabled exists', typeof tree.setUpdateEnabled, 'function');
frame();
let before = root.changes;
attempt('enable', () => tree.setUpdateEnabled(true));
tree.setData('a', 1, 0);
frame();
check('changes are told while the updates are enabled', root.changes > before, true);
attempt('disable', () => tree.setUpdateEnabled(false));
frame();
before = root.changes;
tree.setData('a', 2, 0);
tree.setData('b', 3, 0);
frame();
check('nothing is told while they are disabled', root.changes, before);
check('the data is still written', [tree.getData('a', 0), tree.getData('b', 0)], [2, 3]);
attempt('enable again', () => tree.setUpdateEnabled(true));
tree.setData('a', 4, 0);
frame();
check('and told again after they are enabled', root.changes > before, true);
check('no change was lost in the value', tree.getData('a', 0), 4);

// insertNewItemWithParameters
const rows = root.lonely;
check('insertNewItemWithParameters exists', typeof rows.insertNewItemWithParameters, 'function');
rows.setData('Name', 'first', 0);
check('one row', rows.getItemsCount(), 1);
attempt('insert at 0', () => rows.insertNewItemWithParameters(0, { Name: 'zero', Level: 2, Opened: true }));
check('a row was added', rows.getItemsCount(), 2);
check('at the index', rows.getData('Name', 0), 'zero');
check('with every key of the object', [rows.getData('Level', 0), rows.getData('Opened', 0)], [2, true]);
check('the old row moved down', rows.getData('Name', 1), 'first');
attempt('insert at the end', () => rows.insertNewItemWithParameters(2, { Name: 'last' }));
check('the index after the last row is allowed', [rows.getItemsCount(), rows.getData('Name', 2)], [3, 'last']);
attempt('insert out of range', () => rows.insertNewItemWithParameters(9, { Name: 'far' }));
attempt('insert negative', () => rows.insertNewItemWithParameters(-1, { Name: 'neg' }));
check('an index out of range is ignored', rows.getItemsCount(), 3);
const nested = tree.addTreeModel('Nested', 1);
attempt('insert a model as a value', () => rows.insertNewItemWithParameters(0, { Name: 'holder', Sub: nested }));
check('a model in the object becomes a child model', attempt('sub', () => rows.getTreeItemModel('Sub', 0)) === nested, true);
check('and its parent is the model that holds it', attempt('sub parent', () => nested.getParent()) === rows, true);

check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
