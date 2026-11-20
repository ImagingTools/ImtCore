// qsTr(text, disambiguation, n) as in Qt: `%n` of the text is the count, also without a translation; a plural message of a catalog
// takes the form of the count in its language. qsTranslate and qsTrId take the count too.
// `node test/plural-translations.mjs`
import { parseTsCatalog } from '../compiler/translations.js';
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

	property int count: 3
	property string counted: qsTr("Shared by me: %n item(s)", "", count)
	property string unchanged: qsTr("%n item(s) without a count")
	property string translated: qsTranslate("Files", "%n file(s)", "", count)
	property string byId: qsTrId("%n folder(s)", count)
}
`,
};

const CATALOG = `<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
	<name>Files</name>
	<message numerus="yes">
		<source>%n file(s)</source>
		<translation>
			<numerusform>%n файл</numerusform>
			<numerusform>%n файла</numerusform>
			<numerusform>%n файлов</numerusform>
		</translation>
	</message>
	<message>
		<source>Name</source>
		<translation>Имя</translation>
	</message>
</context>
</TS>
`;

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

const catalog = parseTsCatalog(CATALOG);
check('a plural message of a catalog keeps all its forms', catalog['%n file(s)'], ['%n файл', '%n файла', '%n файлов']);
check('a message without a count is a text', catalog['Name'], 'Имя');

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

check('without a translation %n is the count', root.counted, 'Shared by me: 3 item(s)');
check('without a count %n stays', root.unchanged, '%n item(s) without a count');
check('qsTranslate takes the count', root.translated, '3 file(s)');
check('qsTrId takes the count', root.byId, '3 folder(s)');
root.count = 0;
check('a change of the count changes the text', root.counted, 'Shared by me: 0 item(s)');

engine.translations = { ru_RU: catalog };
engine.language = 'ru_RU';
const forms = [1, 2, 5, 11, 21, 22, 25, 101, 112].map((n) => engine.translate('%n file(s)', n));
check('the form of the count in the language', forms, ['1 файл', '2 файла', '5 файлов', '11 файлов', '21 файл', '22 файла', '25 файлов', '101 файл', '112 файлов']);
check('a translation in a binding takes the form', root.translated, '0 файлов');
check('%Ln is the count in the system locale', engine.translate('%Ln of %n', 1234), `${(1234).toLocaleString(typeof navigator !== 'undefined' && navigator.language ? navigator.language : 'en-US')} of 1234`);
check('without a count a plural message takes its first form', engine.translate('%n file(s)'), '%n файл');

engine.translations = { de: { '%n file(s)': ['%n Datei', '%n Dateien'] } };
engine.language = 'de_DE';
check('two forms of German', [1, 2, 0].map((n) => engine.translate('%n file(s)', n)), ['1 Datei', '2 Dateien', '0 Dateien']);
check('no errors', engine.errors, []);

if (failures > 0) {
	console.log(`${failures} check(s) failed`);
	process.exit(1);
}
