// A property of a `pragma Singleton` file that is set after an item has read it (`Style.baseColor` when the theme is applied,
// `Style.logo` when the server tells the logo): the binding of the item reads it again.
// `node test/singleton-binding.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick
import styles 1.0

Item {
	id: root
	property alias panel: panelRect
	property alias icon: iconItem

	Rectangle {
		id: panelRect
		color: Style.baseColor
	}

	Item {
		id: iconItem
		property string source: Style.logo
		property int size: Style.size
	}

	function applyTheme() {
		Style.baseColor = "#f1f2f7"
		Style.logoIconName = "logo.svg"
		Style.size = 20
	}
}
`,
	'lib/qmldir': 'module styles\nsingleton Style 1.0 Style.qml\n',
	'lib/Style.qml': `pragma Singleton
import QtQuick
QtObject {
	property string baseColor: "#ffffff"
	property string logoIconName
	property string logo: !logoIconName ? "" : "/Light/" + logoIconName
	property int size: 10
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
const project = new Project(new MemoryFS(), { modules: { styles: ['lib'] } });
await project.load('main.qml');
viewer.load(project);
for (let i = 0; i < 4; i++) scene.drawFrame(i * 0.016);
const root = viewer.content.$children[0];

check('the colour before the theme', String(root.panel.color).toLowerCase(), '#ffffffff');
check('the source before the logo is known', root.icon.source, '');

root.applyTheme();
for (let i = 0; i < 4; i++) scene.drawFrame((i + 4) * 0.016);

check('the colour follows the singleton', String(root.panel.color).toLowerCase(), '#fff1f2f7');
check('the source follows the singleton', root.icon.source, '/Light/logo.svg');
check('a number follows the singleton', root.icon.size, 20);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
