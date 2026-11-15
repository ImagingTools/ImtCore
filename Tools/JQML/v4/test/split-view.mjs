// The actual SplitView must initialize its model before its completion handler continues.
// node test/split-view.mjs
import assert from 'node:assert/strict';
import { existsSync } from 'node:fs';
import { readFile } from 'node:fs/promises';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { Engine } from '../src/engine.js';
import { Item } from '../src/items.js';
import { Project } from '../src/project.js';

const core = fileURLToPath(new URL('../../../../', import.meta.url));
for (const incremental of [true, false]) {
	for (const orientation of ['Horizontal', 'Vertical']) {
		const source = `
			import QtQuick 2.12
			import imtcontrols 1.0
			Loader {
				active: false
				sourceComponent: Component {
					SplitView {
						width: 792
						height: 192
						orientation: Qt.${orientation}
						Rectangle { width: 200; height: 50 }
						Rectangle { width: 200; height: 50 }
						Rectangle { width: 200; height: 50 }
						Component.onCompleted: console.log("loaded", ready, sizeModel.getItemsCount(), height, width)
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
		engine.incremental = incremental;
		const logs = [];
		engine.onLog = (line) => logs.push(line);
		const host = new Item();
		host.width = 792;
		host.height = 192;
		const loader = engine.load(project, host);
		engine.addBinding(loader, ['active'], () => true, 0);
		engine.evaluateBindings();
		assert.deepEqual(engine.errors, []);
		assert.equal(loader.item.ready, true);
		assert.equal(loader.item.sizeModel.getItemsCount(), 3);
		assert.deepEqual(logs.filter((line) => line.startsWith('loaded ')), ['loaded true 3 192 792']);
		for (let n = 0; n < 8; n++) {
			host.$layout();
			engine.evaluateBindings();
		}
		assert.deepEqual(engine.errors, []);
		assert.equal(loader.item.sizeModel.getItemsCount(), 3);
		assert.equal(logs.filter((line) => line.startsWith('loaded ')).length, 1, 'settling must not repeat initialization');
		assert.equal(loader.item.children.length, 1);
		assert.equal(loader.item.children[0].children.length, 5);
		console.log(`ok   SplitView ${orientation}, incremental=${incremental}: model filled before completion`);
	}
}
