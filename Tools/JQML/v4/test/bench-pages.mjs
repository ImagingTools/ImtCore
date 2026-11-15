// How long the pages of test/page-leak.mjs (the real controls of IotPlatform: the Table, the address tree...) take to open and to run:
// the walk is made several times, and the time of the frames of each page is added up. The first round builds the pages and fills the
// caches, the others are the ones that count. `JQML_NORESOLVE=1` runs the expressions through `with` as the engine did before.
//
//   node test/bench-pages.mjs [rounds]
import { existsSync } from 'node:fs';
import { readFile } from 'node:fs/promises';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const here = dirname(fileURLToPath(import.meta.url));
const imtCore = resolve(here, '..', '..', '..', '..');
const modules = [join(imtCore, 'Qml'), join(imtCore, '..', 'IotPlatform', 'Include', 'iotqml', 'Qml')];
if (!modules.every((dir) => existsSync(dir))) {
	console.log(`The pages need the folders ${modules.join(' and ')}: skipped`);
	process.exit(0);
}

const FILES = {
	'Main.qml': `
import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
	id: root
	width: 1200
	height: 800
	property alias loader: loader
	property string page: ""

	Component { id: tablePage; TablePage {} }
	Component { id: treePage; TreePage {} }
	Component { id: listPage; ListPage {} }
	Component { id: gridPage; GridPage {} }
	Component { id: repeaterPage; RepeaterPage {} }
	Component { id: flickPage; FlickPage {} }
	Component { id: mapPage; MapPage {} }
	Component { id: objectsPage; ObjectsPage {} }

	Loader {
		id: loader
		anchors.fill: parent
		sourceComponent: root.page === "TablePage.qml" ? tablePage : root.page === "TreePage.qml" ? treePage : root.page === "ListPage.qml" ? listPage : root.page === "GridPage.qml" ? gridPage : root.page === "RepeaterPage.qml" ? repeaterPage : root.page === "FlickPage.qml" ? flickPage : root.page === "MapPage.qml" ? mapPage : root.page === "ObjectsPage.qml" ? objectsPage : undefined
	}
}
`,
	'TablePage.qml': `
import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
	id: page
	TreeItemModel { id: headerModel }
	TreeItemModel { id: rowModel }
	Table {
		id: table
		anchors.fill: parent
		itemHeight: 40
		headerHeight: 40
		elements: rowModel
	}
	Component.onCompleted: {
		var headers = []
		var rows = []
		for (var c = 0; c < 12; c++) headers.push({ id: "c" + c, name: "Column " + c })
		for (var i = 0; i < 200; i++) {
			var row = { id: "r" + i }
			for (var c = 0; c < 12; c++) row["c" + c] = "text " + i + " " + c
			rows.push(row)
		}
		headerModel.createFromJson(JSON.stringify(headers))
		rowModel.createFromJson(JSON.stringify(rows))
		table.headers = headerModel
	}
}
`,
	'TreePage.qml': `
import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
	id: page
	TreeItemModel { id: reply }
	function fill(index) {
		var items = []
		for (var i = 0; i < 100; i++) items.push({ id: "h" + i, name: "House " + i, hasChildren__: false, parentIds: "" })
		reply.clear()
		reply.createFromJson(JSON.stringify(items))
		tree.insertTree(index, reply)
		reply.clear()
	}
	TreeViewGql {
		id: tree
		anchors.fill: parent
		hasSelection: true
		onRequestSignal: page.fill(index)
	}
	Component.onCompleted: {
		var streets = []
		for (var i = 0; i < 20; i++) streets.push({ id: "s" + i, name: "Street " + i, hasChildren__: true, parentIds: "" })
		reply.createFromJson(JSON.stringify(streets))
		tree.insertTree(-1, reply)
		reply.clear()
	}
}
`,
	'GridPage.qml': `
import QtQuick

Item {
	id: page
	ListModel { id: model }
	GridView {
		id: grid
		anchors.fill: parent
		cellWidth: 100
		cellHeight: 60
		model: model
		delegate: Rectangle {
			width: 90
			height: 50
			color: index % 2 ? "white" : "lightgray"
			Text { anchors.centerIn: parent; text: name + " " + index }
		}
	}
	Timer { interval: 50; running: true; repeat: true; onTriggered: model.append({ name: "n" }) }
	Component.onCompleted: { for (var i = 0; i < 300; i++) model.append({ name: "cell" }) }
}
`,
	'RepeaterPage.qml': `
import QtQuick

Item {
	id: page
	property int total: 5
	ListModel { id: model }
	Column {
		id: column
		Repeater {
			id: repeater
			model: model
			delegate: Rectangle {
				width: 200
				height: 20 + index
				color: index % 2 ? "white" : "lightgray"
				Text { text: name + page.total }
			}
		}
		Repeater {
			model: page.total
			Rectangle { width: 20; height: 10; color: "red" }
		}
	}
	Timer { interval: 50; running: true; repeat: true; onTriggered: { page.total = page.total + 1; model.append({ name: "n" }); if (model.count > 30) model.remove(0, 10) } }
	Component.onCompleted: { for (var i = 0; i < 20; i++) model.append({ name: "row" }) }
}
`,
	'FlickPage.qml': `
import QtQuick

Item {
	id: page
	property real phase: 0
	Flickable {
		id: flick
		anchors.fill: parent
		contentWidth: content.width
		contentHeight: content.height
		Column {
			id: content
			width: 400
			Repeater {
				model: 40
				Rectangle {
					width: 400
					height: 30
					color: index % 2 ? "white" : "lightgray"
					opacity: 0.5 + page.phase / 2
					Behavior on opacity { NumberAnimation { duration: 100 } }
					states: State { name: "on"; when: page.phase > 0.5; PropertyChanges { target: text; color: "red" } }
					Text { id: text; text: "item " + index }
				}
			}
		}
	}
	Connections { target: page; function onPhaseChanged() { flick.contentY = flick.contentY } }
	SequentialAnimation on phase { loops: Animation.Infinite; NumberAnimation { from: 0; to: 1; duration: 300 } NumberAnimation { from: 1; to: 0; duration: 300 } }
	Loader { sourceComponent: Component { Rectangle { width: 10; height: 10; color: page.phase > 0.5 ? "blue" : "green" } } }
}
`,
	'MapPage.qml': `
import QtQuick
import QtLocation
import QtPositioning

Item {
	id: page
	Map {
		id: map
		anchors.fill: parent
		center: QtPositioning.coordinate(55.75, 37.61)
		zoomLevel: 10
		MapQuickItem {
			coordinate: QtPositioning.coordinate(55.75, 37.61)
			sourceItem: Rectangle { width: 10; height: 10; color: page.width > 0 ? "red" : "blue" }
		}
	}
}
`,
	'ObjectsPage.qml': `
import QtQuick

Item {
	id: page
	property int seen: 0
	// The objects of a reply (SDL data): made by a script, never destroyed by it, and watched by a handler of a property.
	Component {
		id: reply
		QtObject {
			property string m_id: ""
			property string m_name: ""
			Component.onCompleted: { m_idChanged.connect(function() { page.seen++ }); m_nameChanged.connect(function() { page.seen++ }) }
		}
	}
	property var kept: []
	Timer {
		interval: 30; running: true; repeat: true
		onTriggered: { var o = reply.createObject(null); o.m_id = "x" + page.seen; o.m_name = "n"; page.kept = [o] }
	}
	Text { text: page.seen }
}
`,
	'ListPage.qml': `
import QtQuick
import Acf 1.0
import imtcontrols 1.0

Item {
	id: page
	ListModel { id: model }
	ListView {
		id: view
		anchors.fill: parent
		model: model
		delegate: Rectangle {
			width: 300
			height: 30
			color: index % 2 ? "white" : "lightgray"
			Text { anchors.centerIn: parent; text: name + " " + index }
		}
	}
	Timer { interval: 50; running: true; repeat: true; onTriggered: model.append({ name: "n" }) }
	Component.onCompleted: { for (var i = 0; i < 100; i++) model.append({ name: "row" }) }
}
`,
};

class DirFS {
	async readText(path) {
		if (path in FILES) return FILES[path];
		try {
			return await readFile(path, 'utf8');
		} catch {
			return null;
		}
	}

	async readBytes(path) {
		try {
			const data = await readFile(path);
			return data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength);
		} catch {
			return null;
		}
	}
}

const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(1200, 800, 1);
const project = new Project(new DirFS(), { importPaths: modules });
await project.load('Main.qml');
viewer.load(project);
let t = 0;
const frames = (n = 6) => {
	for (let i = 0; i < n; i++) {
		scene.root.$updateRequested = true;
		scene.drawFrame((t += 0.016));
	}
};
frames(10);
const root = viewer.content.$children[0];


// The time inside the compiled scripts and inside the evaluation of the bindings (a script, the write of its value, what that sets going).
const timing = { scripts: 0, calls: 0, evaluations: 0, evaluationMs: 0 };
const compile = engine.compile.bind(engine);
engine.compile = (kind, code, params, scope, self) => {
	const fn = compile(kind, code, params, scope, self);
	return function (...args) {
		const start = performance.now();
		try {
			return fn.apply(this, args);
		} finally {
			timing.scripts += performance.now() - start;
			timing.calls++;
		}
	};
};
const evaluateOne = engine.evaluateOne.bind(engine);
engine.evaluateOne = (binding, silent) => {
	const start = performance.now();
	try {
		return evaluateOne(binding, silent);
	} finally {
		timing.evaluationMs += performance.now() - start;
		timing.evaluations++;
	}
};
const spent = new Map();
async function visit(page) {
	const start = performance.now();
	root.page = page;
	for (let i = 0; i < 5; i++) {
		frames(6);
		await new Promise((resolve) => setTimeout(resolve, 0));
	}
	scene.wheel(600, 300, -3);
	frames(4);
	spent.set(page, (spent.get(page) || 0) + performance.now() - start);
}

const pages = ['TablePage.qml', 'TreePage.qml', 'ListPage.qml', 'GridPage.qml', 'RepeaterPage.qml', 'FlickPage.qml', 'MapPage.qml', 'ObjectsPage.qml'];
const rounds = Number(process.argv[2]) || 6;
const walks = [];
for (let round = 0; round < rounds; round++) {
	spent.clear();
	const start = performance.now();
	for (const page of pages) await visit(page);
	await visit('');
	walks.push({ total: performance.now() - start, pages: new Map(spent) });
}
const counted = walks.slice(1);
const median = (list) => [...list].sort((a, b) => a - b)[list.length >> 1];
console.log(`mode: ${engine.resolveNames ? 'resolved names' : 'with chain'}; expressions ${engine.resolveStats.resolved} resolved, ${engine.resolveStats.withChain} with`);
console.log(`first walk ${walks[0].total.toFixed(0)} ms; later walks (median of ${counted.length}) ${median(counted.map((w) => w.total)).toFixed(0)} ms`);
for (const page of pages) console.log(`  ${page.padEnd(18)} ${median(counted.map((w) => w.pages.get(page))).toFixed(0)} ms`);
console.log(`all walks: scripts ${timing.scripts.toFixed(0)} ms in ${timing.calls} calls (${((timing.scripts / timing.calls) * 1000).toFixed(1)} us each), evaluations ${timing.evaluationMs.toFixed(0)} ms in ${timing.evaluations}`);
console.log(`bindings ${engine.bindings.size}, errors ${engine.errors.length}`);
process.exit(0);
