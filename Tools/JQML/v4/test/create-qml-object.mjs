// Qt.createQmlObject(qml, parent[, url]) beyond what the comparison with desktop Qt in test/desktop/CreateQmlObject_* checks: the text
// of the errors, a type or a property that does not exist (an error of a strict engine, a log of a lenient one), the object that is
// not left behind after an error, a type of the directory of the url, the text that is made again, the destruction with the parent.
// `node test/create-qml-object.mjs`
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
	property int base: 3
}
`,
	'parts/Badge.qml': `
import QtQuick

Rectangle {
	property string label: "badge"
	width: 10
	height: 10
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

async function open(lenient) {
	const engine = new Engine();
	engine.lenient = lenient;
	const scene = new Scene(new Renderer(null), engine);
	const viewer = new Viewer(scene, engine, () => {});
	scene.resize(300, 200, 1);
	const project = new Project(new MemoryFS());
	await project.load('main.qml');
	// what an application loads up front: the files its QML names; this one is only named by the text of the test
	await project.addDoc('parts/Badge.qml', true);
	viewer.load(project);
	let t = 0;
	const frames = (n) => {
		for (let i = 0; i < n; i++) {
			scene.root.$updateRequested = true;
			scene.drawFrame((t += 0.016));
		}
	};
	frames(3);
	return { engine, project, root: viewer.content.$children[0], frames };
}

function attempt(call) {
	try {
		const result = call();
		return { result };
	} catch (error) {
		return { name: error.name, message: error.message };
	}
}

const { engine, root, frames } = await open(false);
const make = (text, parent = root, url) => (url === undefined ? engine.createQmlObject(text, parent) : engine.createQmlObject(text, parent, url));

// the text of the errors, one place per error
check('no parent', attempt(() => make('import QtQuick\nItem { }', null)).message, 'Qt.createQmlObject(): Missing parent object');
check('too few arguments', attempt(() => engine.createQmlObject('import QtQuick\nItem { }')).message, 'Qt.createQmlObject(): Insufficient arguments');
check('empty text gives null', make('   \n'), null);
const syntax = attempt(() => make('import QtQuick\nRectangle { width: }', root, 'file:///some/dyn.qml'));
check('a syntax error names the url, the line and the column', [syntax.name, /^Qt\.createQmlObject\(\): failed to create object: \n {4}file:\/\/\/some\/dyn\.qml:2:20: /.test(syntax.message)], ['Error', true]);
const unknownType = attempt(() => make('import QtQuick\nFoo { }'));
check('a type that does not exist', [unknownType.name, /inline:2:1: Unknown type 'Foo'/.test(unknownType.message)], ['Error', true]);
const before = root.$children.length;
const unknownProperty = attempt(() => make('import QtQuick\nItem {\n\tfoo: 3\n}'));
check('a property that does not exist', [unknownProperty.name, /inline:3:\d+: Cannot assign to non-existent property 'foo'/.test(unknownProperty.message) || unknownProperty.message], ['Error', true]);
frames(2);
check('no object stays after an error', root.$children.length, before);

// objects
const item = make('import QtQuick\nRectangle { width: root.base * 2; height: parent.height; property int n: base }');
check('the ids of the context of the parent', [item.width, item.height, item.$parent === root], [6, root.height, true]);
root.base = 5;
frames(2);
check('a binding follows them', item.width, 10);
const documents = engine.inlineDocs.size;
check('the same text again is another object, and not another document', [make('import QtQuick\nItem { objectName: "z" }') !== make('import QtQuick\nItem { objectName: "z" }'), engine.inlineDocs.size - documents], [true, 1]);
check('a runtime error of a binding is only logged', attempt(() => make('import QtQuick\nItem { property int a: nothere.x }')).name, undefined);

// a type of the directory of the url, and what the text imports itself
const badge = make('import QtQuick\nimport "parts"\nBadge { label: "x" }', root, 'main-inline.qml');
check('a type of an imported directory', [badge.label, badge.width], ['x', 10]);

// the object goes with its parent
const holder = make('import QtQuick\nItem { }');
const child = make('import QtQuick\nItem { }', holder);
holder.destroy();
frames(2);
check('it is destroyed with the parent', child.$destroyed, true);

// a lenient engine (the one of the applications) logs a type and a property that do not exist and still makes the object
const lenient = await open(true);
const loose = lenient.engine.createQmlObject('import QtQuick\nItem { foo: 3; Foo { } }', lenient.root);
check('lenient: the object is made', [loose.$typeName, lenient.root.$children.includes(loose)], ['Item', true]);

check('nothing else failed', engine.errors.filter((text) => !/nothere/.test(text)), []);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
