// A script that still holds a destroyed item (ModalDialogManager.topItem after the tooltip is closed) tests `item.closeAnimFunc !== undefined`
// before calling it (CustomTooltip.hide). The functions of the destroyed item must read as undefined, not null: with null the test passes and
// the call throws, and the throw aborted the rebuild of the permission tree of Lisa. `node test/destroyed-item-function.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';
const FILES = { 'main.qml': `
import QtQuick
Item {
	id: root
	width: 100
	height: 100
	property Item top: null
	property var out: []
	Component { id: tip; Rectangle { function closeAnimFunc() { return 1 } } }
	function hide() {
		var item = root.top
		if (item && item.closeAnimFunc !== undefined) {
			item.closeAnimFunc()
			return "called"
		}
		return "skipped"
	}
	Timer {
		interval: 20
		running: true
		onTriggered: {
			root.top = tip.createObject(root)
			var res = [root.hide()]
			root.top.destroy()
			root.out = res
		}
	}
	Timer {
		interval: 200
		running: true
		onTriggered: root.out = root.out.concat([root.hide()])
	}
}` };
class MemoryFS { async readText(p) { return p in FILES ? FILES[p] : null; } async readBytes() { return null; } }
const engine = new Engine(); engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(100, 100, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml'); viewer.load(project);
let t = 0; for (let i = 0; i < 60; i++) { scene.drawFrame((t += 0.016)); await Promise.resolve(); }
const root = viewer.content.$children[0];
const same = JSON.stringify(root.out) === JSON.stringify(['called', 'skipped']);
console.log(`${same ? '[+]' : '[-]'} hide() of a live item calls it, of a destroyed one skips it: ${JSON.stringify(root.out)}`);
process.exit(same ? 0 : 1);
