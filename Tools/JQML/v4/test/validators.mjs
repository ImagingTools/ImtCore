// IntValidator and DoubleValidator of an input: their ranges are set directly and through an alias (the bottom and top of a field
// of numbers), and acceptableInput follows them.
// `node test/validators.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick

Item {
	width: 200
	height: 100

	property alias intInput: intInput
	property alias doubleInput: doubleInput
	property alias bottomValue: intValidator.bottom
	property alias topValue: intValidator.top

	bottomValue: 1
	topValue: 10

	TextInput {
		id: intInput
		validator: IntValidator { id: intValidator }
	}

	TextInput {
		id: doubleInput
		validator: DoubleValidator {
			bottom: 0
			top: 100
			decimals: 2
			locale: "C"
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
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
scene.drawFrame(0.016);
const root = viewer.content.$children[0];

const accepts = (input, text) => {
	input.text = text;
	return input.acceptableInput;
};

check('the alias reaches the bottom of the IntValidator', root.bottomValue, 1);
check('an integer in the range', accepts(root.intInput, '7'), true);
check('an integer above the top', accepts(root.intInput, '11'), false);
check('an integer below the bottom', accepts(root.intInput, '0'), false);
check('not an integer', accepts(root.intInput, '7.5'), false);
root.topValue = 20;
check('a new top through the alias', accepts(root.intInput, '11'), true);

check('a number in the range', accepts(root.doubleInput, '12.34'), true);
check('too many decimals', accepts(root.doubleInput, '12.345'), false);
check('above the top', accepts(root.doubleInput, '100.5'), false);
check('not a number', accepts(root.doubleInput, 'abc'), false);
check('no errors', engine.errors, []);

if (failures > 0) {
	console.log(`${failures} check(s) failed`);
	process.exit(1);
}
