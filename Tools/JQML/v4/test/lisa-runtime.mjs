// Runtime regressions: mouse input, image transitions, object lifetime, bindings, models and CORS.
// node test/lisa-runtime.mjs
import assert from 'node:assert/strict';
import { Engine } from '../src/engine.js';
import { ImageItem, Item, ListModel, modelScopeData, MouseArea, QtObject, RegularExpressionValidator, TreeItemModel } from '../src/items.js';
import { Project } from '../src/project.js';
import { Scene } from '../src/scene.js';
import { attachCanvas } from '../src/host.js';

async function load(source, files = {}) {
	const engine = new Engine();
	engine.lenient = true;
	const project = new Project({
		async readText(path) { return path === 'Main.qml' ? source : files[path] ?? null; },
		async readBytes() { return null; },
	});
	await project.load('Main.qml');
	const host = new Item();
	const root = engine.load(project, host);
	const settle = () => {
		for (let i = 0; i < 5; i++) {
			host.$layout();
			engine.evaluateBindings();
		}
		assert.deepEqual(engine.errors, []);
	};
	settle();
	return { engine, root, settle };
}

const checks = {
	async 'Loader completion inside a binding pass updates dependent properties before the next statement'() {
		for (const incremental of [true, false]) {
			const { root, engine, settle } = await load(`
				import QtQuick
				Item {
					id: root
					property var events: []
					property bool showPage: true
					property int input: 1
					property int overridden: root.input * 2
					property alias pageLoader: loader
					Loader {
						id: loader
						active: false
						sourceComponent: Component {
							Item {
								id: page
								width: 792
								height: 192
								property bool compl: false
								property bool ready: page.compl && page.height > 0 && page.width > 0
								property int count: page.ready ? 3 : 0
								onReadyChanged: if (page.ready) root.events.push("fill")
								Component.onCompleted: {
									page.compl = true
									root.overridden = 77
									root.input = 3
									root.events.push("completed:" + page.ready + ":" + page.count)
								}
							}
						}
					}
				}
			`);
			engine.incremental = incremental;
			engine.addBinding(root.pageLoader, ['active'], () => root.showPage, 0);
			engine.evaluateBindings();
			assert.deepEqual(root.events, ['fill', 'completed:true:3'], `incremental=${incremental}`);
			assert.equal(root.pageLoader.item.ready, true);
			assert.equal(root.overridden, 77, 'completion assignments must replace bindings');
			settle();
			assert.deepEqual(root.events, ['fill', 'completed:true:3']);
			assert.equal(root.overridden, 77);
			root.showPage = false;
			settle();
			assert.equal(root.pageLoader.item, null, 'the outer Loader binding must remain active');
		}
	},

	async 'Flickable content item uses viewport width when contentWidth is omitted'() {
		const { root, settle } = await load(`
			import QtQuick
			Item {
				width: 400
				height: 300
				Flickable {
					id: flickable
					width: 200
					height: 100
					contentHeight: 500
					Item {
						id: centered
						width: 100
						height: 20
						anchors.horizontalCenter: parent.horizontalCenter
					}
				}
			}
		`);
		const flickable = root.$children[0];
		const centered = flickable.contentItem.$children[0];

		assert.equal(flickable.contentItem.width, 200);
		assert.equal(centered.x, 50);

		flickable.width = 300;
		settle();
		assert.equal(flickable.contentItem.width, 300);
		assert.equal(centered.x, 100);

		flickable.contentWidth = 450;
		settle();
		assert.equal(flickable.contentItem.width, 450);
		assert.equal(centered.x, 175);
	},

	'native double-click events reach the scene with canvas-relative coordinates'() {
		const savedWindow = globalThis.window;
		const savedResizeObserver = globalThis.ResizeObserver;
		const savedRequestAnimationFrame = globalThis.requestAnimationFrame;
		try {
			const handlers = {};
			let doubleClick = null;
			globalThis.window = { devicePixelRatio: 1, addEventListener() {} };
			globalThis.ResizeObserver = class {
				observe() {}
			};
			globalThis.requestAnimationFrame = () => {};
			const canvas = {
				clientWidth: 100,
				clientHeight: 100,
				style: {},
				addEventListener(name, handler) { handlers[name] = handler; },
				getBoundingClientRect() { return { left: 4, top: 6 }; },
			};
			const scene = {
				resize() {},
				pointerDoubleClick(...args) { doubleClick = args; },
				pressed: null,
			};
			attachCanvas(canvas, scene, {});
			handlers.dblclick({ button: 0, clientX: 16, clientY: 30 });
			assert.deepEqual(doubleClick, [12, 24, 0]);
		} finally {
			if (savedWindow === undefined) delete globalThis.window;
			else globalThis.window = savedWindow;
			if (savedResizeObserver === undefined) delete globalThis.ResizeObserver;
			else globalThis.ResizeObserver = savedResizeObserver;
			if (savedRequestAnimationFrame === undefined) delete globalThis.requestAnimationFrame;
			else globalThis.requestAnimationFrame = savedRequestAnimationFrame;
		}
	},

	'double-click dispatch preserves the pressed mouse button'() {
		const engine = new Engine();
		const scene = new Scene(null, engine);
		const area = new MouseArea();
		let emitted = null;
		scene.root.$itemAt = (x, y, wheel, button) => {
			assert.deepEqual([x, y, wheel, button], [12, 24, false, 2]);
			return area;
		};
		area.$emit = (name, args) => {
			emitted = { name, mouse: args[0] };
		};
		scene.pointerDoubleClick(12, 24, 2);
		assert.equal(emitted.name, 'doubleClicked');
		assert.equal(emitted.mouse.button, 'RightButton');
		assert.equal(emitted.mouse.buttons, 'RightButton');
	},

	'images keep their current picture visible while a replacement loads'() {
		const savedDocument = globalThis.document;
		const savedImage = globalThis.Image;
		try {
			new Engine();
			globalThis.Image = class {
				set src(_) {}
			};
			globalThis.document = {
				createElement() {
					return {
						width: 0,
						height: 0,
						getContext() {
							return { drawImage() {}, getImageData() { return {}; } };
						},
					};
				},
			};
			const image = new ImageItem();
			const current = { naturalWidth: 16, naturalHeight: 16 };
			image.$aw = image.$ah = 16;
			image.$setImage(current, 1);
			image.source = 'replacement.png';
			let painted = false;
			image.$paint({ dpr: 1, ma: 1, image() { painted = true; } }, 1);
			assert.equal(image.status, 1);
			assert.equal(image.$img, current);
			assert.equal(painted, true);
		} finally {
			if (savedDocument === undefined) delete globalThis.document;
			else globalThis.document = savedDocument;
			if (savedImage === undefined) delete globalThis.Image;
			else globalThis.Image = savedImage;
		}
	},

	async 'Ready-bound tab icon slots do not collapse during source changes'() {
		const savedImage = globalThis.Image;
		const savedDocument = globalThis.document;
		const requests = [];
		try {
			globalThis.Image = class {
				naturalWidth = 16;
				naturalHeight = 16;
				set src(source) { requests.push({ source, picture: this }); }
			};
			globalThis.document = {};
			const { root, settle } = await load(`
				import QtQuick
				Item {
					id: tab
					property bool selected: false
					property alias icon: picture
					property alias slot: iconSlot
					Item {
						id: iconSlot
						visible: picture.status === Image.Ready
						width: visible ? picture.width : 0
						height: 16
						Image {
							id: picture
							width: 16
							height: 16
							source: tab.selected ? "active.png" : "normal.png"
						}
					}
				}
			`);
			assert.equal(root.icon.status, 2);
			assert.equal(root.slot.visible, false);
			assert.equal(root.slot.width, 0);
			requests[0].picture.onload();
			settle();
			const current = root.icon.$img;
			assert.equal(root.slot.visible, true);
			assert.equal(root.slot.width, 16);

			root.selected = true;
			settle();
			assert.equal(requests[1].source, 'active.png');
			assert.equal(root.icon.status, 1);
			assert.equal(root.icon.$img, current);
			assert.equal(root.slot.visible, true);
			assert.equal(root.slot.width, 16);
			requests[1].picture.onload();
			settle();
			assert.equal(root.icon.$img, requests[1].picture);

			root.selected = false;
			settle();
			root.selected = true;
			settle();
			requests[2].picture.onload();
			requests[2].picture.onerror();
			settle();
			assert.equal(root.icon.$img, requests[1].picture, 'a stale replacement must not overwrite the current picture');
			assert.equal(root.slot.visible, true);
			assert.equal(root.slot.width, 16);
			requests[3].picture.onload();
			settle();
			assert.equal(root.icon.$img, requests[3].picture);

			root.icon.source = 'missing.png';
			settle();
			assert.equal(root.slot.visible, true);
			requests[4].picture.onerror();
			settle();
			assert.equal(root.icon.status, 3);
			assert.equal(root.icon.$img, null);
			assert.equal(root.slot.visible, false);
			assert.equal(root.slot.width, 0);

			root.icon.source = 'retry.png';
			settle();
			assert.equal(root.icon.status, 2);
			requests[5].picture.onload();
			settle();
			assert.equal(root.slot.visible, true);
			root.icon.source = 'pending.png';
			settle();
			assert.equal(root.slot.visible, true);
			root.icon.source = '';
			settle();
			assert.equal(root.icon.status, 0);
			assert.equal(root.icon.$img, null);
			assert.equal(root.slot.visible, false);
			assert.equal(root.slot.width, 0);
			requests[6].picture.onload();
			settle();
			assert.equal(root.icon.status, 0, 'clearing a source must invalidate pending callbacks');
		} finally {
			if (savedImage === undefined) delete globalThis.Image;
			else globalThis.Image = savedImage;
			if (savedDocument === undefined) delete globalThis.document;
			else globalThis.document = savedDocument;
		}
	},

	async 'documentAdded sees the document index updated synchronously after model mutations'() {
		const { root, engine, settle } = await load(`
			import QtQuick
			Item {
				id: root
				property ListModel documentsModel: ListModel {}
				property int documentsCount: documentsModel.count
				property var documentIndexById: ({})
				property var opened: []
				signal documentAdded(string documentId)
				onDocumentsCountChanged: {
					let map = ({})
					for (let i = 0; i < documentsModel.count; i++){
						map[documentsModel.get(i).documentData.documentId] = i
					}
					documentIndexById = map
				}
				function getDocumentDataById(documentId){
					let index = documentIndexById[documentId]
					return index === undefined ? null : documentsModel.get(index).documentData
				}
				function addDocument(id){
					documentsModel.append({"documentData": {"documentId": id, "viewComp": id + "View"}})
					documentAdded(id)
				}
				function removeDocument(index) { documentsModel.remove(index) }
				function clearDocuments() { documentsModel.clear() }
				Connections {
					target: root
					function onDocumentAdded(documentId){
						let data = root.getDocumentDataById(documentId)
						root.opened.push(data.documentId + ":" + data.viewComp)
					}
				}
			}
		`);
		root.addDocument('new');
		assert.deepEqual(engine.errors, []);
		assert.deepEqual(root.opened, ['new:newView']);
		assert.equal(root.documentsCount, 1);
		assert.equal(root.documentIndexById.new, 0);
		root.addDocument('existing');
		assert.deepEqual(root.opened, ['new:newView', 'existing:existingView']);
		root.removeDocument(0);
		assert.equal(root.documentsCount, 1);
		assert.equal(root.documentIndexById.existing, 0);
		assert.equal(root.documentIndexById.new, undefined);
		root.clearDocuments();
		assert.equal(root.documentsCount, 0);
		assert.deepEqual(root.documentIndexById, {});
		settle();
	},

	async 'subscription handlers see bindings declared after the ready property'() {
		const { root, settle } = await load(`
			import QtQuick
			Item {
				id: window
				property var client: checker
				property alias subscriptionManager: manager
				QtObject {
					id: manager
					property int registrations: 0
					function registerSubscriptionEvent() { registrations++ }
				}
				ConnectionChecker {
					id: checker
					gqlCommandId: "PumaWsConnection"
					subscriptionManager: window.subscriptionManager
				}
			}
		`, {
			'SubscriptionClient.qml': `
				import QtQuick
				QtObject {
					property string gqlCommandId
					property bool ok: gqlCommandId !== ""
					onOkChanged: if (ok) registerSubscription()
					function registerSubscription() {}
				}
			`,
			'ConnectionChecker.qml': `
				import QtQuick
				SubscriptionClient {
					property var subscriptionManager
					function registerSubscription() { subscriptionManager.registerSubscriptionEvent() }
				}
			`,
		});
		assert.equal(root.subscriptionManager.registrations, 1);
		assert.equal(root.client.subscriptionManager, root.subscriptionManager);
		root.client.gqlCommandId = '';
		settle();
		root.client.gqlCommandId = 'NextSubscription';
		settle();
		assert.equal(root.subscriptionManager.registrations, 2);
	},

	async 'regular expression validator ids exist and acceptableInput follows their expressions'() {
		const { root, settle } = await load(`
			import QtQuick
			Item {
				property var nameInput: nameInput
				property var emailInput: emailInput
				property var expression: notEmptyRegexp
				TextInput { id: nameInput; validator: notEmptyRegexp }
				RegularExpressionValidator { id: notEmptyRegexp; regularExpression: /^(?!\\s*$).+/ }
				TextInput { id: emailInput; validator: mailValid }
				RegularExpressionValidator { id: mailValid; regularExpression: /\\w+@\\w+\\.\\w+/ }
			}
		`);
		assert.equal(root.nameInput.acceptableInput, false);
		root.nameInput.text = '   ';
		root.emailInput.text = 'invalid';
		settle();
		assert.equal(root.nameInput.acceptableInput, false);
		assert.equal(root.emailInput.acceptableInput, false);
		root.nameInput.text = 'Alice';
		root.emailInput.text = 'alice@example.test';
		settle();
		assert.equal(root.nameInput.acceptableInput, true);
		assert.equal(root.emailInput.acceptableInput, true);
		root.emailInput.text = '!alice@example.test!';
		settle();
		assert.equal(root.emailInput.acceptableInput, false);
		root.expression.regularExpression = /Bob/;
		settle();
		assert.equal(root.nameInput.acceptableInput, false);
		root.nameInput.text = 'Bob';
		settle();
		assert.equal(root.nameInput.acceptableInput, true);
		const alternatives = new RegularExpressionValidator();
		alternatives.regularExpression = /a|ab/g;
		assert.equal(alternatives.accepts('ab'), true);
		assert.equal(alternatives.accepts('ab'), true);
		assert.equal(alternatives.accepts('abc'), false);
		alternatives.regularExpression = /a|a\n/m;
		assert.equal(alternatives.accepts('a\n'), true);
		assert.equal(alternatives.accepts('x\na'), false);
	},

	async 'attached destruction keeps the object alive and fires once'() {
		const { engine, root } = await load(`
			import QtQuick
			Item {
				id: root
				property var events: []
				property var view: child
				function destroyed() { events.push("attached " + child.objectName) }
				Item {
					id: child
					objectName: "view"
					property QtObject helper: QtObject { property string value: "alive" }
					Component.onDestruction: root.events.push(helper.value)
				}
				Component.onCompleted: view.Component.destruction.connect(root.destroyed)
			}
		`);
		const child = root.view;
		assert.equal(child.Component, child.Component);
		assert.equal(root.Component.Ready, engine.globals.Component.Ready);
		child.destroy();
		child.destroy();
		assert.deepEqual(root.events, ['alive', 'attached view']);
		assert.deepEqual(engine.errors, []);
	},

	async 'built-in notify signals connect, update, disconnect and release'() {
		const { engine, root, settle } = await load(`
			import QtQuick
			Item {
				id: root
				property var events: []
				property var view: child
				property var observer: watcher
				function changed() { events.push("changed") }
				Item { id: child; width: 20; height: 10 }
				QtObject {
					id: watcher
					function observe() { root.events.push("observed") }
				}
				Component.onCompleted: {
					child.visibleChanged.connect(root.changed)
					child.heightChanged.connect(root.changed)
					child.widthChanged.connect(root.changed)
					child.widthChanged.connect(watcher.observe)
				}
			}
		`);
		assert.equal(root.view.visibleChanged, root.view.visibleChanged);
		root.view.visible = false;
		settle();
		root.view.height = 30;
		settle();
		root.view.width = 40;
		settle();
		assert.deepEqual(root.events, ['changed', 'changed', 'changed', 'observed']);
		root.observer.destroy();
		root.view.widthChanged.disconnect(root.changed);
		root.view.visibleChanged.disconnect(root.changed);
		root.view.heightChanged.disconnect(root.changed);
		root.view.visible = true;
		root.view.width = 50;
		root.view.height = 60;
		settle();
		assert.equal(root.events.length, 4);
		root.view.destroy();
		assert.equal(engine.changeHandlers.some((h) => h.item === root.view), false);
	},

	'nested TreeItemModel roles expose getData through every delegate access path'() {
		new Engine();
		const model = new TreeItemModel();
		model.createFromJson([{ user: { name: 'Alice', url: { path: '/users/alice' } }, count: 2 }]);
		const scope = modelScopeData(model, 0);
		for (const row of [scope.roles, scope.model, scope.modelData]) {
			assert.ok(row.user instanceof TreeItemModel);
			assert.equal(row.user.getData('name'), 'Alice');
			assert.equal(row.user.getData('url').getData('path'), '/users/alice');
			assert.equal(row.count, 2);
		}
		const object = new QtObject();
		model.setData('object', object);
		assert.equal(model.getData('object'), object);
		assert.equal(modelScopeData(model, 0).model.object, object);
		model.setData('user', { name: 'Bob' });
		assert.equal(scope.model.user.getData('name'), 'Bob');
		const list = new ListModel();
		list.append({ user: { name: 'plain' } });
		assert.equal(modelScopeData(list, 0).model.user.name, 'plain');
		const array = [{ user: { name: 'array' } }];
		assert.equal(modelScopeData(array, 0).model.user, array[0].user);
	},

	async 'QML delegates receive nested table values as models'() {
		const { root, settle } = await load(`
			import QtQuick
			Item {
				property var sourceModel: tree
				property var delegates: rows
				TreeItemModel { id: tree }
				Repeater {
					id: rows
					model: tree
					delegate: Item {
						property string userName: model.user.getData("name")
						property string rowName: modelData.user.getData("name")
						property string directName: user.getData("name")
						function getValue() { return model.user }
					}
				}
			}
		`);
		root.sourceModel.createFromJson([{ user: { name: 'Alice', url: { path: '/alice' } } }]);
		settle();
		const cell = root.delegates.itemAt(0);
		assert.equal(cell.userName, 'Alice');
		assert.equal(cell.rowName, 'Alice');
		assert.equal(cell.directName, 'Alice');
		assert.equal(cell.getValue().getData('url').getData('path'), '/alice');
		root.sourceModel.setData('user', { name: 'Bob', url: { path: '/bob' } });
		settle();
		assert.equal(root.delegates.itemAt(0).userName, 'Bob');
		assert.equal(root.delegates.itemAt(0).getValue().getData('url').getData('path'), '/bob');
	},

	'images enable anonymous CORS before starting every request'() {
		const savedImage = globalThis.Image;
		const savedDocument = globalThis.document;
		const requests = [];
		class Picture {
			set src(value) {
				requests.push({ picture: this, source: value, cors: this.crossOrigin });
			}
		}
		try {
			globalThis.Image = Picture;
			globalThis.document = {};
			const engine = new Engine();
			engine.resourceRoot = 'https://assets.example.test/';
			const image = new ImageItem();
			image.source = 'icon.png';
			assert.equal(requests[0].cors, 'anonymous');
			requests[0].picture.onload();
			assert.equal(image.status, 1);
			assert.equal(image.$img, requests[0].picture);
			image.source = 'denied.png';
			requests[1].picture.onerror();
			assert.equal(image.status, 3);
			assert.equal(image.$img, null);
			image.source = 'data:image/svg+xml,<svg/>';
			assert.equal(requests[2].cors, 'anonymous');
			requests[2].picture.onload();
			assert.equal(image.status, 1);
			engine.resourceRoot = null;
			image.$directory = 'components';
			image.source = 'icons/icon.png';
			requests[3].picture.onerror();
			assert.equal(requests[4].cors, 'anonymous');
			assert.notEqual(requests[3].source, requests[4].source);
			requests[4].picture.onload();
			assert.equal(image.status, 1);
		} finally {
			if (savedImage === undefined) delete globalThis.Image;
			else globalThis.Image = savedImage;
			if (savedDocument === undefined) delete globalThis.document;
			else globalThis.document = savedDocument;
		}
	},

	async 'SVG loading preserves the source and ignores stale responses'() {
		const savedImage = globalThis.Image;
		const savedDocument = globalThis.document;
		const savedFetch = globalThis.fetch;
		const requests = [];
		const fetches = [];
		try {
			globalThis.Image = class {
				set src(source) { requests.push({ source, cors: this.crossOrigin, picture: this }); }
			};
			globalThis.document = {};
			globalThis.fetch = (source) => new Promise((resolve) => fetches.push({ source, resolve }));
			const engine = new Engine();
			engine.resourceRoot = 'https://assets.example.test/';
			const image = new ImageItem();
			image.source = 'old.svg';
			image.source = 'admin.svg?version=1';
			assert.equal(requests.length, 0);
			const svg = '<svg xmlns="http://www.w3.org/2000/svg"><switch><foreignObject requiredExtensions="adobe" /><path d="M0 0h1v1" /></switch></svg>';
			fetches[0].resolve({ ok: true, text: async () => svg });
			fetches[1].resolve({ ok: true, text: async () => svg });
			await new Promise((resolve) => setTimeout(resolve, 0));
			assert.equal(requests.length, 1);
			assert.equal(fetches[1].source, 'https://assets.example.test/admin.svg?version=1');
			assert.equal(requests[0].cors, 'anonymous');
			assert.equal(decodeURIComponent(requests[0].source.split(',')[1]), svg);
			requests[0].picture.onload();
			assert.equal(image.status, 1);
			image.source = 'missing.svg';
			fetches[2].resolve({ ok: false, status: 404 });
			await new Promise((resolve) => setTimeout(resolve, 0));
			assert.equal(image.status, 3);
			assert.match(engine.errors[0], /missing\.svg.*HTTP 404/);
		} finally {
			if (savedImage === undefined) delete globalThis.Image;
			else globalThis.Image = savedImage;
			if (savedDocument === undefined) delete globalThis.document;
			else globalThis.document = savedDocument;
			globalThis.fetch = savedFetch;
		}
	},

	'a tainted image reports its source once without aborting every frame'() {
		const savedDocument = globalThis.document;
		try {
			const failure = new Error('Tainted canvas');
			failure.name = 'SecurityError';
			globalThis.document = {
				createElement() {
					return { getContext() { return { drawImage() {}, getImageData() { throw failure; } }; } };
				},
			};
			const engine = new Engine();
			const image = new ImageItem();
			image.$source = 'unsafe.svg';
			image.$setImage({ width: 10, height: 10 }, 1);
			image.$aw = image.$ah = 10;
			const renderer = { dpr: 1, ma: 1, image() { assert.fail('A tainted page must not reach WebGL'); } };
			image.$paint(renderer, 1);
			image.$paint(renderer, 1);
			assert.equal(image.status, 3);
			assert.equal(engine.errors.length, 1);
			assert.match(engine.errors[0], /Image 'unsafe\.svg'.*Tainted canvas/);
		} finally {
			if (savedDocument === undefined) delete globalThis.document;
			else globalThis.document = savedDocument;
		}
	},
};

let failed = 0;
for (const [name, check] of Object.entries(checks)) {
	try {
		await check();
		console.log(`ok   ${name}`);
	} catch (error) {
		failed++;
		console.error(`FAIL ${name}`, error);
	}
}
process.exit(failed === 0 ? 0 : 1);
