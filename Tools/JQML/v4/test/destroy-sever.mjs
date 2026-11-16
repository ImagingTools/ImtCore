// A destroyed item that a script variable still points at keeps nothing of its page alive. In Qt the reference reads as null and the
// object is freed; the engine cannot find the references, so the husk lets go of its parent, its children, its scope and the values of its
// properties. The page of a closed document tab held by the dialog manager of an application is the case (a `property var topItem`).
// `node test/destroy-sever.mjs`
import v8 from 'node:v8';
import vm from 'node:vm';
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

v8.setFlagsFromString('--expose-gc');
const collect = vm.runInNewContext('gc');

const FILES = {
	'main.qml': `
import QtQuick

Item {
	id: root
	width: 300
	height: 200
	property var keep: null
	property string seen: ""
	property alias loader: loader

	Loader {
		id: loader
		active: true
		sourceComponent: Component {
			Item {
				id: page
				property var payload: ({ name: "payload" })
				property string title: "page"
				Rectangle {
					id: first
					width: 10
					height: 10
					Rectangle { id: nested; width: 4; height: 4 }
				}
				Text { id: label; text: page.title }
				Item { id: tail }
				Item { id: tail2 }
				Item { id: tail3 }
				Component.onCompleted: root.keep = page
			}
		}
	}
	Item { id: sibling }
	Component.onCompleted: seen = "completed"
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
const loader = root.loader;

const page = root.keep;
check('the page was created and the root keeps it', page === null ? null : page.$typeName, 'Item');
// The weak references are made in a function of their own: a variable of this script would keep the last item it saw alive.
const track = (root) => {
	const refs = [];
	const walk = (item) => {
		refs.push(new WeakRef(item));
		for (const child of item.$children) walk(child);
	};
	// The items that were made last stay referenced by the engine until the next one is made: they are left out.
	for (const child of root.$children.slice(0, -3)) walk(child);
	refs.push(new WeakRef(root.payload));
	return refs;
};
const weak = track(page);
check('items below the page', weak.length >= 4, true);

// The script that destroys the page can still read it: the husk is emptied after it returns.
let readAfterDestroy = '';
loader.active = false;
readAfterDestroy = page.title;
frames(4);
check('the loader dropped the page', loader.$children.length, 0);
check('a script that destroys an item can still read it', readAfterDestroy, 'page');
check('the destroyed page is marked', page.$destroyed, true);

// Only `root.keep` points at the destroyed page now. Its own children and values must be free.
for (let i = 0; i < 12; i++) {
	collect();
	await new Promise((resolve) => setTimeout(resolve, 20));
}
check('children and values of a destroyed page are freed', weak.map((ref, i) => (ref.deref() === undefined ? null : i + ':' + (ref.deref().$typeName || 'object'))).filter((x) => x !== null), []);
check('the husk has no parent, children or scope', [page.$parent, page.$children.length, page.$scope], [null, 0, null]);
check('a property of the husk reads null', page.title, null);
check('nothing failed', engine.errors.length, 0);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
