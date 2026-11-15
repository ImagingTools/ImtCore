// Actual SegmentedButton decorators created while a Loader binding is being evaluated.
// node test/segmented-button.mjs
import assert from 'node:assert/strict';
import { existsSync } from 'node:fs';
import { readFile } from 'node:fs/promises';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { Engine } from '../src/engine.js';
import { Item } from '../src/items.js';
import { Project } from '../src/project.js';

const core = fileURLToPath(new URL('../../../../', import.meta.url));
const source = `
	import QtQuick 2.12
	import imtcontrols 1.0
	Item {
	Loader {
		sourceComponent: Component {
			SegmentedButton {
				height: 32
				isExclusive: true
				firstSegmentDecorator: Component { ExternalSegmentDecorator {} }
				middleSegmentDecorator: Component { ButtonDecorator {} }
				lastSegmentdecorator: Component { ExternalSegmentDecorator { isLast: true } }
				Button { text: "Info"; iconSource: "info.svg" }
				Button { text: "Warning"; iconSource: "warning.svg" }
				Button { text: "Error"; iconSource: "error.svg" }
			}
		}
	}
	}
`;
const project = new Project({
	async readText(path) {
		if (path === 'Main.qml') return source;
		if (path === 'test/Acf/qmldir') return 'module Acf\nsingleton Style 1.0 Style.qml';
		if (path === 'test/Acf/Style.qml') return 'pragma Singleton\nimport imtcontrols 1.0\nStyleBase {}';
		const file = join(core, path);
		return existsSync(file) ? readFile(file, 'utf8') : null;
	},
	async readBytes() { return null; },
}, {
	modules: {
		imtcontrols: ['Qml/imtcontrols/Menu/Internal', 'Qml/imtcontrols/Popup/Internal', 'Qml/imtcontrols'],
		Acf: ['test/Acf'],
	},
});
await project.load('Main.qml');
const engine = new Engine();
engine.lenient = true;
const host = new Item();
// The Loader has no size of its own (a Loader that fills a host gives its size to the item it loads, as in Qt).
const loader = engine.load(project, host).$children[0];
const settle = () => {
	for (let n = 0; n < 8; n++) {
		host.$layout();
		engine.evaluateBindings();
	}
	assert.deepEqual(engine.errors, []);
};
settle();
loader.active = false;
engine.addBinding(loader, ['active'], () => true, 0);
settle();
const segmented = loader.item;
const buttons = segmented.rowAlias.children;
assert.equal(buttons.length, 3);
for (const button of buttons) {
	assert.ok(button.width > 36, `${button.text} must fit its icon, text and margins`);
	assert.equal(button.width, button.decorator_.decoratorWidth, button.text);
}
assert.equal(segmented.width, buttons.reduce((sum, button) => sum + button.width, 0));
buttons[0].text = 'Longer information';
settle();
assert.equal(buttons[0].width, buttons[0].decorator_.decoratorWidth);
buttons[0].mouseArea.$mouseClicked();
buttons[2].mouseArea.$mouseClicked();
assert.deepEqual(buttons.map(button => button.checked), [false, false, true]);
assert.equal(segmented.selectedIndex, 2);
buttons[0].width = 300;
settle();
assert.equal(buttons[0].width, 300);
assert.equal(buttons[0].decorator_.width, 300);
console.log('ok   SegmentedButton content sizing and exclusive selection after Loader activation');
