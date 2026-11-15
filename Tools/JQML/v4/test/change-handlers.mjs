// A handler of a property that was declared in QML runs when the property is assigned, so it is not polled again in every frame: the
// handlers of a table (some 9000 of them) were read in every frame. A handler of a property that is not assigned by the script (a
// size that the layout gives, an alias) is still polled.
// `node test/change-handlers.mjs`
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
	property alias holder: box
	property int count: 0
	property int viaBinding: root.count * 2
	property int log: 0
	property int sizeLog: 0
	property string trace: ""

	onCountChanged: root.trace += "c" + root.count + ","
	onViaBindingChanged: root.trace += "b" + root.viaBinding + ","

	Item {
		id: box
		width: 50
		height: 50
		property alias inner: root.count
		onInnerChanged: root.trace += "i" + root.count + ","
		onWidthChanged: root.sizeLog = root.sizeLog + 1
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
	for (let i = 0; i < n; i++) {
		scene.root.$updateRequested = true;
		scene.drawFrame(t++ * 0.016);
	}
};
frames(6);
const root = viewer.content.$children[0];

const polled = new Map();
const poll = engine.runChangeHandler.bind(engine);
engine.runChangeHandler = (handler) => {
	polled.set(handler.name, (polled.get(handler.name) || 0) + 1);
	return poll(handler);
};
polled.clear();
frames(10);
check('a handler of a declared property is not polled', [polled.get('onCountChanged'), polled.get('onViaBindingChanged')], [undefined, undefined]);
check('a handler of an alias is still polled, one of a size only when the layout moved it', [polled.get('onInnerChanged') > 0, polled.get('onWidthChanged')], [true, undefined]);

root.trace = '';
root.count = 3;
check('a script assignment runs the handlers at once', root.trace.includes('c3,'), true);
frames(4);
check('and so does the binding that depends on it', root.trace.includes('b6,'), true);
check('and the handler of the alias', root.trace.includes('i3,'), true);
check('each one ran once', root.trace.split(',').filter(Boolean).sort(), ['b6', 'c3', 'i3']);

root.holder.width = 80;
frames(4);
check('a change of a size runs its handler', root.sizeLog >= 1, true);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
