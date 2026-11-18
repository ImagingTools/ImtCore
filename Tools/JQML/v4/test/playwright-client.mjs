// The client for Playwright (playwright/qml.mjs) against a page whose `evaluate` runs in this process on window.qmlTest: chained
// locators, a search from a found item, waiting for an item that appears later, and the messages of the failures.
// Real Playwright is not needed. `node test/playwright-client.mjs`
import { Engine } from '../src/engine.js';
import { Flickable, ListView } from '../src/items.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { createTestApi } from '../src/testapi.js';
import { Viewer } from '../src/viewer.js';
import { qml } from '../playwright/qml.mjs';

const FILES = {
	'main.qml': `
import QtQuick

Item {
	id: root
	width: 400
	height: 300
	property string clicked: ""
	property bool shown: false

	Rectangle {
		objectName: "form"
		width: 200; height: 100
		Text { objectName: "caption"; text: "First" }
		MouseArea { objectName: "ok"; y: 30; width: 80; height: 30; onClicked: root.clicked = root.clicked + "first;" }
	}
	Rectangle {
		objectName: "form"
		x: 200; width: 200; height: 100
		Text { objectName: "caption"; text: "Second" }
		MouseArea { objectName: "ok"; y: 30; width: 80; height: 30; onClicked: root.clicked = root.clicked + "second;" }
	}
	Timer { interval: 100; running: true; onTriggered: root.shown = true }
	Loader {
		active: root.shown
		sourceComponent: Component { Text { objectName: "late"; y: 150; text: "arrived" } }
	}
	TextInput { objectName: "edit"; y: 200; width: 100; height: 20 }
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
async function failsWith(name, fn, part) {
	let message = '';
	try {
		await fn();
	} catch (error) {
		message = String(error.message);
	}
	const ok = message.includes(part);
	if (!ok) failures++;
	console.log(`${ok ? '[+]' : '[-]'} ${name}${ok ? '' : `: got '${message}', expected a message with '${part}'`}`);
}

const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(400, 400, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);

// A frame every 16 ms, like requestAnimationFrame.
let t = 0;
const timer = setInterval(() => {
	scene.root.$updateRequested = true;
	scene.drawFrame((t += 0.016));
}, 16);
const api = createTestApi({ engine, scene, classes: { Flickable, ListView } });
api.markStarted();
globalThis.window = { qmlTest: api };
const page = {
	evaluate: async (fn, arg) => JSON.parse(JSON.stringify((await fn(arg)) ?? null)),
	waitForFunction: async (fn) => {
		while (!fn()) await new Promise((resolve) => setTimeout(resolve, 10));
	},
};
const root = viewer.content.$children[0];

const app = qml(page, { timeout: 1500 });
await app.ready();

check('the same name in two forms is two items', await app.locator('caption').count(), 2);
await failsWith('a locator that finds two items does not guess', () => app.locator('ok').click(), 'found 2 items');
check('the first one', await app.locator('ok').first().isVisible(), true);

const second = app.locator('form').nth(1);
await second.locator('ok').click();
check('a click from the second form goes to its button', root.clicked, 'second;');
await app.locator('form').first().locator('ok').click();
check('and from the first form to its own', root.clicked, 'second;first;');
check('the text of a nested item', await second.locator('caption').text(), 'Second');
check('a locator is resolved again at every action', await second.locator('caption').get('text'), 'Second');
check('a locator has a readable name', String(second.locator('ok')), 'form [1] >> ok');

// an item that appears later
await app.locator('late').waitForText('arrived');
check('and then it is', await app.locator('late').text(), 'arrived');
await app.locator('late').waitFor({ state: 'visible' });

await failsWith('a name that is never there', () => app.locator('missing').click({ timeout: 300 }), "the item does not exist (timeout 300 ms)");
await failsWith('a wrong text', () => app.locator('late').waitForText('other', { timeout: 300 }), 'the last value was "arrived"');

// the keyboard
const edit = app.locator('edit');
await edit.fill('abc');
check('fill', await edit.get('text'), 'abc');
await edit.type('d');
check('type', await edit.get('text'), 'abcd');
await edit.press('Backspace');
check('press', await edit.get('text'), 'abc');

check('the dump of a form', (await second.dump()).includes('#ok'), true);
check('nothing failed', engine.errors.length, 0);
clearInterval(timer);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
