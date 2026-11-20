// Qt.locale() and the Locale formats of QML: Date.toLocaleString(Qt.locale(), Locale.ShortFormat) in a document and in a JavaScript
// library, a format text of Qt, the separators of numbers. A Date method without a Locale is the one of JavaScript.
// `node test/locale.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const FILES = {
	'main.qml': `
import QtQuick
import "Format.js" as Format

Item {
	width: 200
	height: 100

	property var time: new Date(2026, 10, 17, 9, 5, 7)
	property string shortText: time.toLocaleString(Qt.locale(), Locale.ShortFormat)
	property string shortDate: time.toLocaleDateString(Qt.locale(), Locale.ShortFormat)
	property string shortTime: time.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
	property string longDate: time.toLocaleDateString(Qt.locale())
	property string qtText: time.toLocaleDateString(Qt.locale(), "dd.MM.yyyy") + " " + time.toLocaleTimeString(Qt.locale(), "HH:mm")
	property string libraryText: Format.formatTime(time)
	property string germanDate: time.toLocaleDateString(Qt.locale("de_DE"), Locale.ShortFormat)
	property string germanPoint: Qt.locale("de_DE").decimalPoint
	property string localeName: Qt.locale("de_DE").name
}
`,
	'Format.js': `
.pragma library

function formatTime(time) {
	return time.toLocaleString(Qt.locale(), Locale.ShortFormat);
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

const time = new Date(2026, 10, 17, 9, 5, 7);
const tag = typeof navigator !== 'undefined' && navigator.language ? navigator.language : 'en-US';
const intl = (options, locale = tag) => new Intl.DateTimeFormat(locale, options).format(time);

check('Locale.ShortFormat: the short date and time of the locale', root.shortText, intl({ dateStyle: 'short', timeStyle: 'short' }));
check('the short date', root.shortDate, intl({ dateStyle: 'short' }));
check('the short time', root.shortTime, intl({ timeStyle: 'short' }));
check('without a format: LongFormat', root.longDate, intl({ dateStyle: 'full' }));
check('a format text of Qt', root.qtText, '17.11.2026 09:05');
check('in a JavaScript library', root.libraryText, root.shortText);
check('a named locale', root.germanDate, intl({ dateStyle: 'short' }, 'de-DE'));
check('its decimal point', root.germanPoint, ',');
check('its name as in Qt', root.localeName, 'de_DE');
check('a Date method without a Locale is the one of JavaScript', time.toLocaleDateString('en-US', { month: 'long' }), 'November');
check('no errors', engine.errors, []);

if (failures > 0) {
	console.log(`${failures} check(s) failed`);
	process.exit(1);
}
