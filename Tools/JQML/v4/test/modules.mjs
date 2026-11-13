// A module that several directories of the compiler config declare: what they declare together is the module, and what
// two of them declare comes from the last one (the application overrides a library, like in JQML v3).
// `node test/modules.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick
import Acf 1.0

Item {
	property string styleName: Style.name
	property string badgeName: badge.name
	property string commonName: common.name

	Badge { id: badge }
	Common { id: common }
}
`,
	// the library: imported first by the config
	'lib/qmldir': 'module Acf\nsingleton Style 1.0 Style.qml\nBadge 1.0 Badge.qml\nCommon 1.0 Common.qml\n',
	'lib/Style.qml': 'pragma Singleton\nimport QtQuick\nQtObject {\n\tproperty string name: "library"\n\tproperty string libraryOnly: "library only"\n}\n',
	'lib/Badge.qml': 'import QtQuick\nItem {\n\tproperty string name: "library badge"\n}\n',
	'lib/Common.qml': 'import QtQuick\nItem {\n\tproperty string name: "library common"\n}\n',
	// the application: declares its own Style and Badge for the same module
	'app/qmldir': 'module Acf\nsingleton Style 1.0 Style.qml\nBadge 1.0 Badge.qml\n',
	'app/Style.qml': 'pragma Singleton\nimport QtQuick\nQtObject {\n\tproperty string name: "application"\n}\n',
	'app/Badge.qml': 'import QtQuick\nItem {\n\tproperty string name: "application badge"\n}\n',
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

async function run(dirs) {
	const engine = new Engine();
	const scene = new Scene(new Renderer(null), engine);
	const viewer = new Viewer(scene, engine, () => {});
	scene.resize(200, 100, 1);
	const project = new Project(new MemoryFS(), { modules: { Acf: dirs } });
	await project.load('main.qml');
	viewer.load(project);
	for (let i = 0; i < 4; i++) scene.drawFrame(i * 0.016);
	const root = viewer.content.$children[0];
	return { engine, root };
}

// the config lists the library before the application
const app = await run(['lib', 'app']);
check('engine errors', app.engine.errors, []);
check('a singleton of the last directory wins', app.root.styleName, 'application');
check('a type of the last directory wins', app.root.badgeName, 'application badge');
check('a type that only an earlier directory declares stays', app.root.commonName, 'library common');

// the other order gives the other result
const swapped = await run(['app', 'lib']);
check('the order of the directories decides', [swapped.root.styleName, swapped.root.badgeName], ['library', 'library badge']);

// ----- the fonts of a Qt resource -----

// A FontLoader names `/Fonts/Name.ttf` or `qrc:/Fonts/Name.ttf`: the file is next to the Qml folder of the module.
const fontProject = new Project(new MemoryFS(), { modules: { Acf: ['lib/Qml/Acf'] } });
const slash = fontProject.fontFileCandidates('/Fonts/Ubuntu-Light.ttf');
check('a font of /Fonts is looked up next to the module', slash.includes('lib/Qml/Fonts/Ubuntu-Light.ttf'), true);
check('a font of /Fonts is looked up in Resources/Fonts of the library', slash.includes('lib/Resources/Fonts/Ubuntu-Light.ttf'), true);
check('qrc:/Fonts is looked up the same way', fontProject.fontFileCandidates('qrc:/Fonts/Ubuntu-Light.ttf').includes('lib/Qml/Fonts/Ubuntu-Light.ttf'), true);
check('another path is not rewritten', fontProject.fontFileCandidates('Other/x.ttf'), ['Other/x.ttf']);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
