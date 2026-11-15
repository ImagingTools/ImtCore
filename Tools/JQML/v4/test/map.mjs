// The map of QtLocation and the pointer handlers that move it, driven through the scene without a browser.
// A Map is not interactive in Qt 6: a WheelHandler and a DragHandler inside of it zoom and pan it.
// `node test/map.mjs`
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const SOURCE = `
import QtQuick
import QtLocation
import QtPositioning

Item {
	width: 800
	height: 600
	property var log: []
	property int translations: 0

	Plugin {
		id: osm
		name: "osm"
		PluginParameter { name: "osm.mapping.custom.host"; value: "https://tiles.example.com/maps/" }
	}

	// A map without handlers: the mouse goes through it to the item below.
	MouseArea {
		objectName: "under"
		x: 0
		y: 0
		width: 200
		height: 150
		onClicked: log.push("under")
	}

	Map {
		id: still
		objectName: "still"
		x: 0
		y: 0
		width: 200
		height: 150
		plugin: osm
		center: QtPositioning.coordinate(50, 10)
		zoomLevel: 6
	}

	Map {
		id: live
		objectName: "live"
		x: 300
		y: 0
		width: 400
		height: 300
		plugin: osm
		center: QtPositioning.coordinate(50, 10)
		zoomLevel: 6
		property int wheelEvents: 0
		property real lastAngle: 0

		WheelHandler {
			id: wheel
			objectName: "wheel"
			property: "zoomLevel"
			rotationScale: 1 / 120
			onWheel: (event) => {
				live.wheelEvents++
				live.lastAngle = event.angleDelta.y
			}
		}

		DragHandler {
			id: pan
			objectName: "pan"
			target: null
			onTranslationChanged: {
				translations++
				live.pan(-delta.x, -delta.y)
			}
		}

		MapQuickItem {
			id: marker
			coordinate: QtPositioning.coordinate(50, 10)
			anchorPoint.x: 10
			anchorPoint.y: 10
			sourceItem: Rectangle {
				width: 20
				height: 20
				MouseArea {
					anchors.fill: parent
					onClicked: log.push("marker")
				}
			}
		}
	}

	Rectangle {
		id: free
		objectName: "free"
		x: 0
		y: 300
		width: 100
		height: 100
		DragHandler { id: freeDrag }
	}

	Rectangle {
		id: limited
		objectName: "limited"
		x: 150
		y: 300
		width: 100
		height: 100
		DragHandler {
			xAxis.enabled: false
			yAxis.maximum: 340
		}
	}

	Rectangle {
		id: card
		objectName: "card"
		x: 450
		y: 320
		width: 100
		height: 100
		MouseArea {
			objectName: "cardArea"
			anchors.fill: parent
			onClicked: log.push("card")
		}
		DragHandler { id: cardDrag }
	}

	Rectangle {
		id: locked
		objectName: "locked"
		x: 600
		y: 320
		width: 100
		height: 100
		MouseArea {
			objectName: "lockedArea"
			anchors.fill: parent
			preventStealing: true
			onClicked: log.push("locked")
		}
		DragHandler { id: lockedDrag }
	}
}
`;

class MemoryFS {
	async readText(path) {
		return path === 'main.qml' ? SOURCE : null;
	}

	async readBytes() {
		return null;
	}
}

const engine = new Engine();
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(900, 700, 1);
const project = new Project(new MemoryFS());
await project.load('main.qml');
viewer.load(project);
const frame = () => {
	for (let i = 0; i < 4; i++) scene.drawFrame((frame.time += 0.016));
};
frame.time = 0;
frame();

const find = (name, item = viewer.content) => {
	if (item.$objectName === name) return item;
	for (const child of item.$children) {
		const hit = find(name, child);
		if (hit !== null) return hit;
	}
	return null;
};
const root = viewer.content.$children[0];
const still = find('still');
const live = find('live');
const wheel = find('wheel');
const pan = find('pan');
const free = find('free');
const limited = find('limited');
const card = find('card');

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}
const near = (a, b) => Math.abs(a - b) < 1e-6;

// A position inside of an item in the coordinates of the scene.
const at = (item, x, y) => [item.$sx + x, item.$sy + y];
const click = (item, x, y) => {
	const [sx, sy] = at(item, x, y);
	scene.pointerMove(sx, sy);
	scene.pointerDown(sx, sy);
	scene.pointerUp(sx, sy);
	frame();
};
// Presses at a position of the item and moves by (dx, dy) in steps; the button stays down unless `release` is false.
const drag = (item, x, y, dx, dy, release = true, steps = 5) => {
	const [sx, sy] = at(item, x, y);
	scene.pointerMove(sx, sy);
	scene.pointerDown(sx, sy);
	for (let i = 1; i <= steps; i++) scene.pointerMove(sx + (dx * i) / steps, sy + (dy * i) / steps);
	if (release) scene.pointerUp(sx + dx, sy + dy);
	frame();
};
const wheelAt = (item, x, y, notches) => {
	const [sx, sy] = at(item, x, y);
	scene.wheel(sx, sy, notches);
	frame();
};

check('engine errors', engine.errors, []);
check('the map is ready after its first layout', [still.mapReady, live.mapReady], [true, true]);
check('a map has no gesture property', typeof live.gesture, 'undefined');

// ----- a Map without handlers is not interactive -----

drag(still, 100, 75, 60, 30);
wheelAt(still, 100, 75, 3);
check('a map without handlers does not pan', [still.center.latitude, still.center.longitude], [50, 10]);
check('a map without handlers does not zoom', still.zoomLevel, 6);
root.log.length = 0;
click(still, 50, 50);
check('the mouse goes through a map without handlers', root.log, ['under']);

// ----- WheelHandler -----

wheelAt(live, 100, 100, 2);
check('the wheel changes the property by angle * rotationScale', live.zoomLevel, 8);
check('the wheel signal has the angle', [live.wheelEvents, live.lastAngle], [1, 240]);
check('the rotation of the handler adds up', wheel.rotation, 2);
wheelAt(live, 100, 100, -1);
check('a wheel turned back zooms out', [live.zoomLevel, wheel.rotation], [7, 1]);
const outside = at(live, 0, 0);
scene.wheel(outside[0] - 20, outside[1] - 20, 2);
frame();
check('a wheel outside of the map does nothing', live.zoomLevel, 7);
wheel.enabled = false;
wheelAt(live, 100, 100, 2);
check('a disabled handler does nothing', live.zoomLevel, 7);
wheel.enabled = true;
wheel.acceptedDevices = 2;
wheelAt(live, 100, 100, 2);
check('a handler that does not accept the mouse does nothing', live.zoomLevel, 7);
wheel.acceptedDevices = 0x7fffffff;
wheelAt(live, 100, 100, 100);
check('the zoom level keeps within its limits', live.zoomLevel, 30);
live.zoomLevel = 6;
wheelAt(live, 100, 100, 0);

// ----- DragHandler on a map: onTranslationChanged pans it -----

const start = live.center;
const origin = live.fromCoordinate(start, false);
drag(live, 200, 150, 100, 50);
const moved = live.fromCoordinate(start, false);
check('a drag pans the map by the distance of the pointer', [near(moved.x, origin.x + 100), near(moved.y, origin.y + 50)], [true, true]);
check('a handler is not active after the release', pan.active, false);

drag(live, 200, 150, -30, 0, false);
check('a handler is active while the button is down', pan.active, true);
check('the translation follows the pointer', [pan.translation.x, pan.translation.y], [70, 50]);
check('the active translation does not include earlier drags', [pan.activeTranslation.x, pan.activeTranslation.y], [-30, 0]);
const [px, py] = at(live, 170, 150);
scene.pointerUp(px, py);
frame();
check('the translation stays for the next drag', [pan.persistentTranslation.x, pan.persistentTranslation.y], [70, 50]);

const before = [root.translations, live.center.latitude, live.center.longitude];
drag(live, 200, 150, 4, 3);
check('a move below the drag threshold does nothing', [root.translations, live.center.latitude, live.center.longitude], before);

// ----- the item inside of the map, below the handler -----

root.log.length = 0;
const markerPoint = live.fromCoordinate(QtMarker(), false);
function QtMarker() {
	return live.$children.find((child) => child.$typeName === 'MapQuickItem').coordinate;
}
click(live, markerPoint.x, markerPoint.y);
check('a click reaches the MouseArea of a map item', root.log, ['marker']);
const centerBefore = [live.center.latitude, live.center.longitude];
root.log.length = 0;
drag(live, markerPoint.x, markerPoint.y, 40, 0);
check('a drag that starts on a map item pans the map', live.center.longitude < centerBefore[1], true);
check('the item does not get the click of a drag', root.log, []);

// ----- DragHandler on a plain item -----

drag(free, 50, 50, 30, 20);
check('the target of a handler is its parent', [free.x, free.y], [30, 320]);
drag(limited, 50, 50, 30, 60);
check('an axis can be switched off and limited', [limited.x, limited.y], [150, 340]);

// ----- a handler and a MouseArea on the same item -----

root.log.length = 0;
click(card, 50, 50);
check('a click without a drag is a click of the MouseArea', [root.log, card.x], [['card'], 450]);
drag(card, 50, 50, 40, 0);
check('a drag takes the press from the MouseArea', [root.log, card.x], [['card'], 490]);

const locked = find('locked');
drag(locked, 50, 50, 40, 0);
check('preventStealing keeps the press', locked.x, 600);

// ----- the map items follow the map -----

check('the item of a map is where its coordinate is', near(marker0().x, live.fromCoordinate(marker0().coordinate, false).x - 10), true);
function marker0() {
	return live.$children.find((child) => child.$typeName === 'MapQuickItem');
}

// ----- bearing: the compass direction at the top of the map -----

live.bearing = 0;
const middle = live.center;
const north = live.$coordinateAt(live.width / 2, 0);
check('without a bearing the north is at the top', [near(north.longitude, middle.longitude), north.latitude > middle.latitude], [true, true]);
check('the bearing starts at 0', live.bearing, 0);
live.bearing = 90;
check('the bearing is the direction at the top', live.bearing, 90);
const east = live.$coordinateAt(live.width / 2, 0);
check('with the bearing 90 the east is at the top', [east.longitude > middle.longitude, near(east.latitude, middle.latitude)], [true, true]);
const eastPoint = live.fromCoordinate(east, false);
check('and a coordinate goes back to where it was taken', [near(eastPoint.x, live.width / 2), near(eastPoint.y, 0)], [true, true]);
live.bearing = 450;
check('a bearing is wrapped to 0..360', live.bearing, 90);
live.bearing = -90;
check('a negative bearing is wrapped as well', live.bearing, 270);
live.bearing = 180;
const south = live.$coordinateAt(live.width / 2, 0);
check('with the bearing 180 the south is at the top', south.latitude < middle.latitude, true);
live.bearing = 90;
const turnedStart = live.center;
const turnedOrigin = live.fromCoordinate(turnedStart, false);
live.pan(0, -30);
const turnedMoved = live.fromCoordinate(turnedStart, false);
check('a pan is made in the frame of the turned map', [near(turnedMoved.x, turnedOrigin.x), near(turnedMoved.y, turnedOrigin.y + 30)], [true, true]);
const aligned = live.center;
live.alignCoordinateToPoint(aligned, { x: 10, y: 20 });
const alignedPoint = live.fromCoordinate(aligned, false);
check('a coordinate is aligned to a point of the turned map', [near(alignedPoint.x, 10), near(alignedPoint.y, 20)], [true, true]);
live.bearing = 0;
live.center = middle;
scene.drawFrame(1);
check('a turned map is painted without errors', engine.errors, []);

// ----- the tiles of the plugin -----

check('osm.mapping.custom.host is the tile server', live.$tileTemplate(), 'https://tiles.example.com/maps/{z}/{x}/{y}.png');
check('the tiles are painted without a browser', engine.errors, []);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
