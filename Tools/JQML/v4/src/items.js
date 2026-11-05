// The item tree: Item, Rectangle, Text, Canvas, MouseArea, GridLayout, ListView, FontLoader, Timer.
//
// The properties of an item are accessors, so scripts read and write them as plain JavaScript
// properties. Everything that is not a QML property starts with `$` (internal state and methods).
// Geometry follows the model of the C++ viewer: `$resolve()` applies anchors, `$arrange()` lets a
// layout item position its children, `$render()` draws through the renderer with a matrix stack.

import { CanvasContext, replayCommands } from './canvas.js';
import { fonts } from './fonts.js';
import { KEY, MODIFIERS } from './keys.js';
import { QmlError } from './lexer.js';
import { colorToString, parseColor, toColor, toFloat } from './util.js';

// ----- Hooks the engine fills in -----

export const hooks = {
	// Called when `Qt.binding(fn)` is assigned to a property: path is e.g. ['anchors', 'margins'].
	installBinding(item, path, fn) {
		throw new QmlError('Qt.binding is not available');
	},
	report(error, where) {
		console.error(`QML error: ${where}: ${error.message}`);
	},
	loadFont(loader, source) {
		loader.$name = '';
		loader.$status = 'Error';
	},
	// (image item, source) => void: the engine resolves the path and calls image.$setImage(img, status).
	loadImage(image, source) {
		image.$setImage(null, 3);
	},
	// (item, name, args) => boolean: true if the signal emission was postponed.
	defer: null,
	// (model) => void: a ListModel changed while Component.onCompleted is running.
	syncModel: null,
	// (repeater) => void: its delegate component was replaced and the items must exist before the next line.
	syncRepeater: null,
	// (item) => void: a Binding or Connections item changed and must apply before the next statement.
	syncBinding: null,
	syncConnection: null,
	// () => boolean: true while Component.onCompleted handlers run.
	completing: null,
	// (item, name) => void: a custom property that has `onNameChanged` handlers got a new value.
	changed: null,
	release: null,
	// > 0 while the engine itself writes a value (a binding result, an initial value): such writes keep the binding.
	engineWrite: 0,
	// > 0 while the engine runs an animation or a timer of a hidden item (Engine.setTime): what a hidden item asks for is
	// remembered in `quietDirty` instead of requesting a frame, the engine repaints later in one go.
	quiet: 0,
	quietDirty: false,
	// The binding whose function is running, or null. Getters subscribe it to the property they return.
	reading: null,
	// (item, name) => void. Null until the engine connects them.
	depend: null,
	notify: null,
	// (item, name) => void: readers of a property whose value the layout changed are marked to run again.
	dirty: null,
	// (loader) => void: source, sourceComponent or active changed after construction.
	reloadLoader: null,
	// (item, name) => void: a script assigned a value over a bound property, which removes the binding.
	breakBinding: null,
	// (text, html) => void / () => { text, html }: the clipboard of the page.
	clipboardWrite() {},
	clipboardRead() {
		return { text: '', html: '' };
	},
	// A model changed: a new frame is needed.
	touch() {},
	// (item) => void: an editor got the focus and its text cursor has to blink.
	caret() {},
	// () => Item|null: the QML document host. `mapToItem(null)` / `mapFromItem(null)` use its
	// origin so coordinates match the window content item (not viewer chrome above the host).
	mapOrigin: null,
};

// The result of Qt.binding(function): assigning it to a property creates a binding.
export class BindingValue {
	constructor(fn) {
		this.fn = fn;
	}
}

const WHITE = parseColor('#ffffff');
const BLACK = parseColor('#000000');
// Item.TransformOrigin: an int enum like in Qt, so `property int o: Item.Center` is valid.
export const TRANSFORM_ORIGIN = Object.freeze({
	TopLeft: 0, Top: 1, TopRight: 2, Left: 3, Center: 4, Right: 5, BottomLeft: 6, Bottom: 7, BottomRight: 8,
});
const ORIGIN_FRACTIONS = [[0, 0], [0.5, 0], [1, 0], [0, 0.5], [0.5, 0.5], [1, 0.5], [0, 1], [0.5, 1], [1, 1]];

const same = (a, b) => a === b || (a !== a && b !== b);

// Intermediate results of coordinate mapping (avoids allocations).
let hitX = 0;
let hitY = 0;

let destroyObserver = null;
export function setDestroyObserver(callback) {
	destroyObserver = callback;
}

// ----- Keyboard focus -----

// One focused child per focus scope. The root (and every FocusScope) is a scope.
// `activeFocus` is that chain only after the window has been shown: during the
// first Component.onCompleted the offscreen window is not active yet.
let activeLeaf = null;
let chainLeaf = null;
let sceneActive = false;
let activeSet = new Set();

export function getFocusItem() {
	return activeLeaf !== null ? activeLeaf : chainLeaf;
}

export function resetFocus() {
	activeLeaf = null;
	chainLeaf = null;
	sceneActive = false;
	activeSet = new Set();
	focusQueue = [];
	forceGen = 0;
}

export function setSceneActive(active) {
	sceneActive = Boolean(active);
}

// Drop the focus held by an item that is going away.
export function setFocusItem(item) {
	if (item !== null && item !== undefined) {
		item.forceActiveFocus();
		return;
	}
	activeLeaf = null;
	chainLeaf = null;
	refreshActive();
	hooks.touch();
}

function nearestScope(item) {
	let scope = item.$parent;
	while (scope !== null && !scope.$isFocusScope) scope = scope.$parent;
	return scope;
}

function emitFocus(item) {
	if (item.$focusBusy) return;
	item.$focusBusy = true;
	try {
		noteWrite(item, 'focus');
		if (item.$sigs !== null && item.$sigs.has('focusChanged')) item.$emit('focusChanged', []);
	} finally {
		item.$focusBusy = false;
	}
}

function emitActive(item) {
	noteWrite(item, 'activeFocus');
	if (item.$sigs !== null && item.$sigs.has('activeFocusChanged')) item.$emit('activeFocusChanged', []);
}

function focusLeaf(item) {
	let cur = item;
	const seen = new Set();
	while (cur !== null && cur.$isFocusScope && cur.$scopeFocus !== null && !seen.has(cur)) {
		seen.add(cur);
		cur = cur.$scopeFocus;
	}
	return cur;
}

function enclosingListView(item) {
	for (let p = item.$parent; p !== null; p = p.$parent) {
		if (p instanceof ListView) return p;
	}
	return null;
}

// True when `leaf` is the list or an ancestor of it (host FocusScope already owns this subtree).
function focusHostOwnsList(list, leaf) {
	for (let p = list; p !== null; p = p.$parent) {
		if (p === leaf) return true;
	}
	return false;
}

function updateChain(item) {
	let root = item;
	while (root.$parent !== null) root = root.$parent;
	chainLeaf = root.$isFocusScope && root.$scopeFocus !== null ? focusLeaf(root) : null;
}

function canBeActive(item) {
	for (let it = item; it !== null; it = it.$parent) {
		if (!it.$visible || it.$enabled === false) return false;
	}
	return true;
}

function refreshActive() {
	const next = new Set();
	if (sceneActive && activeLeaf !== null && activeLeaf.$destroyed !== true && canBeActive(activeLeaf)) {
		next.add(activeLeaf);
		for (let p = activeLeaf.$parent; p !== null; p = p.$parent) {
			if (p.$isFocusScope) next.add(p);
		}
	}
	const lost = [];
	const gained = [];
	for (const it of activeSet) if (!next.has(it)) lost.push(it);
	for (const it of next) if (!activeSet.has(it)) gained.push(it);
	activeSet = next;
	for (const it of lost) emitActive(it);
	for (const it of gained) emitActive(it);
}

// The first `focus: true` in a scope wins. Later siblings are cleared once their
// onFocusChanged handler is connected. Parents are settled before their children.
let focusQueue = [];
let forceGen = 0;

export function queueDeclarativeFocus(item) {
	focusQueue.push(item);
}

export function flushDeclarativeFocus() {
	const batch = focusQueue;
	if (batch.length === 0) return;
	focusQueue = [];
	const declined = [];
	for (const item of batch) {
		const lost = settleDeclarativeFocus(item);
		if (lost !== null) declined.push(lost);
	}
	// A nested item is cleared before its ancestor, so the deepest loss is logged first.
	// Siblings keep their source order.
	declined.sort((a, b) => {
		if (isFocusAncestor(a, b)) return 1;
		if (isFocusAncestor(b, a)) return -1;
		return 0;
	});
	for (const item of declined) emitFocus(item);
}

function isFocusAncestor(ancestor, item) {
	for (let p = item.$parent; p !== null; p = p.$parent) if (p === ancestor) return true;
	return false;
}

export function settleDeclarativeFocus(item) {
	if (!item.$focus) return null;
	const scope = nearestScope(item);
	if (scope === null) return null;
	if (scope.$scopeFocus === null || scope.$scopeFocus === item) {
		scope.$scopeFocus = item;
		updateChain(item);
		return null;
	}
	item.$focus = false;
	item.$focusDeclined = true;
	return item;
}

function setItemFocus(item, value, gen) {
	const fromForce = Boolean(gen);
	const next = Boolean(value);
	const scope = nearestScope(item);
	const already = item.$focus === next && (!next || scope === null || scope.$scopeFocus === item);
	if (already) {
		if (fromForce && next) emitFocus(item);
		return;
	}
	if (next) {
		const prev = scope === null ? null : scope.$scopeFocus;
		const declined = item.$focusDeclined === true;
		item.$focus = true;
		item.$focusDeclined = false;
		if (scope !== null) scope.$scopeFocus = item;
		if (fromForce) {
			item.$forceGen = gen;
			item.$forceOnce = prev === null || prev === item;
		}
		// A child that took an empty scope is notified again when its scope gains focus.
		if (fromForce && item.$isFocusScope && item.$scopeFocus !== null && item.$scopeFocus !== item && item.$scopeFocus.$forceGen === gen && item.$scopeFocus.$forceOnce === true) emitFocus(item.$scopeFocus);
		emitFocus(item);
		if (prev !== null && prev !== item && prev.$focus) {
			prev.$focus = false;
			emitFocus(prev);
		}
		// A scope assignment notifies twice. An item that was not turned down at creation
		// notifies again after the previous owner loses focus.
		if ((!fromForce && item.$isFocusScope) || (prev !== null && prev !== item && !declined)) emitFocus(item);
	} else if (item.$focus) {
		if (scope !== null && scope.$scopeFocus === item) scope.$scopeFocus = null;
		item.$focus = false;
		emitFocus(item);
		if (!fromForce && item.$isFocusScope) emitFocus(item);
	}
	updateChain(item);
}

function itemEnabled(item) {
	for (let it = item; it !== null; it = it.$parent) {
		if (it.$enabled === false) return false;
	}
	return true;
}

function takesTabFocus(item) {
	if (item.$tabFocus === true) return true;
	const policy = item.focusPolicy;
	return typeof policy === 'number' && (policy & 1) !== 0;
}

// ----- Property helpers -----

// A script writing a property that has a binding replaces the binding, as in Qt.
function scriptWrite(item, name) {
	const stack = hooks.changeStack;
	if (stack !== undefined) {
		for (let i = stack.length - 1; i >= 0; i--) {
			if (stack[i][0] === item && stack[i][1] === name) return;
		}
	}
	if (hooks.engineWrite === 0 && item.$bindings !== null && hooks.breakBinding !== null) hooks.breakBinding(item, name);
}

// While `fn` runs, a script write of this property does not remove its binding.
export function duringChange(item, name, fn) {
	if (hooks.changeStack === undefined) hooks.changeStack = [];
	hooks.changeStack.push([item, name]);
	try {
		fn();
	} finally {
		hooks.changeStack.pop();
	}
}

// While a binding function runs, the read subscribes that binding to the property. `trusted` says that every change of
// the value is announced with noteWrite or markReaders; a binding that read anything else is re-run on every pass.
function noteRead(item, name, trusted = false) {
	if (hooks.reading !== null && item !== null && item !== undefined) hooks.depend(item, name, trusted);
}

// The running binding read something whose changes are not announced (a model's rows, the scene position of an item).
export function readUntracked(what = 'untracked') {
	const binding = hooks.reading;
	if (binding !== null) {
		binding.volatileNow = true;
		if (binding.why === null) binding.why = what;
	}
}

// Announces that the layout or the tree changed a value that bindings read. Readers are only marked: the layout is running.
function markReaders(item, name) {
	if (hooks.dirty !== null) hooks.dirty(item, name);
}

// The tree above `item` changed: what depends on its parent, on it being visible or on it being enabled is read again
// (the readers below it follow the item's own `visible` and `enabled`).
function markReparented(item) {
	markReaders(item, 'parent');
	markReaders(item, 'visible');
	markReaders(item, 'enabled');
}

// A new value: bindings that read the property run before the setter returns.
function noteWrite(item, name) {
	if (item !== null && item !== undefined && hooks.notify !== null) hooks.notify(item, name);
}

// A script write (and a binding it updates inside onCompleted) reports onXChanged before the next line.
// Binding updates during a normal sweep stay with the polled pass, so width still runs before height.
function notifyGeometry(item, name) {
	if (hooks.changed === null) return;
	if (hooks.engineWrite > 0 && (hooks.completing === null || !hooks.completing())) return;
	hooks.changed(item, name);
}

let layingOut = false;

// True while the scene runs its layout and binding passes: only there a geometry read may take the frame request.
let frameLoop = false;

export function setFrameLoop(on) {
	frameLoop = on;
}

// Bumped by every $update(): an item that found the layout current at this value still finds it so.
let layoutEpoch = 0;

// A view lays out from its model, which changes without calling $update on the view: the model reaches the
// views that last arranged from it.
function watchModel(view) {
	const model = view.$model;
	if (!(model instanceof ListModel || model instanceof TreeItemModel)) return;
	if (model.$views === undefined) model.$views = new Set();
	model.$views.add(view);
}

function notifyViews(model) {
	if (model.$views === undefined) return;
	for (const view of model.$views) {
		if (view.$destroyed || view.$model !== model) model.$views.delete(view);
		else view.$update();
	}
}

// A function of a QML object connected to a signal of another object is disconnected when its object is destroyed.
export function linkToOwner(source, fn, undo) {
	const owner = fn.$owner;
	if (owner === undefined || owner === source) return;
	if (owner.$links === undefined) owner.$links = [];
	owner.$links.push(undo);
}

// Script assignment replaces a binding on these Item properties (Qt). `visible` must be
// included: CustomTooltip.open sets `backgroundItem.visible = false` over
// `visible: modalDialogs.visible`; without dropping that binding the overlay MouseArea
// stays hit-testable, steals hover from the button, and the tooltip open/close flickers.
const BREAKS_BINDING = new Set(['x', 'y', 'width', 'height', 'visible', 'opacity', 'enabled']);

function defineProp(proto, name, get, set, trusted = false) {
	Object.defineProperty(proto, name, {
		get() {
			noteRead(this, name, trusted);
			return get.call(this);
		},
		set(value) {
			if (value instanceof BindingValue) {
				hooks.installBinding(this, [name], value.fn);
				return;
			}
			if (BREAKS_BINDING.has(name)) scriptWrite(this, name);
			set.call(this, value);
			noteWrite(this, name);
		},
		enumerable: true,
		configurable: true,
	});
}

function defineReadOnly(proto, name, get, message = `Cannot assign to read-only property '${name}'`, trusted = false) {
	Object.defineProperty(proto, name, {
		get() {
			noteRead(this, name, trusted);
			return get.call(this);
		},
		set() {
			throw new QmlError(message);
		},
		enumerable: true,
		configurable: true,
	});
}

// A property backed by `field`; `coerce` converts the assigned value. Only its setter changes the field.
function defineField(proto, name, field, coerce) {
	defineProp(
		proto,
		name,
		function () {
			return this[field];
		},
		function (value) {
			const v = coerce(value);
			if (!same(this[field], v)) {
				this[field] = v;
				this.$update();
			}
		},
		true,
	);
}

// A property of a group object (anchors, border, font) owned by an item.
function defineGroupProp(proto, group, name, get, set) {
	const key = `${group}.${name}`;
	Object.defineProperty(proto, name, {
		get() {
			noteRead(this.$owner, key, true);
			return get.call(this);
		},
		set(value) {
			if (value instanceof BindingValue) {
				hooks.installBinding(this.$owner, [group, name], value.fn);
				return;
			}
			set.call(this, value);
			noteWrite(this.$owner, key);
		},
		enumerable: true,
		configurable: true,
	});
}

function defineGroupField(proto, group, name, field, coerce) {
	defineGroupProp(
		proto,
		group,
		name,
		function () {
			return this[field];
		},
		function (value) {
			const v = coerce(value);
			if (!same(this[field], v)) {
				this[field] = v;
				this.$owner.$update();
			}
		},
	);
}

const toBool = (v) => Boolean(v);
const toStr = (v) => String(v);

// The int of Item.TransformOrigin; the name is accepted too (`transformOrigin: "Center"`, as QML allows for enums).
function toTransformOrigin(value) {
	if (typeof value === 'number' && Number.isInteger(value) && value >= 0 && value < ORIGIN_FRACTIONS.length) return value;
	if (typeof value === 'string' && Object.prototype.hasOwnProperty.call(TRANSFORM_ORIGIN, value)) return TRANSFORM_ORIGIN[value];
	throw new QmlError(
		'transformOrigin must be one of Item.TopLeft, Item.Top, Item.TopRight, Item.Left, Item.Center, Item.Right, Item.BottomLeft, Item.Bottom, Item.BottomRight',
	);
}

// Pixels per wheel notch. The page reports a pixel wheel delta as delta / 100 notches, so 100 scrolls 1:1 with the
// pointer (a trackpad), and a mouse notch (100 pixels) scrolls as far as in a browser.
const WHEEL_NOTCH_PX = 100;

// ----- Anchors -----

export class AnchorLine {
	constructor(item, edge) {
		this.item = item;
		this.edge = edge; // 0 left, 1 right, 2 top, 3 bottom, 4 horizontal center, 5 vertical center
	}
}

const sameLine = (a, b) => (a === null && b === null) || (a !== null && b !== null && a.item === b.item && a.edge === b.edge);

function toAnchorLine(value) {
	if (value === undefined || value === null) return null;
	if (value instanceof AnchorLine) return value;
	throw new QmlError('An anchor must be set to an item edge such as parent.left');
}

function toItem(value) {
	if (value === undefined || value === null) return null;
	if (value instanceof Item) return value;
	throw new QmlError('An item is expected');
}

class Anchors {
	constructor(owner) {
		this.$owner = owner;
		this.$left = null;
		this.$right = null;
		this.$top = null;
		this.$bottom = null;
		this.$hcenter = null;
		this.$vcenter = null;
		this.$fill = null;
		this.$centerIn = null;
		this.$lm = 0;
		this.$rm = 0;
		this.$tm = 0;
		this.$bm = 0;
		this.$hco = 0;
		this.$vco = 0;
	}

	get $affectsX() {
		return this.$fill !== null || this.$centerIn !== null || this.$left !== null || this.$right !== null || this.$hcenter !== null;
	}

	get $affectsY() {
		return this.$fill !== null || this.$centerIn !== null || this.$top !== null || this.$bottom !== null || this.$vcenter !== null;
	}

	get $affectsWidth() {
		return this.$fill !== null || (this.$left !== null && this.$right !== null);
	}

	get $affectsHeight() {
		return this.$fill !== null || (this.$top !== null && this.$bottom !== null);
	}

	get $active() {
		return this.$affectsX || this.$affectsY;
	}
}

function defineAnchorLine(name, field) {
	defineGroupProp(
		Anchors.prototype,
		'anchors',
		name,
		function () {
			return this[field];
		},
		function (value) {
			const line = toAnchorLine(value);
			if (!sameLine(this[field], line)) {
				this[field] = line;
				this.$owner.$update();
			}
		},
	);
}

defineAnchorLine('left', '$left');
defineAnchorLine('right', '$right');
defineAnchorLine('top', '$top');
defineAnchorLine('bottom', '$bottom');
defineAnchorLine('horizontalCenter', '$hcenter');
defineAnchorLine('verticalCenter', '$vcenter');
defineGroupField(Anchors.prototype, 'anchors', 'fill', '$fill', toItem);
defineGroupField(Anchors.prototype, 'anchors', 'centerIn', '$centerIn', toItem);
defineGroupField(Anchors.prototype, 'anchors', 'leftMargin', '$lm', toFloat);
defineGroupField(Anchors.prototype, 'anchors', 'rightMargin', '$rm', toFloat);
defineGroupField(Anchors.prototype, 'anchors', 'topMargin', '$tm', toFloat);
defineGroupField(Anchors.prototype, 'anchors', 'bottomMargin', '$bm', toFloat);
defineGroupField(Anchors.prototype, 'anchors', 'horizontalCenterOffset', '$hco', toFloat);
defineGroupField(Anchors.prototype, 'anchors', 'verticalCenterOffset', '$vco', toFloat);
defineGroupProp(
	Anchors.prototype,
	'anchors',
	'margins',
	function () {
		return this.$lm;
	},
	function (value) {
		const v = toFloat(value);
		if (this.$lm !== v || this.$rm !== v || this.$tm !== v || this.$bm !== v) {
			this.$lm = this.$rm = this.$tm = this.$bm = v;
			this.$owner.$update();
			// The four margins share this write; readers of each one are subscribed under their own name.
			noteWrite(this.$owner, 'anchors.leftMargin');
			noteWrite(this.$owner, 'anchors.rightMargin');
			noteWrite(this.$owner, 'anchors.topMargin');
			noteWrite(this.$owner, 'anchors.bottomMargin');
		}
	},
);

// ----- Item -----

const OBJECT_NAMES_UNSCOPABLE = [
	'constructor', 'hasOwnProperty', 'isPrototypeOf', 'propertyIsEnumerable', 'toLocaleString', 'toString', 'valueOf',
	'__defineGetter__', '__defineSetter__', '__lookupGetter__', '__lookupSetter__', '__proto__',
];

export class Item {
	constructor() {
		this.$typeName = 'Item';
		this.$parent = null;
		this.$children = [];
		this.$x = 0;
		this.$y = 0;
		this.$w = 0;
		this.$h = 0;
		this.$z = 0;
		this.$opacity = 1;
		this.$rotation = 0;
		this.$scale = 1;
		this.$origin = TRANSFORM_ORIGIN.Center;
		this.$visible = true;
		this.$clip = false;
		this.$focus = false;
		this.$isFocusScope = false;
		this.$scopeFocus = null;
		this.$enabled = true;
		this.$objectName = '';
		this.$tabFocus = false;
		this.$iw = NaN; // implicitWidth / implicitHeight assigned by a script; NaN: the natural size of the type
		this.$ih = NaN;
		this.$anchors = null;
		this.$ax = 0;
		this.$ay = 0;
		this.$aw = 0;
		this.$ah = 0;
		this.$gx = NaN; // the geometry the readers of x, y, width and height were last told about
		this.$gy = NaN;
		this.$gw = NaN;
		this.$gh = NaN;
		this.$sx = 0;
		this.$sy = 0;
		this.$state = 0; // 0 pending, 1 resolving, 2 resolved
		this.$updateRequested = true;
		this.$layoutNeeded = true;
		this.$cleanEpoch = -1;
		// Incremental layout: $pass asks for a pass over the children, $dirtyKids lists the children (queued) that have a
		// pending pass somewhere below them, $lw..$lsy are the geometry the children were last laid out for.
		this.$pass = true;
		this.$dirtyKids = null;
		this.$queued = false;
		this.$lw = NaN;
		this.$lh = NaN;
		this.$lsx = NaN;
		this.$lsy = NaN;
		this.$nonVisual = false;
		this.$open = false; // lenient mode: unknown properties become plain custom properties

		// Set by the engine.
		this.$id = '';
		this.$scope = null;
		this.$directory = '';
		this.$sigs = null;
		this.$declareSignal('focusChanged', []);
		this.$declareSignal('activeFocusChanged', []);
		this.$bindings = null;
		this.$ownerIndex = -1;
		this.$constructing = false;
		this.$custom = null;
		this.$functions = null;
		this.$pendingAliasWrites = null;
		this.$completed = null;
	}

	// ----- tree -----

	$adopt(child) {
		child.$parent = this;
		markReparented(child);
		if (this.$children.indexOf(child) < 0) this.$children.push(child);
		this.$update();
	}

	$clearChildren() {
		for (const child of this.$children) child.$destroy();
		this.$children = [];
		this.$update();
	}

	$removeChild(child) {
		const index = this.$children.indexOf(child);
		if (index >= 0) {
			this.$children.splice(index, 1);
			child.$destroy();
			this.$update();
		}
	}

	// QML `item.destroy()`: releases the item's bindings and handlers and removes it from its parent.
	destroy() {
		if (this.$destroyed) return;
		if (hooks.release !== null) hooks.release(this);
		if (this.$parent !== null) this.$parent.$removeChild(this);
		else this.$destroy();
	}

	$destroy() {
		for (const child of this.$children) child.$destroy();
		if (this.$owned !== undefined) for (const owned of this.$owned) owned.$destroy();
		this.$destroyed = true;
		if (destroyObserver !== null) destroyObserver(this);
	}

	// Requests a new layout and repaint. This item needs a pass over its children, and so does its parent (it has to
	// place this item again); a Stack or Loader parent may itself change size, so its parent is asked too. The chain up
	// to the root is queued, so the next layout finds this item without walking the whole tree.
	$update() {
		layoutEpoch++;
		this.$pass = true;
		let item = this;
		let placeAgain = true;
		while (item.$parent !== null) {
			const parent = item.$parent;
			if (placeAgain) {
				parent.$pass = true;
				placeAgain = parent.$contentSized;
			}
			if (!item.$queued) {
				item.$queued = true;
				if (parent.$dirtyKids === null) parent.$dirtyKids = [item];
				else parent.$dirtyKids.push(item);
			}
			item = parent;
		}
		item.$layoutNeeded = true;
		if (hooks.quiet > 0 && this.visible === false) {
			hooks.quietDirty = true;
			return;
		}
		item.$updateRequested = true;
	}

	// A script that changes geometry and then reads it must see the new anchors. One layout at a time.
	$ensureGeometry() {
		if (layingOut || this.$cleanEpoch === layoutEpoch) return;
		let root = this;
		while (root.$parent !== null) {
			// Rebinding a pooled ListView delegate must not nest a full layout mid-$arrange
			// (that duplicated children and stacked rows on scroll).
			if (root.$arranging === true) return;
			root = root.$parent;
		}
		if (root.$arranging === true) return;
		if (root.$constructing) return;
		if (!root.$layoutNeeded) {
			this.$cleanEpoch = layoutEpoch;
			return;
		}
		// A read in a script must not swallow the frame that still has to evaluate the bindings.
		if (frameLoop) root.$updateRequested = false;
		const epoch = layoutEpoch;
		root.$layout();
		if (layoutEpoch === epoch) this.$cleanEpoch = epoch;
	}

	// A loaded item grew: the Loader follows on the axes it does not size itself, and the item's own handler runs after that.
	$notifyLoaderSize(name) {
		const parent = this.$parent;
		if (parent === null || parent.$typeName !== 'Loader' || parent.$sizing === true) return;
		parent.$adoptItemSize();
		if (hooks.changed !== null) hooks.changed(this, name);
	}

	$takeUpdateRequest() {
		const requested = this.$updateRequested;
		this.$updateRequested = false;
		return requested;
	}

	// ----- signals -----

	$declareSignal(name, params) {
		if (this.$sigs === null) this.$sigs = new Map();
		let signal = this.$sigs.get(name);
		if (signal === undefined) {
			signal = { params, handlers: [] };
			this.$sigs.set(name, signal);
		} else {
			signal.params = params;
		}
		// A property of the same name (MouseArea.pressed) stays a property.
		if (!(name in this)) {
			const emit = (...args) => this.$emit(name, args);
			// signal.connect(fn) / signal.disconnect(fn)
			emit.connect = (fn) => {
				if (typeof fn !== 'function') return;
				signal.handlers.push(fn);
				linkToOwner(this, fn, () => emit.disconnect(fn));
			};
			emit.disconnect = (fn) => {
				const at = signal.handlers.indexOf(fn);
				if (at >= 0) signal.handlers.splice(at, 1);
			};
			Object.defineProperty(this, name, {
				value: emit,
				enumerable: true,
				configurable: true,
				writable: true,
			});
		}
		return signal;
	}

	$connect(name, handler) {
		const signal = this.$sigs === null ? undefined : this.$sigs.get(name);
		if (signal === undefined) return false;
		signal.handlers.push(handler);
		return true;
	}

	$emit(name, args = []) {
		const signal = this.$sigs === null ? undefined : this.$sigs.get(name);
		if (signal === undefined || signal.handlers.length === 0 || this.$blocked) return;
		if (hooks.defer !== null && hooks.defer(this, name, args)) return;
		// A handler is a script even when the signal comes out of a value the engine is writing: what it assigns
		// replaces bindings (a cell resized by `widthRecalc` must not be reset by its own old binding).
		const engineWrite = hooks.engineWrite;
		hooks.engineWrite = 0;
		try {
			for (const handler of signal.handlers.slice()) handler(...args);
		} finally {
			hooks.engineWrite = engineWrite;
		}
	}

	// QObject::blockSignals: returns the previous state.
	blockSignals(block) {
		const previous = this.$blocked === true;
		this.$blocked = Boolean(block);
		return previous;
	}

	signalsBlocked() {
		return this.$blocked === true;
	}

	// ----- custom properties -----

	$defineCustom(name, initial, coerce = null, readonly = false) {
		const store = this.$custom === null ? (this.$custom = Object.create(null)) : this.$custom;
		store[name] = initial;
		Object.defineProperty(this, name, {
			get() {
				noteRead(this, name, true);
				return store[name];
			},
			set(value) {
				if (value instanceof BindingValue) {
					if (readonly) throw new QmlError(`Cannot assign a binding to read-only property '${name}'`);
					hooks.installBinding(this, [name], value.fn);
					return;
				}
				// The declaration's own initializer and its binding updates are engine writes. A script assignment is not.
				if (readonly && hooks.engineWrite === 0) throw new QmlError(`Cannot assign to read-only property '${name}'`);
				if (coerce !== null) value = coerce(value);
				scriptWrite(this, name);
				if (!same(store[name], value)) {
					store[name] = value;
					this.$update();
					if (hooks.changed !== null && this.$chg !== undefined) hooks.changed(this, name);
					noteWrite(this, name);
				}
				// A Binding or Connections item has to publish the new value before the statement that caused it returns.
				if (this.$syncing === true) return;
				if (this.$bind !== undefined && (name === 'target' || name === 'property' || name === 'value' || name === 'when') && hooks.syncBinding !== null) {
					this.$syncing = true;
					try {
						hooks.syncBinding(this);
					} finally {
						this.$syncing = false;
					}
				} else if (this.$conn !== undefined && name === 'target' && hooks.syncConnection !== null) {
					this.$syncing = true;
					try {
						hooks.syncConnection(this);
					} finally {
						this.$syncing = false;
					}
				}
			},
			enumerable: true,
			configurable: true,
		});
	}

	// ----- geometry -----

	get anchors() {
		return this.$anchors !== null ? this.$anchors : (this.$anchors = new Anchors(this));
	}

	// Attached `Layout.*` (preferred size, fill, alignment). Created on first use.
	get Layout() {
		return this.$lay !== undefined ? this.$lay : (this.$lay = new LayoutAttached(this));
	}

	get $implicitWidth() {
		return 0;
	}

	get $implicitHeight() {
		return 0;
	}

	// The implicit size in use: the assigned one, else the natural one.
	get $effW() {
		return this.$iw === this.$iw ? this.$iw : this.$implicitWidth;
	}

	get $effH() {
		return this.$ih === this.$ih ? this.$ih : this.$implicitHeight;
	}

	// Geometry of the root of a layout pass: the position and size properties.
	$layout() {
		if (layingOut) return;
		layingOut = true;
		this.$layoutNeeded = false;
		try {
			this.$ax = this.$x;
			this.$ay = this.$y;
			this.$aw = this.$w;
			this.$ah = this.$h;
			this.$sx = this.$ax;
			this.$sy = this.$ay;
			this.$state = 2;
			this.$commitGeometry();
			this.$visit();
		} finally {
			layingOut = false;
		}
	}

	// The bindings that read x, y, width or height of this item run again when the laid-out value differs from the one
	// they were last told. The shadow fields are always updated, or a later reader would miss a change back.
	$commitGeometry() {
		if (!Object.is(this.$ax, this.$gx)) {
			this.$gx = this.$ax;
			markReaders(this, 'x');
		}
		if (!Object.is(this.$ay, this.$gy)) {
			this.$gy = this.$ay;
			markReaders(this, 'y');
		}
		if (!Object.is(this.$aw, this.$gw)) {
			this.$gw = this.$aw;
			markReaders(this, 'width');
		}
		if (!Object.is(this.$ah, this.$gh)) {
			this.$gh = this.$ah;
			markReaders(this, 'height');
		}
	}

	// Brings the geometry below this (resolved) item up to date. A part that did not change, keeps its size and
	// position and has nothing queued below it is skipped.
	$visit() {
		if (this.$destroyed) return;
		if (this.$pass || this.$aw !== this.$lw || this.$ah !== this.$lh || this.$sx !== this.$lsx || this.$sy !== this.$lsy) {
			this.$layoutChildren();
			return;
		}
		const kids = this.$dirtyKids;
		if (kids === null || kids.length === 0) return;
		// Kids queued while these are visited wait for the next layout.
		const count = kids.length;
		for (let i = 0; i < count; i++) {
			const kid = kids[i];
			kid.$queued = false;
			kid.$visit();
		}
		kids.splice(0, count);
	}

	$layoutChildren() {
		this.$pass = false;
		this.$lw = this.$aw;
		this.$lh = this.$ah;
		this.$lsx = this.$sx;
		this.$lsy = this.$sy;
		const kids = this.$dirtyKids;
		if (kids !== null && kids.length > 0) {
			for (let i = 0; i < kids.length; i++) kids[i].$queued = false;
			kids.length = 0;
		}
		// What the parent resolved this item with; if the children change it, the parent has to resolve it again.
		const sized = this.$contentSized;
		const w0 = sized ? this.$effW : 0;
		const h0 = sized ? this.$effH : 0;
		const children = this.$children;
		for (let i = 0; i < children.length; i++) children[i].$state = 0;
		for (let i = 0; i < children.length; i++) children[i].$resolve();
		this.$arrange();
		const list = this.$children;
		const ox = this.$ox;
		const oy = this.$oy;
		for (let i = 0; i < list.length; i++) {
			const child = list[i];
			child.$sx = this.$sx + child.$ax - ox;
			child.$sy = this.$sy + child.$ay - oy;
			child.$commitGeometry();
			child.$visit();
		}
		if (sized && (this.$effW !== w0 || this.$effH !== h0)) this.$update();
	}

	// Scroll offset of the children (Flickable).
	get $ox() {
		return 0;
	}

	get $oy() {
		return 0;
	}

	$arrange() {}

	// Applies anchors to the position and size of this item, in the coordinates of its parent.
	$resolve() {
		if (this.$state !== 0) return;
		this.$state = 1;

		const an = this.$anchors;
		let w = this.$w > 0 ? this.$w : this.$effW;
		let h = this.$h > 0 ? this.$h : this.$effH;
		let x = this.$x;
		let y = this.$y;

		if (an !== null && an.$active) {
			let hs = NaN;
			let he = NaN;
			let hc = NaN;
			let vs = NaN;
			let ve = NaN;
			let vc = NaN;
			if (an.$fill !== null) {
				hs = this.$lineValue(an.$fill, 0);
				he = this.$lineValue(an.$fill, 1);
				vs = this.$lineValue(an.$fill, 2);
				ve = this.$lineValue(an.$fill, 3);
			} else {
				if (an.$left !== null) hs = this.$lineValue(an.$left.item, an.$left.edge);
				if (an.$right !== null) he = this.$lineValue(an.$right.item, an.$right.edge);
				if (an.$top !== null) vs = this.$lineValue(an.$top.item, an.$top.edge);
				if (an.$bottom !== null) ve = this.$lineValue(an.$bottom.item, an.$bottom.edge);
			}
			if (an.$centerIn !== null) {
				hc = this.$lineValue(an.$centerIn, 4);
				vc = this.$lineValue(an.$centerIn, 5);
			} else {
				if (an.$hcenter !== null) hc = this.$lineValue(an.$hcenter.item, an.$hcenter.edge);
				if (an.$vcenter !== null) vc = this.$lineValue(an.$vcenter.item, an.$vcenter.edge);
			}

			if (hs === hs && he === he) {
				x = hs + an.$lm;
				w = Math.max(0, he - an.$rm - x);
			} else if (hs === hs) {
				x = hs + an.$lm;
			} else if (he === he) {
				x = he - an.$rm - w;
			} else if (hc === hc) {
				x = hc - w / 2 + an.$hco;
			}

			if (vs === vs && ve === ve) {
				y = vs + an.$tm;
				h = Math.max(0, ve - an.$bm - y);
			} else if (vs === vs) {
				y = vs + an.$tm;
			} else if (ve === ve) {
				y = ve - an.$bm - h;
			} else if (vc === vc) {
				y = vc - h / 2 + an.$vco;
			}
		}

		this.$ax = x;
		this.$ay = y;
		this.$aw = w;
		this.$ah = h;
		this.$state = 2;
	}

	// An anchor line of the parent or of a sibling in the coordinates of the parent; NaN if invalid.
	$lineValue(target, edge) {
		const parent = this.$parent;
		if (target instanceof ViewContent && target.$view === parent) target = parent;
		if (target === null || parent === null) return NaN;
		let tx = 0;
		let ty = 0;
		let tw = 0;
		let th = 0;
		if (target === parent) {
			tw = target.$aw;
			th = target.$ah;
		} else if (target.$parent === parent && target !== this) {
			target.$resolve();
			tx = target.$ax;
			ty = target.$ay;
			tw = target.$aw;
			th = target.$ah;
		} else {
			return NaN;
		}
		switch (edge) {
			case 0: return tx;
			case 1: return tx + tw;
			case 2: return ty;
			case 3: return ty + th;
			case 4: return tx + tw / 2;
			default: return ty + th / 2;
		}
	}

	// ----- drawing -----

	$render(r, parentOpacity) {
		if (!this.$visible) return;
		const opacity = parentOpacity * this.$opacity;
		const a = r.ma, b = r.mb, c = r.mc, d = r.md, e = r.me, f = r.mf;
		r.translate(this.$ax, this.$ay);
		if (this.$rotation !== 0 || this.$scale !== 1) {
			if (this.$scale !== 0) {
				const fraction = ORIGIN_FRACTIONS[this.$origin];
				const radians = (this.$rotation * Math.PI) / 180;
				r.rotateScaleAround(fraction[0] * this.$aw, fraction[1] * this.$ah, Math.cos(radians), Math.sin(radians), this.$scale);
				this.$renderContent(r, opacity);
			}
		} else {
			this.$renderContent(r, opacity);
		}
		r.setMatrix(a, b, c, d, e, f);
	}

	$renderContent(r, opacity) {
		this.$paint(r, opacity);

		let clipToken = null;
		if (this.$clip) {
			clipToken = r.beginClip(this.$aw, this.$ah);
			if (clipToken === null) return; // nothing of the subtree can be visible
		}

		const children = this.$children;
		if (!this.$hasZOrder()) {
			for (let i = 0; i < children.length; i++) children[i].$render(r, opacity);
		} else {
			for (const child of this.$childrenByZ()) child.$render(r, opacity);
		}
		this.$paintOverlay(r, opacity);

		if (clipToken !== null) r.endClip(clipToken);
	}

	$paint(r, opacity) {}

	$paintOverlay(r, opacity) {}

	// Sorting is only needed when some child has a non-default z.
	$hasZOrder() {
		const children = this.$children;
		for (let i = 0; i < children.length; i++) {
			if (children[i].$z !== 0) return true;
		}
		return false;
	}

	$childrenByZ() {
		const ordered = this.$children.map((child, index) => ({ child, index }));
		ordered.sort((l, rr) => l.child.$z - rr.child.$z || l.index - rr.index);
		return ordered.map((entry) => entry.child);
	}

	// ----- input -----

	// The topmost visible item that accepts mouse (or wheel) input at a scene position. `button` is the pressed one
	// (0 left, 2 right); -1 is a pointer that only moves.
	$itemAt(px, py, wheel = false, button = -1) {
		if (!this.$visible) return null;

		if (this.$rotation !== 0 || this.$scale !== 1) {
			if (this.$scale === 0) return null;
			const fraction = ORIGIN_FRACTIONS[this.$origin];
			const ox = this.$sx + fraction[0] * this.$aw;
			const oy = this.$sy + fraction[1] * this.$ah;
			const radians = (this.$rotation * Math.PI) / 180;
			const cos = Math.cos(radians);
			const sin = Math.sin(radians);
			const dx = px - ox;
			const dy = py - oy;
			px = ox + (dx * cos + dy * sin) / this.$scale;
			py = oy + (-dx * sin + dy * cos) / this.$scale;
		}

		const inside = px >= this.$sx && py >= this.$sy && px < this.$sx + this.$aw && py < this.$sy + this.$ah;
		if (this.$clip && !inside) return null;

		const children = this.$children;
		if (!this.$hasZOrder()) {
			for (let i = children.length - 1; i >= 0; i--) {
				const hit = children[i].$itemAt(px, py, wheel, button);
				if (hit !== null) return hit;
			}
		} else {
			const ordered = this.$childrenByZ();
			for (let i = ordered.length - 1; i >= 0; i--) {
				const hit = ordered[i].$itemAt(px, py, wheel, button);
				if (hit !== null) return hit;
			}
		}
		return (wheel ? this.$acceptsWheel : this.$acceptsButton(button)) && inside ? this : null;
	}

	get $acceptsMouse() {
		return false;
	}

	$acceptsButton(button) {
		return button <= 0 && this.$acceptsMouse;
	}

	get $acceptsWheel() {
		return false;
	}

	$mouseWheel(notches) {}

	forceActiveFocus() {
		const gen = ++forceGen;
		setItemFocus(this, true, gen);
		// Qt: ListView is a FocusScope and may set `focus: true` on the current row without
		// giving that row activeFocus until the view itself is in the active chain. A later
		// forceActiveFocus on that row (e.g. control chrome rebuild) must not climb out of the
		// still-inactive view — that would activate the row when the dialog/popup takes focus.
		const list = enclosingListView(this);
		const scopedListCurrent =
			list !== null && list.$scopeFocus === this && !list.$focus && (activeLeaf === null || !focusHostOwnsList(list, activeLeaf));
		if (!scopedListCurrent) {
			let parent = this.$parent;
			while (parent !== null) {
				if (parent.$isFocusScope && nearestScope(parent) !== null) setItemFocus(parent, true, gen);
				parent = parent.$parent;
			}
		}
		// A handler may already have moved the focus to another item.
		if (!sceneActive || !this.$focus) return;
		if (scopedListCurrent) return;
		activeLeaf = focusLeaf(this);
		refreshActive();
	}

	// Tab order: enabled, effectively visible items that take the focus by Tab (activeFocusOnTab, or a `focusPolicy` with
	// Qt.TabFocus), depth-first, wrapping.
	nextItemInFocusChain(forward = true) {
		let root = this;
		while (root.$parent !== null) root = root.$parent;
		const order = [];
		const walk = (item) => {
			order.push(item);
			for (const child of item.$children) walk(child);
		};
		walk(root);
		const chain = order.filter((item) => takesTabFocus(item) && item.visible && itemEnabled(item));
		if (chain.length === 0) return null;
		const at = order.indexOf(this);
		if (forward !== false) {
			for (let i = at + 1; i < order.length; i++) if (chain.includes(order[i])) return order[i];
			return chain[0];
		}
		for (let i = at - 1; i >= 0; i--) if (chain.includes(order[i])) return order[i];
		return chain[chain.length - 1];
	}

	// The JQML runtime hook of a finished item: nothing to do here.
	$complete() {}

	mapToItem(item, x, y) {
		readUntracked('mapToItem()');
		const target = item || null;
		this.$ensureGeometry();
		if (target !== null) target.$ensureGeometry();
		let ox = 0;
		let oy = 0;
		if (target !== null) {
			ox = target.$sx;
			oy = target.$sy;
		} else {
			// Qt: null maps into the root QML / window content item, not chrome above it.
			const origin = hooks.mapOrigin !== null ? hooks.mapOrigin() : null;
			if (origin !== null && origin !== undefined) {
				ox = origin.$sx;
				oy = origin.$sy;
			}
		}
		return { x: this.$sx + x - ox, y: this.$sy + y - oy };
	}

	mapFromItem(item, x, y) {
		readUntracked('mapFromItem()');
		const source = item || null;
		this.$ensureGeometry();
		if (source !== null) source.$ensureGeometry();
		let ox = 0;
		let oy = 0;
		if (source !== null) {
			ox = source.$sx;
			oy = source.$sy;
		} else {
			const origin = hooks.mapOrigin !== null ? hooks.mapOrigin() : null;
			if (origin !== null && origin !== undefined) {
				ox = origin.$sx;
				oy = origin.$sy;
			}
		}
		return { x: ox + x - this.$sx, y: oy + y - this.$sy };
	}

	mapToGlobal(x, y) {
		readUntracked('mapToGlobal()');
		this.$ensureGeometry();
		return { x: this.$sx + x, y: this.$sy + y };
	}

	$mousePressAt(sceneX, sceneY) {}

	$mouseMoveAt(sceneX, sceneY) {}

	$hoverChanged(hovered) {}

	$pressChanged(pressed) {}

	$mouseClicked() {}
}

// Types whose own size follows their children: a change below them changes how their parent places them.
Item.prototype.$contentSized = false;

Object.defineProperty(Item.prototype, Symbol.unscopables, {
	value: Object.fromEntries(OBJECT_NAMES_UNSCOPABLE.map((name) => [name, true])),
});

defineProp(
	Item.prototype,
	'x',
	function () {
		this.$ensureGeometry();
		return this.$anchors !== null && this.$anchors.$affectsX ? this.$ax : this.$x;
	},
	function (v) {
		v = toFloat(v);
		if (this.$x !== v) {
			this.$x = v;
			this.$update();
			notifyGeometry(this, 'x');
		}
	},
	true,
);
defineProp(
	Item.prototype,
	'y',
	function () {
		this.$ensureGeometry();
		return this.$anchors !== null && this.$anchors.$affectsY ? this.$ay : this.$y;
	},
	function (v) {
		v = toFloat(v);
		if (this.$y !== v) {
			this.$y = v;
			this.$update();
			notifyGeometry(this, 'y');
		}
	},
	true,
);
defineProp(
	Item.prototype,
	'width',
	function () {
		this.$ensureGeometry();
		// `$w === 0` means unset (use the laid-out `$aw`). An explicit negative width must be
		// readable as-is: ButtonDecorator cancels contentSpacing with `width: -Style.marginM`.
		if (this.$anchors !== null && this.$anchors.$affectsWidth) return this.$aw;
		if (this.$w < 0) return this.$w;
		return this.$w <= 0 ? this.$aw : this.$w;
	},
	function (v) {
		v = toFloat(v);
		if (this.$w !== v) {
			this.$w = v;
			this.$update();
			this.$notifyLoaderSize('width');
			notifyGeometry(this, 'width');
		}
	},
	true,
);
defineProp(
	Item.prototype,
	'height',
	function () {
		this.$ensureGeometry();
		if (this.$anchors !== null && this.$anchors.$affectsHeight) return this.$ah;
		if (this.$h < 0) return this.$h;
		return this.$h <= 0 ? this.$ah : this.$h;
	},
	function (v) {
		v = toFloat(v);
		if (this.$h !== v) {
			this.$h = v;
			this.$update();
			this.$notifyLoaderSize('height');
			notifyGeometry(this, 'height');
		}
	},
	true,
);
defineField(Item.prototype, 'z', '$z', toFloat);
defineField(Item.prototype, 'opacity', '$opacity', toFloat);
defineField(Item.prototype, 'rotation', '$rotation', toFloat);
defineField(Item.prototype, 'scale', '$scale', toFloat);
defineField(Item.prototype, 'transformOrigin', '$origin', toTransformOrigin);
// `visible` is the effective value: an item is hidden when it or any ancestor is hidden.
defineProp(
	Item.prototype,
	'visible',
	function () {
		for (let item = this; item !== null; item = item.$parent) {
			// The reader follows the `visible` of every ancestor it looked at.
			if (item !== this) noteRead(item, 'visible', true);
			if (!item.$visible) return false;
		}
		return true;
	},
	function (value) {
		const v = toBool(value);
		if (this.$visible !== v) {
			this.$visible = v;
			// Showing or hiding is always seen, also when a hidden animation's handler does it.
			const quiet = hooks.quiet;
			hooks.quiet = 0;
			try {
				this.$update();
			} finally {
				hooks.quiet = quiet;
			}
		}
	},
	true,
);
defineField(Item.prototype, 'clip', '$clip', toBool);
for (const [name, field, natural] of [['implicitWidth', '$iw', '$effW'], ['implicitHeight', '$ih', '$effH']]) {
	defineProp(
		Item.prototype,
		name,
		function () {
			return this[natural];
		},
		function (value) {
			const v = Number(value);
			if (!same(this[field], v)) {
				this[field] = v;
				this.$update();
			}
		},
	);
}
// `focus` is the focused child of the enclosing focus scope. The first declarative
// `focus: true` wins; a later assignment takes it. forceActiveFocus() also focuses ancestor scopes.
defineProp(
	Item.prototype,
	'focus',
	function () {
		return this.$focus;
	},
	function (value) {
		const v = toBool(value);
		if (this.$constructing) {
			this.$focus = v;
			return;
		}
		setItemFocus(this, v, false);
	},
	true,
);
defineReadOnly(
	Item.prototype,
	'activeFocus',
	function () {
		return activeSet.has(this);
	},
	undefined,
	true,
);
defineProp(
	Item.prototype,
	'enabled',
	function () {
		for (let item = this; item !== null; item = item.$parent) {
			if (item !== this) noteRead(item, 'enabled', true);
			if (item.$enabled === false) return false;
		}
		return true;
	},
	function (value) {
		const v = toBool(value);
		if (this.$enabled !== v) {
			this.$enabled = v;
			if (!v && activeLeaf !== null) refreshActive();
		}
	},
	true,
);
defineProp(
	Item.prototype,
	'objectName',
	function () {
		return this.$objectName;
	},
	function (value) {
		this.$objectName = value === undefined || value === null ? '' : String(value);
	},
);
defineProp(
	Item.prototype,
	'activeFocusOnTab',
	function () {
		return this.$tabFocus;
	},
	function (value) {
		this.$tabFocus = toBool(value);
	},
);

// KeyNavigation holds item references; the engine moves the focus to them when a key reaches the item.
class KeyNavGroup {
	constructor(item) {
		this.$owner = item;
		this.tab = null;
		this.backtab = null;
		this.left = null;
		this.right = null;
		this.up = null;
		this.down = null;
	}
}
Object.defineProperty(Item.prototype, 'KeyNavigation', {
	get() {
		if (this.$keyNav === undefined) this.$keyNav = new KeyNavGroup(this);
		return this.$keyNav;
	},
	enumerable: true,
	configurable: true,
});
// Assigning a list reparents the items: the listed items become the children, the others are detached.
defineProp(
	Item.prototype,
	'children',
	function () {
		return this.$children.some((child) => child.$nonVisual) ? this.$children.filter((child) => !child.$nonVisual) : this.$children;
	},
	function (list) {
		const items = Array.from(list || []).filter((item) => item instanceof Item && item !== this);
		for (const child of this.$children) {
			if (!child.$nonVisual && !items.includes(child)) {
				child.$parent = null;
				markReparented(child);
			}
		}
		for (const item of items) {
			if (item.$parent !== null && item.$parent !== this) {
				const siblings = item.$parent.$children;
				const at = siblings.indexOf(item);
				if (at >= 0) siblings.splice(at, 1);
				item.$parent.$update();
			}
			item.$parent = this;
			markReparented(item);
		}
		this.$children = items.concat(this.$children.filter((child) => child.$nonVisual && !items.includes(child)));
		this.$update();
	},
);
// Item's default property. One object is parented here; a list replaces the direct children.
defineProp(
	Item.prototype,
	'data',
	function () {
		return this.children;
	},
	function (value) {
		if (Array.isArray(value)) {
			this.children = value;
			return;
		}
		if (value instanceof Item) value.parent = this;
	},
);
// Assigning the parent moves the item below another item (or detaches it).
defineProp(
	Item.prototype,
	'parent',
	function () {
		const parent = this.$parent;
		return parent instanceof ListView && !(this instanceof ViewContent) ? parent.contentItem : parent;
	},
	function (value) {
		const target = value instanceof Item && value !== this ? value : null;
		if (target === this.$parent) return;
		const old = this.$parent;
		if (old !== null) {
			const at = old.$children.indexOf(this);
			if (at >= 0) old.$children.splice(at, 1);
			old.$update();
		}
		this.$parent = null;
		if (target !== null) target.$adopt(this);
		this.$update();
	},
	true,
);
for (const [name, edge] of [['left', 0], ['right', 1], ['top', 2], ['bottom', 3], ['horizontalCenter', 4], ['verticalCenter', 5]]) {
	defineReadOnly(
		Item.prototype,
		name,
		function () {
			return new AnchorLine(this, edge);
		},
		undefined,
		true,
	);
}

// ----- Rectangle -----

class BorderGroup {
	constructor(owner) {
		this.$owner = owner;
	}
}
BorderGroup.prototype.$group = 'border';

defineGroupProp(
	BorderGroup.prototype,
	'border',
	'color',
	function () {
		return colorToString(this.$owner.$borderColor);
	},
	function (v) {
		this.$owner.$borderColor = toColor(v);
		this.$owner.$penValid = this.$owner.$borderColor.a > 0;
		this.$owner.$update();
	},
);
defineGroupProp(
	BorderGroup.prototype,
	'border',
	'width',
	function () {
		return this.$owner.$borderWidth;
	},
	function (v) {
		v = toFloat(v);
		const owner = this.$owner;
		owner.$penValid = owner.$borderColor.a > 0 && Math.round(v) >= 1;
		if (owner.$borderWidth !== v) owner.$borderWidth = v;
		owner.$update();
	},
);

const ROUND_SEGMENTS = 8;

function roundedOutline(x, y, w, h, radius) {
	const points = [];
	const centers = [
		[x + w - radius, y + radius],
		[x + w - radius, y + h - radius],
		[x + radius, y + h - radius],
		[x + radius, y + radius],
	];
	for (let corner = 0; corner < 4; corner++) {
		const start = ((-90 + corner * 90) * Math.PI) / 180;
		for (let s = 0; s <= ROUND_SEGMENTS; s++) {
			const angle = start + ((Math.PI / 2) * s) / ROUND_SEGMENTS;
			const px = centers[corner][0] + radius * Math.cos(angle);
			const py = centers[corner][1] + radius * Math.sin(angle);
			const n = points.length;
			if (n === 0 || (px - points[n - 2]) ** 2 + (py - points[n - 1]) ** 2 > 1e-8) {
				points.push(px, py);
			}
		}
	}
	const n = points.length;
	if (n > 2 && (points[0] - points[n - 2]) ** 2 + (points[1] - points[n - 1]) ** 2 <= 1e-8) {
		points.length = n - 2;
	}
	return points;
}

function fillRounded(r, color, x, y, w, h, radius, aa) {
	if (w <= 0 || h <= 0) return;
	radius = Math.min(Math.max(radius, 0), Math.min(w, h) / 2);
	if (radius <= 0) {
		r.fillQuad(x, y, w, h, color);
		return;
	}
	r.fillPolygon(roundedOutline(x, y, w, h, radius), color, aa);
}

// Keeps the part of a convex polygon (flat x, y list) whose gradient coordinate t = (c - origin) * inv is
// above (or below) `limit`.
function clipConvex(points, horizontal, origin, inv, limit, keepAbove) {
	const out = [];
	const n = points.length / 2;
	for (let i = 0; i < n; i++) {
		const j = (i + 1) % n;
		const ax = points[2 * i];
		const ay = points[2 * i + 1];
		const bx = points[2 * j];
		const by = points[2 * j + 1];
		const ta = ((horizontal ? ax : ay) - origin) * inv;
		const tb = ((horizontal ? bx : by) - origin) * inv;
		const ina = keepAbove ? ta >= limit : ta <= limit;
		const inb = keepAbove ? tb >= limit : tb <= limit;
		if (ina) out.push(ax, ay);
		if (ina !== inb) {
			const s = (limit - ta) / (tb - ta);
			out.push(ax + (bx - ax) * s, ay + (by - ay) * s);
		}
	}
	return out;
}

function mixColors(a, b, f) {
	return { R: Math.round(a.R + (b.R - a.R) * f), G: Math.round(a.G + (b.G - a.G) * f), B: Math.round(a.B + (b.B - a.B) * f), a: a.a + (b.a - a.a) * f };
}

// The border of a rectangle as a ring (the fill is drawn separately, so a see-through fill shows what is behind).
function fillRing(r, color, x, y, w, h, radius, border, aa) {
	if (radius <= 0) {
		const quad = aa ? (qx, qy, qw, qh) => r.fillQuadAA(qx, qy, qw, qh, color) : (qx, qy, qw, qh) => r.fillQuad(qx, qy, qw, qh, color);
		quad(x, y, w, border);
		quad(x, y + h - border, w, border);
		quad(x, y + border, border, h - 2 * border);
		quad(x + w - border, y + border, border, h - 2 * border);
		return;
	}
	const inner = Math.max(radius - border, 0);
	const centers = [
		[x + w - radius, y + radius],
		[x + w - radius, y + h - radius],
		[x + radius, y + h - radius],
		[x + radius, y + radius],
	];
	const outerPoints = [];
	const innerPoints = [];
	for (let corner = 0; corner < 4; corner++) {
		const start = ((-90 + corner * 90) * Math.PI) / 180;
		for (let s = 0; s <= ROUND_SEGMENTS; s++) {
			const angle = start + ((Math.PI / 2) * s) / ROUND_SEGMENTS;
			const cos = Math.cos(angle);
			const sin = Math.sin(angle);
			outerPoints.push([centers[corner][0] + radius * cos, centers[corner][1] + radius * sin]);
			innerPoints.push([centers[corner][0] + inner * cos, centers[corner][1] + inner * sin]);
		}
	}
	const n = outerPoints.length;
	for (let i = 0; i < n; i++) {
		const j = i + 1 === n ? 0 : i + 1;
		r.fillPolygon([...outerPoints[i], ...outerPoints[j], ...innerPoints[j], ...innerPoints[i]], color, aa);
	}
}

export function drawRectangle(r, colorRgba, borderRgba, borderWidth, radius, aa, x, y, w, h) {
	const border = Math.min(borderWidth, w / 2, h / 2);
	if (border > 0 && colorRgba >>> 24 < 255) {
		fillRing(r, borderRgba, x, y, w, h, Math.min(radius, w / 2, h / 2), border, aa);
		if (colorRgba >>> 24 !== 0) {
			const ix = x + border;
			const iy = y + border;
			const iw = w - 2 * border;
			const ih = h - 2 * border;
			if (radius > 0) fillRounded(r, colorRgba, ix, iy, iw, ih, Math.max(radius - border, 0), aa);
			else if (aa) r.fillQuadAA(ix, iy, iw, ih, colorRgba);
			else r.fillQuad(ix, iy, iw, ih, colorRgba);
		}
		return;
	}
	if (radius > 0) {
		fillRounded(r, border > 0 ? borderRgba : colorRgba, x, y, w, h, radius, aa);
		if (border > 0) {
			fillRounded(r, colorRgba, x + border, y + border, w - 2 * border, h - 2 * border, Math.max(radius - border, 0), aa);
		}
		return;
	}
	if (aa) {
		if (border > 0) {
			r.fillQuadAA(x, y, w, h, borderRgba);
			r.fillQuad(x + border, y + border, w - 2 * border, h - 2 * border, colorRgba);
		} else {
			r.fillQuadAA(x, y, w, h, colorRgba);
		}
		return;
	}
	r.fillQuad(x, y, w, h, colorRgba);
	if (border > 0) {
		r.fillQuad(x, y, w, border, borderRgba);
		r.fillQuad(x, y + h - border, w, border, borderRgba);
		r.fillQuad(x, y + border, border, h - 2 * border, borderRgba);
		r.fillQuad(x + w - border, y + border, border, h - 2 * border, borderRgba);
	}
}

export class Rectangle extends Item {
	constructor() {
		super();
		this.$typeName = 'Rectangle';
		this.$color = WHITE;
		this.$borderColor = BLACK;
		this.$borderWidth = 1;
		this.$penValid = false; // the border is drawn once a visible color or a width of 1 or more has been set, as in Qt
		this.$radius = 0;
		this.$aaSpecified = false;
		this.$aaValue = false;
		this.$border = null;
		this.$gradient = null;
	}

	get border() {
		return this.$border !== null ? this.$border : (this.$border = new BorderGroup(this));
	}

	get $drawnBorder() {
		return this.$penValid ? this.$borderWidth : 0;
	}

	get $antialiasing() {
		return this.$aaSpecified ? this.$aaValue : this.$radius > 0;
	}

	// A gradient fills the rectangle slice by slice (one slice between two stops, clipped to the outline).
	$paintGradient(r, opacity, stops) {
		const horizontal = this.$gradient.$orientation === 'Horizontal';
		const bw = Math.min(this.$drawnBorder, this.$aw / 2, this.$ah / 2);
		if (bw > 0) fillRounded(r, r.pack(this.$borderColor, opacity), 0, 0, this.$aw, this.$ah, this.$radius, this.$antialiasing);
		const x = bw;
		const y = bw;
		const w = this.$aw - 2 * bw;
		const h = this.$ah - 2 * bw;
		if (w <= 0 || h <= 0) return;
		const radius = Math.min(Math.max(this.$radius - bw, 0), Math.min(w, h) / 2);
		const outline = radius > 0 ? roundedOutline(x, y, w, h, radius) : [x, y, x + w, y, x + w, y + h, x, y + h];
		const origin = horizontal ? x : y;
		const inv = 1 / (horizontal ? w : h);

		const slices = [];
		const first = stops[0];
		const last = stops[stops.length - 1];
		if (first.p > 0) slices.push({ lo: 0, hi: first.p, a: first.color, b: first.color });
		for (let i = 0; i + 1 < stops.length; i++) {
			if (stops[i + 1].p > stops[i].p) slices.push({ lo: stops[i].p, hi: stops[i + 1].p, a: stops[i].color, b: stops[i + 1].color });
		}
		if (last.p < 1) slices.push({ lo: last.p, hi: 1, a: last.color, b: last.color });

		for (const slice of slices) {
			let piece = clipConvex(outline, horizontal, origin, inv, slice.lo, true);
			piece = clipConvex(piece, horizontal, origin, inv, slice.hi, false);
			const n = piece.length / 2;
			if (n < 3) continue;
			const colors = [];
			for (let i = 0; i < n; i++) {
				const t = ((horizontal ? piece[2 * i] : piece[2 * i + 1]) - origin) * inv;
				const f = Math.min(Math.max((t - slice.lo) / (slice.hi - slice.lo), 0), 1);
				colors.push(r.pack(mixColors(slice.a, slice.b, f), opacity));
			}
			r.fillFan(piece, colors);
		}
	}

	$paint(r, opacity) {
		if (this.$gradient !== null) {
			const stops = this.$gradient.$stops;
			if (stops.length >= 2) {
				this.$paintGradient(r, opacity, stops);
				return;
			}
			if (stops.length === 1) {
				drawRectangle(r, r.pack(stops[0].color, opacity), r.pack(this.$borderColor, opacity), this.$drawnBorder, this.$radius, this.$antialiasing, 0, 0, this.$aw, this.$ah);
				return;
			}
		}
		drawRectangle(
			r,
			r.pack(this.$color, opacity),
			r.pack(this.$borderColor, opacity),
			this.$drawnBorder,
			this.$radius,
			this.$antialiasing,
			0,
			0,
			this.$aw,
			this.$ah,
		);
	}
}

defineProp(
	Rectangle.prototype,
	'color',
	function () {
		return colorToString(this.$color);
	},
	function (v) {
		this.$color = toColor(v);
		this.$update();
	},
	true,
);
defineField(Rectangle.prototype, 'radius', '$radius', (v) => Math.max(0, toFloat(v)));
defineProp(
	Rectangle.prototype,
	'gradient',
	function () {
		return this.$gradient;
	},
	function (v) {
		const gradient = v instanceof Gradient ? v : null;
		if (gradient !== this.$gradient) {
			this.$gradient = gradient;
			this.$update();
		}
	},
);
defineProp(
	Rectangle.prototype,
	'antialiasing',
	function () {
		return this.$antialiasing;
	},
	function (v) {
		v = toBool(v);
		const changed = this.$antialiasing !== v;
		this.$aaSpecified = true;
		this.$aaValue = v;
		if (changed) this.$update();
	},
);

// ----- Text -----

// Rich text keeps explicit breaks (`<br>`, `</p>`) and turns other whitespace, including a raw newline, into a single space.
function richPlain(text) {
	const mark = '\u0000';
	const stripped = text.replace(/<br\s*\/?>/gi, mark).replace(/<\/p>/gi, mark).replace(/<[^>]*>/g, '');
	const decoded = stripped.replace(/&nbsp;/g, ' ').replace(/&lt;/g, '<').replace(/&gt;/g, '>').replace(/&quot;/g, '"').replace(/&amp;/g, '&');
	return decoded
		.split(mark)
		.map((part) => part.replace(/\s+/g, ' ').replace(/^ | $/g, ''))
		.join('\n');
}

function textWidth(info, text) {
	let width = 0;
	for (const ch of text) width += info.advance(ch);
	return width;
}

function wrapParagraphLines(info, para, limit) {
	if (para === '') return [''];
	const space = info.advance(' ');
	const words = para.split(' ');
	const lines = [];
	let current = '';
	let line = 0;
	for (const word of words) {
		const wordWidth = textWidth(info, word);
		if (current === '') {
			current = word;
			line = wordWidth;
			continue;
		}
		if (line + space + wordWidth > limit) {
			lines.push(current);
			current = word;
			line = wordWidth;
		} else {
			current += ` ${word}`;
			line += space + wordWidth;
		}
	}
	if (current !== '' || lines.length === 0) lines.push(current);
	return lines;
}

function wrapParagraph(info, para, limit) {
	return wrapParagraphLines(info, para, limit).map((line) => textWidth(info, line));
}

function blockLines(info, text, wrap, limit) {
	const paras = text.split('\n');
	const lines = [];
	for (const para of paras) {
		if (!wrap) lines.push(para);
		else lines.push(...wrapParagraphLines(info, para, limit));
	}
	return lines.length > 0 ? lines : [''];
}

function blockMetrics(info, text, wrap, limit) {
	const lines = blockLines(info, text, wrap, limit);
	let max = 0;
	for (const line of lines) max = Math.max(max, textWidth(info, line));
	return { max, count: lines.length === 0 ? 1 : lines.length, lines };
}

// Qt's elidedText keeps the result strictly inside the width. The box font's ellipsis is one cell.
function elidedWidth(info, text, limit) {
	const ellipsis = info.advance('\u2026');
	if (!(ellipsis < limit)) return 0;
	let acc = 0;
	let used = ellipsis;
	for (const ch of text) {
		const width = info.advance(ch);
		if (acc + width + ellipsis < limit) {
			acc += width;
			used = acc + ellipsis;
		} else {
			break;
		}
	}
	return used;
}

function wraps(mode) {
	if (typeof mode === 'number') return mode === 1 || mode === 3 || mode === 4;
	return mode === 'WordWrap' || mode === 'Wrap' || mode === 'WrapAnywhere' || mode === 'WrapAtWordBoundaryOrAnywhere';
}

class FontGroup {
	constructor(owner) {
		this.$owner = owner;
	}
}
FontGroup.prototype.$group = 'font';

function defineFontField(name, field, coerce) {
	defineGroupProp(
		FontGroup.prototype,
		'font',
		name,
		function () {
			return this.$owner[field];
		},
		function (value) {
			const v = coerce(value);
			if (!same(this.$owner[field], v)) {
				this.$owner[field] = v;
				if (this.$owner.$fontChanged !== undefined) this.$owner.$fontChanged();
				else this.$owner.$lay = null;
				this.$owner.$update();
			}
		},
	);
}

defineFontField('pixelSize', '$pixelSize', toFloat);
defineFontField('family', '$family', toStr);
defineFontField('bold', '$bold', toBool);
defineFontField('italic', '$italic', toBool);
defineFontField('underline', '$underline', toBool);
defineFontField('strikeout', '$strikeout', toBool);

export class Text extends Item {
	constructor() {
		super();
		this.$typeName = 'Text';
		this.$text = '';
		this.$color = BLACK;
		this.$pixelSize = 12; // Qt's default font is 9pt, 12px at 96 dpi. Without fonts that is also the box-font cell.
		this.$family = '';
		this.$bold = false;
		this.$italic = false;
		this.$underline = false;
		this.$strikeout = false;
		this.$wrap = 0;
		this.$elide = 3;
		this.$hAlign = 1;
		this.$format = 'PlainText';
		this.$lineHeight = 1;
		this.$font = null;
		this.$lay = null;
	}

	get font() {
		return this.$font !== null ? this.$font : (this.$font = new FontGroup(this));
	}

	// Measured text: the font, the lines and their widths. Honours wrapMode once a width is known.
	$measure() {
		let lay = this.$lay;
		const family = this.$family !== '' ? this.$family : fonts.defaultFamily();
		const rich = this.$format === 'RichText' || this.$format === 'StyledText';
		const source = rich ? richPlain(this.$text) : this.$text;
		const explicit = this.$w > 0 ? this.$w : this.$aw > 0 ? this.$aw : 0;
		const wrap = explicit > 0 && wraps(this.$wrap);
		const limit = wrap ? explicit : Infinity;
		if (
			lay !== null &&
			lay.version === fonts.version &&
			lay.family === family &&
			lay.source === source &&
			lay.wrap === wrap &&
			lay.limit === limit
		) {
			return lay;
		}
		const info = fonts.get(family, this.$pixelSize, this.$bold, this.$italic);
		const lines = blockLines(info, source, wrap, limit);
		const widths = new Array(lines.length);
		let width = 0;
		for (let i = 0; i < lines.length; i++) {
			widths[i] = textWidth(info, lines[i]);
			width = Math.max(width, widths[i]);
		}
		lay = { version: fonts.version, family, info, source, wrap, limit, lines, widths, width };
		this.$lay = lay;
		return lay;
	}

	$fontInfo() {
		const family = this.$family !== '' ? this.$family : fonts.defaultFamily();
		return fonts.get(family, this.$pixelSize > 0 ? this.$pixelSize : 12, this.$bold, this.$italic);
	}

	// Laid-out size. Plain text uses the font height; rich text uses the wider line spacing.
	// An explicit width makes rich text report that width, and a lineHeight other than 1 shortens the implicit height by the extra leading.
	$metrics() {
		const info = this.$fontInfo();
		const rich = this.$format === 'RichText' || this.$format === 'StyledText';
		const source = rich ? richPlain(this.$text) : this.$text;
		const explicit = this.$w > 0 ? this.$w : 0;
		const wrap = explicit > 0 && wraps(this.$wrap);
		const natural = blockMetrics(info, source, false, Infinity);
		const laid = wrap ? blockMetrics(info, source, true, explicit) : natural;
		let contentWidth = laid.max;
		if (!rich && !wrap && this.$elide !== 3 && this.$elide !== 'ElideNone' && explicit > 0 && natural.max > explicit) contentWidth = elidedWidth(info, source, explicit);
		if (rich && explicit > 0) contentWidth = Math.max(explicit, laid.max);
		const factor = this.$lineHeight > 0 ? this.$lineHeight : 1;
		const step = (rich ? info.lineSpacing : info.lineHeight) * factor;
		const contentHeight = (source === '' ? 1 : laid.count) * step;
		const implicitHeight = rich && explicit > 0 ? contentHeight - Math.floor(this.$pixelSize * (factor - 1)) : contentHeight;
		return { implicitWidth: natural.max, implicitHeight, contentWidth, contentHeight };
	}

	get $implicitWidth() {
		return this.$metrics().implicitWidth;
	}

	get $implicitHeight() {
		return this.$metrics().implicitHeight;
	}

	$paint(r, opacity) {
		if (this.$text.length === 0 || this.$pixelSize <= 0) return;
		const lay = this.$measure();
		const info = lay.info;
		const color = r.pack(this.$color, opacity);
		const box = this.$aw > 0 ? this.$aw : lay.width;
		const align = this.$hAlign;
		for (let li = 0; li < lay.lines.length; li++) {
			const baseline = li * info.lineHeight + info.ascent;
			let pen = 0;
			if (align === 2 || align === 'AlignRight') pen = box - lay.widths[li];
			else if (align === 4 || align === 'AlignHCenter' || align === 132 || align === 'AlignCenter') pen = (box - lay.widths[li]) / 2;
			for (const ch of lay.lines[li]) {
				const glyph = info.glyph(ch);
				if (glyph !== null) r.glyph(glyph, pen, baseline, color);
				pen += info.advance(ch);
			}
			const decoX = align === 2 || align === 'AlignRight' ? box - lay.widths[li] : align === 4 || align === 'AlignHCenter' || align === 132 || align === 'AlignCenter' ? (box - lay.widths[li]) / 2 : 0;
			if (this.$underline) r.fillQuad(decoX, li * info.lineHeight + info.underlineY, lay.widths[li], info.lineWidth, color);
			if (this.$strikeout) r.fillQuad(decoX, li * info.lineHeight + info.strikeY, lay.widths[li], info.lineWidth, color);
		}
	}
}

defineProp(
	Text.prototype,
	'text',
	function () {
		return this.$text;
	},
	function (v) {
		v = toStr(v);
		if (this.$text !== v) {
			this.$text = v;
			this.$lay = null;
			this.$update();
		}
	},
	true,
);
defineProp(
	Text.prototype,
	'color',
	function () {
		return colorToString(this.$color);
	},
	function (v) {
		this.$color = toColor(v);
		this.$update();
	},
	true,
);
const WRAP_VALUE = {
	NoWrap: 0, WordWrap: 1, WrapAnywhere: 3, Wrap: 4, WrapAtWordBoundaryOrAnywhere: 4,
};
const ELIDE_VALUE = {
	ElideLeft: 0, ElideRight: 1, ElideMiddle: 2, ElideNone: 3,
};
const toWrapMode = (value) => {
	if (value === undefined || value === null) return 0;
	if (typeof value === 'number') return value;
	return Object.prototype.hasOwnProperty.call(WRAP_VALUE, value) ? WRAP_VALUE[value] : 0;
};
const toElideMode = (value) => {
	if (value === undefined || value === null) return 3;
	if (typeof value === 'number') return value;
	return Object.prototype.hasOwnProperty.call(ELIDE_VALUE, value) ? ELIDE_VALUE[value] : 3;
};
defineField(Text.prototype, 'wrapMode', '$wrap', toWrapMode);
defineField(Text.prototype, 'elide', '$elide', toElideMode);
defineField(Text.prototype, 'horizontalAlignment', '$hAlign', toFloat);
defineField(Text.prototype, 'textFormat', '$format', (value) => (value === undefined || value === null ? 'PlainText' : String(value)));
defineField(Text.prototype, 'lineHeight', '$lineHeight', toFloat);
defineReadOnly(Text.prototype, 'contentWidth', function () {
	return this.$metrics().contentWidth;
});
defineReadOnly(Text.prototype, 'contentHeight', function () {
	return this.$metrics().contentHeight;
});

// ----- Canvas -----

export class Canvas extends Item {
	constructor() {
		super();
		this.$typeName = 'Canvas';
		this.$aa = true;
		this.$cmds = [];
		this.$painted = false;
		this.$paintedWidth = -1;
		this.$paintedHeight = -1;
		this.$context = null;
		this.$measureContext = null;
		this.$renderTarget = 0; // Canvas.Image
		this.$renderStrategy = 0; // Canvas.Immediate
		this.$declareSignal('paint', []);
		this.$declareSignal('painted', []);
		this.$declareSignal('imageLoaded', []);
	}

	// The first paint happens while the image is still loading, then imageLoaded is delivered. That is the order Qt logs.
	loadImage(src) {
		const url = toStr(src);
		if (this.$images === undefined) this.$images = new Map();
		const state = this.$images.get(url);
		if (state === 'ready' || state === 'loading') return;
		this.$images.set(url, 'loading');
		this.$painted = false;
		this.$ensurePainted();
		this.$images.set(url, 'ready');
		this.$emit('imageLoaded');
	}

	unloadImage(src) {
		if (this.$images !== undefined) this.$images.delete(toStr(src));
	}

	isImageLoaded(src) {
		return this.$images !== undefined && this.$images.get(toStr(src)) === 'ready';
	}

	isImageLoading(src) {
		return this.$images !== undefined && this.$images.get(toStr(src)) === 'loading';
	}

	// Inside onPaint this is the recording context. Outside of it (e.g. a hidden canvas used only
	// to measure text) it is a persistent context that draws nothing.
	getContext() {
		if (this.$context !== null) return this.$context;
		if (this.$measureContext === null) this.$measureContext = new CanvasContext(1, 1, null);
		return this.$measureContext;
	}

	requestPaint() {
		this.$painted = false;
		this.$update();
	}

	$ensurePainted() {
		if (this.$painted && this.$paintedWidth === this.$aw && this.$paintedHeight === this.$ah) return;
		this.$painted = true;
		this.$paintedWidth = this.$aw;
		this.$paintedHeight = this.$ah;
		this.$cmds = [];
		const signal = this.$sigs.get('paint');
		if (signal.handlers.length > 0) {
			this.$context = new CanvasContext(this.$aw, this.$ah, this.$cmds);
			try {
				this.$emit('paint');
			} finally {
				this.$context.$finish();
				this.$context = null;
			}
		}
	}

	$paint(r, opacity) {
		this.$ensurePainted();
		replayCommands(r, this.$cmds, opacity, this.$aa);
	}
}

// Canvas.RenderTarget / Canvas.RenderStrategy: an int, or the name as QML allows for enums.
const CANVAS_ENUM_VALUE = { Image: 0, FramebufferObject: 1, Immediate: 0, Threaded: 1, Cooperative: 2 };
function toCanvasEnum(value) {
	if (typeof value === 'number' && Number.isInteger(value) && value >= 0 && value <= 2) return value;
	if (typeof value === 'string' && Object.prototype.hasOwnProperty.call(CANVAS_ENUM_VALUE, value)) return CANVAS_ENUM_VALUE[value];
	throw new QmlError('Cannot assign ' + String(value) + ' to a Canvas enum property');
}

defineField(Canvas.prototype, 'antialiasing', '$aa', toBool);
// Accepted for compatibility: every Canvas renders the same way here.
defineField(Canvas.prototype, 'renderTarget', '$renderTarget', toCanvasEnum);
defineField(Canvas.prototype, 'renderStrategy', '$renderStrategy', toCanvasEnum);
defineReadOnly(Canvas.prototype, 'available', function () {
	return this.$context !== null;
});
defineReadOnly(Canvas.prototype, 'canvasSize', function () {
	const width = this.width;
	const height = this.height;
	return { width, height, toString() { return `QSizeF(${width}, ${height})`; } };
});

// ----- MouseArea -----

export class MouseArea extends Item {
	constructor() {
		super();
		this.$typeName = 'MouseArea';
		this.$containsMouse = false;
		this.$pressed = false;
		this.$hoverEnabled = false;
		this.$preventStealing = false;
		this.$cursorShape = 0; // Qt.ArrowCursor
		this.$buttons = 1;
		this.$acceptedValue = 'LeftButton';
		this.$pressButton = 0;
		this.$mouseX = 0;
		this.$mouseY = 0;
		this.$sceneX = NaN;
		this.$sceneY = NaN;
		this.$declareSignal('clicked', ['mouse']);
		this.$declareSignal('pressed', ['mouse']);
		this.$declareSignal('released', ['mouse']);
		this.$declareSignal('doubleClicked', ['mouse']);
		this.$declareSignal('positionChanged', ['mouse']);
		this.$declareSignal('entered', []);
		this.$declareSignal('exited', []);
		this.$declareSignal('wheel', ['wheel']);
	}

	$mouseEvent() {
		const button = this.$pressButton === 2 ? 'RightButton' : 'LeftButton';
		return { x: this.$mouseX, y: this.$mouseY, button, buttons: button, modifiers: 0, accepted: true, wasHeld: false, isClick: true };
	}

	get $acceptsMouse() {
		return true;
	}

	$acceptsButton(button) {
		return button < 0 || (this.$buttons & (button === 2 ? 2 : button === 1 ? 4 : 1)) !== 0;
	}

	$mousePressAt(sceneX, sceneY) {
		this.$trackMouse(sceneX, sceneY, false);
		this.$emit('pressed', [this.$mouseEvent()]);
	}

	$mouseMoveAt(sceneX, sceneY) {
		// Without hoverEnabled, Qt only reports moves while a button is held.
		this.$trackMouse(sceneX, sceneY, this.$pressed || this.$hoverEnabled);
	}

	$trackMouse(sceneX, sceneY, notify) {
		const x = sceneX - this.$sx;
		const y = sceneY - this.$sy;
		// The pointer moved if its scene position changed, even when its position inside the area did not: a scrollbar
		// thumb dragged by the pointer moves along with it (the layout catches up on the next frame), and dropping
		// those moves made the drag lag far behind the cursor.
		const moved = sceneX !== this.$sceneX || sceneY !== this.$sceneY;
		this.$sceneX = sceneX;
		this.$sceneY = sceneY;
		if (x !== this.$mouseX || y !== this.$mouseY) {
			this.$mouseX = x;
			this.$mouseY = y;
			this.$update();
			if (notify) this.$emit('positionChanged', [this.$mouseEvent()]);
		} else if (notify && moved) {
			this.$emit('positionChanged', [this.$mouseEvent()]);
		}
	}

	$hoverChanged(hovered) {
		if (!this.$hoverEnabled) return;
		if (this.$containsMouse !== hovered) {
			this.$containsMouse = hovered;
			markReaders(this, 'containsMouse');
			markReaders(this, 'containsPress');
			this.$update();
			this.$emit(hovered ? 'entered' : 'exited', []);
		}
	}

	$pressChanged(pressed) {
		if (this.$pressed !== pressed) {
			this.$pressed = pressed;
			markReaders(this, 'pressed');
			markReaders(this, 'containsPress');
			// Without hoverEnabled, Qt still reports containsMouse while a press is held inside.
			if (!this.$hoverEnabled && this.$containsMouse !== pressed) {
				this.$containsMouse = pressed;
				markReaders(this, 'containsMouse');
				this.$emit(pressed ? 'entered' : 'exited', []);
			}
			this.$update();
			if (!pressed) this.$emit('released', [this.$mouseEvent()]);
		}
	}

	$mouseClicked() {
		this.$emit('clicked', [this.$mouseEvent()]);
	}
}

defineReadOnly(MouseArea.prototype, 'containsMouse', function () {
	return this.$containsMouse;
}, undefined, true);
defineReadOnly(MouseArea.prototype, 'pressed', function () {
	return this.$pressed;
}, undefined, true);
// Qt: pressed && containsMouse — used by MovingItem cursorShape and similar bindings.
defineReadOnly(MouseArea.prototype, 'containsPress', function () {
	return this.$pressed && this.$containsMouse;
}, undefined, true);
defineField(MouseArea.prototype, 'hoverEnabled', '$hoverEnabled', toBool);
defineField(MouseArea.prototype, 'preventStealing', '$preventStealing', toBool);
// `Qt.LeftButton | Qt.RightButton` is 0 here (the enumerations are names), which stands for every button.
defineProp(
	MouseArea.prototype,
	'acceptedButtons',
	function () {
		return this.$acceptedValue;
	},
	function (value) {
		this.$acceptedValue = value;
		const bits = { NoButton: 0, LeftButton: 1, RightButton: 2, MiddleButton: 4, AllButtons: 7 };
		if (typeof value === 'string') this.$buttons = value in bits ? bits[value] : 1;
		else this.$buttons = typeof value === 'number' && value !== 0 ? value : 7;
	},
);
defineField(MouseArea.prototype, 'cursorShape', '$cursorShape', (value) => value);
defineReadOnly(MouseArea.prototype, 'mouseX', function () {
	return this.$mouseX;
});
defineReadOnly(MouseArea.prototype, 'mouseY', function () {
	return this.$mouseY;
});

// ----- Qt Quick Layouts -----

const H_LEFT = 1;
const H_RIGHT = 2;
const H_CENTER = 4;
const V_TOP = 0x20;
const V_BOTTOM = 0x40;
const V_CENTER = 0x80;
const ALIGN_NAME = {
	AlignLeft: H_LEFT,
	AlignRight: H_RIGHT,
	AlignHCenter: H_CENTER,
	AlignJustify: 8,
	AlignTop: V_TOP,
	AlignBottom: V_BOTTOM,
	AlignVCenter: V_CENTER,
	AlignCenter: H_CENTER | V_CENTER,
};

function alignmentBits(value) {
	if (typeof value === 'number') return value;
	if (typeof value === 'string' && ALIGN_NAME[value] !== undefined) return ALIGN_NAME[value];
	return 0;
}

// Unspecified horizontal alignment stays at the start of the cell. Unspecified vertical alignment
// centers in the cell: that is what ColumnLayout does when only AlignRight is set.
function axisPos(start, cell, size, bits, vertical) {
	const flag = vertical ? bits & (V_TOP | V_BOTTOM | V_CENTER) : bits & (H_LEFT | H_RIGHT | H_CENTER | 8);
	if (vertical) {
		if (flag & V_BOTTOM) return start + cell - size;
		if (flag & V_CENTER) return start + (cell - size) / 2;
		if (flag & V_TOP) return start;
		return start + (cell - size) / 2;
	}
	if (flag & H_RIGHT) return start + cell - size;
	if (flag & H_CENTER) return start + (cell - size) / 2;
	return start;
}

function layoutOf(item) {
	return item.$lay === undefined ? null : item.$lay;
}

function preferred(item, vertical) {
	const lay = layoutOf(item);
	if (lay !== null) {
		const spec = vertical ? lay.$ph : lay.$pw;
		if (spec === spec) return Math.max(0, spec);
	}
	return Math.max(0, Stack.$size(item, vertical));
}

function wantsFill(item, vertical) {
	const lay = layoutOf(item);
	return lay !== null && (vertical ? lay.$fh === true : lay.$fw === true);
}

// A size the user or anchors fixed. 0 means the layout may choose the implicit size.
function explicitSpan(item, vertical) {
	const an = item.$anchors;
	if (an !== null && (vertical ? an.$affectsHeight : an.$affectsWidth)) return vertical ? item.$ah : item.$aw;
	const own = vertical ? item.$h : item.$w;
	return own > 0 ? own : 0;
}

// Writes the readable geometry and reports the changes in x, y, width, height order.
function commitGeometry(item, x, y, w, h) {
	const writes = [];
	if (item.$x !== x) {
		item.$x = x;
		writes.push('x');
	}
	item.$ax = x;
	if (item.$y !== y) {
		item.$y = y;
		writes.push('y');
	}
	item.$ay = y;
	if (item.$w !== w) {
		item.$w = w;
		writes.push('width');
	}
	item.$aw = w;
	if (item.$h !== h) {
		item.$h = h;
		writes.push('height');
	}
	item.$ah = h;
	for (const name of writes) notifyGeometry(item, name);
}

function commitOwnSize(item, w, h) {
	const an = item.$anchors;
	if (!((an !== null && an.$affectsWidth) || item.$w > 0) && item.$aw !== w) {
		item.$aw = w;
		notifyGeometry(item, 'width');
	}
	if (!((an !== null && an.$affectsHeight) || item.$h > 0) && item.$ah !== h) {
		item.$ah = h;
		notifyGeometry(item, 'height');
	}
}

function commitMany(placed) {
	for (let i = placed.length - 1; i >= 0; i--) {
		const p = placed[i];
		commitGeometry(p.item, p.x, p.y, p.w, p.h);
	}
}

// ColumnLayout / RowLayout. Extra main-axis space goes to fill items. With none, cells grow in
// proportion to the preferred size and each item is aligned inside its cell.
function arrangeLinear(layout, vertical) {
	const items = [];
	for (const child of layout.$children) {
		if (child.$nonVisual || !child.$visible) continue;
		items.push(child);
	}
	const n = items.length;
	const spacing = layout.$spacing > 0 ? layout.$spacing : 0;
	const gaps = n > 1 ? spacing * (n - 1) : 0;
	const prefs = [];
	const crossPrefs = [];
	const fills = [];
	const crossFills = [];
	let cross = 0;
	let prefSum = 0;
	let fillCount = 0;
	for (let i = 0; i < n; i++) {
		prefs.push(preferred(items[i], vertical));
		crossPrefs.push(preferred(items[i], !vertical));
		fills.push(wantsFill(items[i], vertical));
		crossFills.push(wantsFill(items[i], !vertical));
		cross = Math.max(cross, crossPrefs[i]);
		prefSum += prefs[i];
		if (fills[i]) fillCount++;
	}
	const mainExplicit = explicitSpan(layout, vertical);
	const crossExplicit = explicitSpan(layout, !vertical);
	const mainSize = mainExplicit > 0 ? mainExplicit : prefSum + gaps;
	const crossSize = crossExplicit > 0 ? crossExplicit : cross;
	const sizes = prefs.slice();
	const cells = prefs.slice();
	const extra = Math.max(0, mainSize - prefSum - gaps);
	if (fillCount > 0) {
		const share = extra / fillCount;
		for (let i = 0; i < n; i++) {
			if (fills[i]) sizes[i] = prefs[i] + share;
			cells[i] = sizes[i];
		}
	} else if (prefSum > 0) {
		const avail = mainSize - gaps;
		for (let i = 0; i < n; i++) cells[i] = (avail * prefs[i]) / prefSum;
	}
	const placed = [];
	let pos = 0;
	for (let i = 0; i < n; i++) {
		const lay = layoutOf(items[i]);
		const bits = alignmentBits(lay === null ? 0 : lay.$align);
		const main = axisPos(pos, cells[i], sizes[i], bits, vertical);
		const crossItem = crossFills[i] ? crossSize : crossPrefs[i];
		const crossAt = axisPos(0, crossSize, crossItem, bits, !vertical);
		placed.push({
			item: items[i],
			x: vertical ? crossAt : main,
			y: vertical ? main : crossAt,
			w: vertical ? crossItem : sizes[i],
			h: vertical ? sizes[i] : crossItem,
		});
		pos += cells[i] + spacing;
	}
	if (vertical) commitOwnSize(layout, crossSize, mainSize);
	else commitOwnSize(layout, mainSize, crossSize);
	commitMany(placed);
	for (const p of placed) {
		if (typeof p.item.$syncTarget === 'function') p.item.$syncTarget();
	}
}

class LayoutAttached {
	constructor(owner) {
		this.$owner = owner;
		this.$pw = NaN;
		this.$ph = NaN;
		this.$minw = NaN;
		this.$minh = NaN;
		this.$maxw = NaN;
		this.$maxh = NaN;
		this.$fw = false;
		this.$fh = false;
		this.$align = 0;
		this.$row = -1;
		this.$col = -1;
		this.$rowSpan = 1;
		this.$colSpan = 1;
		this.$left = 0;
		this.$right = 0;
		this.$top = 0;
		this.$bottom = 0;
	}
}

function defineLay(name, field, coerce) {
	Object.defineProperty(LayoutAttached.prototype, name, {
		get() {
			return this[field];
		},
		set(value) {
			if (value instanceof BindingValue) {
				hooks.installBinding(this.$owner, ['Layout', name], value.fn);
				return;
			}
			const v = coerce(value);
			if (!same(this[field], v)) {
				this[field] = v;
				this.$owner.$update();
			}
		},
		enumerable: true,
		configurable: true,
	});
}

defineLay('preferredWidth', '$pw', toFloat);
defineLay('preferredHeight', '$ph', toFloat);
defineLay('minimumWidth', '$minw', toFloat);
defineLay('minimumHeight', '$minh', toFloat);
defineLay('maximumWidth', '$maxw', toFloat);
defineLay('maximumHeight', '$maxh', toFloat);
defineLay('fillWidth', '$fw', toBool);
defineLay('fillHeight', '$fh', toBool);
defineLay('alignment', '$align', alignmentBits);
defineLay('row', '$row', toFloat);
defineLay('column', '$col', toFloat);
defineLay('rowSpan', '$rowSpan', toFloat);
defineLay('columnSpan', '$colSpan', toFloat);
defineLay('leftMargin', '$left', toFloat);
defineLay('rightMargin', '$right', toFloat);
defineLay('topMargin', '$top', toFloat);
defineLay('bottomMargin', '$bottom', toFloat);

// ----- GridLayout -----

export class GridLayout extends Item {
	constructor() {
		super();
		this.$typeName = 'GridLayout';
		this.$columns = 1;
		this.$columnSpacing = 0;
		this.$rowSpacing = 0;
	}

	$arrange() {
		const all = [];
		const items = [];
		for (const child of this.$children) {
			if (child.$nonVisual) continue;
			all.push(child);
			if (child.$visible) items.push(child);
		}
		const count = all.length;
		if (count === 0) return;
		// A grid with no Layout.* spec keeps equal cells, so a filled grid of plain items still divides the parent.
		if (!all.some((item) => item.$lay !== undefined)) {
			const cols = Math.max(1, Math.floor(this.$columns));
			const rows = Math.ceil(count / cols);
			const columnGap = Math.max(0, this.$columnSpacing);
			const rowGap = Math.max(0, this.$rowSpacing);
			const cellWidth = Math.max(0, (this.$aw - columnGap * (cols - 1)) / cols);
			const cellHeight = Math.max(0, (this.$ah - rowGap * (rows - 1)) / rows);
			for (let i = 0; i < count; i++) {
				const item = all[i];
				item.$ax = (i % cols) * (cellWidth + columnGap);
				item.$ay = Math.floor(i / cols) * (cellHeight + rowGap);
				item.$aw = cellWidth;
				item.$ah = cellHeight;
			}
			return;
		}
		const cols = Math.max(1, Math.floor(this.$columns) || 1);
		const colGap = Math.max(0, this.$columnSpacing);
		const rowGap = Math.max(0, this.$rowSpacing);
		const cells = [];
		let col = 0;
		let row = 0;
		for (const item of items) {
			const lay = layoutOf(item);
			const fixed = lay !== null && (lay.$row >= 0 || lay.$col >= 0);
			const r = lay !== null && lay.$row >= 0 ? lay.$row : row;
			const c = lay !== null && lay.$col >= 0 ? lay.$col : col;
			cells.push({ item, r, c });
			if (!fixed) {
				col++;
				if (col >= cols) {
					col = 0;
					row++;
				}
			}
		}
		let rowCount = 1;
		let colCount = cols;
		for (const cell of cells) {
			rowCount = Math.max(rowCount, cell.r + 1);
			colCount = Math.max(colCount, cell.c + 1);
		}
		const colW = Array(colCount).fill(0);
		const rowH = Array(rowCount).fill(0);
		const colFill = Array(colCount).fill(false);
		const rowFill = Array(rowCount).fill(false);
		for (const cell of cells) {
			colW[cell.c] = Math.max(colW[cell.c], preferred(cell.item, false));
			rowH[cell.r] = Math.max(rowH[cell.r], preferred(cell.item, true));
			if (wantsFill(cell.item, false)) colFill[cell.c] = true;
			if (wantsFill(cell.item, true)) rowFill[cell.r] = true;
		}
		const baseW = colW.reduce((sum, value) => sum + value, 0) + colGap * Math.max(0, colCount - 1);
		const baseH = rowH.reduce((sum, value) => sum + value, 0) + rowGap * Math.max(0, rowCount - 1);
		const gridW = explicitSpan(this, false);
		const gridH = explicitSpan(this, true);
		if (gridW > baseW) {
			const growing = colFill.filter(Boolean).length;
			if (growing > 0) {
				const share = (gridW - baseW) / growing;
				for (let i = 0; i < colCount; i++) if (colFill[i]) colW[i] += share;
			}
		}
		if (gridH > baseH) {
			const growing = rowFill.filter(Boolean).length;
			if (growing > 0) {
				const share = (gridH - baseH) / growing;
				for (let i = 0; i < rowCount; i++) if (rowFill[i]) rowH[i] += share;
			}
		}
		const placed = [];
		for (const cell of cells) {
			let x = 0;
			for (let i = 0; i < cell.c; i++) x += colW[i] + colGap;
			let y = 0;
			for (let i = 0; i < cell.r; i++) y += rowH[i] + rowGap;
			const lay = layoutOf(cell.item);
			const bits = alignmentBits(lay === null ? 0 : lay.$align);
			const pw = preferred(cell.item, false);
			const ph = preferred(cell.item, true);
			const w = wantsFill(cell.item, false) ? colW[cell.c] : pw;
			const h = wantsFill(cell.item, true) ? rowH[cell.r] : ph;
			placed.push({ item: cell.item, x: axisPos(x, colW[cell.c], w, bits, false), y: axisPos(y, rowH[cell.r], h, bits, true), w, h });
		}
		commitOwnSize(this, gridW > 0 ? gridW : baseW, gridH > 0 ? gridH : baseH);
		commitMany(placed);
	}
}

defineField(GridLayout.prototype, 'columns', '$columns', toFloat);
defineField(GridLayout.prototype, 'columnSpacing', '$columnSpacing', toFloat);
defineField(GridLayout.prototype, 'rowSpacing', '$rowSpacing', toFloat);

// ----- ListView -----

const MAX_POOLED_DELEGATES = 20;
const clamp = (v, lo, hi) => Math.min(Math.max(v, lo), hi);

function listViewHorizontal(value) {
	return value === 1 || value === 'Horizontal';
}

// What a delegate reports as its `parent`: the content item of the view (`parent.parent` is the view). The delegates are
// laid out as children of the view itself, so this only stands in for the content item.
class ViewContent extends Item {
	constructor(view) {
		super();
		this.$view = view;
		this.$parent = view;
		this.$nonVisual = true;
	}

	$update() {
		this.$view.$update();
	}

	$visit() {}
}
Object.defineProperty(ViewContent.prototype, '$children', {
	get() {
		return this.$view.$children;
	},
	set() {},
});
for (const [name, get] of [
	['$sx', (view) => view.$sx - view.$contentX],
	['$sy', (view) => view.$sy - view.$contentY],
	['$aw', (view) => (view.contentWidth > 0 ? view.contentWidth : view.$aw)],
	['$ah', (view) => (view.contentHeight > 0 ? view.contentHeight : view.$ah)],
]) {
	Object.defineProperty(ViewContent.prototype, name, {
		get() {
			return get(this.$view);
		},
		set() {},
	});
}
for (const [name, get] of [
	['x', (view) => -view.contentX],
	['y', (view) => -view.contentY],
	['width', (view) => (view.contentWidth > 0 ? view.contentWidth : view.width)],
	['height', (view) => (view.contentHeight > 0 ? view.contentHeight : view.height)],
]) {
	defineProp(
		ViewContent.prototype,
		name,
		function () {
			return get(this.$view);
		},
		function () {},
	);
}

export class ListView extends Item {
	// The delegates are the children of the view itself.
	get contentItem() {
		return this.$viewContent !== undefined ? this.$viewContent : (this.$viewContent = new ViewContent(this));
	}

	constructor() {
		super();
		this.$typeName = 'ListView';
		this.$isFocusScope = true;
		this.$clip = true;
		this.$model = 0;
		// Qt starts at -1; assigning a non-empty model selects row 0 (see $syncCurrentIndex).
		this.$currentIndex = -1;
		this.$spacing = 0;
		this.$orientation = 2; // Qt.Vertical
		this.$contentX = 0;
		this.$contentY = 0;
		this.$rowHeight = 0;
		this.$builtModel = undefined;
		this.$builtVersion = -1;
		this.$builtCount = -1;
		this.$delegates = new Map();
		this.$reuseItems = false;
		this.$pool = []; // delegates kept for reuse (`reuseItems: true`), detached from the children
		this.$extents = []; // measured width (horizontal) / height (vertical) per index for variable-size rows
		this.$extentSum = 0; // sum of the measured extents, so the content length does not visit every row
		this.$extentCount = 0; // rows with a measured extent
		this.$oddExtents = 0; // measured rows whose extent differs from $rowHeight; 0 means uniform rows
		this.$arranging = false; // re-entrancy guard: binding refresh during reuse must not nest $arrange
		this.createDelegate = null; // (index) => Item | null
		this.releaseDelegate = null; // (item) => void
		this.rebindDelegate = null; // (item, index) => void: points a pooled delegate at another row
		this.poolDelegate = null; // (item) => void: a delegate was put into the pool
	}

	get $horizontal() {
		return listViewHorizontal(this.$orientation);
	}

	// A pooled delegate for reuse, attached to the view again; null if the pool is empty.
	$takePooled() {
		const item = this.$pool.pop();
		if (item === undefined) return null;
		item.$parent = this;
		item.$visible = true;
		markReaders(item, 'visible');
		if (this.$children.indexOf(item) < 0) this.$children.push(item);
		this.$update();
		return item;
	}

	// Destroys the pooled delegates.
	$drainPool() {
		const pool = this.$pool;
		this.$pool = [];
		for (const item of pool) {
			if (this.releaseDelegate !== null) this.releaseDelegate(item);
			item.$destroy();
		}
	}

	$destroy() {
		for (const item of this.$pool) item.$destroy();
		this.$pool = [];
		super.$destroy();
	}

	get $rowCount() {
		return modelCount(this.$model);
	}

	get $rowStep() {
		return this.$rowHeight + Math.max(0, this.$spacing);
	}

	// Main-axis size of the delegate at `index` (width if horizontal, height if vertical).
	// Measured items update `$extents`; off-screen rows fall back to the last known size or the probe.
	$extentAt(index) {
		const item = this.$delegates.get(index);
		if (item !== undefined) {
			const size = this.$horizontal
				? item.$w > 0
					? item.$w
					: item.$aw > 0
						? item.$aw
						: 0
				: item.$h > 0
					? item.$h
					: item.$ah > 0
						? item.$ah
						: 0;
			if (size > 0) {
				this.$recordExtent(index, size);
				return size;
			}
		}
		const remembered = this.$extents[index];
		if (remembered > 0) return remembered;
		return this.$rowHeight > 0 ? this.$rowHeight : 40;
	}

	$recordExtent(index, size) {
		const previous = this.$extents[index];
		if (previous === size) return;
		const base = this.$rowHeight;
		if (previous === undefined) {
			this.$extentCount++;
		} else {
			this.$extentSum -= previous;
			if (previous !== base) this.$oddExtents--;
		}
		this.$extentSum += size;
		if (size !== base) this.$oddExtents++;
		this.$extents[index] = size;
	}

	$resetExtents() {
		this.$rowHeight = 0;
		this.$extents = [];
		this.$extentSum = 0;
		this.$extentCount = 0;
		this.$oddExtents = 0;
	}

	// Rows never measured count as $rowHeight; the live delegates are re-measured so a size that changed
	// since the last arrange is reflected at once.
	get $contentLength() {
		const count = this.$rowCount;
		if (count <= 0) return 0;
		for (const index of this.$delegates.keys()) this.$extentAt(index);
		const base = this.$rowHeight > 0 ? this.$rowHeight : 40;
		const measured = Math.min(this.$extentCount, count);
		return this.$extentSum + (count - measured) * base + Math.max(0, count - 1) * Math.max(0, this.$spacing);
	}

	get $maxContentX() {
		return Math.max(0, this.$contentLength - this.$aw);
	}

	get $maxContentY() {
		return Math.max(0, this.$contentLength - this.$ah);
	}

	get $acceptsWheel() {
		return true;
	}

	$mouseWheel(notches) {
		const delta = notches * WHEEL_NOTCH_PX;
		if (this.$horizontal) this.contentX = clamp(this.$contentX - delta, 0, this.$maxContentX);
		else this.contentY = clamp(this.$contentY - delta, 0, this.$maxContentY);
	}

	$releaseWhere(keep) {
		for (const [index, item] of [...this.$delegates]) {
			if (keep(index)) continue;
			this.$delegates.delete(index);
			if (this.$reuseItems && this.rebindDelegate !== null && this.$pool.length < MAX_POOLED_DELEGATES) {
				const at = this.$children.indexOf(item);
				if (at >= 0) this.$children.splice(at, 1);
				item.$parent = this;
				item.$visible = false;
				markReaders(item, 'visible');
				this.$pool.push(item);
				if (this.poolDelegate !== null) this.poolDelegate(item);
				this.$update();
				continue;
			}
			if (this.releaseDelegate !== null) this.releaseDelegate(item);
			this.$removeChild(item);
		}
	}

	// Creates the delegates of the visible rows and removes the others.
	$arrange() {
		if (this.$arranging) return;
		this.$arranging = true;
		try {
			this.$arrangeBody();
			this.$syncDelegateChildren();
			// contentY/X can call $arrange outside a layout pass — keep screen coords in sync for hit-testing.
			if (!layingOut) {
				const sx = this.$sx;
				const sy = this.$sy;
				for (const child of this.$children) {
					child.$sx = sx + child.$ax;
					child.$sy = sy + child.$ay;
				}
			}
		} finally {
			this.$arranging = false;
		}
	}

	// Only active delegates belong under the view. Pool entries stay parented but off `$children`.
	$syncDelegateChildren() {
		const live = new Set(this.$delegates.values());
		const children = this.$children;
		for (let i = children.length - 1; i >= 0; i--) {
			if (!live.has(children[i])) children.splice(i, 1);
		}
	}

	$arrangeBody() {
		if (!this.$reuseItems && this.$pool.length > 0) this.$drainPool();
		const dynamic = this.delegate;
		if (dynamic instanceof ComponentItem && dynamic !== this.$dynamicDelegate) {
			// `delegate: someComponent`: set at run time, not as an inline object.
			this.$dynamicDelegate = dynamic;
			if (hooks.setupDelegate !== undefined && hooks.setupDelegate(this, dynamic)) this.$builtModel = undefined;
		}
		const count = this.$rowCount;
		const version = modelVersion(this.$model);
		watchModel(this);
		if (this.$builtModel !== this.$model || this.$builtVersion !== version || this.$builtCount !== count) {
			// The model changed: the delegates are created again. The old current item does not log a focus loss.
			this.$scopeFocus = null;
			this.$releaseWhere(() => false);
			this.$resetExtents();
			this.$builtModel = this.$model;
			this.$builtVersion = version;
			this.$builtCount = count;
		}
		if (this.createDelegate === null || count === 0) {
			this.$releaseWhere(() => false);
			this.$syncCurrentIndex(0);
			return;
		}
		this.$syncCurrentIndex(count);

		const horizontal = this.$horizontal;
		if (this.$rowHeight <= 0) {
			const probe = this.createDelegate(0);
			if (probe === null) {
				this.createDelegate = null;
				return;
			}
			// Prefer the explicit size binding; fall back to the laid-out size after create.
			const extent = horizontal
				? probe.$w > 0
					? probe.$w
					: probe.$aw
				: probe.$h > 0
					? probe.$h
					: probe.$ah;
			this.$rowHeight = extent > 0 ? extent : 40;
			this.$delegates.set(0, probe);
			this.$extentAt(0);
		}

		const maxOffset = horizontal ? this.$maxContentX : this.$maxContentY;
		const offset = clamp(horizontal ? this.$contentX : this.$contentY, 0, maxOffset);
		if (horizontal) {
			if (offset !== this.$contentX) this.contentX = offset;
		} else if (offset !== this.$contentY) {
			this.contentY = offset;
		}

		const spacing = Math.max(0, this.$spacing);
		const viewSpan = horizontal ? this.$aw : this.$ah;
		let pos = 0;
		let first = 0;
		if (this.$oddExtents === 0) {
			// Every measured row has the probe's size: the first row is a division, not a walk over 100k rows.
			const step = this.$rowHeight + spacing;
			first = Math.max(0, Math.min(count - 1, Math.floor(offset / step)));
			pos = first * step;
			while (first > 0 && pos > offset) {
				first--;
				pos -= step;
			}
		}
		while (first < count - 1 && pos + this.$extentAt(first) + spacing <= offset) {
			pos += this.$extentAt(first) + spacing;
			first++;
		}
		let last = first;
		let end = pos;
		while (last < count - 1 && end < offset + viewSpan) {
			end += this.$extentAt(last) + spacing;
			last++;
		}

		this.$releaseWhere((index) => index >= first && index <= last);
		for (let i = first; i <= last; i++) {
			if (this.$delegates.has(i)) continue;
			const delegate = this.createDelegate(i);
			if (delegate === null) {
				this.createDelegate = null;
				break;
			}
			this.$delegates.set(i, delegate);
		}

		// Newly created delegates may have real sizes — grow the window if the estimate was short.
		while (last < count - 1) {
			let span = 0;
			for (let i = first; i <= last; i++) span += this.$extentAt(i) + (i < last ? spacing : 0);
			if (pos + span >= offset + viewSpan) break;
			last++;
			if (this.$delegates.has(last)) continue;
			const delegate = this.createDelegate(last);
			if (delegate === null) {
				this.createDelegate = null;
				last--;
				break;
			}
			this.$delegates.set(last, delegate);
		}

		let cursor = pos;
		for (let i = first; i <= last; i++) {
			const item = this.$delegates.get(i);
			if (item === undefined) continue;
			const ext = this.$extentAt(i);
			if (horizontal) {
				item.$x = cursor - offset;
				item.$y = 0;
				item.$ax = item.$x;
				item.$ay = item.$y;
				item.$aw = ext;
				item.$ah = this.$ah;
			} else {
				item.$x = 0;
				item.$y = cursor - offset;
				item.$ax = item.$x;
				item.$ay = item.$y;
				item.$aw = this.$aw;
				item.$ah = ext;
			}
			cursor += ext + spacing;
		}
		this.$focusCurrent();
	}

	itemAtIndex(index) {
		const i = Math.trunc(Number(index));
		if (!Number.isFinite(i) || i < 0) return null;
		const item = this.$delegates.get(i);
		return item === undefined ? null : item;
	}

	// Qt: a non-empty model with no current selects 0; an explicit currentIndex (e.g. `currentIndex: 1`
	// before the model is ready) must survive empty intermediate arranges — do not force -1 on count 0.
	$syncCurrentIndex(count) {
		if (count <= 0) return;
		if (this.$currentIndex < 0) this.$currentIndex = 0;
		else if (this.$currentIndex >= count) this.$currentIndex = count - 1;
	}

	// The current delegate is the focused item of the view, without taking the window's focus.
	$focusCurrent() {
		const index = this.$currentIndex;
		if (!(index >= 0)) return;
		const current = this.$delegates.get(index);
		if (current !== undefined && !current.$focus) {
			current.focus = true;
			// The first current item notifies twice; later rebuilds notify once.
			if (this.$announcedCurrent !== true) {
				this.$announcedCurrent = true;
				emitFocus(current);
			}
		}
	}
}

defineProp(
	ListView.prototype,
	'model',
	function () {
		return this.$model;
	},
	function (v) {
		if (v !== this.$model) {
			this.$model = v;
			this.$update();
			markReaders(this, 'count');
			if (!this.$constructing && !layingOut) this.$arrange();
		}
	},
	true,
);
defineField(ListView.prototype, 'spacing', '$spacing', toFloat);
defineField(ListView.prototype, 'reuseItems', '$reuseItems', toBool);
defineProp(
	ListView.prototype,
	'currentIndex',
	function () {
		return this.$currentIndex;
	},
	function (value) {
		const n = Math.trunc(Number(value));
		this.$currentIndex = Number.isFinite(n) ? n : -1;
	},
);
function defineListContent(name, field) {
	defineProp(
		ListView.prototype,
		name,
		function () {
			return this[field];
		},
		function (value) {
			const v = toFloat(value);
			if (this[field] === v) return;
			this[field] = v;
			this.$update();
			if (hooks.changed !== null && this.$chg !== undefined) hooks.changed(this, name);
			if (!this.$constructing && !layingOut) this.$arrange();
		},
	);
}
defineListContent('contentX', '$contentX');
defineListContent('contentY', '$contentY');
defineProp(
	ListView.prototype,
	'orientation',
	function () {
		return this.$orientation;
	},
	function (value) {
		const next = value === 'Horizontal' || value === 1 ? 1 : value === 'Vertical' || value === 2 ? 2 : value;
		if (next === this.$orientation) return;
		this.$orientation = next;
		this.$resetExtents();
		this.$update();
		if (!this.$constructing && !layingOut) this.$arrange();
	},
);
defineReadOnly(ListView.prototype, 'count', function () {
	return this.$rowCount;
});
defineProp(
	ListView.prototype,
	'contentHeight',
	function () {
		if (this.$contentHeightSet !== undefined) return this.$contentHeightSet;
		// Qt keeps the cross-axis content size at -1 (unset) for ListView.
		return this.$horizontal ? -1 : this.$contentLength;
	},
	function (v) {
		this.$contentHeightSet = toFloat(v);
	},
);
defineProp(
	ListView.prototype,
	'contentWidth',
	function () {
		if (this.$contentWidthSet !== undefined) return this.$contentWidthSet;
		return this.$horizontal ? this.$contentLength : -1;
	},
	function (v) {
		this.$contentWidthSet = toFloat(v);
	},
);
defineReadOnly(ListView.prototype, 'originX', function () {
	return 0;
});
defineReadOnly(ListView.prototype, 'originY', function () {
	return 0;
});
for (const name of ['positionViewAtIndex', 'positionViewAtBeginning', 'positionViewAtEnd']) {
	ListView.prototype[name] = function () {};
}
ListView.prototype.forceLayout = function () {
	this.$arrange();
};

// Cells in row-major order. `columns` is how many cell widths fit in the view.
export class GridView extends ListView {
	constructor() {
		super();
		this.$typeName = 'GridView';
		this.$cellWidth = 100;
		this.$cellHeight = 100;
		this.$currentIndex = 0;
	}

	$columns() {
		const cell = this.$cellWidth > 0 ? this.$cellWidth : 1;
		const width = this.$aw > 0 ? this.$aw : this.$w;
		return Math.max(1, Math.floor(width / cell));
	}

	$arrange() {
		const dynamic = this.delegate;
		if (dynamic instanceof ComponentItem && dynamic !== this.$dynamicDelegate) {
			this.$dynamicDelegate = dynamic;
			if (hooks.setupDelegate !== undefined && hooks.setupDelegate(this, dynamic)) this.$builtModel = undefined;
		}
		const count = this.$rowCount;
		const version = modelVersion(this.$model);
		watchModel(this);
		if (this.$builtModel !== this.$model || this.$builtVersion !== version || this.$builtCount !== count) {
			this.$releaseWhere(() => false);
			this.$builtModel = this.$model;
			this.$builtVersion = version;
			this.$builtCount = count;
		}
		if (this.createDelegate === null || count === 0) {
			this.$releaseWhere(() => false);
			return;
		}
		for (let i = 0; i < count; i++) {
			if (this.$delegates.has(i)) continue;
			const delegate = this.createDelegate(i);
			if (delegate === null) {
				this.createDelegate = null;
				break;
			}
			this.$delegates.set(i, delegate);
		}
		const cols = this.$columns();
		const cw = this.$cellWidth;
		const ch = this.$cellHeight;
		const y0 = this.$contentY;
		for (const [index, item] of this.$delegates) {
			const col = index % cols;
			const row = Math.floor(index / cols);
			const x = col * cw;
			const y = row * ch - y0;
			const an = item.$anchors;
			// Delegates may pin themselves with anchors (GridExtendingBaseDelegate shifts rows
			// below an open panel via topMargin). Anchors win over the grid slot, as in Qt; the
			// baked content offset is still applied because our contentItem does not move.
			if (an !== null && (an.$affectsX || an.$affectsY)) {
				item.$state = 0;
				item.$resolve();
				if (an.$affectsX) {
					item.$x = item.$ax;
				} else {
					item.$x = x;
					item.$ax = x;
				}
				if (an.$affectsY) {
					item.$ay -= y0;
					item.$y = item.$ay;
				} else {
					item.$y = y;
					item.$ay = y;
				}
			} else {
				item.$x = x;
				item.$y = y;
				item.$ax = x;
				item.$ay = y;
			}
		}
	}

	itemAtIndex(index) {
		const i = Math.trunc(Number(index));
		if (!Number.isFinite(i) || i < 0) return null;
		if (!this.$delegates.has(i) && this.createDelegate !== null && i < this.$rowCount) {
			const delegate = this.createDelegate(i);
			if (delegate !== null) this.$delegates.set(i, delegate);
		}
		const item = this.$delegates.get(i);
		return item === undefined ? null : item;
	}

	positionViewAtBeginning() {
		this.contentY = 0;
	}
}

defineField(GridView.prototype, 'cellWidth', '$cellWidth', toFloat);
defineField(GridView.prototype, 'cellHeight', '$cellHeight', toFloat);
defineProp(
	GridView.prototype,
	'currentIndex',
	function () {
		return this.$currentIndex;
	},
	function (value) {
		const n = Math.trunc(Number(value));
		this.$currentIndex = Number.isFinite(n) ? n : 0;
	},
);
defineProp(
	GridView.prototype,
	'contentWidth',
	function () {
		if (this.$contentWidthSet !== undefined) return this.$contentWidthSet;
		// Qt keeps the cross-axis content size at -1 for a vertical GridView.
		return -1;
	},
	function (v) {
		this.$contentWidthSet = toFloat(v);
	},
);
defineProp(
	GridView.prototype,
	'contentHeight',
	function () {
		if (this.$contentHeightSet !== undefined) return this.$contentHeightSet;
		const cols = this.$columns();
		const rows = cols > 0 ? Math.ceil(this.$rowCount / cols) : 0;
		return rows * this.$cellHeight;
	},
	function (v) {
		this.$contentHeightSet = toFloat(v);
	},
);

// ----- ScrollBar -----

const MIN_THUMB_HEIGHT = 24;

// A vertical scroll bar outside of its `target` ListView: it follows `target.contentY` and scrolls it by dragging.
export class ScrollBar extends Item {
	constructor() {
		super();
		this.$typeName = 'ScrollBar';
		this.$target = null;
		this.$dragging = false;
	}

	get $scrollable() {
		const target = this.$target;
		return target instanceof ListView && target.$maxContentY > 0;
	}

	get $thumbHeight() {
		const target = this.$target;
		const contentHeight = target.$maxContentY + target.$ah;
		return Math.min(Math.max((this.$ah * target.$ah) / contentHeight, MIN_THUMB_HEIGHT), this.$ah);
	}

	get $acceptsMouse() {
		return this.$scrollable;
	}

	$mousePressAt(sceneX, sceneY) {
		this.$dragging = true;
		this.$dragTo(sceneY);
	}

	$mouseMoveAt(sceneX, sceneY) {
		if (this.$dragging) this.$dragTo(sceneY);
	}

	$pressChanged(pressed) {
		if (!pressed && this.$dragging) {
			this.$dragging = false;
			this.$update();
		}
	}

	$dragTo(pointerY) {
		if (!this.$scrollable) return;
		const thumb = this.$thumbHeight;
		const travel = this.$ah - thumb;
		if (travel <= 0) return;
		const ratio = clamp((pointerY - this.$sy - thumb / 2) / travel, 0, 1);
		this.$target.contentY = ratio * this.$target.$maxContentY;
	}

	$paint(r, opacity) {
		if (!this.$scrollable) return;
		const thumb = this.$thumbHeight;
		const thumbY = clamp(this.$target.$contentY / this.$target.$maxContentY, 0, 1) * (this.$ah - thumb);
		r.fillQuad(0, 0, this.$aw, this.$ah, r.packRgba(255, 255, 255, 0.08 * opacity));
		r.fillQuad(1, thumbY, this.$aw - 2, thumb, r.packRgba(255, 255, 255, (this.$dragging ? 0.6 : 0.35) * opacity));
	}
}

defineProp(
	ScrollBar.prototype,
	'target',
	function () {
		return this.$target;
	},
	function (v) {
		if (v !== this.$target) {
			this.$target = v || null;
			this.$update();
		}
	},
);

// ----- FontLoader -----

export class FontLoader extends Item {
	constructor() {
		super();
		this.$typeName = 'FontLoader';
		this.$nonVisual = true;
		this.$source = '';
		this.$name = '';
		this.$status = 'Null';
	}
}

defineProp(
	FontLoader.prototype,
	'source',
	function () {
		return this.$source;
	},
	function (v) {
		v = toStr(v);
		this.$source = v;
		hooks.loadFont(this, v);
		this.$update();
	},
);
defineReadOnly(FontLoader.prototype, 'name', function () {
	return this.$name;
});
defineReadOnly(FontLoader.prototype, 'status', function () {
	return this.$status;
});

// ----- Image -----

const IMAGE_FILL = { Stretch: 0, PreserveAspectFit: 1, PreserveAspectCrop: 2, Tile: 3, TileVertically: 4, TileHorizontally: 5, Pad: 6 };

export class ImageItem extends Item {
	constructor() {
		super();
		this.$typeName = 'Image';
		this.$source = '';
		this.$fillMode = 0;
		this.$hAlign = 4;
		this.$vAlign = 128;
		this.$img = null;
		this.$status = 0;
		this.$page = null;
		this.$pageKey = '';
		this.sourceSize = { width: 0, height: 0 };
	}

	$setImage(img, status) {
		this.$img = img;
		this.$status = status;
		this.$page = null;
		this.$pageKey = '';
		this.$update();
	}

	get $naturalWidth() {
		return this.$img === null ? 0 : this.$img.naturalWidth || this.$img.width || 0;
	}

	get $naturalHeight() {
		return this.$img === null ? 0 : this.$img.naturalHeight || this.$img.height || 0;
	}

	get $implicitWidth() {
		return this.$naturalWidth;
	}

	get $implicitHeight() {
		return this.$naturalHeight;
	}

	// The painted rectangle (local coordinates) and the part of the picture it shows (0..1).
	$fit() {
		const w = this.$aw;
		const h = this.$ah;
		const nw = this.$naturalWidth || w;
		const nh = this.$naturalHeight || h;
		const fit = { x: 0, y: 0, w, h, u0: 0, v0: 0, u1: 1, v1: 1 };
		if (nw <= 0 || nh <= 0) return fit;
		switch (this.$fillMode) {
			case 1: {
				const s = Math.min(w / nw, h / nh);
				fit.w = nw * s;
				fit.h = nh * s;
				fit.x = (w - fit.w) / 2;
				fit.y = (h - fit.h) / 2;
				break;
			}
			case 2: {
				const s = Math.max(w / nw, h / nh);
				const su = w / (nw * s);
				const sv = h / (nh * s);
				fit.u0 = (1 - su) / 2;
				fit.u1 = fit.u0 + su;
				fit.v0 = (1 - sv) / 2;
				fit.v1 = fit.v0 + sv;
				break;
			}
			case 6:
				fit.w = nw;
				fit.h = nh;
				fit.x = (w - nw) / 2;
				fit.y = (h - nh) / 2;
				break;
			default:
		}
		return fit;
	}

	$paint(r, opacity) {
		if (this.$status !== 1 || this.$img === null || this.$aw <= 0 || this.$ah <= 0) return;
		const fit = this.$fit();
		if (fit.w <= 0 || fit.h <= 0) return;
		const k = r.dpr * Math.max(1, Math.abs(r.ma));
		// The picture is rasterized for the painted size, so vector images stay sharp.
		const full = { w: fit.w / (fit.u1 - fit.u0), h: fit.h / (fit.v1 - fit.v0) };
		const pw = Math.min(2048, Math.max(1, Math.round(full.w * k)));
		const ph = Math.min(2048, Math.max(1, Math.round(full.h * k)));
		const nw = this.$naturalWidth;
		const nh = this.$naturalHeight;
		// Integer (or near-1:1) scales keep bitmap icons crisp; fractional scales need smoothing.
		const sx = nw > 0 ? pw / nw : 0;
		const sy = nh > 0 ? ph / nh : 0;
		const integerScale =
			sx > 0 &&
			sy > 0 &&
			Math.abs(sx - Math.round(sx)) < 0.02 &&
			Math.abs(sy - Math.round(sy)) < 0.02;
		const nearest = integerScale || (sx > 0 && sy > 0 && sx <= 1.02 && sy <= 1.02);
		const key = `${pw}x${ph}:${nearest ? 1 : 0}`;
		if (this.$page === null || this.$pageKey !== key) {
			if (typeof document === 'undefined') return;
			const canvas = document.createElement('canvas');
			canvas.width = pw;
			canvas.height = ph;
			const ctx = canvas.getContext('2d');
			ctx.imageSmoothingEnabled = !nearest;
			ctx.drawImage(this.$img, 0, 0, pw, ph);
			if (this.$page !== null) r.dropPage(this.$page);
			this.$page = { canvas, context: null, texture: null, dirty: true, size: Math.max(pw, ph), nearest };
			this.$pageKey = key;
		}
		r.image(this.$page, fit.x, fit.y, fit.w, fit.h, fit.u0, fit.v0, fit.u1, fit.v1, opacity);
	}
}

defineProp(
	ImageItem.prototype,
	'source',
	function () {
		return this.$source;
	},
	function (v) {
		v = toStr(v);
		if (v === this.$source) return;
		this.$source = v;
		this.$status = v === '' ? 0 : 2;
		hooks.loadImage(this, v);
		this.$update();
	},
	true,
);
defineProp(
	ImageItem.prototype,
	'fillMode',
	function () {
		return this.$fillMode;
	},
	function (v) {
		const next = typeof v === 'string' && IMAGE_FILL[v] !== undefined ? IMAGE_FILL[v] : toFloat(v);
		if (next !== this.$fillMode) {
			this.$fillMode = next;
			this.$update();
		}
	},
);
defineField(ImageItem.prototype, 'horizontalAlignment', '$hAlign', toFloat);
defineField(ImageItem.prototype, 'verticalAlignment', '$vAlign', toFloat);
defineReadOnly(ImageItem.prototype, 'status', function () {
	return this.$status;
});
defineReadOnly(ImageItem.prototype, 'progress', function () {
	return this.$status === 1 ? 1 : 0;
});
defineReadOnly(ImageItem.prototype, 'paintedWidth', function () {
	return this.$fit().w;
});
defineReadOnly(ImageItem.prototype, 'paintedHeight', function () {
	return this.$fit().h;
});

// ----- Flickable -----

// A viewport onto its children: they are shifted by (-contentX, -contentY). Scrolls with the wheel and by
// dragging an empty part of the viewport (a child that accepts the mouse keeps its own presses); no kinetic flick.
export class Flickable extends Item {
	constructor() {
		super();
		this.$typeName = 'Flickable';
		this.$contentX = 0;
		this.$contentY = 0;
		this.$contentW = 0;
		this.$contentH = 0;
		this.$interactive = true;
		this.$dragFrom = null;
		// As in Qt the visual children live in contentItem, which is the only entry of `children`.
		this.$content = new Item();
		super.$adopt(this.$content);
	}

	get contentItem() {
		return this.$content;
	}

	// A Repeater is a visual item in Qt: its delegates share its parent, the content item.
	$adopt(child) {
		if (child === this.$content || (child.$nonVisual && !(child instanceof Repeater))) super.$adopt(child);
		else this.$content.$adopt(child);
	}

	get $maxContentX() {
		return Math.max(0, this.$contentW - this.$aw);
	}

	get $maxContentY() {
		return Math.max(0, this.$contentH - this.$ah);
	}

	get $ox() {
		return this.$contentX;
	}

	get $oy() {
		return this.$contentY;
	}

	$renderContent(r, opacity) {
		this.$paint(r, opacity);
		let clipToken = null;
		if (this.$clip) {
			clipToken = r.beginClip(this.$aw, this.$ah);
			if (clipToken === null) return;
		}
		const a = r.ma, b = r.mb, c = r.mc, d = r.md, e = r.me, f = r.mf;
		r.translate(-this.$contentX, -this.$contentY);
		const children = this.$hasZOrder() ? this.$childrenByZ() : this.$children;
		for (let i = 0; i < children.length; i++) children[i].$render(r, opacity);
		r.setMatrix(a, b, c, d, e, f);
		this.$paintOverlay(r, opacity);
		if (clipToken !== null) r.endClip(clipToken);
	}

	get $acceptsMouse() {
		return this.$interactive && (this.$maxContentX > 0 || this.$maxContentY > 0);
	}

	get $acceptsWheel() {
		return this.$interactive && (this.$maxContentX > 0 || this.$maxContentY > 0);
	}

	$mouseWheel(notches) {
		const delta = notches * WHEEL_NOTCH_PX;
		if (this.$maxContentY > 0) this.contentY = clamp(this.$contentY - delta, 0, this.$maxContentY);
		else this.contentX = clamp(this.$contentX - delta, 0, this.$maxContentX);
	}

	$mousePressAt(sceneX, sceneY) {
		this.$dragFrom = { x: sceneX, y: sceneY, cx: this.$contentX, cy: this.$contentY };
	}

	$mouseMoveAt(sceneX, sceneY) {
		const from = this.$dragFrom;
		if (from === null || !this.$interactive) return;
		this.contentX = clamp(from.cx - (sceneX - from.x), 0, this.$maxContentX);
		this.contentY = clamp(from.cy - (sceneY - from.y), 0, this.$maxContentY);
	}

	$pressChanged(pressed) {
		if (!pressed) this.$dragFrom = null;
	}

	flick() {}

	cancelFlick() {}

	returnToBounds() {
		this.contentX = clamp(this.$contentX, 0, this.$maxContentX);
		this.contentY = clamp(this.$contentY, 0, this.$maxContentY);
	}
}

function defineContent(name, field, sizeField = null) {
	defineProp(
		Flickable.prototype,
		name,
		function () {
			return this[field];
		},
		function (value) {
			const v = toFloat(value);
			if (this[field] === v) return;
			this[field] = v;
			if (sizeField !== null) {
				this.$content[sizeField] = v;
				this.$content.$update();
			}
			this.$update();
			if (hooks.changed !== null && this.$chg !== undefined) hooks.changed(this, name);
		},
	);
}
defineContent('contentX', '$contentX');
defineContent('contentY', '$contentY');
defineContent('contentWidth', '$contentW', '$w');
defineContent('contentHeight', '$contentH', '$h');
defineField(Flickable.prototype, 'interactive', '$interactive', toBool);
defineReadOnly(Flickable.prototype, 'atXBeginning', function () {
	return this.$contentX <= 0;
});
defineReadOnly(Flickable.prototype, 'atYBeginning', function () {
	return this.$contentY <= 0;
});
defineReadOnly(Flickable.prototype, 'atXEnd', function () {
	return this.$contentX >= this.$maxContentX;
});
defineReadOnly(Flickable.prototype, 'atYEnd', function () {
	return this.$contentY >= this.$maxContentY;
});

// ----- TextEdit and TextInput -----

const CARET_WIDTH = 1;
const CARET_BLINK_SECONDS = 0.53;
const MULTI_CLICK_MS = 450;
const PASSWORD_CHAR = '\u25cf';
const HIGHLIGHT = parseColor('#0078d7');
const HIGHLIGHTED_TEXT = parseColor('#ffffff');
const WORD_CHAR = /[\p{L}\p{N}_]/u;
const ALIGN_BITS = { AlignLeft: 1, AlignRight: 2, AlignHCenter: 4, AlignJustify: 8, AlignTop: 0x20, AlignBottom: 0x40, AlignVCenter: 0x80 };
const ECHO_VALUE = { Normal: 0, NoEcho: 1, Password: 2, PasswordEchoOnEdit: 3 };
// What a change of the text changes for bindings that read the editor.
const TEXT_DEPENDENT = ['text', 'length', 'displayText', 'selectedText', 'contentWidth', 'contentHeight', 'paintedWidth', 'paintedHeight', 'lineCount', 'implicitWidth', 'implicitHeight'];
// What a change of the font, the wrapping or the padding changes.
const LAYOUT_DEPENDENT = ['displayText', 'contentWidth', 'contentHeight', 'paintedWidth', 'paintedHeight', 'lineCount', 'implicitWidth', 'implicitHeight', 'cursorRectangle'];

function clampIndex(value, length) {
	const n = Number(value);
	if (!Number.isFinite(n)) return 0;
	return Math.max(0, Math.min(length, Math.trunc(n)));
}

const isHighSurrogate = (c) => c >= 0xd800 && c <= 0xdbff;
const isLowSurrogate = (c) => c >= 0xdc00 && c <= 0xdfff;

function stepBack(text, i) {
	if (i <= 0) return 0;
	return i >= 2 && isLowSurrogate(text.charCodeAt(i - 1)) && isHighSurrogate(text.charCodeAt(i - 2)) ? i - 2 : i - 1;
}

function stepForward(text, i) {
	if (i >= text.length) return text.length;
	return i + 1 < text.length && isHighSurrogate(text.charCodeAt(i)) && isLowSurrogate(text.charCodeAt(i + 1)) ? i + 2 : i + 1;
}

function wordLeft(text, i) {
	let p = i;
	while (p > 0 && !WORD_CHAR.test(text[p - 1])) p--;
	while (p > 0 && WORD_CHAR.test(text[p - 1])) p--;
	return p;
}

function wordRight(text, i) {
	let p = i;
	while (p < text.length && WORD_CHAR.test(text[p])) p++;
	while (p < text.length && !WORD_CHAR.test(text[p])) p++;
	return p;
}

function toAlign(value) {
	if (typeof value === 'string') return value in ALIGN_BITS ? ALIGN_BITS[value] : 1;
	const n = Number(value);
	return Number.isFinite(n) ? n : 1;
}

function toEcho(value) {
	if (typeof value === 'string') return value in ECHO_VALUE ? ECHO_VALUE[value] : 0;
	const n = Number(value);
	return Number.isFinite(n) ? n : 0;
}

// The lines of a text as index ranges: a hard break and the space a wrap consumes belong to no line.
function textRanges(info, text, limit, anywhere) {
	const out = [];
	let start = 0;
	for (;;) {
		const nl = text.indexOf('\n', start);
		const stop = nl < 0 ? text.length : nl;
		if (limit === Infinity) out.push({ start, end: stop });
		else wrapRanges(info, text, start, stop, limit, anywhere, out);
		if (nl < 0) return out;
		start = nl + 1;
	}
}

function wrapRanges(info, text, start, stop, limit, anywhere, out) {
	const space = info.advance(' ');
	let lineStart = start;
	let width = 0;
	let pos = start;
	while (pos <= stop) {
		let wordEnd = text.indexOf(' ', pos);
		if (wordEnd < 0 || wordEnd > stop) wordEnd = stop;
		const w = textWidth(info, text.slice(pos, wordEnd));
		if (pos > lineStart && width + space + w > limit) {
			out.push({ start: lineStart, end: pos - 1 });
			lineStart = pos;
			width = 0;
		}
		width = pos > lineStart ? width + space + w : w;
		if (anywhere && w > limit) {
			// A word wider than the line is broken between characters.
			let acc = 0;
			for (let i = pos; i < wordEnd; ) {
				const next = stepForward(text, i);
				const cw = info.advance(text.slice(i, next));
				if (acc + cw > limit && i > lineStart) {
					out.push({ start: lineStart, end: i });
					lineStart = i;
					acc = 0;
				}
				acc += cw;
				i = next;
			}
			width = acc;
		}
		pos = wordEnd + 1;
	}
	out.push({ start: lineStart, end: stop });
}

// The editing core of TextInput (one line) and TextEdit (many lines). The cursor and the other end of the selection
// (`$anchor`) are indexes into `$text`; undo steps are snapshots taken before a change.
class TextEditor extends Item {
	constructor(multiline) {
		super();
		this.$multi = multiline;
		this.$text = '';
		this.$color = BLACK;
		this.$pixelSize = 12;
		this.$family = '';
		this.$bold = false;
		this.$italic = false;
		this.$underline = false;
		this.$strikeout = false;
		this.$font = null;
		this.$cursor = 0;
		this.$anchor = 0;
		this.$readOnly = false;
		this.$echo = 0;
		this.$passChar = PASSWORD_CHAR;
		this.$maxLength = 32767;
		this.$wrap = 0;
		this.$format = 'PlainText';
		this.$hAlign = 1;
		this.$vAlign = 0x20;
		this.$selColor = HIGHLIGHT;
		this.$selTextColor = HIGHLIGHTED_TEXT;
		this.$selectByMouse = true;
		this.$focusOnPress = true;
		this.$persistent = false;
		this.$padL = 0;
		this.$padR = 0;
		this.$padT = 0;
		this.$padB = 0;
		this.$validator = null;
		this.$mask = '';
		this.$undo = [];
		this.$redo = [];
		this.$run = ''; // what the last change was ('type', 'delete'): the next one of the same kind joins its undo step
		this.$hscroll = 0;
		this.$cache = null;
		this.$caretOn = true;
		this.$blinkReset = false;
		this.$blinkStart = 0;
		this.$goalX = -1; // the x a vertical cursor move keeps
		this.$clicks = 0;
		this.$lastPress = 0;
		this.$lastX = 0;
		this.$lastY = 0;
		this.$dragging = false;
		this.$seen = { cursor: 0, lo: 0, hi: 0, undo: 0, redo: 0 };
		this.$arrangedH = NaN;
		this.$cursorShape = 4; // Qt.IBeamCursor
		this.$tabFocus = true;
		this.$focused = false;
		this.$declareSignal('accepted', []);
		this.$declareSignal('editingFinished', []);
		this.$declareSignal('textEdited', []);
		this.$connect('focusChanged', () => this.$focusEdge());
		this.$connect('activeFocusChanged', () => this.$focusEdge());
	}

	get font() {
		return this.$font !== null ? this.$font : (this.$font = new FontGroup(this));
	}

	$fontChanged() {
		this.$layoutChanged();
	}

	$layoutChanged() {
		this.$cache = null;
		for (const name of LAYOUT_DEPENDENT) noteWrite(this, name);
	}

	// Rich text is shown as plain text and cannot be edited or selected by the mouse.
	get $rich() {
		return this.$multi && this.$format === 'RichText';
	}

	get $editable() {
		return !this.$readOnly && !this.$rich;
	}

	get $lo() {
		return Math.min(this.$cursor, this.$anchor);
	}

	get $hi() {
		return Math.max(this.$cursor, this.$anchor);
	}

	get $hasSelection() {
		return this.$cursor !== this.$anchor;
	}

	// The key events of the window come here: `focus: true` alone has no activeFocus before the first forceActiveFocus().
	get $active() {
		return this.$focus && (activeSet.has(this) || getFocusItem() === this);
	}

	// ----- layout -----

	// What is drawn: the text itself, bullets for a password, nothing without echo. Same length as the text.
	$shown() {
		if (this.$rich) return richPlain(this.$text);
		if (!this.$multi) {
			if (this.$echo === 2 || this.$echo === 3) return this.$passChar.repeat(this.$text.length);
			if (this.$echo === 1) return '';
		}
		return this.$text;
	}

	$laid() {
		const family = this.$family !== '' ? this.$family : fonts.defaultFamily();
		const info = fonts.get(family, this.$pixelSize > 0 ? this.$pixelSize : 12, this.$bold, this.$italic);
		const shown = this.$shown();
		let limit = Infinity;
		if (this.$multi && wraps(this.$wrap)) {
			const w = (this.$w > 0 ? this.$w : this.$aw) - this.$padL - this.$padR;
			if (w > 0) limit = w;
		}
		const cached = this.$cache;
		if (cached !== null && cached.info === info && cached.version === fonts.version && cached.shown === shown && cached.limit === limit) return cached;
		const step = this.$multi && shown !== '' ? info.lineSpacing : info.lineHeight;
		const lines = this.$multi ? textRanges(info, shown, limit, this.$wrap === 3 || this.$wrap === 4) : [{ start: 0, end: shown.length }];
		let width = 0;
		for (const line of lines) {
			line.width = textWidth(info, shown.slice(line.start, line.end));
			width = Math.max(width, line.width);
		}
		const lay = { info, version: fonts.version, shown, limit, step, lines, width, height: lines.length * step, natural: -1 };
		this.$cache = lay;
		return lay;
	}

	// The widest line without wrapping.
	$naturalWidth(lay) {
		if (lay.natural < 0) {
			let widest = 0;
			for (const part of lay.shown.split('\n')) widest = Math.max(widest, textWidth(lay.info, part));
			lay.natural = widest;
		}
		return lay.natural;
	}

	get $implicitWidth() {
		const lay = this.$laid();
		return (this.$multi ? this.$naturalWidth(lay) : lay.width) + this.$padL + this.$padR;
	}

	get $implicitHeight() {
		return this.$laid().height + this.$padT + this.$padB;
	}

	// The height of wrapped text follows the width the layout gave the item.
	$arrange() {
		if (!this.$multi) return;
		const h = this.$effH;
		if (h !== this.$arrangedH) {
			this.$arrangedH = h;
			this.$layoutChanged();
			this.$update();
		}
	}

	$originX(lay, line) {
		const free = this.$aw - this.$padL - this.$padR - line.width;
		if (free > 0) {
			if ((this.$hAlign & 4) !== 0) return this.$padL + free / 2;
			if ((this.$hAlign & 2) !== 0) return this.$padL + free;
			return this.$padL;
		}
		return this.$padL - this.$hscroll;
	}

	$originY(lay) {
		const free = this.$ah - this.$padT - this.$padB - lay.height;
		if (free > 0) {
			if ((this.$vAlign & 0x80) !== 0) return this.$padT + free / 2;
			if ((this.$vAlign & 0x40) !== 0) return this.$padT + free;
		}
		return this.$padT;
	}

	$lineOf(lay, index) {
		const lines = lay.lines;
		let row = 0;
		for (let i = 1; i < lines.length && index >= lines[i].start; i++) row = i;
		return row;
	}

	// The line, the x and the y (item coordinates) of the cursor at an index.
	$caretAt(lay, index) {
		const i = Math.min(index, lay.shown.length);
		const row = this.$lineOf(lay, i);
		const line = lay.lines[row];
		const x = this.$originX(lay, line) + textWidth(lay.info, lay.shown.slice(line.start, Math.min(i, line.end)));
		return { row, x, y: this.$originY(lay) + row * lay.step };
	}

	// The index nearest to an x (item coordinates) in a line.
	$indexInLine(lay, row, x) {
		const line = lay.lines[row];
		const at = x - this.$originX(lay, line);
		let pos = line.start;
		let acc = 0;
		while (pos < line.end) {
			const next = stepForward(lay.shown, pos);
			const w = lay.info.advance(lay.shown.slice(pos, next));
			if (at < acc + w / 2) break;
			acc += w;
			pos = next;
		}
		return pos;
	}

	$indexAt(sceneX, sceneY) {
		return this.positionAt(sceneX - this.$sx, sceneY - this.$sy);
	}

	// Keeps the cursor inside a single line that is wider than the item.
	$scrollToCursor() {
		if (this.$multi) return;
		const box = this.$aw - this.$padL - this.$padR;
		if (box <= 0) return;
		const lay = this.$laid();
		const width = lay.lines[0].width;
		let h = this.$hscroll;
		if (width <= box) {
			h = 0;
		} else {
			const x = textWidth(lay.info, lay.shown.slice(0, Math.min(this.$cursor, lay.shown.length)));
			if (x - h > box - CARET_WIDTH) h = x - box + CARET_WIDTH;
			if (x - h < 0) h = x;
			h = Math.max(0, Math.min(h, width - box + CARET_WIDTH));
		}
		this.$hscroll = h;
	}

	// ----- changes -----

	$textChanged() {
		for (const name of TEXT_DEPENDENT) noteWrite(this, name);
		if (hooks.changed !== null) hooks.changed(this, 'text');
	}

	// Tells what read the cursor or the selection that they moved, and asks for a frame.
	$sync() {
		const seen = this.$seen;
		const lo = this.$lo;
		const hi = this.$hi;
		if (seen.cursor !== this.$cursor) {
			seen.cursor = this.$cursor;
			noteWrite(this, 'cursorPosition');
			noteWrite(this, 'cursorRectangle');
		}
		if (seen.lo !== lo) {
			seen.lo = lo;
			noteWrite(this, 'selectionStart');
			noteWrite(this, 'selectedText');
		}
		if (seen.hi !== hi) {
			seen.hi = hi;
			noteWrite(this, 'selectionEnd');
			noteWrite(this, 'selectedText');
		}
		if (seen.undo !== this.$undo.length) {
			seen.undo = this.$undo.length;
			noteWrite(this, 'canUndo');
		}
		if (seen.redo !== this.$redo.length) {
			seen.redo = this.$redo.length;
			noteWrite(this, 'canRedo');
		}
		this.$update();
	}

	// Something about the text, the cursor or the selection changed: the cursor shows and stays in view.
	$touched() {
		this.$blinkReset = true;
		this.$caretOn = true;
		this.$scrollToCursor();
		this.$sync();
	}

	$moveTo(index, extend, keepGoal = false) {
		const at = clampIndex(index, this.$text.length);
		this.$cursor = at;
		if (!extend) this.$anchor = at;
		this.$run = '';
		if (!keepGoal) this.$goalX = -1;
		this.$touched();
	}

	// Replaces [start, end) with `value`. With `move` the cursor and the selection follow the change as in Qt's insert()
	// and remove(); otherwise the cursor goes behind the new text.
	$splice(start, end, value, kind, move) {
		const room = this.$maxLength - (this.$text.length - (end - start));
		if (value.length > room) {
			value = value.slice(0, Math.max(0, room));
			if (value !== '' && isHighSurrogate(value.charCodeAt(value.length - 1))) value = value.slice(0, -1);
		}
		if (start === end && value === '') return false;
		const mapped = (p) => (p >= end ? p + value.length - (end - start) : p > start ? start : p);
		const joins = (kind === 'type' || kind === 'delete') && this.$run === kind;
		if (!joins) {
			this.$undo.push(this.$snapshot());
			if (this.$undo.length > 500) this.$undo.shift();
		}
		this.$run = kind;
		this.$redo.length = 0;
		this.$text = this.$text.slice(0, start) + value + this.$text.slice(end);
		if (move) {
			this.$cursor = mapped(this.$cursor);
			this.$anchor = mapped(this.$anchor);
		} else {
			this.$cursor = start + value.length;
			this.$anchor = this.$cursor;
		}
		this.$textChanged();
		this.$touched();
		return true;
	}

	$snapshot() {
		return { text: this.$text, cursor: this.$cursor, anchor: this.$anchor };
	}

	$restore(state) {
		const before = this.$text;
		const textChanged = state.text !== before;
		this.$text = state.text;
		this.$cursor = state.cursor;
		this.$anchor = state.anchor;
		if (this.$multi) {
			// A document puts the cursor behind the part of the text that came back.
			let head = 0;
			const shortest = Math.min(before.length, state.text.length);
			while (head < shortest && before[head] === state.text[head]) head++;
			let tail = 0;
			while (tail < shortest - head && before[before.length - 1 - tail] === state.text[state.text.length - 1 - tail]) tail++;
			this.$cursor = state.text.length - tail;
			this.$anchor = this.$cursor;
		}
		this.$run = '';
		if (textChanged) this.$textChanged();
		this.$touched();
	}

	// A script assigned the text: the history is gone, the cursor goes to the end (one line) or the start (many).
	$assignText(value) {
		let next = value === undefined || value === null ? '' : String(value);
		if (next.length > this.$maxLength) next = next.slice(0, this.$maxLength);
		if (next === this.$text) return;
		this.$text = next;
		this.$cursor = this.$multi ? 0 : next.length;
		this.$anchor = this.$cursor;
		this.$undo.length = 0;
		this.$redo.length = 0;
		this.$run = '';
		this.$hscroll = 0;
		this.$textChanged();
		this.$touched();
	}

	// ----- the API of the QML types -----

	select(start, end) {
		const n = this.$text.length;
		this.$anchor = clampIndex(start, n);
		this.$cursor = clampIndex(end, n);
		this.$run = '';
		this.$touched();
	}

	selectAll() {
		this.select(0, this.$text.length);
	}

	deselect() {
		this.$anchor = this.$cursor;
		this.$touched();
	}

	selectWord() {
		const t = this.$text;
		let at = this.$cursor;
		if ((at >= t.length || !WORD_CHAR.test(t[at])) && at > 0 && WORD_CHAR.test(t[at - 1])) at--;
		let start = at;
		let end = at;
		while (start > 0 && WORD_CHAR.test(t[start - 1])) start--;
		while (end < t.length && WORD_CHAR.test(t[end])) end++;
		this.select(start, end);
	}

	getText(start, end) {
		const n = this.$text.length;
		const a = clampIndex(start, n);
		const b = clampIndex(end, n);
		return this.$text.slice(Math.min(a, b), Math.max(a, b));
	}

	insert(position, value) {
		if (!(position >= 0 && position <= this.$text.length)) return;
		this.$splice(position, position, String(value), 'api', true);
	}

	remove(start, end) {
		const n = this.$text.length;
		const a = clampIndex(start, n);
		const b = clampIndex(end, n);
		this.$splice(Math.min(a, b), Math.max(a, b), '', 'api', true);
	}

	clear() {
		this.$splice(0, this.$text.length, '', 'api', false);
	}

	copy() {
		if (this.$rich) {
			if (!this.$hasSelection) return;
			const plain = this.$text.replace(/<br\s*\/?>|<\/p>/gi, '\n').replace(/<[^>]*>/g, '').replace(/&nbsp;/g, ' ').replace(/&lt;/g, '<').replace(/&gt;/g, '>').replace(/&quot;/g, '"').replace(/&amp;/g, '&');
			hooks.clipboardWrite(plain.trim(), this.$text);
			return;
		}
		if (!this.$multi && this.$echo !== 0) return;
		const selected = this.getText(this.$anchor, this.$cursor);
		if (selected !== '') hooks.clipboardWrite(selected, '');
	}

	cut() {
		if (!this.$editable || !this.$hasSelection || (!this.$multi && this.$echo !== 0)) return;
		this.copy();
		this.$splice(this.$lo, this.$hi, '', 'cut', false);
		this.$emit('textEdited', []);
	}

	paste() {
		if (this.$readOnly) return;
		const clip = hooks.clipboardRead();
		if (this.$rich) {
			this.$assignText(clip.html);
			this.$moveTo(this.$text.length, false);
			return;
		}
		const text = this.$multi ? clip.text.replace(/\r\n?/g, '\n') : clip.text.replace(/\r?\n|\r/g, ' ');
		if (text === '' && !this.$hasSelection) return;
		this.$splice(this.$lo, this.$hi, text, 'paste', false);
		this.$emit('textEdited', []);
	}

	undo() {
		if (this.$undo.length === 0 || (!this.$multi && this.$readOnly)) return;
		this.$redo.push(this.$snapshot());
		this.$restore(this.$undo.pop());
		this.$emit('textEdited', []);
	}

	redo() {
		if (this.$redo.length === 0 || (!this.$multi && this.$readOnly)) return;
		this.$undo.push(this.$snapshot());
		this.$restore(this.$redo.pop());
		this.$emit('textEdited', []);
	}

	ensureVisible(position) {
		const keep = this.$cursor;
		this.$cursor = clampIndex(position, this.$text.length);
		this.$scrollToCursor();
		this.$cursor = keep;
		this.$update();
	}

	positionAt(x, y = 0) {
		if (this.$rich) return 0;
		const lay = this.$laid();
		const row = this.$multi ? Math.max(0, Math.min(lay.lines.length - 1, Math.floor((y - this.$originY(lay)) / lay.step))) : 0;
		return this.$indexInLine(lay, row, x);
	}

	positionToRectangle(position) {
		const lay = this.$laid();
		const at = this.$caretAt(lay, clampIndex(position, this.$text.length));
		return { x: at.x, y: at.y, width: CARET_WIDTH, height: lay.info.lineHeight };
	}

	moveCursorSelection(position) {
		this.$moveTo(position, true);
	}

	isRightToLeft() {
		return false;
	}

	// ----- keys -----

	$keyPressed(event) {
		const mods = event.modifiers;
		const handled = this.$handleKey(event.key, (mods & MODIFIERS.ControlModifier) !== 0, (mods & MODIFIERS.ShiftModifier) !== 0, (mods & MODIFIERS.AltModifier) !== 0, event.text);
		if (handled) {
			this.$blinkReset = true;
			this.$caretOn = true;
			this.$update();
		}
		return handled;
	}

	$handleKey(key, ctrl, shift, alt, text) {
		if (ctrl && !alt) {
			switch (key) {
				case KEY.Key_A:
					this.selectAll();
					return true;
				case KEY.Key_C:
					this.copy();
					return true;
				case KEY.Key_X:
					this.cut();
					return true;
				case KEY.Key_V:
					this.paste();
					return true;
				case KEY.Key_Z:
					if (shift) this.redo();
					else this.undo();
					return true;
				case KEY.Key_Y:
					this.redo();
					return true;
				default:
					break;
			}
		}
		switch (key) {
			case KEY.Key_Left:
				return this.$arrow(-1, ctrl, shift);
			case KEY.Key_Right:
				return this.$arrow(1, ctrl, shift);
			case KEY.Key_Home:
				this.$moveTo(ctrl || !this.$multi ? 0 : this.$caretLine().start, shift);
				return true;
			case KEY.Key_End:
				this.$moveTo(ctrl || !this.$multi ? this.$text.length : this.$caretLine().end, shift);
				return true;
			case KEY.Key_Up:
			case KEY.Key_Down:
			case KEY.Key_PageUp:
			case KEY.Key_PageDown:
				return this.$multi && !this.$rich ? this.$vertical(key, shift) : false;
			case KEY.Key_Backspace:
				return this.$erase(-1, ctrl);
			case KEY.Key_Delete:
				return this.$erase(1, ctrl);
			case KEY.Key_Return:
			case KEY.Key_Enter:
				return this.$enter();
			default:
				break;
		}
		if (text !== '' && !ctrl && !alt) {
			const c = text.charCodeAt(0);
			if (c >= 0x20 && c !== 0x7f) return this.$type(text);
		}
		return false;
	}

	$caretLine() {
		const lay = this.$laid();
		return lay.lines[this.$lineOf(lay, Math.min(this.$cursor, lay.shown.length))];
	}

	// Left and right keep the event when the cursor cannot move, so that a KeyNavigation above can take it.
	$arrow(direction, ctrl, shift) {
		const t = this.$text;
		const from = this.$cursor;
		let to;
		if (!shift && this.$hasSelection) to = direction < 0 ? this.$lo : this.$hi;
		else if (ctrl) to = direction < 0 ? wordLeft(t, from) : wordRight(t, from);
		else to = direction < 0 ? stepBack(t, from) : stepForward(t, from);
		if (to === from && !this.$hasSelection && !shift) return false;
		this.$moveTo(to, shift);
		return true;
	}

	$vertical(key, shift) {
		const lay = this.$laid();
		const at = this.$caretAt(lay, this.$cursor);
		const page = Math.max(1, Math.floor((this.$ah - this.$padT - this.$padB) / lay.step));
		const delta = key === KEY.Key_Up ? -1 : key === KEY.Key_Down ? 1 : key === KEY.Key_PageUp ? -page : page;
		const goal = this.$goalX >= 0 ? this.$goalX : at.x;
		const row = at.row + delta;
		let to;
		if (row < 0) to = 0;
		else if (row >= lay.lines.length) to = this.$text.length;
		else to = this.$indexInLine(lay, row, goal);
		this.$moveTo(to, shift, true);
		this.$goalX = goal;
		return true;
	}

	$erase(direction, ctrl) {
		if (!this.$editable) return false;
		const t = this.$text;
		if (this.$hasSelection) {
			this.$splice(this.$lo, this.$hi, '', 'delete', false);
		} else {
			const from = this.$cursor;
			const to = direction < 0 ? (ctrl ? wordLeft(t, from) : stepBack(t, from)) : ctrl ? wordRight(t, from) : stepForward(t, from);
			if (to === from) return true;
			this.$splice(Math.min(from, to), Math.max(from, to), '', 'delete', false);
		}
		this.$emit('textEdited', []);
		return true;
	}

	// Return ends the input of a TextInput and is left to the items above; a TextEdit starts a new line.
	$enter() {
		if (this.$multi) return this.$editable ? this.$type('\n') : false;
		this.$emit('accepted', []);
		this.$emit('editingFinished', []);
		return false;
	}

	$type(text) {
		if (!this.$editable) return false;
		const value = this.$multi ? text : text.replace(/[\r\n]/g, ' ');
		this.$splice(this.$lo, this.$hi, value, 'type', false);
		this.$emit('textEdited', []);
		return true;
	}

	// ----- focus and the text cursor -----

	$focusEdge() {
		const now = this.$active;
		if (now === this.$focused) return;
		this.$focused = now;
		if (now) {
			this.$caretOn = true;
			this.$blinkReset = true;
			hooks.caret(this);
			this.$update();
			return;
		}
		if (!this.$persistent) this.$anchor = this.$cursor;
		this.$run = '';
		this.$emit('editingFinished', []);
		this.$sync();
	}

	$blink(seconds) {
		if (this.$blinkReset) {
			this.$blinkReset = false;
			this.$blinkStart = seconds;
		}
		const on = Math.floor((seconds - this.$blinkStart) / CARET_BLINK_SECONDS) % 2 === 0;
		if (on !== this.$caretOn) {
			this.$caretOn = on;
			this.$update();
		}
	}

	// ----- mouse -----

	get $acceptsMouse() {
		return itemEnabled(this);
	}

	$mousePressAt(sceneX, sceneY) {
		if (this.$focusOnPress) this.forceActiveFocus();
		const index = this.$indexAt(sceneX, sceneY);
		const now = Date.now();
		const again = now - this.$lastPress < MULTI_CLICK_MS && Math.abs(sceneX - this.$lastX) < 4 && Math.abs(sceneY - this.$lastY) < 4;
		this.$clicks = again ? this.$clicks + 1 : 1;
		this.$lastPress = now;
		this.$lastX = sceneX;
		this.$lastY = sceneY;
		this.$dragging = false;
		if (this.$selectByMouse && this.$clicks >= 2 && !this.$rich) {
			this.$moveTo(index, false);
			if (this.$clicks === 2) {
				this.selectWord();
			} else {
				const t = this.$text;
				this.select(this.$multi ? t.lastIndexOf('\n', index - 1) + 1 : 0, this.$multi ? (t.indexOf('\n', index) < 0 ? t.length : t.indexOf('\n', index)) : t.length);
			}
			return;
		}
		this.$moveTo(index, false);
		this.$dragging = this.$selectByMouse && !this.$rich;
	}

	$mouseMoveAt(sceneX, sceneY) {
		if (this.$dragging) this.$moveTo(this.$indexAt(sceneX, sceneY), true);
	}

	$pressChanged(pressed) {
		if (!pressed) this.$dragging = false;
	}

	// ----- painting -----

	// A line of text wider than its item is cut off.
	$renderContent(r, opacity) {
		let clipToken = null;
		if (this.$clip || (!this.$multi && this.$laid().width > this.$aw - this.$padL - this.$padR)) {
			clipToken = r.beginClip(this.$aw, this.$ah);
			if (clipToken === null) return;
		}
		this.$paint(r, opacity);
		const children = this.$hasZOrder() ? this.$childrenByZ() : this.$children;
		for (let i = 0; i < children.length; i++) children[i].$render(r, opacity);
		this.$paintOverlay(r, opacity);
		if (clipToken !== null) r.endClip(clipToken);
	}

	$paint(r, opacity) {
		this.$scrollToCursor();
		const lay = this.$laid();
		const info = lay.info;
		const text = lay.shown;
		const color = r.pack(this.$color, opacity);
		const selected = !this.$rich && this.$hasSelection;
		const lo = this.$lo;
		const hi = this.$hi;
		const selectionBack = r.pack(this.$selColor, opacity);
		const selectionText = r.pack(this.$selTextColor, opacity);
		const y0 = this.$originY(lay);
		for (let row = 0; row < lay.lines.length; row++) {
			const line = lay.lines[row];
			const top = y0 + row * lay.step;
			const x0 = this.$originX(lay, line);
			if (selected) {
				const from = Math.max(lo, line.start);
				const to = Math.min(hi, line.end);
				const newline = hi > line.end && lo <= line.end && row < lay.lines.length - 1;
				if (from < to || newline) {
					const left = x0 + textWidth(info, text.slice(line.start, Math.min(from, line.end)));
					let right = x0 + textWidth(info, text.slice(line.start, Math.max(from, to)));
					if (newline) right += info.advance(' ');
					r.fillQuad(left, top, right - left, lay.step, selectionBack);
				}
			}
			let pen = x0;
			for (let i = line.start; i < line.end; ) {
				const next = stepForward(text, i);
				const ch = text.slice(i, next);
				const glyph = info.glyph(ch);
				if (glyph !== null) r.glyph(glyph, pen, top + info.ascent, selected && i >= lo && i < hi ? selectionText : color);
				pen += info.advance(ch);
				i = next;
			}
		}
		if (this.$caretOn && this.$active && !this.$readOnly && !this.$rich) {
			const at = this.$caretAt(lay, this.$cursor);
			r.fillQuad(Math.floor(at.x), at.y, CARET_WIDTH, info.lineHeight, color);
		}
		// An editor that got the focus by `focus: true` is told nothing: it notices at the first frame it shows it.
		if (this.$active && !this.$focused) {
			this.$focused = true;
			hooks.caret(this);
		}
	}
}

export class TextEdit extends TextEditor {
	constructor() {
		super(true);
		this.$typeName = 'TextEdit';
	}

	append(value) {
		this.$splice(this.$text.length, this.$text.length, (this.$text === '' ? '' : '\n') + String(value), 'api', true);
	}
}

export class TextInput extends TextEditor {
	constructor() {
		super(false);
		this.$typeName = 'TextInput';
	}
}

defineProp(
	TextEditor.prototype,
	'text',
	function () {
		return this.$text;
	},
	function (value) {
		this.$assignText(value);
	},
	true,
);
defineProp(
	TextEditor.prototype,
	'cursorPosition',
	function () {
		return this.$cursor;
	},
	function (value) {
		this.$moveTo(value, false);
	},
);
defineProp(
	TextEditor.prototype,
	'maximumLength',
	function () {
		return this.$maxLength;
	},
	function (value) {
		const n = Math.trunc(Number(value));
		this.$maxLength = Number.isFinite(n) && n >= 0 ? n : 32767;
		// Qt drops the undo history when the limit is set.
		this.$undo.length = 0;
		this.$redo.length = 0;
		this.$run = '';
		if (this.$text.length > this.$maxLength) this.$assignText(this.$text.slice(0, this.$maxLength));
		else this.$sync();
	},
);
defineProp(
	TextEditor.prototype,
	'passwordCharacter',
	function () {
		return this.$passChar;
	},
	function (value) {
		const s = value === undefined || value === null ? '' : String(value);
		this.$passChar = s.length === 1 ? s : PASSWORD_CHAR;
		this.$layoutChanged();
		this.$update();
	},
);
defineProp(
	TextEditor.prototype,
	'padding',
	function () {
		return this.$padL;
	},
	function (value) {
		const v = toFloat(value);
		this.$padL = this.$padR = this.$padT = this.$padB = v;
		this.$layoutChanged();
		this.$update();
	},
);
// A property whose change moves the size of the text.
for (const [name, field, coerce] of [
	['leftPadding', '$padL', toFloat],
	['rightPadding', '$padR', toFloat],
	['topPadding', '$padT', toFloat],
	['bottomPadding', '$padB', toFloat],
	['wrapMode', '$wrap', toWrapMode],
	['echoMode', '$echo', toEcho],
	['textFormat', '$format', (value) => (value === undefined || value === null ? 'PlainText' : String(value))],
]) {
	defineProp(
		TextEditor.prototype,
		name,
		function () {
			return this[field];
		},
		function (value) {
			const v = coerce(value);
			if (same(this[field], v)) return;
			this[field] = v;
			this.$layoutChanged();
			this.$update();
		},
	);
}
const colorProp = (name, field) =>
	defineProp(
		TextEditor.prototype,
		name,
		function () {
			return colorToString(this[field]);
		},
		function (value) {
			this[field] = toColor(value);
			this.$update();
		},
	);
colorProp('color', '$color');
colorProp('selectionColor', '$selColor');
colorProp('selectedTextColor', '$selTextColor');
defineField(TextEditor.prototype, 'readOnly', '$readOnly', toBool);
defineField(TextEditor.prototype, 'horizontalAlignment', '$hAlign', toAlign);
defineField(TextEditor.prototype, 'verticalAlignment', '$vAlign', toAlign);
defineField(TextEditor.prototype, 'selectByMouse', '$selectByMouse', toBool);
defineField(TextEditor.prototype, 'activeFocusOnPress', '$focusOnPress', toBool);
defineField(TextEditor.prototype, 'persistentSelection', '$persistent', toBool);
defineField(TextEditor.prototype, 'validator', '$validator', (value) => value);
defineField(TextEditor.prototype, 'inputMask', '$mask', (value) => (value === undefined || value === null ? '' : String(value)));
const readOnlyProp = (name, get) => defineReadOnly(TextEditor.prototype, name, get);
readOnlyProp('selectionStart', function () {
	return this.$lo;
});
readOnlyProp('selectionEnd', function () {
	return this.$hi;
});
readOnlyProp('selectedText', function () {
	return this.getText(this.$anchor, this.$cursor);
});
readOnlyProp('length', function () {
	return this.$text.length;
});
readOnlyProp('displayText', function () {
	return this.$shown();
});
readOnlyProp('contentWidth', function () {
	return this.$laid().width;
});
readOnlyProp('contentHeight', function () {
	return this.$laid().height;
});
readOnlyProp('paintedWidth', function () {
	return this.$laid().width;
});
readOnlyProp('paintedHeight', function () {
	return this.$laid().height;
});
readOnlyProp('lineCount', function () {
	return this.$laid().lines.length;
});
readOnlyProp('cursorRectangle', function () {
	return this.positionToRectangle(this.$cursor);
});
readOnlyProp('cursorVisible', function () {
	return this.$active;
});
readOnlyProp('acceptableInput', function () {
	return true;
});
readOnlyProp('canUndo', function () {
	return this.$undo.length > 0 && (this.$multi || !this.$readOnly);
});
readOnlyProp('canRedo', function () {
	return this.$redo.length > 0 && (this.$multi || !this.$readOnly);
});
readOnlyProp('canPaste', function () {
	return !this.$readOnly && hooks.clipboardRead().text !== '';
});

export class Shortcut extends Item {
	constructor() {
		super();
		this.$typeName = 'Shortcut';
		this.$nonVisual = true;
		this.$declareSignal('activated', []);
	}
}

// ----- Timer -----

export class Timer extends Item {
	constructor(engine) {
		super();
		this.$typeName = 'Timer';
		this.$nonVisual = true;
		this.$engine = engine;
		this.$interval = 1000;
		this.$running = false;
		this.$repeat = false;
		this.$deadline = 0;
		this.$declareSignal('triggered', []);
	}

	start() {
		this.$running = true;
		this.$deadline = this.$engine.time + this.$interval / 1000;
		this.$update();
	}

	stop() {
		this.$running = false;
		this.$update();
	}

	restart() {
		this.start();
	}
}

defineProp(
	Timer.prototype,
	'interval',
	function () {
		return this.$interval;
	},
	function (v) {
		const interval = Number(v);
		if (!Number.isFinite(interval) || interval < 0 || interval > 2147483647) {
			throw new QmlError('Timer.interval must be a non-negative integer');
		}
		this.$interval = Math.trunc(interval);
		if (this.$running) this.$deadline = this.$engine.time + this.$interval / 1000;
	},
);
defineProp(
	Timer.prototype,
	'running',
	function () {
		return this.$running;
	},
	function (v) {
		this.$running = toBool(v);
		this.$deadline = this.$engine.time + this.$interval / 1000;
	},
);
defineProp(
	Timer.prototype,
	'repeat',
	function () {
		return this.$repeat;
	},
	function (v) {
		this.$repeat = toBool(v);
	},
);

// ----- Gradient -----

// `Gradient { GradientStop { position: ..; color: .. } ... }`: used by Rectangle.gradient.
export class Gradient extends Item {
	constructor() {
		super();
		this.$typeName = 'Gradient';
		this.$nonVisual = true;
		this.$orientation = 'Vertical';
	}

	// The stops sorted by position, with the position clamped to 0..1.
	get $stops() {
		const stops = [];
		for (const child of this.$children) {
			if (child instanceof GradientStop) stops.push({ p: Math.min(Math.max(child.$position, 0), 1), color: child.$color });
		}
		return stops.sort((a, b) => a.p - b.p);
	}
}

defineField(Gradient.prototype, 'orientation', '$orientation', (v) => (v === 'Horizontal' ? 'Horizontal' : 'Vertical'));

export class GradientStop extends Item {
	constructor() {
		super();
		this.$typeName = 'GradientStop';
		this.$nonVisual = true;
		this.$position = 0;
		this.$color = WHITE;
	}
}

defineField(GradientStop.prototype, 'position', '$position', toFloat);
defineProp(
	GradientStop.prototype,
	'color',
	function () {
		return colorToString(this.$color);
	},
	function (v) {
		this.$color = toColor(v);
		this.$update();
	},
);

// ----- Animations -----

export const ANIMATION_TYPES = new Set([
	'PauseAnimation', 'NumberAnimation', 'PropertyAnimation', 'ColorAnimation', 'RotationAnimation', 'SmoothedAnimation',
	'SpringAnimation', 'Vector3dAnimation', 'SequentialAnimation', 'ParallelAnimation',
]);

// All animation types. A leaf animation moves `property` of `target` linearly from `from` to `to` in
// `duration` ms (an easing curve is not applied); a Sequential/ParallelAnimation runs its child animations;
// a PauseAnimation only waits. The engine ticks the running animations with its clock.
export class Animation extends Item {
	constructor(engine, kind) {
		super();
		this.$typeName = kind;
		this.$nonVisual = true;
		this.$engine = engine;
		this.$kind = kind;
		this.$duration = 250;
		this.$loops = 1;
		this.$running = false;
		this.$paused = false;
		this.$target = null;
		this.$property = kind === 'RotationAnimation' ? 'rotation' : '';
		this.$properties = '';
		this.$from = undefined;
		this.$to = undefined;
		this.$began = 0;
		this.$pausedAt = 0;
		this.$captured = null;
		this.$declareSignal('started', []);
		this.$declareSignal('stopped', []);
		this.$declareSignal('finished', []);
	}

	get $isGroup() {
		return this.$kind === 'SequentialAnimation' || this.$kind === 'ParallelAnimation';
	}

	$animations() {
		return this.$children.filter((child) => child instanceof Animation);
	}

	$totalDuration() {
		if (!this.$isGroup) return Math.max(0, this.$duration);
		const durations = this.$animations().map((a) => a.$totalDuration() * (a.$loops > 0 ? a.$loops : 1));
		if (this.$kind === 'SequentialAnimation') return durations.reduce((sum, d) => sum + d, 0);
		return durations.reduce((max, d) => Math.max(max, d), 0);
	}

	$resolveTarget() {
		if (this.$target !== null && typeof this.$target === 'object') return this.$target;
		let p = this.$parent;
		while (p instanceof Animation) {
			if (p.$target !== null && typeof p.$target === 'object') return p.$target;
			p = p.$parent;
		}
		return p;
	}

	$propertyNames() {
		const names = [];
		for (const text of [this.$property, this.$properties]) {
			for (const name of String(text || '').split(',')) {
				if (name.trim() !== '') names.push(name.trim());
			}
		}
		return names;
	}

	$reset() {
		this.$captured = null;
		for (const child of this.$animations()) child.$reset();
	}

	// True while this animation changes something that can be seen: a PauseAnimation never does,
	// and the target of the others must not be hidden (by itself or by an ancestor).
	$isVisuallyActive() {
		if (this.$kind === 'PauseAnimation') return false;
		if (this.$isGroup) return this.$animations().some((child) => child.$isVisuallyActive());
		const target = this.$resolveTarget();
		return target !== null && target !== undefined && !(target instanceof Item && target.visible === false);
	}

	// Applies the state at `t` ms after the start. An intermediate state of a hidden target is skipped:
	// writing it would only dirty the scene for a frame nobody can see. The final state is always written.
	$apply(t, intermediate = false) {
		if (this.$kind === 'PauseAnimation') return;
		if (this.$isGroup) {
			const children = this.$animations();
			if (this.$kind === 'ParallelAnimation') {
				for (const child of children) child.$apply(Math.min(t, child.$totalDuration()), intermediate);
				return;
			}
			let rest = t;
			for (const child of children) {
				const d = child.$totalDuration();
				child.$apply(Math.min(rest, d), intermediate);
				if (rest <= d) break;
				rest -= d;
			}
			return;
		}
		const target = this.$resolveTarget();
		if (target === null || target === undefined || this.$to === undefined) return;
		if (intermediate && target instanceof Item && target.visible === false) return;
		const progress = this.$duration > 0 ? Math.min(Math.max(t / this.$duration, 0), 1) : 1;
		if (this.$captured === null) this.$captured = Object.create(null);
		// Animation writes are not script assignments: they must not drop a binding on the
		// animated property (width/opacity/… are in BREAKS_BINDING for real script writes).
		hooks.engineWrite++;
		try {
			for (const name of this.$propertyNames()) {
				if (!(name in this.$captured)) this.$captured[name] = target[name];
				const from = this.$from !== undefined ? this.$from : this.$captured[name];
				const to = this.$to;
				const value = typeof from === 'number' && typeof to === 'number' ? from + (to - from) * progress : progress >= 1 ? to : from;
				if (target[name] !== value) target[name] = value;
			}
		} finally {
			hooks.engineWrite--;
		}
	}

	$tick(now) {
		if (!this.$running || this.$paused) return;
		const total = this.$totalDuration();
		const elapsed = Math.max(0, (now - this.$began) * 1000);
		const loops = this.$loops === 0 ? 1 : this.$loops;
		if (total <= 0 || (loops > 0 && elapsed >= total * loops)) {
			this.$apply(total);
			this.$finish();
			return;
		}
		this.$apply(elapsed % total, true);
	}

	$finish() {
		this.$running = false;
		this.$engine.animations.delete(this);
		this.$emit('stopped');
		this.$emit('finished');
		this.$update();
	}

	start() {
		if (this.$running) return;
		this.$running = true;
		this.$paused = false;
		this.$began = this.$engine.time;
		this.$reset();
		this.$engine.animations.add(this);
		this.$emit('started');
		// A PauseAnimation has no visual effect until `finished`; dirtying the scene on every
		// CustomTooltip.show() restart (each mouse move) would flicker the whole gallery.
		if (this.$kind !== 'PauseAnimation') this.$update();
	}

	stop() {
		if (!this.$running) return;
		this.$running = false;
		this.$engine.animations.delete(this);
		this.$emit('stopped');
		if (this.$kind !== 'PauseAnimation') this.$update();
	}

	restart() {
		if (this.$kind === 'PauseAnimation' && this.$running && !this.$paused) {
			this.$began = this.$engine.time;
			this.$reset();
			return;
		}
		this.stop();
		this.start();
	}

	pause() {
		if (this.$running && !this.$paused) {
			this.$paused = true;
			this.$pausedAt = this.$engine.time;
		}
	}

	resume() {
		if (this.$running && this.$paused) {
			this.$began += this.$engine.time - this.$pausedAt;
			this.$paused = false;
		}
	}

	complete() {
		if (!this.$running) return;
		this.$apply(this.$totalDuration());
		this.$finish();
	}
}

defineField(Animation.prototype, 'duration', '$duration', toFloat);
defineField(Animation.prototype, 'loops', '$loops', toFloat);
defineField(Animation.prototype, 'property', '$property', (v) => (v === undefined || v === null ? '' : String(v)));
defineField(Animation.prototype, 'properties', '$properties', (v) => (v === undefined || v === null ? '' : String(v)));
defineField(Animation.prototype, 'from', '$from', (v) => v);
defineField(Animation.prototype, 'to', '$to', (v) => v);
defineField(Animation.prototype, 'target', '$target', (v) => (v === undefined ? null : v));
defineProp(
	Animation.prototype,
	'running',
	function () {
		return this.$running;
	},
	function (v) {
		if (toBool(v)) this.start();
		else this.stop();
	},
);
defineProp(
	Animation.prototype,
	'paused',
	function () {
		return this.$paused;
	},
	function (v) {
		if (toBool(v)) this.pause();
		else this.resume();
	},
);

// ----- Models -----

const isRecord = (value) => value !== null && typeof value === 'object' && !Array.isArray(value) && !(value instanceof Date) && !(value instanceof Item);

// The number of rows of a model: a number, an array, a ListModel or a TreeItemModel.
export function modelCount(model) {
	if (typeof model === 'number') return Number.isFinite(model) && model >= 1 ? Math.trunc(model) : 0;
	if (model instanceof ListModel || model instanceof TreeItemModel) return model.$rows.length;
	if (Array.isArray(model)) return model.length;
	return 0;
}

// Changes when rows are added, removed or replaced (an array is recognized by its length only).
export function modelVersion(model) {
	if (model instanceof ListModel || model instanceof TreeItemModel) return model.$version;
	if (Array.isArray(model)) return model.length;
	return 0;
}

// The variables a delegate sees for row `index`: `index`, `modelData`, `model` and one variable per role.
export function modelScopeData(model, index) {
	const raw = typeof model === 'number' ? index : model instanceof ListModel || model instanceof TreeItemModel ? model.$rows[index] : Array.isArray(model) ? model[index] : undefined;
	const roles = isRecord(raw) ? raw : {};
	const modelData = raw;
	const proxy = new Proxy(roles, {
		get: (target, key) => (key === 'index' ? index : key === 'modelData' ? modelData : target[key]),
		set: (target, key, value) => {
			target[key] = value;
			markReaders(model, '$rows');
			hooks.touch();
			return true;
		},
		has: (target, key) => key === 'index' || key === 'modelData' || key in target,
	});
	return { index, modelData, model: proxy, roles, source: model };
}

// A binding that reads a role of a delegate follows the rows of the model; the model announces every change of them.
// The rows of an array change without a word, and the wrapper `getData` makes of a nested value is made anew each time.
export function readRows(model) {
	if (hooks.reading === null || typeof model === 'number' || model === null || model === undefined) return;
	if (model instanceof ListModel || (model instanceof TreeItemModel && model.$wrapped !== true)) noteRead(model, '$rows', true);
	else readUntracked('rows');
}

// `ListModel { ListElement { role: value } ... }` with the usual script interface.
export class ListModel extends Item {
	constructor() {
		super();
		this.$typeName = 'ListModel';
		this.$nonVisual = true;
		this.$rows = [];
		this.$version = 0;
		this.$notedCount = 0;
		// How many Repeaters currently use this model. syncModelNow is a no-op at 0 so Table
		// heightModel.append (thousands of times, no Repeater) stays cheap.
		this.$syncWatchers = 0;
	}

	// Takes the ListElement children as rows.
	$collect() {
		for (const child of this.$children) {
			if (child.$row !== undefined) this.$rows.push(child.$row);
		}
		this.$children = [];
		this.$touch();
	}

	$touch() {
		this.$version++;
		hooks.touch();
		markReaders(this, '$rows');
		markReaders(this, 'count');
		notifyViews(this);
		if (hooks.syncModel !== null) hooks.syncModel(this);
		this.$emitCount();
	}

	// `onCountChanged` runs before the next line of the script that changed the model.
	// Rows taken from ListElement children are the initial count: the handler's baseline moves with them.
	$emitCount() {
		const count = this.$rows.length;
		if (this.$notedCount === count) return;
		this.$notedCount = count;
		if (this.$constructing) {
			const list = this.$chg === undefined ? undefined : this.$chg.get('count');
			if (list !== undefined) {
				for (const handler of list) {
					handler.last = count;
					handler.hasLast = true;
				}
			}
			return;
		}
		if (hooks.changed === null) return;
		if (hooks.engineWrite > 0 && (hooks.completing === null || !hooks.completing())) return;
		hooks.changed(this, 'count');
	}

	append(value) {
		for (const row of Array.isArray(value) ? value : [value]) {
			if (isRecord(row)) this.$rows.push({ ...row });
		}
		this.$touch();
	}

	insert(index, value) {
		if (!isRecord(value)) return;
		this.$rows.splice(Math.min(Math.max(Math.trunc(index), 0), this.$rows.length), 0, { ...value });
		this.$touch();
	}

	remove(index, count = 1) {
		if (index < 0 || index >= this.$rows.length) return;
		this.$rows.splice(Math.trunc(index), Math.max(1, Math.trunc(count)));
		this.$touch();
	}

	clear() {
		if (this.$rows.length === 0) return;
		this.$rows = [];
		this.$touch();
	}

	get(index) {
		return this.$rows[index];
	}

	set(index, value) {
		const row = this.$rows[index];
		if (row === undefined || !isRecord(value)) return;
		let grew = false;
		for (const key of Object.keys(value)) {
			if (!(key in row)) grew = true;
			row[key] = value[key];
		}
		if (grew) {
			this.$version++;
			notifyViews(this);
		}
		markReaders(this, '$rows');
		hooks.touch();
	}

	setProperty(index, key, value) {
		const row = this.$rows[index];
		if (row === undefined) return;
		if (!(key in row)) {
			this.$version++;
			notifyViews(this);
		}
		row[key] = value;
		markReaders(this, '$rows');
		hooks.touch();
	}

	move(from, to, count = 1) {
		if (from < 0 || to < 0 || from + count > this.$rows.length || to + count > this.$rows.length) return;
		const moved = this.$rows.splice(from, count);
		this.$rows.splice(to, 0, ...moved);
		this.$touch();
	}

	sync() {}
}

defineReadOnly(
	ListModel.prototype,
	'count',
	function () {
		return this.$rows.length;
	},
	undefined,
	true,
);

// `Repeater` for any model. The engine creates the delegates (as siblings, right after the repeater) and
// rebuilds them when the model changes.
export class Repeater extends Item {
	constructor() {
		super();
		this.$typeName = 'Repeater';
		this.$nonVisual = true;
		this.$model = 0;
		this.$delegate = null; // { def, scope, depth }, set by the engine
		this.$built = [];
		this.$lastModel = undefined;
		this.$lastVersion = -1;
		this.$lastCount = -1;
	}

	itemAt(index) {
		return this.$built[index] || null;
	}
}

defineProp(
	Repeater.prototype,
	'delegate',
	function () {
		return this.$component === undefined ? null : this.$component;
	},
	function (value) {
		if (this.$component === value) return;
		this.$component = value;
		// An inline `delegate: Item {}` is also stored by the engine as a definition. A Component
		// assigned later replaces that definition, and the new items must exist before the next line.
		if (value instanceof ComponentItem) this.$delegate = null;
		this.$update();
		if (!this.$constructing && value instanceof ComponentItem && hooks.syncRepeater !== null) hooks.syncRepeater(this);
	},
);
function noteListModelWatcher(model, delta) {
	if (model instanceof ListModel) model.$syncWatchers += delta;
}

defineProp(
	Repeater.prototype,
	'model',
	function () {
		return this.$model;
	},
	function (v) {
		if (v !== this.$model) {
			noteListModelWatcher(this.$model, -1);
			this.$model = v;
			noteListModelWatcher(v, 1);
			this.$update();
		}
	},
	true,
);
defineReadOnly(Repeater.prototype, 'count', function () {
	return this.$built.length;
});

// A type the engine does not implement (Connections, an unknown component ...): an
// item that only keeps its properties and children, draws nothing and ignores unknown handlers.
export class Placeholder extends Item {
	constructor(typeName) {
		super();
		this.$typeName = typeName;
	}
}

// A non-visual object (`QtObject`): properties, signals, functions and child objects, nothing is drawn.
export class QtObject extends Item {
	constructor() {
		super();
		this.$typeName = 'QtObject';
		this.$nonVisual = true;
	}

	// The properties declared on the object (`property` and `property alias`) as name -> true, like the keys of `$properties` in JQML.
	get $properties() {
		const result = Object.create(null);
		if (this.$custom !== null) {
			for (const name of Object.keys(this.$custom)) result[name] = true;
		}
		if (this.$aliases !== undefined) {
			for (const name of this.$aliases) result[name] = true;
		}
		return result;
	}

	getProperties() {
		return Object.keys(this.$properties);
	}
}

// The data model of ImtCore (`TreeItemModel`): a list of rows, each row is a map key -> value, where a value
// is a plain value or a nested model. Only the commonly used part of its interface is implemented.
/* probe-xhr */ if (typeof XMLHttpRequest !== 'undefined') { const open = XMLHttpRequest.prototype.open; XMLHttpRequest.prototype.open = function () { const r = open.apply(this, arguments); if (XMLHttpRequest.QMLAuthToken) this.setRequestHeader('X-authentication-token', XMLHttpRequest.QMLAuthToken); return r; }; }
export class TreeItemModel extends Item {
	refresh() {}
	constructor() {
		super();
		this.$typeName = 'TreeItemModel';
		this.$nonVisual = true;
		this.$rows = [];
		this.$version = 0;
		this.$declareSignal('modelChanged', ['topLeft', 'bottomRight', 'roles']);
	}

	$touch() {
		this.$version++;
		hooks.touch();
		markReaders(this, '$rows');
		markReaders(this, 'count');
		notifyViews(this);
		if (this.$sigs.get('modelChanged').handlers.length > 0) this.$emit('modelChanged', []);
	}

	static $wrap(value) {
		if (value instanceof TreeItemModel) return value;
		const model = new TreeItemModel();
		model.$rows = Array.isArray(value) ? value : [value];
		model.$wrapped = true; // made anew on every read: nothing can follow it
		return model;
	}

	$ensureRow(index) {
		while (this.$rows.length <= index) this.$rows.push({});
		return this.$rows[index];
	}

	clear() {
		this.$rows = [];
		this.$touch();
	}

	createFromJson(text) {
		try {
			const value = typeof text === 'string' ? JSON.parse(text) : text;
			if (value === null || typeof value !== 'object') return false;
			this.$rows = Array.isArray(value) ? value.filter((row) => row !== null && typeof row === 'object' && !Array.isArray(row)) : [value];
			this.$touch();
			return true;
		} catch {
			return false;
		}
	}

	toJson() {
		return JSON.stringify(this.$rows.length === 1 ? this.$rows[0] : this.$rows, (key, value) => (value instanceof TreeItemModel ? (value.$rows.length === 1 ? value.$rows[0] : value.$rows) : value));
	}

	containsKey(key, index = 0) {
		const row = this.$rows[index];
		return row !== undefined && Object.prototype.hasOwnProperty.call(row, key);
	}

	getData(key, index = 0) {
		const row = this.$rows[index];
		if (row === undefined || !Object.prototype.hasOwnProperty.call(row, key)) return '';
		const value = row[key];
		return value !== null && typeof value === 'object' && !(value instanceof Date) ? TreeItemModel.$wrap(value) : value;
	}

	getTreeItemModel(key, index = 0) {
		const value = this.getData(key, index);
		return value instanceof TreeItemModel ? value : null;
	}

	setData(key, value, index = 0) {
		this.$ensureRow(index)[key] = value;
		this.$touch();
		return true;
	}

	addTreeModel(key, index = 0) {
		const model = new TreeItemModel();
		this.$ensureRow(index)[key] = model;
		this.$touch();
		return model;
	}

	insertNewItem(index) {
		const at = index === undefined || index > this.$rows.length ? this.$rows.length : index;
		this.$rows.splice(at, 0, {});
		this.$touch();
		return at;
	}

	removeItem(index) {
		if (index < 0 || index >= this.$rows.length) return false;
		this.$rows.splice(index, 1);
		this.$touch();
		return true;
	}

	getItemsCount() {
		return this.$rows.length;
	}

	get(index) {
		return this.$rows[index] === undefined ? {} : this.$rows[index];
	}

	getKeys(index = 0) {
		return this.$rows[index] === undefined ? [] : Object.keys(this.$rows[index]);
	}

	isValidData(key, index = 0) {
		return this.containsKey(key, index) && this.$rows[index][key] !== undefined && this.$rows[index][key] !== null;
	}

	copyMe() {
		const model = new TreeItemModel();
		model.copy(this);
		return model;
	}

	copy(other) {
		if (other instanceof TreeItemModel) this.createFromJson(other.toJson());
	}

	// Replaces (or creates) the row `index` with a deep copy of the row `externalIndex` of `other`.
	copyItemDataFromModel(index, other, externalIndex = 0) {
		if (!(other instanceof TreeItemModel) || other.$rows[externalIndex] === undefined) return false;
		const source = new TreeItemModel();
		source.$rows = [other.$rows[externalIndex]];
		const clone = source.copyMe();
		this.$ensureRow(index);
		this.$rows[index] = clone.$rows[0] === undefined ? {} : clone.$rows[0];
		this.$touch();
		return true;
	}
}

defineReadOnly(
	TreeItemModel.prototype,
	'count',
	function () {
		return this.$rows.length;
	},
	undefined,
	true,
);

// `Component { ... }`: the definition is kept, nothing is instantiated. Also the result of
// `Qt.createComponent(url)`, whose definition is the root of another document and whose status may
// still be Loading (Null 0, Ready 1, Loading 2, Error 3, as Qt prints them).
export class ComponentItem extends Item {
	constructor() {
		super();
		this.$typeName = 'Component';
		this.$nonVisual = true;
		this.$def = null;
		this.$defScope = null;
		this.$url = '';
		this.$status = 1;
		this.$errorString = '';
		this.$declareSignal('statusChanged', []);
	}

	$setStatus(status, errorString = '') {
		if (this.$status === status && this.$errorString === errorString) return;
		this.$status = status;
		this.$errorString = errorString;
		this.$emit('statusChanged');
	}

	errorString() {
		return this.$errorString;
	}

	createObject(parent, props) {
		if (this.$status !== 1) {
			const reason = this.$status === 3 ? this.$errorString : this.$status === 2 ? 'the component is still loading, wait for statusChanged' : 'the component has no definition';
			hooks.report(new QmlError(reason), `Component.createObject('${this.$url}')`);
			return null;
		}
		return hooks.instantiate(this, parent, props);
	}
}

defineReadOnly(ComponentItem.prototype, 'status', function () {
	return this.$status;
});
defineReadOnly(ComponentItem.prototype, 'url', function () {
	return this.$url;
});
defineReadOnly(ComponentItem.prototype, 'progress', function () {
	return this.$status === 1 || this.$status === 3 ? 1 : 0;
});

// `Column`, `Row` and `Flow` (and the Layout variants): children are placed one after another.
export class Stack extends Item {
	constructor(kind) {
		super();
		this.$typeName = kind;
		this.$vertical = kind === 'Column' || kind === 'ColumnLayout';
		this.$flow = kind === 'Flow';
		this.$spacing = 0;
		this.$padding = 0;
	}

	static $size(child, vertical) {
		if (vertical) return child.$h > 0 ? child.$h : child.$ah > 0 ? child.$ah : child.$effH;
		return child.$w > 0 ? child.$w : child.$aw > 0 ? child.$aw : child.$effW;
	}

	$content() {
		let main = 0;
		let cross = 0;
		let count = 0;
		for (const child of this.$children) {
			if (!child.$visible || child.$nonVisual) continue;
			const along = Stack.$size(child, this.$vertical);
			const across = Stack.$size(child, !this.$vertical);
			// A child with a zero side is not positioned and adds neither size nor spacing.
			if (along <= 0 || across <= 0) continue;
			main += along;
			cross = Math.max(cross, across);
			count++;
		}
		if (count > 1) main += this.$spacing * (count - 1);
		const pad = this.$padding > 0 ? this.$padding * 2 : 0;
		return this.$vertical ? [cross + pad, main + pad] : [main + pad, cross + pad];
	}

	get $implicitWidth() {
		// ColumnLayout and RowLayout publish their size from $arrange. A non-zero implicit width
		// here would be written by $resolve before the change signal can observe the 0 -> size step.
		if (this.$typeName === 'ColumnLayout' || this.$typeName === 'RowLayout') return 0;
		return this.$content()[0];
	}

	get $implicitHeight() {
		if (this.$typeName === 'ColumnLayout' || this.$typeName === 'RowLayout') return 0;
		return this.$content()[1];
	}

	$arrange() {
		if (this.$typeName === 'ColumnLayout' || this.$typeName === 'RowLayout') {
			arrangeLinear(this, this.$vertical);
			return;
		}
		const vertical = this.$vertical;
		const pad = this.$padding > 0 ? this.$padding : 0;
		let pos = pad;
		let lineStart = pad;
		let lineSize = 0;
		const limit = (vertical ? this.$ah : this.$aw) - pad;
		for (const child of this.$children) {
			if (!child.$visible || child.$nonVisual) continue;
			const mainSize = Stack.$size(child, vertical);
			const crossSize = Stack.$size(child, !vertical);
			if (mainSize <= 0 || crossSize <= 0) continue;
			if (this.$flow && pos > pad && pos + mainSize > limit) {
				lineStart += lineSize + this.$spacing;
				pos = pad;
				lineSize = 0;
			}
			const an = child.$anchors;
			// Positioners own the main axis. On the cross axis, child anchors (e.g. a Row with
			// `horizontalCenter` inside a Column) must win — otherwise Indicators stays left-aligned.
			if (vertical) {
				child.$ay = pos;
				child.$y = pos;
				if (this.$flow || an === null || !an.$affectsX) {
					child.$ax = this.$flow ? lineStart : pad;
					child.$x = child.$ax;
				} else {
					child.$state = 0;
					child.$resolve();
					child.$ay = pos;
					child.$y = pos;
					child.$x = child.$ax;
				}
			} else {
				child.$ax = pos;
				child.$x = pos;
				if (this.$flow || an === null || !an.$affectsY) {
					child.$ay = this.$flow ? lineStart : pad;
					child.$y = child.$ay;
				} else {
					child.$state = 0;
					child.$resolve();
					child.$ax = pos;
					child.$x = pos;
					child.$y = child.$ay;
				}
			}
			pos += mainSize + this.$spacing;
			lineSize = Math.max(lineSize, crossSize);
		}
	}

	// Lay the flow out before the caller reads positions. A script that just called this must see the new size.
	forceLayout() {
		let root = this;
		while (root.$parent !== null) root = root.$parent;
		root.$updateRequested = true;
		root.$layout();
	}
}

defineField(Stack.prototype, 'spacing', '$spacing', toFloat);
defineField(Stack.prototype, 'padding', '$padding', toFloat);
Stack.prototype.$contentSized = true;

// `Loader`: the engine creates the loaded item as the only child.
// Status matches the Qt enum: Null 0, Ready 1, Loading 2, Error 3.
export class Loader extends Item {
	constructor() {
		super();
		this.$typeName = 'Loader';
		this.$source = '';
		this.$sourceComponent = null;
		this.$active = true;
		this.$fromComponent = false;
		this.$initialProps = null;
		this.$keepInitial = false;
		this.$wanted = '';
		this.$inLoad = false;
		this.$sizing = false;
		this.$failed = false;
		this.$statusSent = 0;
		this.$declareSignal('loaded', []);
		this.$declareSignal('itemChanged', []);
		this.$declareSignal('statusChanged', []);
	}

	// An explicit width (or anchors that set it) wins; otherwise the loaded item's width.
	$widthExplicit() {
		return this.$w > 0 || (this.$anchors !== null && this.$anchors.$affectsWidth);
	}

	$heightExplicit() {
		return this.$h > 0 || (this.$anchors !== null && this.$anchors.$affectsHeight);
	}

	get $implicitWidth() {
		const item = this.$children.length > 0 ? this.$children[0] : null;
		if (item === null) return 0;
		return this.$widthExplicit() ? item.$implicitWidth : item.width;
	}

	get $implicitHeight() {
		const item = this.$children.length > 0 ? this.$children[0] : null;
		if (item === null) return 0;
		return this.$heightExplicit() ? item.$implicitHeight : item.height;
	}

	// The axes the Loader sizes itself are copied onto the item. The other axes follow the item.
	// Only when the Loader's size changed or the item is new: a script may resize the item afterwards (Qt).
	$pushSize() {
		const item = this.$children.length > 0 ? this.$children[0] : null;
		if (item === null) {
			this.$pushedItem = null;
			return;
		}
		const fresh = this.$pushedItem !== item;
		this.$pushedItem = item;
		this.$sizing = true;
		try {
			if (this.$widthExplicit()) {
				const width = this.width;
				if (fresh || width !== this.$pushedW) item.width = width;
				this.$pushedW = width;
			}
			if (this.$heightExplicit()) {
				const height = this.height;
				if (fresh || height !== this.$pushedH) item.height = height;
				this.$pushedH = height;
			}
		} finally {
			this.$sizing = false;
		}
	}

	// Anchors can give the Loader a size after the item was loaded at 0×0; push again each layout.
	$arrange() {
		this.$pushSize();
	}

	// The item changed size on an axis the Loader does not control.
	$adoptItemSize() {
		if (this.$sizing === true || hooks.changed === null) return;
		if (!this.$widthExplicit()) hooks.changed(this, 'width');
		if (!this.$heightExplicit()) hooks.changed(this, 'height');
		this.$update();
	}

	// `setSource(url)` or `setSource(url, { prop: value })`. While `active` is false the values wait.
	setSource(url, properties) {
		if (arguments.length > 1) {
			if (properties === null || typeof properties !== 'object') return;
			this.$initialProps = properties;
		} else {
			this.$initialProps = null;
		}
		this.$keepInitial = true;
		try {
			this.source = url === undefined || url === null ? '' : String(url);
		} finally {
			this.$keepInitial = false;
		}
	}
}

defineProp(
	Loader.prototype,
	'source',
	function () {
		return this.$source;
	},
	function (value) {
		if (!this.$keepInitial) this.$initialProps = null;
		const next = value === undefined || value === null ? '' : String(value);
		const same = this.$source === next && this.$fromComponent === false;
		this.$source = next;
		this.$fromComponent = false;
		if (!same) this.$update();
		if (!this.$constructing && hooks.reloadLoader !== null) hooks.reloadLoader(this);
	},
);
defineProp(
	Loader.prototype,
	'sourceComponent',
	function () {
		return this.$sourceComponent;
	},
	function (value) {
		if (!this.$keepInitial) this.$initialProps = null;
		const next = value || null;
		const same = this.$sourceComponent === next && this.$fromComponent === true;
		this.$sourceComponent = next;
		this.$fromComponent = next !== null;
		if (!same) this.$update();
		if (!this.$constructing && hooks.reloadLoader !== null) hooks.reloadLoader(this);
	},
);
defineProp(
	Loader.prototype,
	'active',
	function () {
		return this.$active;
	},
	function (value) {
		const next = toBool(value);
		if (this.$active === next) return;
		this.$active = next;
		this.$update();
		if (!this.$constructing && hooks.reloadLoader !== null) hooks.reloadLoader(this);
	},
);
defineProp(
	Loader.prototype,
	'width',
	function () {
		if (this.$anchors !== null && this.$anchors.$affectsWidth) return this.$aw;
		if (this.$w > 0) return this.$w;
		const item = this.$children.length > 0 ? this.$children[0] : null;
		return item !== null ? item.width : 0;
	},
	function (v) {
		v = toFloat(v);
		if (this.$w !== v) {
			this.$w = v;
			this.$update();
		}
		if (!this.$constructing && this.$sizing !== true) this.$pushSize();
	},
);
defineProp(
	Loader.prototype,
	'height',
	function () {
		if (this.$anchors !== null && this.$anchors.$affectsHeight) return this.$ah;
		if (this.$h > 0) return this.$h;
		const item = this.$children.length > 0 ? this.$children[0] : null;
		return item !== null ? item.height : 0;
	},
	function (v) {
		v = toFloat(v);
		if (this.$h !== v) {
			this.$h = v;
			this.$update();
		}
		if (!this.$constructing && this.$sizing !== true) this.$pushSize();
	},
);
defineReadOnly(Loader.prototype, 'item', function () {
	return this.$children.length > 0 ? this.$children[0] : null;
});
defineReadOnly(Loader.prototype, 'status', function () {
	if (!this.$active) return 0;
	if (this.$failed) return 3;
	return this.$children.length > 0 ? 1 : 0;
});
Loader.prototype.$contentSized = true;

// A list of the C++-generated SDL types (`m_items.addElement(x)`, `m_items.get(i).item`).
export class SdlList extends Item {
	constructor() {
		super();
		this.$typeName = 'SdlList';
		this.$nonVisual = true;
		this.$items = [];
	}

	get count() {
		readUntracked();
		return this.$items.length;
	}

	get length() {
		readUntracked();
		return this.$items.length;
	}

	addElement(item) {
		this.$items.push(item);
		return item;
	}

	append(item) {
		return this.addElement(item);
	}

	get(index) {
		return { item: this.$items[index] };
	}

	remove(index, count = 1) {
		this.$items.splice(index, count);
	}

	clear() {
		this.$items = [];
	}
}

const SDL_ACCESSOR_RE = /^(has|emplace|remove|clear)([A-Z]\w*)$/;
const SDL_FIELD_RE = /^m_[A-Za-z]/;

function sdlField(name) {
	return `m_${name[0].toLowerCase()}${name.slice(1)}`;
}

function sdlSet(target, field, value) {
	if (Object.getOwnPropertyDescriptor(target, field) === undefined) target.$defineCustom(field, value);
	else target[field] = value;
}

function sdlCreate(name) {
	if (/(Filters|SortingInfo|Elements|Options|Items|Params)$/.test(name)) return new SdlList();
	if (/(Ids|Fields)$/.test(name)) return [];
	return createSdlObject(name);
}

const SDL_HANDLER = {
	get(target, name, receiver) {
		if (typeof name !== 'string' || name in target) return Reflect.get(target, name, receiver);
		// `GuiElementContainer.m_elements`: an empty list until something is added.
		if (name === 'm_elements') {
			sdlSet(target, name, new SdlList());
			return target[name];
		}
		const match = SDL_ACCESSOR_RE.exec(name);
		if (match === null) return undefined;
		const field = sdlField(match[2]);
		switch (match[1]) {
			case 'has':
				return () => target[field] !== undefined && target[field] !== null;
			case 'emplace':
				return () => sdlSet(target, field, sdlCreate(match[2]));
			default:
				return () => {
					if (target[field] !== undefined) sdlSet(target, field, null);
				};
		}
	},
	// `m_field = value` inside the object's own functions must not leak into the global scope.
	has(target, name) {
		return name in target || (typeof name === 'string' && (SDL_FIELD_RE.test(name) || SDL_ACCESSOR_RE.test(name)));
	},
	// A script assignment such as `input.text = input.text + ch` has to go through the property setter, so onTextChanged runs.
	set(target, name, value, receiver) {
		if (typeof name === 'string' && target.$open === true && !name.startsWith('$') && !(name in target)) target.$defineCustom(name, undefined);
		return Reflect.set(target, name, value, receiver);
	},
};

// A stand-in for a C++-generated SDL type that is not available: `m_x` fields plus the generated
// `hasX()` / `emplaceX()` / `removeX()` / `clearX()` accessors, and `copyFrom()`.
export function createSdlObject(typeName) {
	const object = new Placeholder(typeName);
	object.$nonVisual = true;
	object.copyFrom = (other) => {
		if (other === null || typeof other !== 'object') return;
		for (const key of Object.keys(other)) {
			if (SDL_FIELD_RE.test(key)) sdlSet(object, key, other[key]);
		}
	};
	return new Proxy(object, SDL_HANDLER);
}

// ----- Settings -----

// `Qt.labs.settings.Settings` backed by the browser's localStorage (a Map outside of a browser).
const settingsFallback = new Map();

export class Settings extends Item {
	constructor() {
		super();
		this.$typeName = 'Settings';
		this.$nonVisual = true;
		this.$defineCustom('category', '');
		this.$defineCustom('fileName', '');
	}

	$key(key) {
		return `qml/${this.category}/${key}`;
	}

	value(key, defaultValue) {
		let raw;
		try {
			raw = typeof localStorage === 'undefined' ? settingsFallback.get(this.$key(key)) : localStorage.getItem(this.$key(key));
		} catch {
			raw = settingsFallback.get(this.$key(key));
		}
		if (raw === undefined || raw === null) return defaultValue;
		try {
			return JSON.parse(raw);
		} catch {
			return defaultValue;
		}
	}

	setValue(key, value) {
		const raw = JSON.stringify(value);
		try {
			if (typeof localStorage === 'undefined') settingsFallback.set(this.$key(key), raw);
			else localStorage.setItem(this.$key(key), raw);
		} catch {
			settingsFallback.set(this.$key(key), raw);
		}
	}

	remove(key) {
		settingsFallback.delete(this.$key(key));
		try {
			if (typeof localStorage !== 'undefined') localStorage.removeItem(this.$key(key));
		} catch {
			// storage is not available
		}
	}
}

// ----- FileDialog -----

export const FILE_DIALOG_MODES = ['OpenFile', 'OpenFiles', 'SaveFile'];

// `FileDialog` backed by a browser file chooser. `file` is the selected `File` (the JQML web convention: it
// answers `toString()` / `replace()` with itself); for saving it is the name typed by the user.
export class FileDialog extends Item {
	constructor() {
		super();
		this.$typeName = 'FileDialog';
		this.$nonVisual = true;
		this.$declareSignal('accepted', []);
		this.$declareSignal('rejected', []);
		for (const [name, initial] of [['title', ''], ['fileMode', 0], ['nameFilters', []], ['selectedNameFilter', null], ['file', null], ['files', []], ['currentFile', ''], ['selectedFile', null], ['selectedFiles', []], ['folder', ''], ['currentFolder', '']]) {
			this.$defineCustom(name, initial);
		}
	}

	open() {
		if (typeof document === 'undefined') return;
		if (this.fileMode === 2) {
			const name = typeof window !== 'undefined' && window.prompt ? window.prompt(String(this.title || 'Save file'), String(this.currentFile || '')) : null;
			if (name === null || name === '') {
				this.$emit('rejected');
				return;
			}
			this.$accept([name]);
			return;
		}
		const input = document.createElement('input');
		input.type = 'file';
		input.multiple = this.fileMode === 1;
		const extensions = [];
		for (const filter of Array.isArray(this.nameFilters) ? this.nameFilters : []) {
			for (const match of String(filter).matchAll(/\*\.(\w+)/g)) extensions.push(`.${match[1]}`);
		}
		if (extensions.length > 0) input.accept = extensions.join(',');
		input.addEventListener('change', () => {
			const files = [...input.files];
			if (files.length === 0) this.$emit('rejected');
			else this.$accept(files);
		});
		input.addEventListener('cancel', () => this.$emit('rejected'));
		input.click();
	}

	close() {}

	$accept(files) {
		const wrapped = files.map((file) => {
			if (typeof file === 'string') return file;
			file.toString = () => file;
			file.replace = () => file;
			return file;
		});
		this.file = wrapped[0];
		this.files = wrapped;
		this.selectedFile = wrapped[0];
		this.selectedFiles = wrapped;
		this.currentFile = wrapped[0];
		this.$emit('accepted');
	}
}

// `StackLayout`: one visible child, resized to the stack. `currentIndex` applies before the next read.
export class StackLayout extends Item {
	constructor() {
		super();
		this.$typeName = 'StackLayout';
		this.$index = 0;
	}

	$kids() {
		const kids = [];
		for (const child of this.$children) if (!child.$nonVisual) kids.push(child);
		return kids;
	}

	$apply() {
		const kids = this.$kids();
		if (kids.length === 0) return;
		let index = this.$index | 0;
		if (index < 0) index = 0;
		if (index >= kids.length) index = kids.length - 1;
		const w = explicitSpan(this, false) || this.$aw;
		const h = explicitSpan(this, true) || this.$ah;
		for (let i = 0; i < kids.length; i++) {
			const child = kids[i];
			const on = i === index;
			if (child.$visible !== on) {
				child.$visible = on;
				markReaders(child, 'visible');
			}
			if (on) commitGeometry(child, 0, 0, w, h);
		}
	}

	$arrange() {
		this.$apply();
	}
}

defineProp(
	StackLayout.prototype,
	'currentIndex',
	function () {
		return this.$index;
	},
	function (value) {
		const index = value | 0;
		if (this.$index !== index) {
			this.$index = index;
			this.$update();
		}
		if (!this.$constructing) this.$apply();
	},
);
defineReadOnly(StackLayout.prototype, 'count', function () {
	return this.$kids().length;
});

// `FlexboxLayout`: one row in the covered tests. fillWidth takes the free main-axis space;
// alignItems centers on the cross axis of the line (the container, when there is a single line).
export class FlexboxLayout extends Item {
	constructor() {
		super();
		this.$typeName = 'FlexboxLayout';
		this.$direction = 'Row';
		this.$wrap = 'NoWrap';
		this.$justify = 'JustifyFlexStart';
		this.$alignItems = 'AlignStretch';
		this.$columnGap = 0;
		this.$rowGap = 0;
	}

	$arrange() {
		const horizontal = this.$direction !== 'Column' && this.$direction !== 'ColumnReverse';
		const gap = horizontal ? this.$columnGap : this.$rowGap;
		const crossGap = horizontal ? this.$rowGap : this.$columnGap;
		const wrap = this.$wrap === 'Wrap' || this.$wrap === 'WrapReverse';
		const mainLimit = explicitSpan(this, !horizontal);
		const crossExplicit = explicitSpan(this, horizontal);
		const kids = [];
		for (const child of this.$children) if (!child.$nonVisual && child.$visible) kids.push(child);
		const lines = [];
		let line = [];
		let used = 0;
		for (const item of kids) {
			const main = preferred(item, !horizontal);
			const add = line.length === 0 ? main : gap + main;
			if (wrap && line.length > 0 && !wantsFill(item, !horizontal) && mainLimit > 0 && used + add > mainLimit) {
				lines.push(line);
				line = [];
				used = 0;
			}
			const next = line.length === 0 ? main : gap + main;
			line.push(item);
			used += next;
		}
		if (line.length > 0) lines.push(line);
		const lineCross = lines.map((row) => {
			let size = 0;
			for (const item of row) size = Math.max(size, preferred(item, horizontal));
			return size;
		});
		const contentCross = lineCross.reduce((sum, size) => sum + size, 0) + crossGap * Math.max(0, lines.length - 1);
		const crossSize = crossExplicit > 0 ? crossExplicit : contentCross;
		if (lines.length === 1) lineCross[0] = crossSize;
		let crossPos = 0;
		let mainContent = 0;
		const placed = [];
		for (let li = 0; li < lines.length; li++) {
			const row = lines[li];
			const prefs = row.map((item) => preferred(item, !horizontal));
			const prefSum = prefs.reduce((sum, size) => sum + size, 0);
			const gaps = gap * Math.max(0, row.length - 1);
			const limit = mainLimit > 0 ? mainLimit : prefSum + gaps;
			let extra = Math.max(0, limit - prefSum - gaps);
			const sizes = prefs.slice();
			const fillCount = row.reduce((sum, item) => sum + (wantsFill(item, !horizontal) ? 1 : 0), 0);
			if (fillCount > 0) {
				const share = extra / fillCount;
				for (let i = 0; i < row.length; i++) if (wantsFill(row[i], !horizontal)) sizes[i] = prefs[i] + share;
				extra = 0;
			}
			let lead = 0;
			let between = gap;
			if (extra > 0 && this.$justify === 'JustifySpaceAround' && row.length > 0) {
				const slot = extra / row.length;
				lead = slot / 2;
				between = gap + slot;
			} else if (extra > 0 && this.$justify === 'JustifySpaceBetween' && row.length > 1) {
				between = gap + extra / (row.length - 1);
			} else if (extra > 0 && (this.$justify === 'JustifyCenter' || this.$justify === 'AlignCenter')) {
				lead = extra / 2;
			} else if (extra > 0 && (this.$justify === 'JustifyFlexEnd' || this.$justify === 'JustifyEnd' || this.$justify === 'AlignRight')) {
				lead = extra;
			}
			let pos = lead;
			for (let i = 0; i < row.length; i++) {
				const item = row[i];
				const crossItem = preferred(item, horizontal);
				const cell = lineCross[li];
				let crossOff = 0;
				const align = this.$alignItems;
				if (align === 'AlignCenter' || align === 'AlignVCenter' || align === 'AlignHCenter') crossOff = (cell - crossItem) / 2;
				else if (align === 'AlignEnd' || align === 'AlignFlexEnd' || align === 'AlignBottom' || align === 'AlignRight') crossOff = cell - crossItem;
				else if (align === 'AlignStretch' && crossItem <= 0) crossOff = 0;
				const x = horizontal ? pos : crossPos + crossOff;
				const y = horizontal ? crossPos + crossOff : pos;
				const w = horizontal ? sizes[i] : align === 'AlignStretch' && crossItem <= 0 ? cell : crossItem;
				const h = horizontal ? (align === 'AlignStretch' && crossItem <= 0 ? cell : crossItem) : sizes[i];
				placed.push({ item, x, y, w, h });
				pos += sizes[i] + between;
			}
			mainContent = Math.max(mainContent, pos - between);
			crossPos += lineCross[li] + crossGap;
		}
		const mainSize = mainLimit > 0 ? mainLimit : mainContent;
		if (horizontal) commitOwnSize(this, mainSize, crossSize);
		else commitOwnSize(this, crossSize, mainSize);
		commitMany(placed);
	}
}

defineField(FlexboxLayout.prototype, 'direction', '$direction', (value) => (value === undefined || value === null ? 'Row' : String(value)));
defineField(FlexboxLayout.prototype, 'wrap', '$wrap', (value) => (value === undefined || value === null ? 'NoWrap' : String(value)));
defineField(FlexboxLayout.prototype, 'justifyContent', '$justify', (value) => (value === undefined || value === null ? 'JustifyFlexStart' : String(value)));
defineField(FlexboxLayout.prototype, 'alignItems', '$alignItems', (value) => (value === undefined || value === null ? 'AlignStretch' : String(value)));
defineField(FlexboxLayout.prototype, 'columnGap', '$columnGap', toFloat);
defineField(FlexboxLayout.prototype, 'rowGap', '$rowGap', toFloat);

// `LayoutItemProxy`: the layout sizes the proxy, and the proxy copies that onto `target`.
export class LayoutItemProxy extends Item {
	constructor() {
		super();
		this.$typeName = 'LayoutItemProxy';
		this.$target = null;
	}

	$syncTarget() {
		const target = this.$target;
		if (target === null) return;
		if (target.$visible !== this.$visible) {
			target.$visible = this.$visible;
			markReaders(target, 'visible');
		}
		const w = this.$w > 0 ? this.$w : this.$aw;
		const h = this.$h > 0 ? this.$h : this.$ah;
		commitGeometry(target, this.$x, this.$y, w, h);
	}
}

defineProp(
	LayoutItemProxy.prototype,
	'target',
	function () {
		return this.$target;
	},
	function (value) {
		this.$target = value instanceof Item ? value : null;
		this.$update();
		if (!this.$constructing && !layingOut) this.$syncTarget();
	},
);
defineProp(
	LayoutItemProxy.prototype,
	'visible',
	function () {
		for (let item = this; item !== null; item = item.$parent) {
			if (!item.$visible) return false;
		}
		return true;
	},
	function (value) {
		const shown = toBool(value);
		if (this.$visible !== shown) {
			this.$visible = shown;
			this.$update();
		}
		if (!this.$constructing) this.$syncTarget();
	},
);

// `GqlModel` of imtcontrols: a TreeItemModel that fills itself from the answer of a GraphQL request (the native
// JQML type; the project's qmldir does not list a file for it). `state`: Loading, Processing, Ready, Unauthorized,
// Forbidden or Error. The access token and the product id live on the XMLHttpRequest constructor, where the
// XmlHttpRequestProxy and the authorization code of the project read them.
export class GqlModel extends TreeItemModel {
	static requestTimeout = 120000;

	constructor() {
		super();
		this.$typeName = 'GqlModel';
		this.$defineCustom('state', '');
		this.$defineCustom('json', '');
		this.$defineCustom('authenticationTokenHeaderId', 'x-authentication-token', null, true);
	}

	SetGlobalAccessToken(token) {
		XMLHttpRequest.QMLAuthToken = token;
	}

	SetRefreshToken(token) {
		XMLHttpRequest.QMLAuthRefreshToken = token;
	}

	SetProductId(productId) {
		XMLHttpRequest.QMLProductId = productId;
	}

	GetGlobalAccessToken() {
		return XMLHttpRequest.QMLAuthToken || '';
	}

	GetProductId() {
		return XMLHttpRequest.QMLProductId || '';
	}

	updateJSONModel() {
		this.createFromJson(this.json);
	}

	updateTreeItemJSONModel() {}

	setGqlQuery(gqlData, headers) {
		this.state = 'Loading';
		const xhr = new XMLHttpRequest();
		xhr.open('POST', '../../graphql');
		xhr.gqlRequest = this;
		if (headers !== null && typeof headers === 'object') {
			for (const name of Object.keys(headers)) xhr.setRequestHeader(name, headers[name]);
		}
		// A request that never answers ends as an error instead of loading forever.
		xhr.timeout = GqlModel.requestTimeout;
		xhr.onreadystatechange = () => {
			if (this.$destroyed || xhr.readyState !== XMLHttpRequest.DONE) return;
			try {
				if (xhr.status === 401) {
					this.state = 'Unauthorized';
				} else if (xhr.status === 403) {
					this.state = 'Forbidden';
				} else if (xhr.status === 200) {
					this.json = xhr.responseText;
					this.state = 'Processing';
					this.updateJSONModel();
					this.updateTreeItemJSONModel();
					this.state = 'Ready';
				} else {
					this.state = 'Error';
				}
			} catch (error) {
				hooks.report(error, 'GqlModel.setGqlQuery');
			}
		};
		xhr.send(gqlData);
	}
}

// `QtWebSockets.WebSocket` over the browser's WebSocket. Status: Connecting 0, Open 1, Closing 2, Closed 3, Error 4.
// The socket is kept while `active` and `url` stay the same; a changed url reconnects, `active: false` closes it.
const NativeWebSocket = typeof WebSocket === 'undefined' ? undefined : WebSocket;

export class WebSocketItem extends Item {
	constructor() {
		super();
		this.$typeName = 'WebSocket';
		this.$nonVisual = true;
		this.$url = '';
		this.$active = false;
		this.$socket = null;
		this.$socketUrl = '';
		this.$declareSignal('textMessageReceived', ['message']);
		this.$declareSignal('binaryMessageReceived', ['message']);
		this.$defineCustom('status', 3);
		this.$defineCustom('errorString', '');
	}

	$setStatus(status, errorString = '') {
		this.errorString = errorString;
		this.status = status;
	}

	$sync() {
		if (this.$destroyed) return;
		if (!this.$active || this.$url === '') {
			this.$close();
			if (this.status !== 3) this.$setStatus(3);
			return;
		}
		if (this.$socket !== null && this.$socketUrl === this.$url) return;
		this.$close();
		this.$socketUrl = this.$url;
		if (NativeWebSocket === undefined) {
			this.$setStatus(4, 'WebSocket is not available');
			return;
		}
		let socket;
		try {
			socket = new NativeWebSocket(this.$url);
		} catch (error) {
			this.$setStatus(4, String(error && error.message ? error.message : error));
			return;
		}
		this.$socket = socket;
		this.$setStatus(0);
		const current = () => this.$socket === socket && !this.$destroyed;
		const run = (fn) => {
			if (!current()) return;
			try {
				fn();
			} catch (error) {
				hooks.report(error, `WebSocket '${this.$url}'`);
			}
		};
		socket.onopen = () => run(() => this.$setStatus(1));
		socket.onmessage = (event) => run(() => this.$emit(typeof event.data === 'string' ? 'textMessageReceived' : 'binaryMessageReceived', [event.data]));
		socket.onerror = () => run(() => this.$setStatus(4, `Cannot connect to ${this.$url}`));
		socket.onclose = () => run(() => {
			this.$socket = null;
			if (this.status !== 4) this.$setStatus(3);
		});
	}

	$close() {
		const socket = this.$socket;
		if (socket === null) return;
		this.$socket = null;
		socket.onopen = socket.onmessage = socket.onerror = socket.onclose = null;
		try {
			socket.close();
		} catch {
			// already closed
		}
	}

	sendTextMessage(message) {
		const text = String(message);
		if (this.$socket === null || this.status !== 1) return 0;
		this.$socket.send(text);
		return text.length;
	}

	sendBinaryMessage(message) {
		if (this.$socket === null || this.status !== 1) return 0;
		this.$socket.send(message);
		return message && message.byteLength !== undefined ? message.byteLength : 0;
	}

	$destroy() {
		this.$close();
		super.$destroy();
	}
}

defineProp(
	WebSocketItem.prototype,
	'url',
	function () {
		return this.$url;
	},
	function (value) {
		const url = value === null || value === undefined ? '' : String(value);
		if (url === this.$url) return;
		this.$url = url;
		this.$sync();
	},
);
defineProp(
	WebSocketItem.prototype,
	'active',
	function () {
		return this.$active;
	},
	function (value) {
		const active = Boolean(value);
		if (active === this.$active) return;
		this.$active = active;
		this.$sync();
	},
);

// The rows of a model change without a property being written: a binding that reads them runs on every pass.
function readsUntracked(type, names) {
	for (const name of names) {
		const original = type.prototype[name];
		type.prototype[name] = function (...args) {
			readUntracked(`${type.name}.${name}()`);
			return original.apply(this, args);
		};
	}
}
// The rows of a model are read through these methods: a binding that calls one follows the rows (readRows). A script
// that takes a row with `get` may change it, so the readers are marked.
function readsRows(type, names, handsOutRows) {
	for (const name of names) {
		const original = type.prototype[name];
		type.prototype[name] = function (...args) {
			if (hooks.reading !== null) readRows(this);
			else if (handsOutRows) markReaders(this, '$rows');
			return original.apply(this, args);
		};
	}
}
readsRows(ListModel, ['get'], true);
readsRows(TreeItemModel, ['toJson', 'containsKey', 'getData', 'getTreeItemModel', 'getItemsCount', 'getKeys', 'isValidData'], false);
readsRows(TreeItemModel, ['get'], true);
readsUntracked(SdlList, ['get']);

export const BUILTIN_TYPES = new Map([
	['Settings', Settings],
	['WebSocket', WebSocketItem],
	['GqlModel', GqlModel],
	['FileDialog', FileDialog],
	['Item', Item],
	['Rectangle', Rectangle],
	['Text', Text],
	['Canvas', Canvas],
	['MouseArea', MouseArea],
	['GridLayout', GridLayout],
	['StackLayout', StackLayout],
	['FlexboxLayout', FlexboxLayout],
	['LayoutItemProxy', LayoutItemProxy],
	['ListView', ListView],
	['ScrollBar', ScrollBar],
	['FontLoader', FontLoader],
	['Image', ImageItem],
	['Flickable', Flickable],
	['TextEdit', TextEdit],
	['TextInput', TextInput],
	['GridView', GridView],
	['Timer', Timer],
	['QtObject', QtObject],
	['Shortcut', Shortcut],
	['Gradient', Gradient],
	['GradientStop', GradientStop],
]);
