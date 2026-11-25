// A click on a row of a ListView that is not active gives the row the active focus (forceActiveFocus in onClicked), so the arrow keys go
// to the Keys.onPressed of the row (the Table of the ControlsGallery). `node test/list-row-focus-click.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';
const FILES = { 'main.qml': `
import QtQuick
Item {
	id: root
	width: 300
	height: 300
	property int selected: -1
	property var log: []
	ListView {
		id: list
		anchors.fill: parent
		model: 10
		delegate: Item {
			id: row
			width: 300
			height: 30
			Keys.onPressed: {
				root.log = root.log.concat([index + ":" + event.key])
				if (event.key === Qt.Key_Down) { root.selected = index + 1; event.accepted = true }
			}
			MouseArea {
				anchors.fill: parent
				onClicked: {
					root.selected = index
					row.forceActiveFocus()
					mouse.accepted = false
				}
			}
		}
	}
}` };
class MemoryFS { async readText(p) { return p in FILES ? FILES[p] : null; } async readBytes() { return null; } }
const engine = new Engine(); engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(300, 300, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml'); viewer.load(project);
let t = 0; const frame = () => { for (let i = 0; i < 4; i++) scene.drawFrame((t += 0.016)); };
frame();
const root = viewer.content.$children[0];
const px = root.$sx + 50;
const py = root.$sy + 45;
scene.pointerMove(px, py);
scene.pointerDown(px, py, 0);
scene.pointerUp(px, py);
frame();
let failures = 0;
const check = (name, actual, expected) => {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
};
check('click selects the row', root.selected, 1);
check('Down goes to the row that the click focused', scene.keyDown({ key: 'ArrowDown', code: 'ArrowDown' }), true);
frame();
check('the handler of the row moved the selection', [root.selected, root.log], [2, ['1:' + 0x01000015]]);
process.exit(failures === 0 ? 0 : 1);
