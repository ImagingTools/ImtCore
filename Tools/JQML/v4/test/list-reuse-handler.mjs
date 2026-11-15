// A script write replaces the binding of the property it writes, so the rebind of a reused delegate does not run that binding again over
// the written value: a row that opens a card from `onOpenChanged` lost the card when its delegate came back from the pool.
// `node test/list-reuse-handler.mjs`
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
	property int selected: 0
	property alias list: view

	Component { id: emptyComp; Item { property int kind: 0 } }
	Component { id: cardComp; Item { property int kind: 1 } }

	ListModel { id: rows }
	Component.onCompleted: {
		for (let i = 0; i < 300; i++) rows.append({ n: i })
	}

	ListView {
		id: view
		anchors.fill: parent
		model: rows
		reuseItems: true
		cacheBuffer: 0
		delegate: Item {
			id: row
			width: 300
			height: 30 + inner.height
			property bool open: model.index === root.selected
			onOpenChanged: {
				if (!row.open) loader.sourceComponent = emptyComp
				else loader.sourceComponent = cardComp
			}
			Item {
				id: inner
				y: 30
				height: row.open ? 100 : 0
				width: 300
				Loader {
					id: loader
					anchors.fill: parent
					sourceComponent: emptyComp
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
scene.resize(300, 300, 1);
const project = new Project(new MemoryFS(), {});
await project.load('main.qml');
viewer.load(project);
let t = 0;
const frames = (n = 4) => {
	for (let i = 0; i < n; i++) {
		scene.root.$updateRequested = true;
		scene.drawFrame(t++ * 0.016);
	}
};
frames(8);
const root = viewer.content.$children[0];
const list = root.list;

const kindOfRow = (index) => {
	const row = list.$delegates.get(index);
	if (row === undefined) return 'no row';
	const find = (item) => {
		if (item.$typeName === 'Loader') return item;
		for (const child of item.$children) {
			const found = find(child);
			if (found !== null) return found;
		}
		return null;
	};
	const loader = find(row);
	return loader === null || loader.item === null ? 'no card' : loader.item.kind;
};

check('the open row shows its card', kindOfRow(0), 1);
let lost = 0;
for (const far of [1500, 4000, 7000]) {
	for (const middle of [0, 1, 3]) {
		list.contentY = 0;
		frames(4);
		list.contentY = far;
		frames(middle);
		list.contentY = 0;
		frames(6);
		if (kindOfRow(0) !== 1) lost++;
	}
}
check('and keeps it after the delegate went to the pool and came back', lost, 0);

// The rebind runs the bindings that are marked (the ones a row's `index` or a full pass marks): the one of the loader is among them.
const row = list.$delegates.get(0);
const find = (item) => {
	if (item.$typeName === 'Loader') return item;
	for (const child of item.$children) {
		const found = find(child);
		if (found !== null) return found;
	}
	return null;
};
const loader = find(row);
check('the loader of the open row shows the card', loader.item.kind, 1);
for (const binding of loader.$bindings) engine.dirty.add(binding);
engine.refreshDelegateBindings(row);
frames(4);
check('a binding the script replaced does not run again over the written value', loader.item.kind, 1);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
