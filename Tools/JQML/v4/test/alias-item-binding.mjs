// `viewer.height: 40` on a use of a component that aliases `viewer` to one of its items replaces the `height` binding that
// item has (`height: parent.height`), as in Qt: when the root of the component grows, the item keeps the height it was given
// (TableRowDelegate of ImtCore: IotPlatform opens a card under a row by growing the row, the viewer of the row stays one row high).
// `node test/alias-item-binding.mjs`
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
	property int extra: 0
	property int rowHeight: 40
	property alias bound: boundRow
	property alias fixed: fixedRow
	property alias plain: plainRow

	Panel {
		id: boundRow
		viewer.height: root.rowHeight
		height: 40 + root.extra
	}

	Panel {
		id: fixedRow
		viewer.height: 30
		height: 40 + root.extra
	}

	Panel {
		id: plainRow
		height: 40 + root.extra
	}
}
`,
	'Panel.qml': `
import QtQuick

Item {
	property alias viewer: view

	Item {
		id: view
		height: parent.height
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
for (let i = 0; i < 4; i++) scene.drawFrame(i * 0.016);
const root = viewer.content.$children[0];

check('a bound height replaces the binding of the item', root.bound.viewer.height, 40);
check('a constant height replaces it as well', root.fixed.viewer.height, 30);
check('without an override the item follows its parent', root.plain.viewer.height, 40);

root.extra = 490;
for (let i = 0; i < 4; i++) scene.drawFrame((i + 4) * 0.016);

check('the bound height stays when the root grows', root.bound.viewer.height, 40);
check('the constant height stays when the root grows', root.fixed.viewer.height, 30);
check('without an override the item still follows its parent', root.plain.viewer.height, 530);

root.rowHeight = 50;
for (let i = 0; i < 4; i++) scene.drawFrame((i + 8) * 0.016);

check('the bound height follows its own input', root.bound.viewer.height, 50);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
