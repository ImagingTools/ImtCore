// The QML engine: instantiates the parsed documents into item trees, evaluates bindings, runs
// handlers and timers.
//
// Scripts are compiled to JavaScript functions by the browser's engine. A script sees, from the inside
// out: its local variables, the members of its item, the ids and root item of its component, the ids
// and root of the enclosing components (delegates have a parent scope), and finally the globals
// (Math, JSON, Date, console, Qt, Item, time).

import { Animation, ANIMATION_TYPES, AnchorLine, BindingValue, BUILTIN_TYPES, COMPONENT_ENUM_VALUE, ComponentItem, Coordinate, createSdlObject, duringChange, FILE_DIALOG_MODES, flushDeclarativeFocus, getFocusItem, hooks, Item, ListModel, ListView, linkToOwner, Loader, MapItemView, modelCount, modelScopeData, modelVersion, Placeholder, POINTER_DEVICE, queueDeclarativeFocus, readRows, readUntracked, Repeater, resetFocus, setSceneActive, Stack, Timer, TRANSFORM_ORIGIN, TreeItemModel } from './items.js';
import { fonts } from './fonts.js';
import { KEY, MODIFIERS, NAMED_KEY_HANDLERS, qtKeyEvent } from './keys.js';
import { QmlError, escapeSourceBreaks, tokenize, topLevelNames } from './lexer.js';
import { callableHandler } from './parser.js';
import { basename, dirname, hslaString, joinPath, normalizePath, parseColor, resolvePath, rgbaString, SCRIPT_IMPORT_RE } from './util.js';

const MAX_COMPONENT_DEPTH = 50;
const MAX_ERRORS = 20;
const QUIET_FLUSH_SECONDS = 1;
const MAX_REPEATER_COUNT = 1000000;

// `Foo` -> `foo`, `_Foo` -> `_foo`: the property behind a handler name (`onFooChanged`, `on_FooChanged`).
function decapitalize(text) {
	const skip = /^_*/.exec(text)[0].length;
	return text.slice(0, skip) + text.charAt(skip).toLowerCase() + text.slice(skip + 1);
}

// The item `KeyNavigation` of `item` hands the focus to for a key, or null.
function keyNavigationTarget(item, key, shift) {
	const nav = item.$keyNav;
	if (nav === undefined) return null;
	let name = '';
	if (key === KEY.Key_Tab) name = shift ? 'backtab' : 'tab';
	else if (key === KEY.Key_Backtab) name = 'backtab';
	else if (key === KEY.Key_Left) name = 'left';
	else if (key === KEY.Key_Right) name = 'right';
	else if (key === KEY.Key_Up) name = 'up';
	else if (key === KEY.Key_Down) name = 'down';
	const target = name === '' ? null : nav[name];
	return target instanceof Item && target !== item ? target : null;
}

// A binding that returns a literal (`[ "#000", ... ]`, `({ a: 1 })`) builds a new object on every evaluation. Such results
// count as unchanged while their content is equal; otherwise a Repeater over the literal would be rebuilt every frame.
function sameValue(a, b, depth = 0) {
	if (a === b) return true;
	if (a !== a && b !== b) return typeof a === 'number' && typeof b === 'number';
	if (typeof a === 'function' && typeof b === 'function') return depth > 0 && a.toString() === b.toString();
	if (depth > 4 || typeof a !== 'object' || typeof b !== 'object' || a === null || b === null) return false;
	// A binding on `anchors.left` makes a new line object every time it runs.
	if (a instanceof AnchorLine) return b instanceof AnchorLine && a.item === b.item && a.edge === b.edge;
	if (a instanceof RegExp) return b instanceof RegExp && a.source === b.source && a.flags === b.flags;
	if (Array.isArray(a)) {
		if (!Array.isArray(b) || a.length !== b.length) return false;
		for (let i = 0; i < a.length; i++) if (!sameValue(a[i], b[i], depth + 1)) return false;
		return true;
	}
	const proto = Object.getPrototypeOf(a);
	const protoB = Object.getPrototypeOf(b);
	const plain = (p) => p === Object.prototype || p === null;
	if (!plain(proto) || !plain(protoB)) return false;
	const keys = Object.keys(a);
	if (keys.length !== Object.keys(b).length) return false;
	for (const key of keys) if (!Object.prototype.hasOwnProperty.call(b, key) || !sameValue(a[key], b[key], depth + 1)) return false;
	return true;
}

// A copy of a literal-like value (arrays, plain objects), taken when a binding result is assigned. The assigned object may
// be modified by scripts afterwards (`inputParams.InsertField(...)`); the binding is re-evaluated by polling, while in Qt it
// would not run again until a dependency changes. A result equal to the copy means "nothing changed": the object is kept.
function snapshot(value, depth = 0) {
	if (typeof value !== 'object' || value === null || depth > 4) return value;
	if (value instanceof RegExp) return value;
	if (Array.isArray(value)) return value.map((v) => snapshot(v, depth + 1));
	const proto = Object.getPrototypeOf(value);
	if (proto !== Object.prototype && proto !== null) return value;
	const copy = {};
	for (const key of Object.keys(value)) copy[key] = snapshot(value[key], depth + 1);
	return copy;
}
const EMPTY = Object.freeze(Object.create(null));
const NOT_CONSTANT = Symbol('not constant');
// Qt's `QString::arg`: replaces every exact occurrence of the lowest `%N` marker. `%10` is marker 10, not marker 1.
function replaceArgMarker(text, n, value) {
	const marker = `%${n}`;
	let out = '';
	for (let i = 0; i < text.length; ) {
		if (text.startsWith(marker, i)) {
			const after = i + marker.length;
			if (!(n < 10 && after < text.length && text.charCodeAt(after) >= 48 && text.charCodeAt(after) <= 57)) {
				out += value;
				i = after;
				continue;
			}
		}
		out += text[i++];
	}
	return out;
}

if (!('arg' in String.prototype)) {
	Object.defineProperty(String.prototype, 'arg', {
		value(value) {
			const text = String(this);
			let lowest = 0;
			for (let i = 0; i < text.length; i++) {
				if (text.charCodeAt(i) !== 37) continue;
				let j = i + 1;
				if (j >= text.length || text.charCodeAt(j) < 48 || text.charCodeAt(j) > 57) continue;
				let num = text.charCodeAt(j) - 48;
				j++;
				if (j < text.length && text.charCodeAt(j) >= 48 && text.charCodeAt(j) <= 57) num = num * 10 + (text.charCodeAt(j) - 48);
				if (num > 0 && (lowest === 0 || num < lowest)) lowest = num;
			}
			if (lowest === 0) return text;
			return replaceArgMarker(text, lowest, String(value));
		},
		configurable: true,
	});
}

const EMPTY_LITERAL_RE = /^\s*(\[\s*\]|\(?\s*\{\s*\}\s*\)?)\s*;?\s*$/;
const CONSTANT_PUNCTUATION = new Set(['+', '-', '*', '/', '%', '(', ')', '!', '<', '>', '<=', '>=', '==', '!=', '===', '!==', '&&', '||', '?', ':']);
const ALIAS_RE = /^\s*([A-Za-z_$][\w$]*(?:\s*\.\s*[A-Za-z_$][\w$]*)*)\s*$/;

// Types that are silently skipped in lenient mode: animations, states and other things that only
// change how an item looks over time.
const IGNORED_TYPES = new Set([
	'Behavior', 'ScriptAction', 'PropertyAction', 'AnchorAnimation', 'ParentAnimation', 'State', 'Transition', 'PropertyChanges',
	'AnchorChanges', 'StateChangeScript', 'ParentChange', 'StateGroup', 'DropShadow', 'Glow', 'RectangularGlow',
	'InnerShadow', 'FastBlur', 'GaussianBlur', 'ColorOverlay', 'OpacityMask', 'LinearGradient', 'RadialGradient', 'Blend',
	'BrightnessContrast', 'Colorize', 'Desaturate', 'DisplacementMap', 'HueSaturation', 'ShaderEffect', 'ShaderEffectSource',
	'Translate', 'Rotation', 'Scale', 'Matrix4x4', 'DoubleValidator', 'IntValidator',
	'RegExpValidator',
]);

// Enumerations whose values are not interpreted by the engine: each name stands for itself.
const QT_ENUMS = [
	'LeftButton', 'RightButton', 'MiddleButton', 'NoButton', 'AllButtons',
	'Key_Return', 'Key_Enter', 'Key_Escape',
	'Key_Tab', 'Key_Backspace', 'Key_Delete', 'Key_Up', 'Key_Down', 'Key_Left', 'Key_Right', 'Key_Space', 'NoModifier',
	'ControlModifier', 'ShiftModifier', 'AltModifier', 'ItemIsEnabled', 'ItemIsSelectable', 'ElideNone', 'ElideLeft',
	'ElideMiddle', 'ElideRight', 'PlainText', 'RichText', 'AutoText', 'StyledText', 'MarkdownText', 'AscendingOrder',
	'DescendingOrder', 'LeftToRight', 'RightToLeft',
	'CaseInsensitive', 'CaseSensitive', 'ScrollBarAlwaysOn', 'ScrollBarAlwaysOff',
	'ScrollBarAsNeeded', 'Dialog', 'Popup', 'Window', 'FramelessWindowHint',
];

// Qt.formatDateTime / formatDate / formatTime — Qt format tokens (d, M, y, h, H, m, s, z, AP/ap, t).
// See doc.qt.io/qt-6/qml-qtqml-qt.html#formatDateTime-method. Non-string formats use locale defaults.
function formatDateTime(value, format, defaultOptions) {
	const date = value instanceof Date ? value : new Date(value);
	if (Number.isNaN(date.getTime())) return '';
	if (typeof format !== 'string') return date.toLocaleString(undefined, defaultOptions);

	const hasAmPm = /[Aa]/.test(format.replace(/'[^']*'/g, ''));
	const pad = (number, size) => String(number).padStart(size, '0');
	const hours12 = date.getHours() % 12 === 0 ? 12 : date.getHours() % 12;
	let result = '';
	let i = 0;
	while (i < format.length) {
		const ch = format[i];
		if (ch === "'") {
			if (format[i + 1] === "'") {
				result += "'";
				i += 2;
				continue;
			}
			i++;
			while (i < format.length) {
				if (format[i] === "'") {
					if (format[i + 1] !== "'") break;
					i++;
				}
				result += format[i];
				i++;
			}
			i++;
			continue;
		}
		if ((ch === 'A' || ch === 'a') && (format[i + 1] === 'P' || format[i + 1] === 'p')) {
			const text = date.getHours() < 12 ? 'AM' : 'PM';
			result += ch === 'A' ? text : text.toLowerCase();
			i += 2;
			continue;
		}
		let count = 1;
		while (format[i + count] === ch) count++;
		switch (ch) {
			case 'd':
				count = Math.min(count, 4);
				if (count <= 2) result += pad(date.getDate(), count);
				else result += date.toLocaleDateString(undefined, { weekday: count === 3 ? 'short' : 'long' });
				break;
			case 'M':
				count = Math.min(count, 4);
				if (count <= 2) {
					result += pad(date.getMonth() + 1, count);
				} else {
					const parts = new Intl.DateTimeFormat(undefined, { day: 'numeric', month: count === 3 ? 'short' : 'long' }).formatToParts(date);
					const month = parts.find((part) => part.type === 'month');
					result += month ? month.value : '';
				}
				break;
			case 'y':
				if (count >= 4) {
					count = 4;
					result += pad(date.getFullYear(), 4);
				} else if (count >= 2) {
					count = 2;
					result += pad(date.getFullYear() % 100, 2);
				} else {
					result += ch;
				}
				break;
			case 'h':
				count = Math.min(count, 2);
				result += pad(hasAmPm ? hours12 : date.getHours(), count);
				break;
			case 'H':
				count = Math.min(count, 2);
				result += pad(date.getHours(), count);
				break;
			case 'm':
				count = Math.min(count, 2);
				result += pad(date.getMinutes(), count);
				break;
			case 's':
				count = Math.min(count, 2);
				result += pad(date.getSeconds(), count);
				break;
			case 'z':
				count = Math.min(count, 3);
				result += count === 1 ? String(date.getMilliseconds()) : pad(date.getMilliseconds(), count);
				break;
			case 'A':
			case 'a':
				count = 1;
				result += date.getHours() < 12 ? (ch === 'A' ? 'AM' : 'am') : (ch === 'A' ? 'PM' : 'pm');
				break;
			case 't': {
				count = 1;
				const zone = date.toLocaleTimeString(undefined, { timeZoneName: 'short' }).split(' ');
				result += zone[zone.length - 1];
				break;
			}
			default:
				result += format.substring(i, i + count);
				break;
		}
		i += count;
	}
	return result;
}

// Qt::CursorShape — numeric so MouseArea.cursorShape bindings match desktop Qt.
const CURSOR = Object.freeze({
	ArrowCursor: 0,
	UpArrowCursor: 1,
	CrossCursor: 2,
	WaitCursor: 3,
	IBeamCursor: 4,
	SizeVerCursor: 5,
	SizeHorCursor: 6,
	SizeBDiagCursor: 7,
	SizeFDiagCursor: 8,
	SizeAllCursor: 9,
	BlankCursor: 10,
	SplitVCursor: 11,
	SplitHCursor: 12,
	PointingHandCursor: 13,
	ForbiddenCursor: 14,
	WhatsThisCursor: 15,
	BusyCursor: 16,
	OpenHandCursor: 17,
	ClosedHandCursor: 18,
});
const TEXT_ENUMS = [
	'PlainText', 'RichText', 'AutoText', 'StyledText', 'MarkdownText', 'Normal', 'Outline', 'Raised', 'Sunken',
];
const FLICKABLE_ENUMS = ['AutoFlickDirection', 'HorizontalFlick', 'VerticalFlick', 'HorizontalAndVerticalFlick', 'AutoFlickIfNeeded', 'StopAtBounds', 'DragOverBounds', 'OvershootBounds', 'DragAndOvershootBounds'];

// Qt::Alignment. AlignCenter is AlignVCenter | AlignHCenter.
const ALIGN = Object.freeze({
	AlignLeft: 1, AlignRight: 2, AlignHCenter: 4, AlignJustify: 8,
	AlignTop: 0x20, AlignBottom: 0x40, AlignVCenter: 0x80, AlignCenter: 0x84,
});

// Text.WrapMode / Text.TextElideMode.
const WRAP = Object.freeze({
	NoWrap: 0, WordWrap: 1, WrapAnywhere: 3, Wrap: 4, WrapAtWordBoundaryOrAnywhere: 4,
});
const ELIDE = Object.freeze({
	ElideLeft: 0, ElideRight: 1, ElideMiddle: 2, ElideNone: 3,
});

// Canvas.RenderTarget / Canvas.RenderStrategy: int enums, so `property int s: Canvas.Threaded` is valid.
const CANVAS = Object.freeze({
	Image: 0, FramebufferObject: 1,
	Immediate: 0, Threaded: 1, Cooperative: 2,
});

// TextInput.EchoMode.
const ECHO = Object.freeze({
	Normal: 0, NoEcho: 1, Password: 2, PasswordEchoOnEdit: 3,
});

function badValue(value) {
	throw new TypeError(`Cannot assign ${value === undefined ? 'undefined' : String(value)} to this property`);
}

// Qt basic types. `undefined` is refused. `null` becomes the type's empty value, except for `int`, which refuses it.
function toQmlString(value) {
	if (value === undefined) badValue(value);
	if (typeof value === 'string') return value;
	if (value === null) return '';
	return String(value);
}

function toQmlInt(value) {
	if (value === undefined || value === null) badValue(value);
	if (typeof value === 'boolean') return value ? 1 : 0;
	if (typeof value === 'number') return Number.isFinite(value) ? Math.trunc(value) : badValue(value);
	if (typeof value === 'string') {
		if (value.trim() === '') badValue(value);
		const n = Number(value);
		return Number.isFinite(n) ? Math.trunc(n) : badValue(value);
	}
	badValue(value);
}

function toQmlReal(value) {
	if (value === undefined) badValue(value);
	if (value === null) return 0;
	if (typeof value === 'boolean') return value ? 1 : 0;
	if (typeof value === 'number') return value;
	if (typeof value === 'string') return value.trim() === '' ? 0 : Number(value);
	badValue(value);
}

function toQmlBool(value) {
	// A script `prop = undefined` is refused. A binding may yield undefined (`flag && missingObject`);
	// Qt keeps the document and the bool becomes false until the expression has a real value.
	if (value === undefined) {
		if (hooks.engineWrite > 0) return false;
		badValue(value);
	}
	if (value === null) return false;
	if (typeof value === 'string') return value.length > 0;
	return Boolean(value);
}

function toQmlColor(value) {
	if (value === undefined) badValue(value);
	if (typeof value === 'string' && value.trim() !== '') {
		try {
			const c = parseColor(value.trim());
			const hex = (v) => v.toString(16).padStart(2, '0');
			return c.A === 255 ? `#${hex(c.R)}${hex(c.G)}${hex(c.B)}` : `#${hex(c.A)}${hex(c.R)}${hex(c.G)}${hex(c.B)}`;
		} catch {
			return '#000000';
		}
	}
	return '#000000';
}

function enumObject(names) {
	const result = Object.create(null);
	for (const name of names) result[name] = name;
	return Object.freeze(result);
}

const STACK_TYPES = new Set(['Column', 'Row', 'Flow', 'ColumnLayout', 'RowLayout']);
// Types that are known but not drawn or implemented: they are placeholders that keep their properties.
// Numeric `property` types start at 0, like in Qt.
const INT_TYPES = new Set(['int', 'uint', 'long', 'short']);
const REAL_TYPES = new Set(['real', 'double', 'qreal', 'float']);
const SILENT_TYPES = new Set(['Connections', 'Binding', 'AnimatedImage', 'BorderImage', 'ScrollView', 'SystemPalette']);

// Ids and the root item of one component instance (or one delegate of a Repeater/ListView).
class Scope {
	constructor(parent, file) {
		this.parent = parent;
		this.file = file;
		this.ids = Object.create(null);
		// The roles, `model` and `modelData` of a delegate. Like in Qt they are properties of the context: below the ids of the
		// component and the properties of the objects (a `Loader` inside of a delegate with the role `item` sees its own
		// `item`).
		this.context = Object.create(null);
		this.root = null;
		this.data = null; // model data of a delegate scope (replaced when a ListView reuses the delegate)
	}
}

function emptyDef(type) {
	return { type, id: '', propertyName: '', properties: [], functions: [], signalDefs: [], imports: [], children: [], line: 0, col: 0 };
}

// Can a value be assigned to the property `name` of `target`?
function isAssignable(target, name) {
	if (name.startsWith('$')) return false;
	for (let o = target; o !== null && o !== Object.prototype; o = Object.getPrototypeOf(o)) {
		const d = Object.getOwnPropertyDescriptor(o, name);
		if (d !== undefined) return d.set !== undefined || (d.writable === true && typeof d.value !== 'function');
	}
	return false;
}

function describe(error) {
	if (error !== null && typeof error === 'object' && error.message !== undefined) return String(error.message);
	return String(error);
}

export class Engine {
	constructor() {
		this.project = null;
		this.time = 0;
		this.usesTime = false;
		this.errors = [];
		this.errorSet = new Set();
		this.bindings = new Set();
		// Incremental evaluation: a pass runs the bindings in `dirty` (a read property was written, the layout moved
		// what they read, they were just created) and the `volatile` ones, which read something that is not announced.
		// `incremental = false` runs every binding on every pass, as before; `audit` then checks the shortcut.
		this.incremental = true;
		this.primed = true; // false after a full sweep: the next incremental pass has to run every binding
		this.bindingSeq = 0;
		this.dirty = new Set();
		this.retry = new Set(); // bound properties a Binding item wrote over: they apply their value on the next pass only
		this.volatile = new Set();
		this.passHeap = []; // bindings of the running pass that still wait, the lowest `seq` first
		this.inPass = false;
		this.cursor = 0; // `seq` of the binding the running pass evaluates
		this.auditing = null; // a list that evaluateOne fills with the bindings whose value changed
		this.auditMode = false;
		this.auditMisses = new Map(); // 'Type.property file:line' -> { count, reads }
		this.timers = new Set();
		this.completed = [];
		this.documentScope = null;
		this.factories = new Map();
		this.constants = new Map();
		this.modules = new Map();
		this.onError = null;
		this.onLog = null;
		this.onQuit = null; // Qt.quit() from a script

		// Lenient mode (for large real-world projects): unknown types become placeholders, unknown
		// properties and handlers are ignored, and a failing property does not stop the load.
		this.lenient = false;
		this.maxErrors = MAX_ERRORS;
		this.unknownTypes = new Map(); // type -> count
		this.ignoredProperties = new Map(); // 'Type.property' -> count
		this.changeHandlers = [];
		this.singletons = new Map(); // name -> { path, item }
		this.singletonEntries = new Map(); // path -> { path, item }: a singleton file is created once
		this.fileLayers = new Map(); // file -> the names that only that file sees (see fileLayer), or null
		this.ambiguousSingletons = new Set(); // names of singletons that several files declare
		this.loaders = new Set();
		this.repeaters = new Set();
		this.connections = new Set(); // Connections items
		this.qmlBindings = new Set(); // Binding items
		this.pendingImages = new Set();
		this.animations = new Set();
		this.carets = new Set(); // editors with the focus: their text cursor blinks
		this.fresh = []; // bindings created since they were last evaluated
		this.subQueue = []; // bindings to re-run because a property they read was written
		this.draining = false;
		this.trackingPass = 0; // > 0 while every binding is already being swept; writes do not fan out
		this.completing = 0; // > 0 while Component.onCompleted handlers run
		this.indexTrack = null; // items that receive the `index` of the ListView delegate being created
		this.deferred = []; // signals emitted meanwhile, delivered afterwards
		this.host = null;
		this.quietSince = null; // the time of the first change a hidden animation or timer made since the last frame it asked for
		this.clipboard = { text: '', html: '' }; // the last copied or pasted content
		this.onClipboardWrite = null; // (text, html) => void: the page puts it on the system clipboard
		this.jsGlobals = [];
		// A compiled application loads its pictures from a URL root the way the JQML runtime does:
		// `resourceRoot + source` without `qrc:/` and `../`. null: relative to the file of the Image item.
		this.resourceRoot = null;
		// language -> { source text -> translation }, filled by the compiler from Qt .ts catalogs.
		this.translations = {};
		// The JQML application context: `context.appId`, `context.language` and the rest that QML code reads and writes.
		// A script reading `language` (through qsTr as well) depends on it, so a language change re-evaluates the bindings.
		this.context = new Item();
		this.context.$typeName = 'Context';
		this.context.$nonVisual = true;
		for (const [name, initial] of [['appId', ''], ['appName', ''], ['appIcon', ''], ['language', ''], ['application', []], ['location', typeof window === 'undefined' ? {} : window.location]]) {
			this.context.$defineCustom(name, initial);
		}

		const engine = this;
		const qt = Object.freeze({
			...enumObject(QT_ENUMS),
			...CURSOR,
			...ALIGN,
			// Qt::Edge.
			TopEdge: 0x1,
			LeftEdge: 0x2,
			RightEdge: 0x4,
			BottomEdge: 0x8,
			// Qt::Orientation, Qt::CheckState.
			Horizontal: 0x1,
			Vertical: 0x2,
			Unchecked: 0,
			PartiallyChecked: 1,
			Checked: 2,
			// Qt::WindowModality — numeric so `property int modality: Qt.ApplicationModal` works.
			NonModal: 0,
			WindowModal: 1,
			ApplicationModal: 2,
			// Qt::FocusPolicy. StrongFocus is TabFocus | ClickFocus | 0x8, WheelFocus adds 0x4.
			NoFocus: 0,
			TabFocus: 0x1,
			ClickFocus: 0x2,
			StrongFocus: 0xb,
			WheelFocus: 0xf,
			...KEY,
			...MODIFIERS,
			platform: Object.freeze({ os: 'web', pluginName: 'web' }),
			application: Object.freeze({ arguments: [], name: '', version: '' }),
			createObject: (type, parent, props) => engine.createObject(type, parent, props),
			createComponent: (url, second, third) => engine.createComponent(url, second, third),
			resolvedUrl: (url) => url,
			quit: () => {
				if (engine.onQuit !== null) engine.onQuit();
			},
			formatDateTime: (d, f) => { const z = (n) => String(n).padStart(2, '0'); return String(f).replace('dd', z(d.getDate())).replace('MM', z(d.getMonth() + 1)).replace('yyyy', d.getFullYear()).replace('hh', z(d.getHours())).replace('mm', z(d.getMinutes())); },
			isQtObject: (v) => v !== null && typeof v === 'object' && v.$typeName !== undefined,
			atob: (s) => atob(s), btoa: (s) => btoa(s),
			locale: () => ({ decimalPoint: (1.5).toLocaleString().charAt(1), name: typeof navigator === 'undefined' ? '' : navigator.language }),
			openUrlExternally: (url) => { window.open(String(url), '_blank'); return true; },
			point: (x, y) => ({ x, y }),
			size: (width, height) => ({ width, height }),
			rect: (x, y, width, height) => ({ x, y, width, height }),
			callLater: (fn, ...args) => setTimeout(() => engine.guard(() => fn(...args), 'Qt.callLater', 0), 0),
			binding: (fn) => {
				if (typeof fn !== 'function') throw new QmlError('Qt.binding needs a function');
				return new BindingValue(fn);
			},
			rgba: (r, g, b, a = 1) => rgbaString(r, g, b, a),
			hsla: (h, s, l, a = 1) => hslaString(h, s, l, a),
			colorEqual: (a, b) => {
				const first = parseColor(String(a));
				const second = parseColor(String(b));
				return first !== null && second !== null && first.rgb === second.rgb && first.A === second.A;
			},
			formatDateTime: (dateTime, format) => formatDateTime(dateTime, format, { dateStyle: 'short', timeStyle: 'short' }),
			formatDate: (date, format) => formatDateTime(date, format, { dateStyle: 'short' }),
			formatTime: (time, format) => formatDateTime(time, format, { timeStyle: 'short' }),
		});
		this.globals = Object.create(null);
		Object.assign(this.globals, {
			Math,
			JSON,
			Date,
			Qt: qt,
			PointerDevice: POINTER_DEVICE,
			QtPositioning: Object.freeze({ coordinate: (latitude, longitude, altitude) => new Coordinate(latitude, longitude, altitude) }),
			context: this.context,
			Item: TRANSFORM_ORIGIN,
			Text: Object.freeze({ ...enumObject(TEXT_ENUMS), ...ALIGN, ...WRAP, ...ELIDE }),
			TextInput: Object.freeze({ ...enumObject(TEXT_ENUMS.concat(['Normal', 'Password', 'NoEcho', 'PasswordEchoOnEdit', 'CursorBeforeChar', 'CursorOnCharacter'])), ...ALIGN, ...WRAP, ...ELIDE, ...ECHO }),
			TextEdit: Object.freeze({ ...enumObject(TEXT_ENUMS), ...ALIGN, ...WRAP, ...ELIDE }),
			// Qt logs Image enums as their numeric values (FillMode, Status, alignment).
			Image: Object.freeze({
				Stretch: 0, PreserveAspectFit: 1, PreserveAspectCrop: 2, Tile: 3, TileVertically: 4, TileHorizontally: 5, Pad: 6,
				Null: 0, Ready: 1, Loading: 2, Error: 3,
				AlignLeft: 1, AlignRight: 2, AlignHCenter: 4, AlignJustify: 8, AlignTop: 32, AlignBottom: 64, AlignVCenter: 128, AlignCenter: 132,
			}),
			Flickable: enumObject(FLICKABLE_ENUMS),
			ListView: Object.freeze({
				Horizontal: 1, Vertical: 2,
				...enumObject(['Beginning', 'Center', 'End', 'Visible', 'Contain', 'SnapToItem', 'NoSnap', 'SnapOneItem']),
			}),
			FlexboxLayout: enumObject(['Row', 'RowReverse', 'Column', 'ColumnReverse', 'NoWrap', 'Wrap', 'WrapReverse', 'JustifyFlexStart', 'JustifyCenter', 'JustifyFlexEnd', 'JustifySpaceBetween', 'JustifySpaceAround', 'JustifySpaceEvenly', 'AlignFlexStart', 'AlignCenter', 'AlignFlexEnd', 'AlignStretch', 'AlignStart', 'AlignEnd']),
			// Qt prints these as numbers: Null 0, Ready 1, Loading 2, Error 3.
			Loader: Object.freeze({ Null: 0, Ready: 1, Loading: 2, Error: 3 }),
			WebSocket: Object.freeze({ Connecting: 0, Open: 1, Closing: 2, Closed: 3, Error: 4 }),
			// Component.status and the mode argument of Qt.createComponent.
			Component: COMPONENT_ENUM_VALUE,
			Canvas: CANVAS,
			Gradient: enumObject(['Vertical', 'Horizontal']),
			FileDialog: Object.freeze(Object.fromEntries(FILE_DIALOG_MODES.map((name, index) => [name, index]))),
			XMLHttpRequest: typeof XMLHttpRequest === 'undefined' ? class XMLHttpRequest { open() {} send() {} setRequestHeader() {} } : XMLHttpRequest,
			Animation: Object.freeze({ Infinite: -1, Forward: 'Forward', Backward: 'Backward' }),
			Easing: enumObject(['Linear', 'InQuad', 'OutQuad', 'InOutQuad', 'OutInQuad', 'InCubic', 'OutCubic', 'InOutCubic', 'OutInCubic', 'InQuart', 'OutQuart', 'InOutQuart', 'InQuint', 'OutQuint', 'InOutQuint', 'InSine', 'OutSine', 'InOutSine', 'InExpo', 'OutExpo', 'InOutExpo', 'InCirc', 'OutCirc', 'InOutCirc', 'InElastic', 'OutElastic', 'InOutElastic', 'InBack', 'OutBack', 'InOutBack', 'InBounce', 'OutBounce', 'InOutBounce', 'BezierSpline']),
			// Qt Quick Test's mouseMove fails the same way under offscreen qml: the call throws and the test catches it.
			mouseMove() {
				throw new Error('QtQuickTest::fail');
			},
			qsTr: (text) => engine.translate(text),
			qsTrId: (text) => engine.translate(text),
			qsTranslate: (context, text) => engine.translate(text),
			QT_TR_NOOP: (text) => text,
			console: {
				log: (...args) => {
					const line = args.map((a) => (typeof a === 'string' ? a : safeStringify(a))).join(' ');
					// A comparison run collects the line itself; the viewer prints it.
					if (engine.onLog !== null) engine.onLog(line);
					else console.log(...args);
				},
				info: (...args) => console.log(...args),
				debug: () => {},
				warn: (...args) => console.warn(...args),
				error: (...args) => console.warn(...args),
				assert: () => {},
			},
		});
		Object.defineProperty(this.globals, 'time', {
			get() {
				engine.usesTime = true;
				readUntracked('time');
				return engine.time;
			},
			enumerable: true,
		});

		hooks.installBinding = (item, path, fn) => this.addBinding(item, path, () => fn.call(item), 0);
		hooks.report = (error, where) => this.report(where, error, 0);
		hooks.instantiate = (component, parent, props) => this.instantiateComponent(component, parent, props);
		hooks.loadFont = (loader, source) => this.loadFont(loader, source);
		hooks.loadImage = (image, source) => this.loadImage(image, source);
		hooks.setupDelegate = (view, component) => {
			if (!component.$def || component.$def.children.length !== 1) return false;
			this.setupListView(view, component.$def.children[0], component.$defScope, 0);
			return true;
		};
		hooks.touch = () => {
			if (this.host !== null) this.host.$update();
		};
		hooks.changed = (item, name) => this.fireChanged(item, name);
		hooks.changeSignal = (item, name) => this.defineChangeSignal(item, name);
		hooks.initializeProperty = (item, name) => this.initializeProperty(item, name);
		hooks.release = (item) => this.releaseSubtree(item);
		hooks.breakBinding = (item, name) => this.dropBinding(item, name);
		hooks.depend = (item, name, trusted) => this.depend(item, name, trusted);
		hooks.notify = (item, name) => this.notify(item, name);
		hooks.dirty = (item, name) => this.markReaders(item, name);
		hooks.reloadLoader = (loader) => this.updateLoader(loader);
		hooks.clipboardWrite = (text, html) => {
			this.clipboard = { text, html };
			if (this.onClipboardWrite !== null) this.onClipboardWrite(text, html);
		};
		hooks.clipboardRead = () => this.clipboard;
		hooks.caret = (item) => this.carets.add(item);
		// Qt delivers a signal before the next line of the script that emitted it, including inside onCompleted.
		hooks.defer = () => false;
		hooks.syncModel = (model) => this.syncModelNow(model);
		hooks.syncRepeater = (repeater) => this.syncRepeaterNow(repeater);
		hooks.syncBinding = (item) => this.syncBindingItems(item);
		hooks.syncConnection = (item) => this.syncConnections(item);
		hooks.completing = () => this.completing > 0;
		// Viewer loads the document under a host below the toolbar; null-target maps use that origin.
		hooks.mapOrigin = () => this.host;
		fonts.onWarning = (message) => console.warn(message);
		fonts.defaultFamily = () => this.mainFontFamily();
	}

	// The font of text that sets no `font.family`: the main font of the `Style` singleton - the family of its
	// `mainFontLoader` when a font file was loaded, otherwise `Style.fontFamily`.
	mainFontFamily() {
		if (this.project === null) return '';
		const style = this.globals.Style;
		if (style === undefined || style === null) return '';
		const loader = style.mainFontLoader;
		if (loader && loader.status === 'Ready' && loader.name) return loader.name;
		const family = style.fontFamily;
		return typeof family === 'string' ? family : '';
	}

	get bindingCount() {
		return this.bindings.size + this.changeHandlers.length;
	}

	get language() {
		return this.context.language;
	}

	set language(value) {
		this.context.language = value;
	}

	// The translation of `text` in the current language: `de_DE` falls back to `de`, a missing entry to the text.
	translate(text) {
		if (typeof text !== 'string' || !this.language) return text;
		const languages = [this.language, this.language.replace('-', '_'), this.language.split(/[-_]/)[0]];
		for (const language of languages) {
			const catalog = this.translations[language];
			if (catalog && typeof catalog[text] === 'string' && catalog[text] !== '') return catalog[text];
		}
		return text;
	}

	get isAnimated() {
		if (this.usesTime) return true;
		// PauseAnimation only waits: ticking it must not force a full scene redraw every frame
		// (CustomTooltip's open delay would flicker the gallery while the pointer rests/moves).
		// An animation of a hidden item (e.g. an indeterminate ProgressBar on another page) keeps running
		// but changes nothing on screen, so it does not force a redraw either.
		for (const animation of this.animations) {
			if (animation.$isVisuallyActive()) return true;
		}
		return false;
	}

	reset() {
		this.bindings.clear();
		this.dirty.clear();
		this.retry.clear();
		this.volatile.clear();
		this.passHeap = [];
		this.inPass = false;
		this.cursor = 0;
		this.timers.clear();
		this.changeHandlers = [];
		this.loaders.clear();
		this.repeaters.clear();
		this.connections.clear();
		this.qmlBindings.clear();
		this.pendingImages.clear();
		this.animations.clear();
		this.carets.clear();
		this.fresh = [];
		this.subQueue = [];
		this.draining = false;
		this.trackingPass = 0;
		this.deferred = [];
		this.completing = 0;
		this.unknownTypes.clear();
		this.ignoredProperties.clear();
		for (const name of this.singletons.keys()) delete this.globals[name];
		for (const name of this.jsGlobals) delete this.globals[name];
		this.jsGlobals = [];
		this.singletons.clear();
		this.singletonEntries.clear();
		this.fileLayers.clear();
		this.ambiguousSingletons.clear();
		this.errors = [];
		this.errorSet.clear();
		this.completed = [];
		this.modules.clear();
		this.usesTime = false;
		this.documentScope = null;
		this.quietSince = null;
		hooks.quiet = 0;
		hooks.quietDirty = false;
		resetFocus();
	}

	// Reports a script or evaluation problem; the same message is only stored once.
	report(where, error, line) {
		const lineText = line ? `: line ${line}` : '';
		const text = `${where}${lineText}: ${describe(error)}`;
		if (this.errorSet.has(text) || this.errors.length >= this.maxErrors) return;
		this.errorSet.add(text);
		this.errors.push(text);
		console.error(text);
		if (this.onError !== null) this.onError(text);
	}

	// ----- loading -----

	// Instantiates the main document of a project below `host`. Throws a QmlError on failure.
	load(project, host) {
		this.reset();
		this.project = project;
		this.host = host;
		this.setupSingletons();
		const doc = project.docs.get(project.mainPath);
		this.documentScope = this.newScope(null, doc.path);
		const root = this.createItem(doc.def, this.documentScope, host, [], undefined, 0);
		root.anchors.fill = host;
		this.evaluateBindings();
		this.runCompletedFrom(0);
		return root;
	}

	// ----- scopes and scripts -----

	// `pragma Singleton` types of the imported modules become global names, created on first use.
	setupSingletons() {
		this.ambiguousSingletons = this.findAmbiguousSingletons();
		for (const [name, path] of this.project.singletons) {
			const entry = this.singletonEntry(path);
			this.singletons.set(name, entry);
			Object.defineProperty(this.globals, name, {
				get: () => this.singleton(entry),
				enumerable: true,
				configurable: true,
			});
		}
		// JavaScript files listed in a qmldir: their top-level names are globals (and so is the module name).
		for (const [name, path] of this.project.jsModules) {
			let module;
			const load = () => {
				if (module === undefined) {
					module = null;
					module = this.guard(() => this.moduleFor(path), `script '${path}'`, 0) || null;
				}
				return module;
			};
			const define = (globalName, read) => {
				if (globalName in this.globals) return;
				this.jsGlobals.push(globalName);
				// The script assigns its own variables through the global scope: keep the values.
				let own;
				Object.defineProperty(this.globals, globalName, {
					get: () => (own !== undefined ? own : read()),
					set: (value) => {
						own = value;
					},
					enumerable: true,
					configurable: true,
				});
			};
			const source = this.project.scripts.get(path);
			let names = [];
			try {
				names = topLevelNames(tokenize((source || '').replace(/^\s*\.(?:pragma|import)\b.*$/gm, '')));
			} catch {
				names = [];
			}
			for (const exported of names) define(exported, () => (load() === null ? undefined : load()[exported]));
			define(name, () => load());
			// A script with no top-level names (a UMD library such as moment.js) is reached through a global that it sets itself:
			// nothing would load it, so it runs at once, like the import of a module in Qt.
			if (source !== undefined && names.length === 0) load();
		}
	}

	singletonEntry(path) {
		let entry = this.singletonEntries.get(path);
		if (entry === undefined) {
			entry = { path, item: undefined };
			this.singletonEntries.set(path, entry);
		}
		return entry;
	}

	// The names that more than one file declares as a singleton (in different modules or directories): a file gets the one
	// of its own imports, the later import first. The other names are global.
	findAmbiguousSingletons() {
		const project = this.project;
		const paths = new Map();
		const add = (name, path) => {
			if (path === undefined) return;
			if (!paths.has(name)) paths.set(name, new Set());
			paths.get(name).add(path);
		};
		// Several versions of a singleton are several files: an import picks one of them. A singleton that the directories
		// of one module declare in the same version is one file, the one of the last directory.
		for (const singletons of project.moduleSingletons.values()) {
			for (const [name, entries] of singletons) {
				const versions = new Set(entries.map((entry) => `${entry.major}.${entry.minor}`));
				if (versions.size > 1) for (const entry of entries) add(name, entry.path);
				else add(name, entries[0].path);
			}
		}
		for (const singletons of project.dirSingletons.values()) {
			for (const [name, path] of singletons) add(name, path);
		}
		const ambiguous = new Set();
		for (const [name, set] of paths) if (set.size > 1) ambiguous.add(name);
		return ambiguous;
	}

	// What a file sees besides the global names: `Alias.Singleton` of what it imports as Alias, and the singletons that
	// several files declare, as its own imports give them. Null if there is nothing.
	fileLayer(file) {
		if (this.fileLayers.has(file)) return this.fileLayers.get(file);
		let layer = null;
		const doc = this.project === null ? undefined : this.project.docs.get(file);
		if (doc && doc.def) {
			const define = (target, name, path) => {
				Object.defineProperty(target, name, {
					get: () => this.singleton(this.singletonEntry(path)),
					enumerable: true,
					configurable: true,
				});
			};
			// The imports in the order of the file: a later one overrides the names of an earlier one.
			for (const imp of this.project.fileImports(doc)) {
				const singletons = new Map();
				if (imp.kind === 'directory') {
					const declared = this.project.dirSingletons.get(resolvePath(imp.path, dirname(file)));
					if (declared !== undefined) for (const [name, path] of declared) singletons.set(name, path);
				} else {
					const declared = this.project.moduleSingletons.get(imp.name);
					if (declared !== undefined) {
						for (const name of declared.keys()) singletons.set(name, this.project.moduleSingletonPath(imp.name, name, imp.version));
					}
				}
				if (singletons.size === 0) continue;
				if (layer === null) layer = Object.create(null);
				if (imp.alias) {
					if (!Object.prototype.hasOwnProperty.call(layer, imp.alias)) layer[imp.alias] = Object.create(null);
					for (const [name, path] of singletons) define(layer[imp.alias], name, path);
				} else {
					for (const [name, path] of singletons) if (this.ambiguousSingletons.has(name)) define(layer, name, path);
				}
			}
			if (layer !== null && Object.keys(layer).length === 0) layer = null;
		}
		this.fileLayers.set(file, layer);
		return layer;
	}

	singleton(entry) {
		if (entry.item !== undefined) return entry.item;
		entry.item = null; // while it is being created
		const doc = this.project.docs.get(entry.path);
		if (doc === undefined || doc.def === null) return null;
		const mark = this.completed.length;
		entry.item = this.guard(() => this.createItem(doc.def, this.newScope(null, doc.path), null, [], undefined, 0), `singleton '${entry.path}'`, 0) || null;
		this.runCompletedFrom(mark);
		return entry.item;
	}

	newScope(parent, file) {
		const scope = new Scope(parent, file);
		const layer = this.project === null ? null : this.fileLayer(file);
		if (layer !== null) scope.ids = Object.create(layer);
		const doc = this.project === null ? undefined : this.project.docs.get(file);
		if (doc && doc.def) {
			for (const imp of doc.def.imports) {
				if (imp.alias && imp.path.endsWith('.js')) {
					scope.ids[imp.alias] = this.moduleFor(resolvePath(imp.path, dirname(file)));
				}
			}
		}
		return scope;
	}

	// A JavaScript file imported as a module: its top-level names as an object. Its own
	// `.import "Other.js" as Alias` lines make `Alias` a name inside the file (resolved lazily, so
	// modules that import each other do not recurse forever).
	moduleFor(path) {
		if (this.modules.has(path)) return this.modules.get(path);
		const source = this.project.scripts.get(path);
		if (source === undefined) throw new QmlError(`Cannot read the JavaScript file '${path}'`);
		const text = source.replace(/^\s*\.(?:pragma|import)\b.*$/gm, '');
		const aliases = {};
		for (const match of source.matchAll(SCRIPT_IMPORT_RE)) {
			const target = resolvePath(match[1], dirname(path));
			Object.defineProperty(aliases, match[2], { get: () => this.moduleFor(target), enumerable: true });
		}
		let factory;
		try {
			const names = topLevelNames(tokenize(text));
			const accessors = names.map((n) => `get ${n}() { return ${n}; }, set ${n}(v) { ${n} = v; }`).join(',\n');
			factory = new Function('__g', '__a', `with (__g) { with (__a) {\n${text}\n;return {\n${accessors}\n};\n} }`);
		} catch (error) {
			throw new QmlError(`${path}: ${describe(error)}`, error.line || 0, error.column || 0);
		}
		let module;
		try {
			module = factory(this.globals, aliases);
		} catch (error) {
			throw new QmlError(`${path}: ${describe(error)}`);
		}
		this.modules.set(path, module);
		return module;
	}

	// Compiles script text into a function. kind: 'expr' (the function returns the expression) or
	// 'body' (the function runs statements). The function is called with the item as `this`.
	compile(kind, code, params, scope, self) {
		const levels = [];
		for (let s = scope; s !== null; s = s.parent) levels.push(s);
		const depth = levels.length;
		// The levels that are delegates have a context (roles) that is looked up after the objects of the level.
		let contexts = '';
		for (const level of levels) contexts += level.data !== null ? '1' : '0';
		const key = `${depth}\n${contexts}\n${kind}\n${params}\n${code}`;
		let factory = this.factories.get(key);
		if (factory === undefined) {
			const names = ['__g'];
			let open = 'with (__g) {';
			let closing = 2 * depth + 2;
			// Like Qt, the ids of the component win over the properties of the item itself (`editorView: editorView`).
			for (let l = depth - 1; l >= 1; l--) {
				if (contexts[l] === '1') {
					open += `with (__c${l}) {`;
					closing++;
				}
				open += `with (__r${l}) { with (__i${l}) {`;
			}
			if (depth > 0 && contexts[0] === '1') {
				open += 'with (__c0) {';
				closing++;
			}
			open += depth > 0 ? 'with (__r0) { with (__self) { with (__i0) {' : 'with (__self) {';
			for (let l = 0; l < depth; l++) {
				if (contexts[l] === '1') names.push(`__c${l}`);
				names.push(`__r${l}`, `__i${l}`);
			}
			names.push('__self');
			const js = escapeSourceBreaks(code);
			const body = kind === 'expr' ? `return (${js}\n);` : js;
			const source = `${open} return function (${params}) {\n${body}\n}; ${'}'.repeat(closing)}`;
			factory = new Function(...names, source);
			this.factories.set(key, factory);
		}
		const args = [this.globals];
		for (const scopeLevel of levels) {
			if (scopeLevel.data !== null) args.push(scopeLevel.context);
			args.push(scopeLevel.root === null ? EMPTY : scopeLevel.root, scopeLevel.ids);
		}
		args.push(self);
		return factory(...args);
	}

	// ----- bindings -----

	initializeProperty(item, name) {
		if (item.$bindings === null) return;
		const binding = item.$bindings.find((entry) => entry.key === name);
		if (binding !== undefined && !binding.hasLast && !binding.evaluating && binding.writing === 0) this.evaluateOne(binding, true);
	}

	addBinding(owner, path, fn, line, file = '') {
		const key = path.join('.');
		if (owner.$bindings === null) owner.$bindings = [];
		for (const existing of owner.$bindings) {
			if (existing.key === key) {
				this.unsubscribe(existing);
				existing.fn = fn;
				existing.failed = false;
				existing.hasLast = false;
				existing.snap = undefined;
				this.markDirty(existing);
				return;
			}
		}
		const binding = { owner, path, key, fn, line, file, last: undefined, hasLast: false, failed: false, failures: 0, dItems: null, dNames: null, nOld: 0, nCur: 0, diverged: false, curItems: null, curNames: null, queued: false, evaluating: false, writing: 0, runs: 0, seq: this.bindingSeq++, volatile: false, volatileNow: false, why: null, inHeap: false };
		owner.$bindings.push(binding);
		this.bindings.add(binding);
		this.fresh.push(binding);
		this.markDirty(binding);
	}

	// A value assigned over a property that a base type binds (`width: 32` on a use of a component whose root has a
	// `width: ...` binding) replaces the binding, as in Qt.
	dropBinding(item, key) {
		const list = item.$bindings;
		if (list === null || list === undefined) return;
		const at = list.findIndex((b) => b.key === key);
		if (at < 0) return;
		const binding = list[at];
		binding.failed = true;
		this.unsubscribe(binding);
		this.bindings.delete(binding);
		this.volatile.delete(binding);
		list.splice(at, 1);
	}

	// A Binding item (or other engine write) may replace the property while the binding still believes
	// `last`/`snap` are applied — e.g. ControlBase stamps width 30 after content measured to 100.
	// Point both at the written value so the next pass re-applies (evaluateOne also compares `snap`).
	markBoundPropertyWritten(item, key, written) {
		const list = item.$bindings;
		if (list === null || list === undefined) return false;
		const binding = list.find((b) => b.key === key);
		if (binding === undefined || binding.failed || !binding.hasLast) return false;
		// The binding's own write: evaluateOne records it (a run nested in the write has recorded a newer one already).
		if (binding.writing > 0) return false;
		if (sameValue(binding.last, written)) return false;
		binding.last = written;
		binding.snap = snapshot(written);
		// Another writer (a Binding item) took the property over: the bound property applies its own value again on the
		// next pass of this evaluation, as a full sweep does. The retry is not kept for later frames: the Binding item
		// writes its value again at the end of every evaluation. The audit must not count the fight either.
		if (!binding.evaluating) {
			binding.overridden = true;
			if (this.incremental) this.retry.add(binding);
		}
		return true;
	}

	// The binding reads `item.name` (and the group key `anchors.leftMargin`). The reads are matched against the ones of the
	// last evaluation by position; only when they differ are they recorded and the subscriptions renewed (endTracking).
	depend(item, name, trusted = false) {
		const binding = hooks.reading;
		if (binding === null) return;
		if (!trusted) {
			binding.volatileNow = true;
			if (binding.why === null) binding.why = `${item.$typeName}.${name}`;
		}
		const n = binding.nCur;
		if (!binding.diverged) {
			if (n < binding.nOld && binding.dItems[n] === item && binding.dNames[n] === name) {
				binding.nCur = n + 1;
				return;
			}
			binding.diverged = true;
			binding.curItems = binding.dItems === null ? [] : binding.dItems.slice(0, n);
			binding.curNames = binding.dNames === null ? [] : binding.dNames.slice(0, n);
		}
		binding.curItems.push(item);
		binding.curNames.push(name);
		binding.nCur = n + 1;
	}

	// Makes the subscriptions match what the evaluation that just ended read.
	endTracking(binding) {
		const same = !binding.diverged && binding.nCur === binding.nOld;
		const diverged = binding.diverged;
		const curItems = binding.curItems;
		const curNames = binding.curNames;
		const read = binding.nCur;
		binding.nCur = 0;
		binding.diverged = false;
		binding.curItems = null;
		binding.curNames = null;
		// A binding that failed while evaluating was unsubscribed already (dItems is null): nothing to renew.
		if (same || binding.failed || (binding.dItems === null && !diverged)) return;
		const items = diverged ? curItems : binding.dItems.slice(0, read);
		const names = diverged ? curNames : binding.dNames.slice(0, read);
		this.unsubscribe(binding);
		for (let i = 0; i < items.length; i++) {
			const item = items[i];
			let subs = item.$subs;
			if (subs === undefined) item.$subs = subs = new Map();
			let set = subs.get(names[i]);
			if (set === undefined) subs.set(names[i], set = new Set());
			set.add(binding);
		}
		binding.dItems = items;
		binding.dNames = names;
		binding.nOld = items.length;
	}

	unsubscribe(binding) {
		const items = binding.dItems;
		if (items === null) return;
		const names = binding.dNames;
		binding.dItems = null;
		binding.dNames = null;
		binding.nOld = 0;
		for (let i = 0; i < items.length; i++) {
			const subs = items[i].$subs;
			const set = subs === undefined ? undefined : subs.get(names[i]);
			if (set === undefined) continue;
			set.delete(binding);
			if (set.size === 0) subs.delete(names[i]);
		}
	}

	// Writes from a script (and the binding updates they cause) re-run readers before the write returns.
	// During a pass the readers are only marked: the pass reaches them (a full sweep visits every binding anyway).
	notify(item, name) {
		const set = item.$subs === undefined ? undefined : item.$subs.get(name);
		if (set === undefined || set.size === 0) return;
		if (this.trackingPass > 0) {
			if (this.incremental) for (const binding of set) this.markDirty(binding);
			return;
		}
		for (const binding of [...set]) this.queueSubscriber(binding);
		this.drainSubscribers();
	}

	// The layout changed a value that bindings read: they run on the next pass, not inside the layout.
	markReaders(item, name) {
		if (!this.incremental) return;
		const set = item.$subs === undefined ? undefined : item.$subs.get(name);
		if (set === undefined) return;
		for (const binding of set) this.markDirty(binding);
	}

	// The binding runs again: later in the running pass if its turn has not come, else in the next pass.
	markDirty(binding) {
		if (!this.incremental || binding.failed || binding.evaluating || binding.writing) return;
		if (this.inPass && binding.seq > this.cursor) {
			if (!binding.inHeap) this.pushPending(binding);
		} else {
			this.dirty.add(binding);
		}
	}

	setVolatile(binding, volatile) {
		if (volatile) {
			if (!binding.volatile) {
				binding.volatile = true;
				this.volatile.add(binding);
			}
		} else if (binding.volatile) {
			binding.volatile = false;
			this.volatile.delete(binding);
		}
	}

	pushPending(binding) {
		const heap = this.passHeap;
		binding.inHeap = true;
		let at = heap.length;
		heap.push(binding);
		while (at > 0) {
			const up = (at - 1) >> 1;
			if (heap[up].seq <= binding.seq) break;
			heap[at] = heap[up];
			at = up;
		}
		heap[at] = binding;
	}

	popPending() {
		const heap = this.passHeap;
		const first = heap[0];
		const last = heap.pop();
		const size = heap.length;
		if (size > 0) {
			let at = 0;
			for (;;) {
				let child = 2 * at + 1;
				if (child >= size) break;
				if (child + 1 < size && heap[child + 1].seq < heap[child].seq) child++;
				if (heap[child].seq >= last.seq) break;
				heap[at] = heap[child];
				at = child;
			}
			heap[at] = last;
		}
		first.inHeap = false;
		return first;
	}

	queueSubscriber(binding) {
		if (binding.queued || binding.failed || binding.evaluating) return;
		if (binding.owner.$destroyed || binding.owner.$bindings === null) return;
		binding.queued = true;
		this.subQueue.push(binding);
	}

	drainSubscribers() {
		// Re-entrant: a binding may reload a Loader (or otherwise run Component.onCompleted) while an
		// outer drain is active. Writes inside those handlers must still flush their dependent
		// bindings before the next script line — Qt does, and TablePage relies on it
		// (`compl = true` then `tableDecorator = decorModel` after `onComplComplChanged` / setDefault).
		// Skipping the nested flush deferred updates until after the whole completion batch, so
		// setDecorators saw an empty model and left widthDecorator null.
		const nested = this.draining;
		this.draining = true;
		let steps = 0;
		try {
			while (this.subQueue.length > 0 && steps < 10000) {
				steps++;
				this.evaluateOne(this.subQueue.shift(), false);
			}
		} finally {
			if (!nested) {
				for (const binding of this.subQueue) binding.queued = false;
				this.subQueue.length = 0;
				this.draining = false;
			}
		}
	}

	// Runs one binding and remembers what it read. `silent` retries a failure on a later sweep.
	evaluateOne(binding, silent) {
		binding.queued = false;
		if (binding.failed || binding.evaluating || binding.owner.$destroyed || binding.owner.$bindings === null) return;
		binding.evaluating = true;
		binding.volatileNow = false;
		binding.why = null;
		const run = ++binding.runs;
		const previous = hooks.reading;
		hooks.reading = binding;
		let value;
		let thrown = false;
		try {
			value = binding.fn.call(binding.owner);
		} catch (error) {
			thrown = true;
			if (!silent) this.failBinding(binding, error);
		} finally {
			hooks.reading = previous;
			binding.evaluating = false;
			this.endTracking(binding);
		}
		// A binding that failed is retried on every pass: what it needs may not be announced.
		if (thrown && binding.why === null) binding.why = 'thrown';
		this.setVolatile(binding, !binding.failed && (thrown || binding.volatileNow));
		if (thrown || binding.failed) return;
		if (binding.hasLast && (sameValue(value, binding.last) || (binding.snap !== undefined && sameValue(value, binding.snap)))) return;
		const audited = this.auditing !== null ? { binding, last: binding.last, value } : null;
		if (audited !== null) this.auditing.push(audited);
		// A value the property refuses must not abort the whole document. The binding is retried like any other failure.
		binding.writing++;
		try {
			this.assign(binding.owner, binding.path, value);
		} catch (error) {
			if (!silent) this.failBinding(binding, error);
			this.setVolatile(binding, !binding.failed);
			if (audited !== null) this.auditing.splice(this.auditing.indexOf(audited), 1);
			return;
		} finally {
			binding.writing--;
		}
		// The write may have run what the binding reads again (a reader of the property it wrote, a handler); that run
		// computed from newer values and has recorded its own result.
		if (binding.runs !== run) return;
		binding.last = value;
		binding.snap = snapshot(value);
		binding.hasLast = true;
	}

	failBinding(binding, error) {
		if (this.lenient && ++binding.failures < 30) return;
		if (this.lenient && error instanceof TypeError && /Cannot read propert(y|ies) of (null|undefined)/.test(error.message)) {
			binding.failed = true;
			this.unsubscribe(binding);
			this.volatile.delete(binding);
			console.warn(`binding '${binding.key}'${binding.file ? ` (${binding.file})` : ''}: ${error.message}`);
			return;
		}
		binding.failed = true;
		this.unsubscribe(binding);
		this.volatile.delete(binding);
		this.report(`binding '${binding.key}'${binding.file ? ` (${binding.file})` : ''}`, error, error instanceof QmlError && error.line ? error.line : binding.line);
	}

	// Evaluates the bindings created since the last evaluation, so that Component.onCompleted handlers see the
	// values of `property var list: []` and the like. Failures are retried silently by the normal passes.
	evaluateFresh() {
		if (this.fresh.length === 0) return;
		const list = this.fresh;
		this.fresh = [];
		this.trackingPass++;
		try {
			for (let pass = 0; pass < 2; pass++) {
				for (const binding of list) this.evaluateOne(binding, true);
			}
		} finally {
			this.trackingPass--;
		}
		if (this.connections.size > 0) this.syncConnections();
	}

	// (Re)creates the delegates of every Repeater whose model changed.
	updateRepeaters() {
		for (const repeater of [...this.repeaters]) {
			if (repeater.$destroyed || repeater.$parent === null) {
				this.repeaters.delete(repeater);
				continue;
			}
			const model = repeater.$model;
			const version = modelVersion(model);
			const count = modelCount(model);
			if (model === repeater.$lastModel && version === repeater.$lastVersion && count === repeater.$lastCount) {
				if (repeater.$lastRows === undefined) this.rememberRepeaterRows(repeater);
				continue;
			}
			repeater.$lastModel = model;
			repeater.$lastVersion = version;
			repeater.$lastCount = count;
			this.rebuildRepeater(repeater, count);
			this.rememberRepeaterRows(repeater);
		}
	}

	// Rows the repeater has already built, so an append during onCompleted adds delegates instead of recreating them.
	rememberRepeaterRows(repeater) {
		const model = repeater.$model;
		repeater.$lastRows = model instanceof ListModel ? model.$rows.slice() : null;
	}

	sameRowPrefix(repeater, model) {
		if (!(model instanceof ListModel)) return false;
		const prev = repeater.$lastRows;
		if (prev === null || prev === undefined) return false;
		const rows = model.$rows;
		if (prev.length > rows.length) return false;
		for (let i = 0; i < prev.length; i++) if (prev[i] !== rows[i]) return false;
		return true;
	}

	// `repeater.delegate = component` inside a script has to rebuild before the next line.
	syncRepeaterNow(repeater) {
		if (repeater.$destroyed || repeater.$growing || repeater.$parent === null) return;
		const count = modelCount(repeater.$model);
		repeater.$lastModel = repeater.$model;
		repeater.$lastVersion = modelVersion(repeater.$model);
		repeater.$lastCount = count;
		this.rebuildRepeater(repeater, count);
		this.rememberRepeaterRows(repeater);
	}

	// ListModel.append must create the new delegates before the next line of the script (Qt):
	// CustomTooltip.openDialog then hides `backgroundItem` that DialogManagerView.onLoaded just set.
	syncModelNow(model) {
		// O(1): Table heightModel and other unwatched ListModels append thousands of times.
		if (!(model instanceof ListModel) || model.$syncWatchers === 0) return;

		let changed = false;
		for (const repeater of [...this.repeaters]) {
			if (repeater.$destroyed || repeater.$growing || repeater.$parent === null || repeater.$model !== model) continue;
			const count = modelCount(model);
			if (repeater.$lastModel !== model || !this.sameRowPrefix(repeater, model)) {
				repeater.$lastModel = model;
				repeater.$lastVersion = modelVersion(model);
				repeater.$lastCount = count;
				this.rebuildRepeater(repeater, count);
				this.rememberRepeaterRows(repeater);
				changed = true;
				continue;
			}
			if (count > repeater.$built.length) {
				this.growRepeater(repeater, count);
				changed = true;
			} else if (count < repeater.$built.length) {
				repeater.$lastModel = model;
				repeater.$lastVersion = modelVersion(model);
				repeater.$lastCount = count;
				this.rebuildRepeater(repeater, count);
				this.rememberRepeaterRows(repeater);
				changed = true;
			}
		}
		// Delegates without onCompleted leave Loader.sourceComponent in `fresh`; apply them now so
		// onLoaded (and backgroundItem) exist before the append caller continues.
		if (changed) this.evaluateFresh();
	}

	repeaterDelegate(repeater) {
		let delegate = repeater.$delegate;
		if (delegate === null && repeater.delegate instanceof ComponentItem && repeater.delegate.$def !== null && repeater.delegate.$def.children.length === 1) {
			delegate = { def: repeater.delegate.$def.children[0], scope: repeater.delegate.$defScope, depth: 0 };
		}
		return delegate;
	}

	createRepeaterDelegate(repeater, delegate, parent, index) {
		const scope = new Scope(delegate.scope, delegate.scope.file);
		this.applyModelData(scope, repeater.$model, index);
		// Every item created below the delegate with the row's index carries its own `index` property: it is kept to give
		// the delegate another index when rows are inserted or removed before it.
		const indexed = [];
		const outer = this.indexTrack;
		this.indexTrack = indexed;
		try {
			const item = this.createItem(delegate.def, scope, parent, [], index, delegate.depth);
			if (item !== null) {
				item.$delegateScope = scope;
				item.$delegateIndexed = indexed;
			}
			return item;
		} catch (error) {
			this.report('Repeater delegate', error, error instanceof QmlError ? error.line : 0);
			return null;
		} finally {
			this.indexTrack = outer;
		}
	}

	// The delegates whose row is still in the model, with the new index of the row: Map(index -> delegate). Like in Qt a
	// delegate stays with its row when other rows are inserted or removed (the dialog below a popup that closes must not be
	// made again). Empty if the model has no rows that can be told apart.
	repeaterDelegatesThatStay(repeater) {
		const stay = new Map();
		const model = repeater.$model;
		const before = repeater.$lastRows;
		if (!(model instanceof ListModel) || before === null || before === undefined) return stay;
		const position = new Map();
		const rows = model.$rows;
		for (let i = 0; i < rows.length; i++) position.set(rows[i], i);
		repeater.$built.forEach((item, index) => {
			if (item.$delegateScope === undefined) return;
			const now = position.get(before[index]);
			if (now !== undefined && !stay.has(now)) stay.set(now, item);
		});
		return stay;
	}

	// Points a delegate at another row (the same row at another index).
	rebindRepeaterDelegate(repeater, item, index) {
		const scope = item.$delegateScope;
		if (scope.data !== null && scope.data.index === index) return;
		this.applyModelData(scope, repeater.$model, index);
		for (const indexed of item.$delegateIndexed) {
			if (indexed.$custom === null || indexed.$custom.index === index) continue;
			indexed.$custom.index = index;
			this.markReaders(indexed, 'index');
			if (indexed.$chg !== undefined) this.fireChanged(indexed, 'index');
		}
		this.refreshDelegateBindings(item);
	}

	// Puts freshly built delegates just after the repeater (or after the delegates already there).
	placeRepeaterItems(repeater, parent, built) {
		const siblings = parent.$children;
		for (const item of built) {
			const at = siblings.lastIndexOf(item);
			if (at >= 0) siblings.splice(at, 1);
		}
		const anchor = repeater.$built.length > 0 ? repeater.$built[repeater.$built.length - 1] : repeater;
		siblings.splice(siblings.indexOf(anchor) + 1, 0, ...built);
	}

	rebuildRepeater(repeater, count) {
		const parent = repeater.$parent;
		if (repeater.$growing) return;
		repeater.$growing = true;
		try {
			const delegate = this.repeaterDelegate(repeater);
			const stay = delegate === null ? new Map() : this.repeaterDelegatesThatStay(repeater);
			const kept = new Set(stay.values());
			for (const old of repeater.$built) {
				if (kept.has(old)) continue;
				this.releaseSubtree(old);
				parent.$removeChild(old);
			}
			repeater.$built = [];

			if (delegate === null) return;
			if (count > MAX_REPEATER_COUNT) count = MAX_REPEATER_COUNT;

			const outer = this.indexTrack;
			this.indexTrack = null; // the repeater's items have their own `index`
			// Each delegate is its own component: complete it before creating the next, in index order.
			for (let i = 0; i < count; i++) {
				if (stay.has(i)) {
					const item = stay.get(i);
					this.rebindRepeaterDelegate(repeater, item, i);
					this.placeRepeaterItems(repeater, parent, [item]);
					repeater.$built.push(item);
					continue;
				}
				const mark = this.completed.length;
				const item = this.createRepeaterDelegate(repeater, delegate, parent, i);
				if (item === null) break;
				this.placeRepeaterItems(repeater, parent, [item]);
				repeater.$built.push(item);
				this.runCompletedFrom(mark);
			}
			this.indexTrack = outer;
			parent.$update();
		} finally {
			repeater.$growing = false;
		}
		// Role bindings (`model.name`) must be applied before the script that changed the model continues.
		if (this.completing > 0) this.evaluateFresh();
	}

	// Appends delegates for the new tail of a model without recreating the ones already completed.
	growRepeater(repeater, count) {
		if (repeater.$growing || repeater.$parent === null) return;
		const delegate = this.repeaterDelegate(repeater);
		if (delegate === null) return;
		repeater.$growing = true;
		const parent = repeater.$parent;
		try {
			if (count > MAX_REPEATER_COUNT) count = MAX_REPEATER_COUNT;
			while (repeater.$built.length < count) {
				const index = repeater.$built.length;
				const mark = this.completed.length;
				const outer = this.indexTrack;
				this.indexTrack = null;
				const item = this.createRepeaterDelegate(repeater, delegate, parent, index);
				this.indexTrack = outer;
				if (item === null) break;
				this.placeRepeaterItems(repeater, parent, [item]);
				repeater.$built.push(item);
				parent.$update();
				this.runCompletedFrom(mark);
				count = Math.min(modelCount(repeater.$model), MAX_REPEATER_COUNT);
			}
			repeater.$lastModel = repeater.$model;
			repeater.$lastVersion = modelVersion(repeater.$model);
			repeater.$lastCount = modelCount(repeater.$model);
			this.rememberRepeaterRows(repeater);
		} finally {
			repeater.$growing = false;
		}
		if (this.completing > 0) this.evaluateFresh();
	}

	// `index`, `modelData`, `model` and the roles of row `index` as variables of a delegate scope.
	applyModelData(scope, model, index) {
		const fresh = scope.data === null;
		scope.data = modelScopeData(model, index);
		if (!fresh) {
			// Reused delegate: the variables read the new row through `scope.data`; only new roles need a variable.
			for (const key of Object.keys(scope.data.roles)) {
				if (Object.getOwnPropertyDescriptor(scope.context, key) === undefined) this.defineRole(scope, key);
			}
			return;
		}
		for (const key of Object.keys(scope.data.roles)) this.defineRole(scope, key);
		Object.defineProperty(scope.context, 'model', { get: () => (readRows(scope.data.source), scope.data.model), set: (v) => (scope.data.model = v), enumerable: true, configurable: true });
		Object.defineProperty(scope.context, 'modelData', { get: () => (readRows(scope.data.source), scope.data.modelData), set: (v) => (scope.data.modelData = v), enumerable: true, configurable: true });
	}

	defineRole(scope, key) {
		Object.defineProperty(scope.context, key, { get: () => (readRows(scope.data.source), scope.data.roles[key]), enumerable: true, configurable: true });
	}

	// Evaluates every binding once and assigns the values that changed.
	evaluateBindings() {
		if (this.loaders.size > 0) this.updateLoaders();
		if (this.repeaters.size > 0) this.updateRepeaters();
		this.runBindingPass();
		this.fresh = [];
		if (this.repeaters.size > 0) this.updateRepeaters();
		if (this.qmlBindings.size > 0) {
			// A Binding item reads values that the pass above may have computed in the wrong order: settle first.
			for (let n = 0; n < 2 && this.bindingItemsPending(); n++) this.runBindingPass();
			// Binding may have overwritten a bound width/height with a pre-layout size. Restore from the
			// target's own binding (two passes: decorator size, then control/Binding.value), then sync
			// again so Binding wins for an explicit control size (`width: 300`) and publishes the
			// restored content size for auto-sized controls. Nothing to restore if no Binding wrote.
			if (this.syncBindingItems()) {
				this.runBindingPass();
				this.runBindingPass();
				// Publish restored sizes through the remaining readers before a Binding can stamp
				// its stale value back onto the target and erase that size notification.
				for (const binding of this.dirty) this.queueSubscriber(binding);
				this.dirty.clear();
				this.drainSubscribers();
				this.syncBindingItems();
			}
			this.retry.clear();
		}
		if (this.connections.size > 0) this.syncConnections();
		this.flushImages();
		this.runChangeHandlers();
	}

	runBindingPass() {
		this.trackingPass++;
		try {
			if (this.incremental) {
				// After a full sweep (or at the start) nothing says what changed: every binding has to run.
				if (!this.primed) {
					this.primed = true;
					for (const binding of this.bindings) this.dirty.add(binding);
				}
				this.runIncrementalPass();
			} else {
				this.primed = false;
				for (const binding of this.bindings) this.evaluateOne(binding, false);
			}
		} finally {
			this.trackingPass--;
		}
	}

	// Runs the marked bindings and the volatile ones in creation order, as a full sweep would reach them. A binding
	// marked by a write while the pass is running runs in this pass if its turn has not come yet, else in the next one.
	runIncrementalPass() {
		const outer = this.inPass;
		this.inPass = true;
		try {
			for (const binding of this.dirty) if (!binding.inHeap) this.pushPending(binding);
			this.dirty.clear();
			for (const binding of this.retry) if (!binding.inHeap && !binding.failed) this.pushPending(binding);
			this.retry.clear();
			for (const binding of this.volatile) if (!binding.inHeap) this.pushPending(binding);
			while (this.passHeap.length > 0) {
				const binding = this.popPending();
				this.cursor = binding.seq;
				this.evaluateOne(binding, false);
			}
		} finally {
			this.inPass = outer;
			this.cursor = 0;
			for (const binding of this.passHeap) {
				binding.inHeap = false;
				this.dirty.add(binding);
			}
			this.passHeap.length = 0;
		}
	}

	// Checks the incremental evaluation: runs every binding once. After a frame has settled nothing should change; a
	// binding that does was missed, and `auditMisses` tells what it reads and where it is declared. A binding a Binding
	// item took over is left alone (applying it would fight the Binding item), and so is one that reads nothing: it
	// runs once, as in Qt (`UuidGenerator.generateUUID()` would give a new value every time).
	auditSweep() {
		const changed = [];
		this.auditing = changed;
		this.trackingPass++;
		try {
			for (const binding of this.bindings) {
				if (binding.overridden === true || (binding.dItems === null && !binding.volatile)) continue;
				this.evaluateOne(binding, false);
			}
		} finally {
			this.trackingPass--;
			this.auditing = null;
		}
		let missed = 0;
		for (const { binding, last, value } of changed) {
			missed++;
			const reads = (binding.dItems || []).map((item, i) => `${item.$typeName}.${binding.dNames[i]}`);
			const key = `${binding.owner.$typeName}.${binding.key} ${binding.file}:${binding.line}`;
			const entry = this.auditMisses.get(key) || { count: 0, volatile: binding.volatile, reads: [...new Set(reads)].slice(0, 6), last: String(last).slice(0, 40), value: String(value).slice(0, 40) };
			entry.count++;
			this.auditMisses.set(key, entry);
		}
		return missed;
	}

	runChangeHandlers() {
		// Qt reports every width change before every height change when a layout sets both.
		const heights = [];
		for (const handler of this.changeHandlers) {
			if (handler.property === 'height') heights.push(handler);
			else this.runChangeHandler(handler);
		}
		for (const handler of heights) this.runChangeHandler(handler);
	}

	runChangeHandler(handler) {
		if (handler.failed) return;
		const value = handler.item[handler.property];
		if (!handler.hasLast) {
			handler.hasLast = true;
			handler.last = value;
			// An object (a Component, a model...) bound from the outside is a change from the null default.
			if (typeof value === 'object' && value !== null) this.runNotified(handler);
		} else if (value !== handler.last && (value === value || handler.last === handler.last)) {
			handler.last = value;
			this.runNotified(handler);
		}
	}

	// (Re)creates the content of every Loader whose source changed.
	updateLoaders() {
		for (const loader of [...this.loaders]) this.updateLoader(loader);
	}

	// What the loader should show now. An inactive loader shows nothing; the last assigned source wins.
	loaderKey(loader) {
		if (!loader.$active) return '';
		if (loader.$fromComponent) return loader.$sourceComponent || '';
		return loader.$source || '';
	}

	// One Loader. The loaded item's onCompleted runs before itemChanged, statusChanged and loaded.
	updateLoader(loader) {
		if (loader.$destroyed) {
			this.loaders.delete(loader);
			return;
		}
		if (loader.$inLoad) return;
		const key = this.loaderKey(loader);
		if (key === loader.$wanted) return;
		// A Component made by Qt.createComponent that is still loading: the Loader follows when it settles.
		if (key instanceof ComponentItem && key.$status === 2) {
			loader.$wanted = key;
			key.$connect('statusChanged', () => {
				if (loader.$destroyed || this.loaderKey(loader) !== key) return;
				loader.$wanted = '';
				this.updateLoader(loader);
			});
			return;
		}
		loader.$inLoad = true;
		loader.$wanted = key;
		let ok = false;
		try {
			for (const child of [...loader.$children]) {
				this.releaseSubtree(child);
				loader.$removeChild(child);
			}
			loader.$failed = false;
			if (key !== '' && key !== null) {
				const mark = this.completed.length;
				this.guard(() => {
					let created = null;
					if (typeof key === 'string') {
						const path = resolvePath(key, loader.$directory || '');
						let doc = this.project === null ? undefined : this.project.docs.get(path);
						// `qrc:/qml/Dir/File.qml`: the resource prefix is not a directory of the project.
						if (doc === undefined && this.project !== null && key.startsWith('qrc:')) doc = this.findDocument(key) || undefined;
						if (doc === undefined || doc === null || doc.def === null) throw new QmlError(`Cannot load '${key}': file not found or not part of the project`);
						// Like the JQML runtime, a loaded page sees the ids of the context that loads it.
						created = this.createItem(doc.def, this.newScope(this.lenient ? loader.$scope : null, doc.path), loader, [], undefined, 0);
						if (created !== null && loader.$initialProps) this.applyInitialProps(created, loader.$initialProps);
						this.runCompletedFrom(mark);
					} else if (key instanceof ComponentItem) {
						if (key.$status !== 1) throw new QmlError(`Cannot load sourceComponent '${key.$url}': ${key.errorString() || 'the component is not ready'}`);
						created = this.instantiateComponent(key, loader, loader.$initialProps || undefined);
					} else {
						throw new QmlError('Cannot load sourceComponent');
					}
					ok = created !== null && created !== undefined;
				}, `Loader '${typeof key === 'string' ? key : 'sourceComponent'}'`, 0);
				if (!ok) {
					loader.$failed = true;
					for (const child of [...loader.$children]) {
						this.releaseSubtree(child);
						loader.$removeChild(child);
					}
				} else {
					// A loaded item with no onCompleted still has bindings (anchors.fill: parent) that must run before geometry is read.
					this.evaluateFresh();
					this.layoutContaining(loader);
					loader.$pushSize();
					this.layoutContaining(loader);
				}
			}
		} finally {
			loader.$inLoad = false;
		}
		loader.$emit('itemChanged');
		this.emitLoaderStatus(loader);
		if (ok) loader.$emit('loaded');
		if (this.loaderKey(loader) !== loader.$wanted) this.updateLoader(loader);
	}

	applyInitialProps(item, props) {
		for (const key of Object.keys(props)) this.assign(item, key.split('.'), props[key]);
	}

	emitLoaderStatus(loader) {
		const next = loader.status;
		if (next === loader.$statusSent) return;
		loader.$statusSent = next;
		loader.$emit('statusChanged');
	}

	// Anchors and implicit size are resolved from the root, so a Loader sees its real geometry before `loaded`.
	layoutContaining(item) {
		let top = item;
		while (top.$parent !== null) top = top.$parent;
		top.$layout();
	}

	// `Component.createObject(parent)`: instantiates the single root of a component definition.
	instantiateComponent(component, parent, props) {
		const def = component.$def;
		if (def === null || def.children.length !== 1) throw new QmlError('A Component must have exactly one root item');
		const scope = new Scope(component.$defScope, component.$defScope.file);
		const mark = this.completed.length;
		const item = this.createItem(def.children[0], scope, parent || null, [], undefined, 0);
		if (item !== null && props !== undefined && props !== null) {
			for (const key of Object.keys(props)) this.guard(() => this.assign(item, key.split('.'), props[key]), `property '${key}'`, 0);
		}
		this.runCompletedFrom(mark);
		return item;
	}

	assign(item, path, value) {
		let target = item;
		for (let i = 0; i < path.length - 1; i++) {
			const next = target[path[i]];
			if (next === undefined || next === null || typeof next !== 'object' || !('$owner' in next || '$typeName' in next)) {
				// `icon.width: 18` where `icon` is bound by a base type (`icon: buttonIcon`): written once the object exists.
				const bound = item.$bindings !== null && item.$bindings.some((b) => b.key === path[0]);
				const aliased = item.$aliases !== undefined && item.$aliases.has(path[0]);
				if (this.lenient && i === 0 && item.$constructing && (next === undefined || next === null) && (bound || aliased)) {
					if (item.$pendingGroupWrites === undefined) item.$pendingGroupWrites = [];
					item.$pendingGroupWrites.push([path, value]);
					return;
				}
				if (this.lenient) {
					this.noteIgnored(item, path);
					return;
				}
				throw new QmlError(`Cannot assign to non-existent property '${path.join('.')}' of ${item.$typeName}`);
			}
			target = next;
		}
		const name = path[path.length - 1];
		if (!isAssignable(target, name)) {
			if (this.lenient) {
				if (target.$open && path.length === 1 && !name.startsWith('$')) {
					target.$defineCustom(name, undefined);
					hooks.engineWrite++;
					try {
						target[name] = value;
					} finally {
						hooks.engineWrite--;
					}
				} else {
					this.noteIgnored(item, path);
				}
				return;
			}
			throw new QmlError(`Cannot assign to non-existent property '${path.join('.')}' of ${item.$typeName}`);
		}
		hooks.engineWrite++;
		try {
			target[name] = value;
		} finally {
			hooks.engineWrite--;
		}
		this.markBoundPropertyWritten(target, name, value);
	}

	noteIgnored(item, path) {
		const key = `${item.$typeName}.${path.join('.')}`;
		this.ignoredProperties.set(key, (this.ignoredProperties.get(key) || 0) + 1);
	}

	// ----- keyboard -----

	// Delivers a key event (a DOM KeyboardEvent or alike) to the item with the active focus and up its parents. Each item
	// offers it to its `Keys` handlers, then to its own key handling (an editor), then to its `KeyNavigation`.
	// Returns true if the event was accepted.
	dispatchKey(domEvent, pressed) {
		const event = qtKeyEvent(domEvent);
		const shift = (event.modifiers & MODIFIERS.ShiftModifier) !== 0;
		for (let item = getFocusItem(); item !== null && item !== undefined; item = item.$parent) {
			const keys = item.$keys;
			if (keys !== undefined) {
				const named = pressed ? keys.byKey.get(event.key) || [] : [];
				const list = (pressed ? keys.pressed : keys.released).map((fn) => ({ fn, auto: false })).concat(named.map((fn) => ({ fn, auto: true })));
				for (const { fn, auto } of list) {
					event.accepted = false;
					this.guard(() => fn.call(item, event), `Keys.on${pressed ? 'Pressed' : 'Released'} (${item.$scope === null ? '' : item.$scope.file})`, 0);
					// `Keys.onReturnPressed` and the like accept the event on their own.
					if (event.accepted || auto) return true;
				}
			}
			if (!pressed) continue;
			if (item.$keyPressed !== undefined && this.guard(() => item.$keyPressed(event), `key ${item.$typeName}`, 0) === true) return true;
			const target = keyNavigationTarget(item, event.key, shift);
			if (target !== null) {
				target.forceActiveFocus();
				return true;
			}
		}
		const focus = getFocusItem() || this.host;
		if (pressed && event.key === KEY.Key_Tab && focus !== null && focus !== undefined) {
			const next = focus.nextItemInFocusChain(!shift);
			if (next !== null && next !== focus) {
				next.forceActiveFocus();
				return true;
			}
		}
		return false;
	}

	// ----- time and timers -----

	setTime(seconds) {
		this.time = seconds;
		this.flushQuiet(seconds);
		if (this.animations.size > 0) {
			for (const animation of [...this.animations]) {
				// An animation inside a hidden subtree that does not animate anything visible (an indicator on
				// another page) must not repaint the whole scene each time it finishes and starts again.
				const quiet = animation.visible === false && !animation.$isVisuallyActive();
				if (quiet) hooks.quiet++;
				try {
					animation.$tick(seconds);
				} finally {
					if (quiet) hooks.quiet--;
				}
			}
		}
		if (this.carets.size > 0) {
			for (const item of [...this.carets]) {
				if (item.$destroyed === true || !item.$active) this.carets.delete(item);
				else item.$blink(seconds);
			}
		}
		if (this.timers.size === 0) return;
		for (const timer of [...this.timers]) {
			if (!timer.$running || this.time < timer.$deadline) continue;
			const quiet = timer.visible === false;
			if (quiet) hooks.quiet++;
			try {
				if (timer.$repeat) timer.$deadline = this.time + timer.$interval / 1000;
				else timer.$running = false;
				timer.$emit('triggered');
				// A running timer does not keep the scene repainting; a tick that fired asks for one frame.
				timer.$update();
			} finally {
				if (quiet) hooks.quiet--;
			}
		}
	}

	// What hidden animations and timers changed is applied with one frame per QUIET_FLUSH_SECONDS: their handlers may
	// still feed a binding of a visible item, so the scene must not stay stale, but it does not need a frame per tick.
	flushQuiet(seconds) {
		if (!hooks.quietDirty) {
			this.quietSince = null;
			return;
		}
		if (this.quietSince === null) this.quietSince = seconds;
		if (seconds - this.quietSince < QUIET_FLUSH_SECONDS) return;
		hooks.quietDirty = false;
		this.quietSince = null;
		if (this.host !== null) this.host.$update();
	}

	// ----- completion handlers -----

	runCompletedFrom(mark) {
		if (this.completed.length <= mark) {
			// Construction is finished even when the root has no onCompleted. The window is active
			// for everything that runs afterwards (timers). Nested completion stays inactive.
			if (this.completing === 0) setSceneActive(true);
			return;
		}
		this.evaluateFresh();
		// Handlers are queued as each item finishes, children first, and a component's own handlers
		// from the base type toward the most derived. Qt runs items in reverse (the parent before its
		// children, a later sibling before an earlier one) and keeps one item's handlers in that base-first order.
		const batch = this.completed.splice(mark);
		const ordered = [];
		for (let i = batch.length - 1; i >= 0; ) {
			const item = batch[i].item;
			let start = i;
			while (start > 0 && batch[start - 1].item === item) start--;
			for (let j = start; j <= i; j++) ordered.push(batch[j]);
			i = start - 1;
		}
		this.completing++;
		// A Loader can complete inside a binding pass. Its handlers are scripts: dependent
		// bindings must update synchronously, and assignments must replace existing bindings.
		const trackingPass = this.trackingPass;
		const engineWrite = hooks.engineWrite;
		const reading = hooks.reading;
		this.trackingPass = 0;
		hooks.engineWrite = 0;
		hooks.reading = null;
		try {
			for (const entry of ordered) {
				this.guard(() => entry.fn.call(entry.item), `handler 'Component.onCompleted' (${entry.file})`, entry.line);
			}
		} finally {
			this.trackingPass = trackingPass;
			hooks.engineWrite = engineWrite;
			hooks.reading = reading;
			this.completing--;
			// The offscreen window becomes active once the first completion batch has finished.
			// Handlers that ran inside it still saw activeFocus as false.
			if (this.completing === 0) setSceneActive(true);
		}
		while (this.completing === 0 && this.deferred.length > 0) {
			for (const [item, name, args] of this.deferred.splice(0)) this.guard(() => item.$emit(name, args), `signal '${name}'`, 0);
		}
	}

	guard(fn, where, line) {
		try {
			return fn();
		} catch (error) {
			this.report(where, error, error instanceof QmlError && error.line ? error.line : line);
			return undefined;
		}
	}

	// ----- instantiation -----

	// `Qt`: a value a use site assigns to a property of the component runs that file's `onXChanged` handlers.
	fireOverlayChanges(item, writes) {
		for (const { name, old } of writes) {
			const list = item.$chg === undefined ? undefined : item.$chg.get(name);
			if (list === undefined) continue;
			let value;
			try {
				value = item[name];
			} catch {
				continue;
			}
			if (sameValue(old, value)) continue;
			for (const handler of list.slice()) {
				if (handler.failed) continue;
				handler.last = value;
				handler.hasLast = true;
				this.runNotified(handler);
			}
		}
	}

	// Creates the item of an object definition below `parent`. `overlays` are the definitions (use
	// sites of a component) that are applied on top of `def`.
	createItem(def, scope, parent, overlays, index, depth) {
		const type = def.type;
		if (this.lenient && IGNORED_TYPES.has(type)) return null;
		let item;
		const builtin = BUILTIN_TYPES.get(type);
		if (this.lenient && STACK_TYPES.has(type)) {
			item = new Stack(type);
		} else if (this.lenient && type === 'Loader') {
			item = new Loader();
			this.loaders.add(item);
		} else if (this.lenient && type === 'TreeItemModel') {
			item = new TreeItemModel();
		} else if (this.lenient && type === 'ListModel') {
			item = new ListModel();
		} else if (this.lenient && (type === 'Repeater' || type === 'MapItemView')) {
			item = type === 'MapItemView' ? new MapItemView() : new Repeater();
			this.repeaters.add(item);
		} else if (this.lenient && ANIMATION_TYPES.has(type)) {
			item = new Animation(this, type);
		} else if (this.lenient && type === 'ListElement') {
			return this.createListElement(def, scope, parent, overlays);
		} else if (this.lenient && type === 'FocusScope') {
			item = new Item();
			item.$typeName = type;
		} else if (this.lenient && SILENT_TYPES.has(type)) {
			item = new Placeholder(type);
		} else if (builtin !== undefined) {
			item = type === 'Timer' ? new Timer(this) : new builtin();
		} else if (type === 'Component' && this.lenient) {
			item = new ComponentItem();
			item.$def = def;
			item.$defScope = scope;
		} else {
			if (depth >= MAX_COMPONENT_DEPTH) {
				throw new QmlError(`Components are nested too deeply; does '${type}' contain itself?`, def.line, def.col);
			}
			const doc = type === 'Repeater' ? null : this.project.resolveType(type, scope.file);
			if (doc === null && !this.lenient) throw new QmlError(`Unknown type '${type}'`, def.line, def.col);
			if (doc !== null) {
				if (doc.error) throw new QmlError(`${doc.path}: ${doc.error.message}`, doc.error.line, doc.error.column);
				// Like a QML delegate context: `model`, `modelData` and the roles are visible inside the document.
				let modelScope = null;
				if (this.lenient) {
					for (let s = scope; s !== null; s = s.parent) {
						if (Object.getOwnPropertyDescriptor(s.context, 'modelData') !== undefined) {
							modelScope = s;
							break;
						}
					}
				}
				const componentScope = this.newScope(modelScope, doc.path);
				return this.createItem(doc.def, componentScope, parent, [{ def, scope }, ...overlays], index, depth + 1);
			}
			this.unknownTypes.set(type, (this.unknownTypes.get(type) || 0) + 1);
			item = this.lenient ? createSdlObject(type) : new Placeholder(type);
		}
		if (this.lenient) item.$open = true;
		item.$scope = scope;
		const chain = [{ def, scope }, ...overlays];
		item.$constructing = true;
		for (const link of chain) {
			if (link.scope.root === null) link.scope.root = item;
			if (link.def.id) link.scope.ids[link.def.id] = item;
		}
		if (item instanceof ComponentItem) {
			item.$constructing = false;
			return item;
		}
		if (parent !== null) parent.$adopt(item);
		if (parent === null || type === 'FocusScope' || type === 'Window') item.$isFocusScope = true;
		if (index !== undefined) {
			item.$defineCustom('index', index);
			if (this.indexTrack !== null) this.indexTrack.push(item);
		}
		if (type === 'Timer') this.timers.add(item);
		if (this.lenient && type === 'Connections') {
			item.$conn = { handlers: [], target: undefined, links: [] };
			this.connections.add(item);
		}
		if (this.lenient && type === 'Binding') {
			item.$bind = { target: undefined, property: undefined, value: undefined, has: false };
			this.qmlBindings.add(item);
		}
		for (const link of chain) this.defineEnums(item, link.def);
		for (const link of chain) {
			for (const signal of link.def.signalDefs) item.$declareSignal(signal.name, signal.params);
		}
		for (const link of chain) {
			for (const fn of link.def.functions) this.defineFunction(item, fn, link.scope);
			// `function onFoo(args) {}` in a Connections item is a handler of the target's signal `foo`.
			if (item.$conn !== undefined) {
				for (const fn of link.def.functions) {
					if (/^on_*[A-Z]/.test(fn.name)) item.$conn.handlers.push({ signal: decapitalize(fn.name.slice(2)), name: fn.name, fn: item[fn.name] });
				}
			}
		}

		const completedHandlers = [];
		// What the use sites of a component assign to its properties: the base file's onXChanged handlers run for it (Qt).
		const overlayWrites = [];
		for (let l = 0; l < chain.length; l++) {
			const link = chain[l];
			item.$directory = dirname(link.scope.file);
			for (const prop of link.def.properties) this.applyProperty(item, prop, link.scope, completedHandlers, l > 0 ? overlayWrites : null);
		}

		queueDeclarativeFocus(item);
		let delegateDef = null;
		if (item instanceof Repeater) {
			try {
				const found = this.findDelegate(chain, type);
				delegateDef = found;
				item.$delegate = { def: found.def, scope: found.scope, depth };
			} catch (error) {
				// the delegate is assigned by a script: the repeater stays empty unless `delegate` is a Component
			}
		}
		if (item instanceof ListView) {
			// A delegate assigned from a script (or a model that is not a number) is not supported: the list stays empty.
			try {
				delegateDef = this.findDelegate(chain, type);
			} catch (error) {
				if (!this.lenient) throw error;
			}
			if (delegateDef !== null) this.setupListView(item, delegateDef.def, delegateDef.scope, depth);
		}
		// A `default property` declared on a type applies to children written at the use site, not to the
		// objects written in the same file (those follow the base type's default property).
		let defaultProperty = '';
		for (let l = 0; l < chain.length; l++) {
			const link = chain[l];
			const lists = new Map();
			for (const child of link.def.children) {
				if (delegateDef !== null && child === delegateDef.def) continue;
				if (this.lenient && this.boundByOverlay(child, chain, l)) continue;
				if (this.lenient) {
					this.createChildLenient(child, link, item, index, depth, lists, chain, defaultProperty, l > 0 ? overlayWrites : null);
				} else if (child.type === 'Repeater') {
					this.createRepeater(child, item, link.scope, index, depth);
				} else {
					this.createItem(child, link.scope, item, [], index, depth);
				}
			}
			for (const [name, list] of lists) this.guard(() => this.assign(item, [name], list), `property '${name}'`, link.def.line);
			for (const prop of link.def.properties) {
				if (prop.isDefault) defaultProperty = prop.name;
			}
		}
		if (item instanceof ListModel) item.$collect();

		item.$constructing = false;
		this.fireOverlayChanges(item, overlayWrites);
		if (parent === null || parent.$constructing !== true) flushDeclarativeFocus();
		if (item.$pendingAliasWrites !== null) {
			const writes = item.$pendingAliasWrites;
			item.$pendingAliasWrites = null;
			for (const [name, value] of writes) item[name] = value;
		}
		if (item.$pendingGroupDrops !== undefined) {
			const drops = item.$pendingGroupDrops;
			item.$pendingGroupDrops = undefined;
			for (const path of drops) this.dropGroupBinding(item, path);
		}
		if (item.$pendingGroupWrites !== undefined) {
			const writes = item.$pendingGroupWrites;
			item.$pendingGroupWrites = undefined;
			for (const [path, value] of writes) {
				const binding = item.$bindings === null ? undefined : item.$bindings.find((b) => b.key === path[0]);
				if (binding !== undefined) {
					try {
						const object = binding.fn.call(item);
						if (object !== undefined && object !== null) {
							hooks.engineWrite++;
							try {
								item[path[0]] = object;
							} finally {
								hooks.engineWrite--;
							}
							binding.last = object;
							binding.snap = snapshot(object);
							binding.hasLast = true;
						}
					} catch {
					}
				}
				this.assign(item, path, value);
			}
		}
		for (const handler of completedHandlers) this.completed.push(handler);
		if (item instanceof Loader) this.updateLoader(item);
		return item;
	}

	// `ListElement { role: value }`: a row of the enclosing ListModel (its values are evaluated once).
	createListElement(def, scope, parent, overlays) {
		const element = new Placeholder('ListElement');
		element.$nonVisual = true;
		element.$row = {};
		for (const link of [{ def, scope }, ...overlays]) {
			for (const prop of link.def.properties) {
				if (prop.isHandler || prop.isDeclaration || prop.text === null) continue;
				let value = this.constantValue(prop.text);
				if (value === NOT_CONSTANT) {
					value = this.guard(() => this.compile('expr', prop.text, '', link.scope, element).call(element), `ListElement '${prop.name}' (${link.scope.file})`, prop.line);
				}
				element.$row[prop.name] = value;
			}
		}
		if (parent !== null) parent.$adopt(element);
		return element;
	}

	findDelegate(chain, type) {
		const unwrap = (found) => {
			if (this.lenient && found.def.type === 'Component' && found.def.children.length === 1) {
				return { def: found.def.children[0], scope: found.scope };
			}
			return found;
		};
		for (const link of chain) {
			for (const child of link.def.children) {
				if (child.propertyName === 'delegate') return unwrap({ def: child, scope: link.scope });
			}
		}
		for (const link of chain) {
			if (link.def.children.length === 1) return unwrap({ def: link.def.children[0], scope: link.scope });
		}
		throw new QmlError(`${type} needs a delegate`, chain[0].def.line, chain[0].def.col);
	}

	// `property TreeItemModel model: TreeItemModel {}` in a component gives a default that a use site's binding of `model`
	// replaces, as in Qt: the bindings of the use sites are applied before the objects of the base file.
	boundByOverlay(child, chain, l) {
		if (child.propertyName === '' || child.inList) return false;
		return chain.slice(l + 1).some((link) => link.def.properties.some((p) => !p.isDeclaration && p.name === child.propertyName));
	}

	// Lenient creation of one child definition: objects given as property values are created detached
	// and assigned to the property, everything else becomes a child item. Failures are reported.
	createChildLenient(child, link, item, index, depth, lists, chain, fallbackProperty = '', capture = null) {
		const propertyName = child.propertyName !== '' ? child.propertyName : fallbackProperty;
		try {
			if (child.type === 'Repeater' && propertyName === '' && !this.lenient) {
				this.createRepeater(child, item, link.scope, index, depth);
			} else if (propertyName !== '') {
				// `sourceComponent: Rectangle {}` is a component, the same as a declared `property Component`.
				if (child.type !== 'Component' && !child.inList && (propertyName === 'sourceComponent' || chain.some((l) => l.def.properties.some((p) => p.isDeclaration && p.typeName === 'Component' && p.name === propertyName)))) {
					// `property Component delegate: Item { ... }`: the object is wrapped into a Component implicitly.
					child = { type: 'Component', id: '', propertyName, inList: false, properties: [], children: [{ ...child, propertyName: '' }], signalDefs: [], functions: [], enums: [], line: child.line, col: child.col };
				}
				const value = this.createItem(child, link.scope, null, [], index, depth);
				if (value === null) return;
				// Like a QObject child in Qt, the object lives as long as the item: released with it (its bindings
				// would otherwise keep the whole subtree the item was in alive).
				if (item.$owned === undefined) item.$owned = [];
				item.$owned.push(value);
				if (child.inList) {
					if (!lists.has(propertyName)) lists.set(propertyName, []);
					lists.get(propertyName).push(value);
				} else {
					// An object given for a property replaces what a base type bound it to (`property Component body: null`).
					this.dropBinding(item, propertyName);
					if (capture !== null) capture.push({ name: propertyName, old: this.readOld(item, propertyName) });
					this.guard(() => this.assign(item, [propertyName], value), `property '${propertyName}'`, child.line);
				}
			} else {
				this.createItem(child, link.scope, item, [], index, depth);
			}
		} catch (error) {
			this.report(`'${child.type}'`, error, error instanceof QmlError && error.line ? error.line : child.line);
		}
	}

	// Exposes the enums declared in a document as properties of the item (`Name.Value` and `Value`).
	defineEnums(item, def) {
		for (const e of def.enums || []) {
			const values = Object.freeze({ ...e.values });
			Object.defineProperty(item, e.name, { value: values, enumerable: true, configurable: true });
			for (const [name, value] of Object.entries(values)) {
				if (!(name in item)) Object.defineProperty(item, name, { value, enumerable: true, configurable: true });
			}
		}
	}

	// Calls the `ListView.onPooled` / `ListView.onReused` handlers of a delegate.
	runDelegateHandlers(item, list, where) {
		if (list === undefined) return;
		for (const entry of list) this.guard(() => entry.fn.call(item), `handler '${where}' (${entry.file})`, entry.line);
	}

	// Re-runs every binding under a reused ListView delegate so `model.role` / `model.index` pick up the new row.
	refreshDelegateBindings(root) {
		const stack = [root];
		while (stack.length > 0) {
			const item = stack.pop();
			if (item.$destroyed) continue;
			const list = item.$bindings;
			if (list !== null && list !== undefined) {
				for (const binding of list) {
					binding.hasLast = false;
					this.evaluateOne(binding, false);
				}
			}
			for (const child of item.$children) stack.push(child);
		}
	}

	setupListView(view, delegateDef, scope, depth) {
		// Pooled delegates of an earlier delegate definition cannot be reused.
		view.$drainPool();
		view.poolDelegate = (item) => this.runDelegateHandlers(item, item.$pooledFns, 'ListView.onPooled');
		view.rebindDelegate = (item, i) => {
			const delegateScope = item.$delegateScope;
			this.applyModelData(delegateScope, view.$model, i);
			// Every item created below the delegate with the row's index carries its own `index` property.
			for (const indexed of item.$delegateIndexed) {
				if (indexed.$custom === null || indexed.$custom.index === i) continue;
				indexed.$custom.index = i;
				this.markReaders(indexed, 'index');
				if (indexed.$chg !== undefined) this.fireChanged(indexed, 'index');
			}
			// Role / `model.*` getters are not tracked as binding deps. Without a refresh, reused
			// Table rows and headers keep the previous row's text (Qt emits on the context object).
			this.refreshDelegateBindings(item);
			this.runDelegateHandlers(item, item.$reusedFns, 'ListView.onReused');
		};
		view.createDelegate = (i) => {
			const mark = this.completed.length;
			const before = view.$children.length;
			const pooled = view.$takePooled();
			if (pooled !== null) {
				view.rebindDelegate(pooled, i);
				return pooled;
			}
			const delegateScope = new Scope(scope, scope.file);
			this.applyModelData(delegateScope, view.$model, i);
			const outer = this.indexTrack;
			const indexed = [];
			this.indexTrack = indexed;
			try {
				const delegate = this.createItem(delegateDef, delegateScope, view, [], i, depth);
				if (delegate !== null) {
					delegate.$delegateScope = delegateScope;
					delegate.$delegateIndexed = indexed;
				}
				this.indexTrack = outer;
				this.runCompletedFrom(mark);
				return delegate;
			} catch (error) {
				this.indexTrack = outer;
				this.report('ListView delegate', error, error instanceof QmlError ? error.line : 0);
				this.completed.length = Math.min(this.completed.length, mark);
				while (view.$children.length > before) {
					const child = view.$children[view.$children.length - 1];
					this.releaseSubtree(child);
					view.$removeChild(child);
				}
				return null;
			}
		};
		view.releaseDelegate = (item) => this.releaseSubtree(item);
	}

	createRepeater(def, parent, scope, index, depth) {
		if (parent === null) throw new QmlError('A Repeater must be a child of an item', def.line, def.col);
		const modelProp = def.properties.find((p) => p.name === 'model' && !p.isHandler && !p.isDeclaration);
		if (modelProp === undefined) throw new QmlError("Repeater needs a numeric 'model'", def.line, def.col);
		let count;
		try {
			count = this.compile('expr', modelProp.text, '', scope, parent).call(parent);
		} catch (error) {
			throw new QmlError(describe(error), modelProp.line, modelProp.col);
		}
		if (typeof count !== 'number' || !Number.isFinite(count)) {
			throw new QmlError("Repeater needs a numeric 'model'", modelProp.line, modelProp.col);
		}
		count = Math.trunc(count);
		if (count < 0 || count > MAX_REPEATER_COUNT) {
			throw new QmlError(`The 'model' of a Repeater must be between 0 and ${MAX_REPEATER_COUNT}`, modelProp.line, modelProp.col);
		}
		let delegateDef = def.children.find((c) => c.propertyName === 'delegate');
		if (delegateDef === undefined && def.children.length === 1) delegateDef = def.children[0];
		if (delegateDef === undefined) throw new QmlError('Repeater needs exactly one delegate item', def.line, def.col);
		const outer = this.indexTrack;
		this.indexTrack = null; // the repeater's items have their own `index`
		try {
			for (let i = 0; i < count; i++) {
				this.createItem(delegateDef, new Scope(scope, scope.file), parent, [], i, depth);
			}
		} finally {
			this.indexTrack = outer;
		}
	}

	defineFunction(item, fn, scope) {
		let compiled;
		try {
			compiled = this.compile('body', fn.body, fn.params, scope, item);
		} catch (error) {
			throw new QmlError(describe(error), fn.line, fn.col);
		}
		const bound = compiled.bind(item);
		bound.$owner = item;
		Object.defineProperty(item, fn.name, { value: bound, enumerable: true, configurable: true, writable: true });
	}

	// ----- properties -----

	applyProperty(item, prop, scope, completedOut, capture = null) {
		try {
			if (prop.isDeclaration) {
				this.declareProperty(item, prop, scope);
			} else if (prop.isHandler) {
				this.connectHandler(item, prop, scope, completedOut);
			} else if (this.lenient && /^[A-Z]/.test(prop.name)) {
				// `Layout.preferredWidth` and the rest of the attached layout spec. Other attached
				// properties (Keys.enabled, ...) stay unsupported.
				if (prop.name.startsWith('Layout.') || prop.name.startsWith('KeyNavigation.')) this.setValue(item, prop.name.split('.'), prop.text, scope, prop);
				else this.noteIgnored(item, prop.name.split('.'));
			} else {
				this.setValue(item, prop.name.split('.'), prop.text, scope, prop, capture);
			}
		} catch (error) {
			let failure = error;
			if (error instanceof QmlError) {
				if (!error.line) {
					error.line = prop.line;
					error.column = prop.col;
				}
			} else {
				failure = new QmlError(describe(error), prop.line, prop.col);
			}
			if (!this.lenient) throw failure;
			this.report(`property '${prop.name}'`, failure, prop.line);
		}
	}

	// `Qt.AlignCenter` and `FlexboxLayout.Row`: a member of a global enum object, assigned once.
	enumConstant(tokens) {
		if (tokens.length !== 4 || tokens[0].t !== 'id' || tokens[1].t !== 'p' || tokens[1].v !== '.' || tokens[2].t !== 'id') return NOT_CONSTANT;
		// A singleton that several files declare is another one in every file: its members are not constant.
		if (this.ambiguousSingletons.has(tokens[0].v)) return NOT_CONSTANT;
		// A property of a `pragma Singleton` file is a property like any other (`Style.baseColor` is set when the theme is
		// applied, `Style.logo` is a binding): it is read again when it changes.
		if (this.singletons.has(tokens[0].v)) return NOT_CONSTANT;
		const obj = this.globals[tokens[0].v];
		if (obj === null || obj === undefined || typeof obj !== 'object') return NOT_CONSTANT;
		const value = obj[tokens[2].v];
		const kind = typeof value;
		if (kind === 'number' || kind === 'string' || kind === 'boolean') return value;
		return NOT_CONSTANT;
	}

	constantValue(text) {
		let result = this.constants.get(text);
		if (result === undefined) {
			result = NOT_CONSTANT;
			try {
				const tokens = tokenize(text);
				let constant = tokens.length > 1;
				for (const t of tokens) {
					if (t.t === 'eof' || t.t === 'num' || t.t === 'str') continue;
					if (t.t === 'id' && (t.v === 'true' || t.v === 'false' || t.v === 'null')) continue;
					if (t.t === 'p' && CONSTANT_PUNCTUATION.has(t.v)) continue;
					constant = false;
					break;
				}
				if (constant) result = new Function(`return (${text}\n)`)();
				else result = this.enumConstant(tokens);
			} catch {
				result = NOT_CONSTANT;
			}
			this.constants.set(text, result);
		}
		return result;
	}

	readOld(item, name) {
		try {
			return item[name];
		} catch {
			return undefined;
		}
	}

	setValue(item, path, text, scope, prop, capture = null) {
		if (prop.isReturnBlock) {
			const block = this.compile('body', text, '', scope, item);
			this.dropGroupBinding(item, path);
			this.addBinding(item, path, () => block.call(item), prop.line, scope.file);
			return;
		}
		const value = this.constantValue(text);
		if (value !== NOT_CONSTANT) {
			this.dropBinding(item, path.join('.'));
			this.dropGroupBinding(item, path);
			if (capture !== null && path.length === 1) capture.push({ name: path[0], old: this.readOld(item, path[0]) });
			this.assign(item, path, value);
			return;
		}
		// `[]` and `({})` are initial values, not bindings: a fresh object per item, never re-evaluated.
		if (EMPTY_LITERAL_RE.test(text)) {
			this.dropBinding(item, path.join('.'));
			this.assign(item, path, text.includes('[') ? [] : {});
			return;
		}
		const fn = this.compile('expr', text, '', scope, item);
		this.dropGroupBinding(item, path);
		this.addBinding(item, path, () => fn.call(item), prop.line, scope.file);
	}

	// `border.width: 0` on a use of a component that aliases `border` of one of its items replaces the binding that item has
	// for `border.width`, as in Qt.
	dropGroupBinding(item, path) {
		if (path.length !== 2) return;
		let group;
		try {
			group = item[path[0]];
		} catch {
			return;
		}
		if (group === undefined || group === null) {
			// The aliased item is created after the properties of the root: wait for it.
			if (item.$constructing && item.$aliases !== undefined && item.$aliases.has(path[0])) {
				if (item.$pendingGroupDrops === undefined) item.$pendingGroupDrops = [];
				item.$pendingGroupDrops.push(path);
			}
			return;
		}
		if (group.$owner === undefined || group.$owner === item || group.$group === undefined) return;
		this.dropBinding(group.$owner, `${group.$group}.${path[1]}`);
	}

	// A property change handler: polled once per evaluation pass and, for custom properties, run at once when the
	// property is assigned (so that a value changed and changed back within one pass is not lost).
	addChangeHandler(handler) {
		this.changeHandlers.push(handler);
		const item = handler.item;
		if (item.$chg === undefined) item.$chg = new Map();
		const list = item.$chg.get(handler.property);
		if (list === undefined) item.$chg.set(handler.property, [handler]);
		else list.push(handler);
	}

	fireChanged(item, name) {
		if (item.$chg === undefined) return;
		const list = item.$chg.get(name);
		if (list === undefined) return;
		// During construction default object values (`property Model m: Model {}`) are assigned after
		// onXChanged was registered with `last === undefined`. Seed `last` without running so the first
		// poll after construction does not treat that default as a change (Qt does not emit it either).
		if (item.$constructing) {
			let value;
			try {
				value = item[name];
			} catch {
				return;
			}
			for (const handler of list) {
				if (handler.failed || !handler.hasLast) continue;
				handler.last = value;
			}
			return;
		}
		// A write after construction runs handlers at once, including inside Component.onCompleted:
		// the next line of the script must see their effect.
		for (const handler of list.slice()) {
			if (handler.failed || !handler.hasLast) continue;
			const value = item[name];
			if (value === handler.last || (value !== value && handler.last !== handler.last)) continue;
			handler.last = value;
			this.runNotified(handler);
		}
	}

	// The handler runs as a script, so an assignment inside it replaces bindings of other properties.
	// Writing the property that is being reported does not: Qt emits the change while that binding is still updating.
	runNotified(handler) {
		const engineWrite = hooks.engineWrite;
		hooks.engineWrite = 0;
		try {
			duringChange(handler.item, handler.property, () => {
				this.guard(() => handler.fn.call(handler.item), `handler '${handler.name}' (${handler.file})`, handler.line);
			});
		} finally {
			hooks.engineWrite = engineWrite;
		}
	}

	// `item.nameChanged.connect(fn)` for a declared property: the property is polled like an `onNameChanged` handler.
	defineChangeSignal(item, name) {
		const signalName = name + 'Changed';
		if (Object.prototype.hasOwnProperty.call(item, signalName)) return item[signalName];
		const connected = [];
		const signal = () => {};
		signal.connect = (fn) => {
			if (typeof fn !== 'function') return;
			const handler = { item, property: name, name: signalName, fn: () => fn(), line: 0, file: '', last: item[name], hasLast: true, failed: false, source: fn };
			connected.push(handler);
			this.addChangeHandler(handler);
			linkToOwner(item, fn, () => signal.disconnect(fn));
		};
		signal.disconnect = (fn) => {
			const at = connected.findIndex((h) => h.source === fn);
			if (at < 0) return;
			connected[at].failed = true;
			const registered = this.changeHandlers.indexOf(connected[at]);
			if (registered >= 0) this.changeHandlers.splice(registered, 1);
			const list = item.$chg === undefined ? undefined : item.$chg.get(name);
			if (list !== undefined && list.indexOf(connected[at]) >= 0) list.splice(list.indexOf(connected[at]), 1);
			connected.splice(at, 1);
		};
		Object.defineProperty(item, signalName, { value: signal, enumerable: false, configurable: true, writable: true });
		return signal;
	}

	declareProperty(item, prop, scope) {
		if (this.lenient) this.defineChangeSignal(item, prop.name);
		if (prop.isAlias) {
			this.defineAlias(item, prop, scope);
			return;
		}
		// `property string` always holds a string.
		if (prop.typeName === 'string') item.$defineCustom(prop.name, '', toQmlString, prop.isReadonly === true);
		else if (INT_TYPES.has(prop.typeName)) item.$defineCustom(prop.name, 0, toQmlInt, prop.isReadonly === true);
		else if (REAL_TYPES.has(prop.typeName)) item.$defineCustom(prop.name, 0, toQmlReal, prop.isReadonly === true);
		else if (prop.typeName === 'bool') item.$defineCustom(prop.name, false, toQmlBool, prop.isReadonly === true);
		else if (prop.typeName === 'color') item.$defineCustom(prop.name, '#000000', toQmlColor, prop.isReadonly === true);
		else if (prop.typeName === 'point') item.$defineCustom(prop.name, { x: 0, y: 0 }, null, prop.isReadonly === true);
		else if (prop.typeName === 'size') item.$defineCustom(prop.name, { width: -1, height: -1 }, null, prop.isReadonly === true);
		else if (prop.typeName === 'rect') item.$defineCustom(prop.name, { x: 0, y: 0, width: 0, height: 0 }, null, prop.isReadonly === true);
		else item.$defineCustom(prop.name, undefined, null, prop.isReadonly === true);
		if (prop.text !== null) this.setValue(item, [prop.name], prop.text, scope, prop);
	}

	// `property alias name: target.property`: reads and writes are forwarded to the target.
	defineAlias(item, prop, scope) {
		const match = ALIAS_RE.exec(prop.text === null ? '' : prop.text);
		if (match === null) throw new QmlError(`Invalid alias target '${prop.text}'`, prop.line, prop.col);
		const parts = match[1].split('.').map((s) => s.trim());
		const last = parts[parts.length - 1];
		const targetFn = this.compile('expr', parts.length > 1 ? parts.slice(0, -1).join('.') : last, '', scope, item);
		const whole = parts.length === 1;
		const name = prop.name;
		const first = parts[0];
		// The target of an alias is an id of the same component (or a property of the item). Until that item is created
		// the name must not be found in the scopes around: a view inside of a view that has an item with the same id
		// (`container`) would write to the outer one.
		const resolveTarget = () => {
			if (!Object.prototype.hasOwnProperty.call(scope.ids, first) && !(first in item)) return undefined;
			return targetFn.call(item);
		};
		if (item.$aliases === undefined) item.$aliases = new Set();
		item.$aliases.add(name);
		Object.defineProperty(item, name, {
			get() {
				try {
					const target = resolveTarget();
					if (whole) return target;
					return target === undefined || target === null ? undefined : target[last];
				} catch {
					return undefined;
				}
			},
			set: (value) => {
				if (whole) throw new QmlError(`Cannot assign to the alias '${name}' of an item`);
				let target;
				try {
					target = resolveTarget();
				} catch {
					target = undefined;
				}
				if (target === undefined || target === null) {
					if (item.$constructing) {
						if (item.$pendingAliasWrites === null) item.$pendingAliasWrites = [];
						item.$pendingAliasWrites.push([name, value]);
						return;
					}
					throw new QmlError(`The target of the alias '${name}' does not exist`);
				}
				// A write through an alias replaces the target's own binding, as in Qt
				// (`extendingInfoComp: detailComp` must win over `sourceComponent: defaultComp`).
				this.dropBinding(target, last);
				this.assign(target, [last], value);
			},
			enumerable: true,
			configurable: true,
		});
	}

	connectHandler(item, prop, scope, completedOut) {
		const name = prop.name;
		const body = prop.text === null ? '' : prop.text;
		if (item.$conn !== undefined && !name.includes('.')) {
			item.$conn.handlers.push({ signal: decapitalize(name.slice(2)), name, body, scope, line: prop.line, fn: null });
			return;
		}
		if (name === 'Component.onCompleted') {
			completedOut.push({ fn: this.compile('body', body, '', scope, item), item, line: prop.line, file: scope.file });
			return;
		}
		if (name === 'ListView.onPooled' || name === 'ListView.onReused') {
			const key = name === 'ListView.onPooled' ? '$pooledFns' : '$reusedFns';
			if (item[key] === undefined) item[key] = [];
			item[key].push({ fn: this.compile('body', body, '', scope, item), file: scope.file, line: prop.line });
			return;
		}
		if (name === 'Component.onDestruction') {
			if (item.$destruction === undefined) item.$destruction = [];
			item.$destruction.push({ fn: this.compile('body', body, '', scope, item), file: scope.file, line: prop.line });
			return;
		}
		if (name === 'Keys.onPressed' || name === 'Keys.onReleased' || NAMED_KEY_HANDLERS.has(name)) {
			const fn = this.compile('body', body, 'event', scope, item);
			if (item.$keys === undefined) item.$keys = { pressed: [], released: [], byKey: new Map() };
			if (name === 'Keys.onPressed') item.$keys.pressed.push(fn);
			else if (name === 'Keys.onReleased') item.$keys.released.push(fn);
			else {
				const code = NAMED_KEY_HANDLERS.get(name);
				if (!item.$keys.byKey.has(code)) item.$keys.byKey.set(code, []);
				item.$keys.byKey.get(code).push(fn);
			}
			return;
		}
		const unsupported = () => new QmlError(`Unsupported handler '${name}' for ${item.$typeName}`, prop.line, prop.col);
		if (name.includes('.')) {
			if (this.lenient) return this.noteIgnored(item, name.split('.'));
			throw unsupported();
		}
		const signalName = decapitalize(name.slice(2));
		const signal = item.$sigs === null ? undefined : item.$sigs.get(signalName);
		if (signal === undefined) {
			if (!this.lenient) throw unsupported();
			if (name.length > 9 && name.endsWith('Changed')) {
				// A property change handler: the property is polled once per evaluation pass.
				const property = decapitalize(name.slice(2, -7));
				const fn = this.compile('body', body, '', scope, item);
				// The handler starts from the value the property has now, so the first assigned or bound value is a change (as in Qt).
				// An alias whose target does not exist yet has no value to start from: its first value is the initial one.
				let initial;
				try {
					initial = item[property];
				} catch {
					initial = undefined;
				}
				const lateAlias = initial === undefined && item.$aliases !== undefined && item.$aliases.has(property);
				this.addChangeHandler({ item, property, name, fn, line: prop.line, file: scope.file, last: initial, hasLast: !lateAlias, failed: false });
			} else {
				this.noteIgnored(item, [name]);
			}
			return;
		}
		let fn;
		if (callableHandler(body)) {
			// The arrow or function expression is the handler, not a statement that builds and drops it.
			const produce = this.compile('expr', body, '', scope, item);
			fn = (...args) => produce.call(item).apply(item, args);
		} else {
			fn = this.compile('body', body, signal.params.join(', '), scope, item);
		}
		item.$connect(signalName, (...args) => this.guard(() => fn.apply(item, args), `handler '${name}' (${scope.file})`, prop.line));
	}

	// ----- dynamic creation and release -----

	// Binding: writes `value` to `property` of `target` whenever one of the three changes.
	// During a binding sweep, `onXChanged` may set `target` before other bindings have produced the
	// value (ControlBase: decorator_ first, control height next). Writing immediately would stamp 0 and
	// leave the target's own height binding with a stale `last` so it never restores. Defer until the
	// sweep finishes; script writes outside a sweep still publish before the next line.
	bindingItemsPending() {
		for (const item of this.qmlBindings) {
			if (!item.$destroyed && this.bindingItemChanged(item)) return true;
		}
		return false;
	}

	bindingItemChanged(item) {
		if (item.when === false) return false;
		const state = item.$bind;
		const target = item.target;
		const property = item.property;
		if (target === null || target === undefined || typeof target !== 'object' || typeof property !== 'string' || property === '') return false;
		if (!(state.has && state.target === target && state.property === property && sameValue(state.value, item.value))) return true;
		// Another binding may have overwritten the target after we last wrote (ControlBase: content
		// width binding vs Binding pushing an explicit control width of 300).
		try {
			let current = target;
			for (const part of property.split('.')) {
				if (current === null || current === undefined) return true;
				current = current[part];
			}
			return !sameValue(current, state.value);
		} catch {
			return true;
		}
	}

	syncBindingItems(only = null) {
		if (this.trackingPass > 0) return false;
		let wrote = false;
		for (const item of only === null ? [...this.qmlBindings] : [only]) {
			if (item.$destroyed) {
				this.qmlBindings.delete(item);
				continue;
			}
			if (!this.bindingItemChanged(item)) continue;
			wrote = true;
			const state = item.$bind;
			const target = item.target;
			const property = item.property;
			const value = item.value;
			state.has = true;
			state.target = target;
			state.property = property;
			state.value = value;
			// Keep the target's own binding (Qt 5): it takes over again when its inputs change. Dropping it
			// freezes ControlBase button width at the pre-layout size (content.width still 0).
			this.guard(() => this.assign(target, property.split('.'), value), `Binding '${property}'`, 0);
		}
		return wrote;
	}

	// Connections: (re)connects the handlers of every Connections item to its current `target`.
	syncConnections(only = null) {
		for (const conn of only === null ? [...this.connections] : [only]) {
			if (conn.$destroyed) {
				this.connections.delete(conn);
				continue;
			}
			const state = conn.$conn;
			const target = conn.target;
			if (target === state.target) continue;
			state.target = target;
			for (const undo of state.links) undo();
			state.links = [];
			if (target === null || target === undefined || typeof target !== 'object') continue;
			for (const handler of state.handlers) this.guard(() => this.linkConnection(conn, target, handler), `Connections '${handler.name}'`, handler.line || 0);
		}
	}

	linkConnection(conn, target, handler) {
		const state = conn.$conn;
		const signal = target.$sigs === null || target.$sigs === undefined ? undefined : target.$sigs.get(handler.signal);
		let call;
		if (handler.fn !== null) {
			call = handler.fn;
		} else {
			const fn = this.compile('body', handler.body, signal === undefined ? '' : signal.params.join(', '), handler.scope, conn);
			call = (...args) => fn.apply(conn, args);
		}
		const run = (...args) => {
			if (conn.enabled === false) return;
			this.guard(() => call(...args), `handler '${handler.name}' (Connections)`, handler.line || 0);
		};
		if (signal !== undefined) {
			signal.handlers.push(run);
			state.links.push(() => {
				const at = signal.handlers.indexOf(run);
				if (at >= 0) signal.handlers.splice(at, 1);
			});
		} else if (handler.name.endsWith('Changed') && handler.name.length > 9) {
			const property = decapitalize(handler.name.slice(2, -7));
			const entry = { item: target, property, name: handler.name, fn: () => run(), line: handler.line || 0, file: '', last: target[property], hasLast: true, failed: false };
			this.addChangeHandler(entry);
			state.links.push(() => {
				entry.failed = true;
				const at = this.changeHandlers.indexOf(entry);
				if (at >= 0) this.changeHandlers.splice(at, 1);
				const list = target.$chg.get(property);
				if (list !== undefined && list.indexOf(entry) >= 0) list.splice(list.indexOf(entry), 1);
			});
		}
	}

	createObject(type, parent, props) {
		if (typeof type !== 'string') throw new QmlError('Qt.createObject needs the name of a type');
		if (!(parent instanceof Item)) throw new QmlError('Qt.createObject needs a parent item');
		const mark = this.completed.length;
		const item = this.createItem(emptyDef(type), this.documentScope, parent, [], undefined, 0);
		if (props !== undefined && props !== null) {
			for (const key of Object.keys(props)) this.assign(item, key.split('.'), props[key]);
		}
		this.runCompletedFrom(mark);
		return item;
	}

	// `Qt.createComponent(url[, mode])` and `Qt.createComponent(module, typeName[, mode])`: a Component whose
	// definition is the root of another document. The project loaded the files named by string literals and the
	// types of its modules up front, so the usual case is Ready at once. A file that is not loaded yet is fetched:
	// the status is Loading until statusChanged reports Ready or Error, as for a remote URL in Qt. The script
	// does not tell which file calls, so a relative URL is tried against the main document, then as a suffix of
	// a loaded path, then as a type name.
	createComponent(url, second, third) {
		const byType = typeof second === 'string';
		if (typeof url !== 'string' || url === '' || (byType && second === '')) throw new QmlError('Qt.createComponent needs the URL of a QML file');
		const mode = byType ? third : second;
		const component = new ComponentItem();
		component.$url = byType ? `${url}/${second}` : url;
		component.$status = 2;
		const fail = (message) => {
			component.$setStatus(3, message);
			this.report('Qt.createComponent', new QmlError(message), 0);
		};
		if (this.project === null) {
			fail(`Cannot load '${component.$url}': no project`);
			return component;
		}
		const ready = (doc) => {
			if (doc.error) {
				fail(`${doc.path}: ${doc.error.message}`);
				return;
			}
			component.$def = { ...emptyDef('Component'), children: [doc.def] };
			component.$defScope = this.newScope(null, doc.path);
			component.$setStatus(1);
		};
		const doc = byType ? this.findModuleTypeDocument(url, second) : this.findDocument(url);
		if (doc !== null) {
			if (mode === 1) setTimeout(() => this.guard(() => ready(doc), 'Qt.createComponent', 0), 0);
			else ready(doc);
			return component;
		}
		const load = byType ? this.loadModuleTypeDocument(url, second) : this.loadDocument(url);
		load.then(
			(loaded) => {
				if (loaded !== null) ready(loaded);
				else fail(`File not found or not part of the project: '${component.$url}'`);
			},
			(error) => fail(`${component.$url}: ${describe(error)}`),
		);
		return component;
	}

	// The path of `url` relative to the main document; `qrc:` and `file:` prefixes are dropped.
	documentPath(url) {
		return resolvePath(url.replace(/^qrc:\/*/, '/'), dirname(this.project.mainPath));
	}

	// The loaded document a URL names, or null.
	findDocument(url) {
		const docs = this.project.docs;
		const exact = docs.get(this.documentPath(url));
		if (exact !== undefined) return exact;
		// The resource prefix (`qrc:/qml/`) and `../` are not directories of the project: leading directories are dropped
		// until a loaded path ends with the rest, keeping at least the last directory and the file name.
		const relative = normalizePath(url.replace(/^(qrc:|file:)?\/*/, '')).replace(/^(\.\.\/)+/, '');
		if (relative === '') return null;
		const parts = relative.split('/');
		for (let from = 0; from < parts.length - 1; from++) {
			const tail = parts.slice(from).join('/');
			for (const [path, doc] of docs) {
				if (path === tail || path.endsWith(`/${tail}`)) return doc;
			}
		}
		// A bare file name: a type of the imports or modules of the main document, then a name that only one file has.
		const name = parts[parts.length - 1];
		const type = name.endsWith('.qml') ? name.slice(0, -4) : '';
		if (/^[A-Z]\w*$/.test(type)) {
			const byImport = this.project.resolveType(type, this.project.mainPath);
			if (byImport !== null) return byImport;
			for (const types of this.project.modules.values()) {
				const path = types === null ? undefined : types.get(type);
				if (path !== undefined && docs.has(path)) return docs.get(path);
			}
		}
		let only = null;
		for (const [path, doc] of docs) {
			if (path !== name && !path.endsWith(`/${name}`)) continue;
			if (only !== null) return null;
			only = doc;
		}
		return only;
	}

	findModuleTypeDocument(module, type) {
		const types = this.project.modules.get(module);
		const path = types === null || types === undefined ? undefined : types.get(type);
		if (path !== undefined) return this.project.docs.get(path) || null;
		return null;
	}

	// Fetches the document a URL names; resolves to the document or null.
	async loadDocument(url) {
		const project = this.project;
		const type = /^[A-Z]\w*\.qml$/.test(url) ? url.slice(0, -4) : '';
		const tasks = [project.addSingleton(this.documentPath(url))];
		if (type !== '') {
			tasks.push(project.findComponent(type, project.mainPath));
			for (const types of project.modules.values()) {
				const path = types === null ? undefined : types.get(type);
				if (path !== undefined) tasks.push(project.addSingleton(path));
			}
		}
		await Promise.all(tasks);
		return this.findDocument(url);
	}

	async loadModuleTypeDocument(module, type) {
		const project = this.project;
		await project.addModule(module);
		const types = project.modules.get(module);
		const path = types === null || types === undefined ? undefined : types.get(type);
		if (path === undefined) return null;
		await project.addSingleton(path);
		return this.findModuleTypeDocument(module, type);
	}
	releaseSubtree(item) {
		const dead = new Set();
		this.releaseItem(item, dead);
		if (dead.size > 0) this.changeHandlers = this.changeHandlers.filter((h) => !dead.has(h));
	}

	releaseItem(item, dead) {
		if (item.$released) return;
		item.$released = true;
		if (item.$destruction !== undefined) {
			const list = item.$destruction;
			item.$destruction = undefined;
			for (const entry of list) this.guard(() => entry.fn.call(item), `handler 'Component.onDestruction' (${entry.file})`, entry.line);
		}
		if (item.$component !== undefined) {
			this.guard(() => item.$component.$emit('destruction'), 'signal \'Component.destruction\'', 0);
		}
		if (item.$links !== undefined) {
			const links = item.$links;
			item.$links = undefined;
			for (const undo of links) undo();
		}
		if (item.$chg !== undefined) {
			for (const list of item.$chg.values()) {
				for (const handler of list) {
					handler.failed = true;
					dead.add(handler);
				}
			}
			item.$chg.clear();
		}
		if (item.$conn !== undefined) {
			for (const undo of item.$conn.links) undo();
			item.$conn.links = [];
			this.connections.delete(item);
		}
		this.qmlBindings.delete(item);
		this.loaders.delete(item);
		if (item.$bindings !== null) {
			for (const binding of item.$bindings) {
				binding.failed = true;
				this.unsubscribe(binding);
				this.bindings.delete(binding);
				this.volatile.delete(binding);
			}
			item.$bindings = null;
		}
		item.$subs = undefined;
		if (item.$typeName === 'Timer') this.timers.delete(item);
		if (item instanceof Repeater) {
			if (item.$model instanceof ListModel) item.$model.$syncWatchers = Math.max(0, item.$model.$syncWatchers - 1);
			this.repeaters.delete(item);
		}
		if (item instanceof Animation) {
			item.$running = false;
			this.animations.delete(item);
		}
		for (const child of item.$children) this.releaseItem(child, dead);
		if (item.$owned !== undefined) {
			for (const owned of item.$owned) this.releaseItem(owned, dead);
		}
		if (item instanceof ListView) {
			for (const pooled of item.$pool) this.releaseItem(pooled, dead);
		}
	}

	// Loads the picture of an Image item. Relative paths are relative to the file of the item; ImtCore builds them
	// for the resource system (`:/Light/Icons/...`), so the path without its leading `../` is tried from the
	// directory of the main file as well.
	// The status change is applied on the next handler pass, so onStatusChanged and onCompleted both see Ready.
	queueImage(image, picture, status) {
		image.$queuedImage = { picture, status, source: image.$source };
		this.pendingImages.add(image);
	}

	flushImages() {
		if (this.pendingImages.size === 0) return;
		for (const image of this.pendingImages) {
			const queued = image.$queuedImage;
			image.$queuedImage = undefined;
			if (queued !== undefined && queued.source === image.$source) image.$setImage(queued.picture, queued.status);
		}
		this.pendingImages.clear();
	}

	async svgImageSource(source) {
		const response = await fetch(source);
		if (!response.ok) throw new QmlError(`HTTP ${response.status} loading '${source}'`);
		const text = await response.text();
		// Chromium taints network SVGs containing foreignObject, even an unused Illustrator switch branch.
		// A data URL preserves the SVG while fetch still enforces the original server's CORS policy.
		return `data:image/svg+xml;charset=utf-8,${encodeURIComponent(text)}`;
	}

	loadImage(image, source) {
		const Picture = globalThis.Image;
		if (source === '') {
			image.$setImage(null, 0);
			return;
		}
		if (typeof Picture !== 'function' || typeof document === 'undefined') {
			// A data URL is ready before handlers are connected, so only onCompleted reports it.
			// A file finishes loading afterwards, so onStatusChanged runs as well.
			if (/^data:image\//i.test(source)) {
				image.$setImage(null, 1);
				return;
			}
			const path = source.split(/[?#]/)[0];
			const ok = /\.(svg|png|jpe?g|gif|webp|bmp|ico)$/i.test(path);
			this.queueImage(image, null, ok ? 1 : 3);
			return;
		}
		let path = source.startsWith('qrc:') ? source.slice(4).replace(/^\/+/, '') : source;
		const candidates = [];
		if (this.resourceRoot !== null && !/^(?:[a-z][a-z0-9+.-]*:)/i.test(path)) {
			candidates.push(this.resourceRoot + path.replace(/^:?\/+/, '').replaceAll('../', ''));
		} else {
			candidates.push(resolvePath(path, image.$directory));
			if (!/^[a-z][a-z0-9+.-]*:/i.test(path) && !path.startsWith('/')) {
				const root = this.project === null ? '' : dirname(this.project.mainPath);
				const stripped = joinPath(root, normalizePath(path).replace(/^(\.\.\/)+/, ''));
				if (!candidates.includes(stripped)) candidates.push(stripped);
			}
		}
		const attempt = (n) => {
			const picture = new Picture();
			picture.crossOrigin = 'anonymous';
			picture.onload = () => {
				if (image.$source === source && !image.$destroyed) image.$setImage(picture, 1);
			};
			const failed = (error) => {
				if (image.$source !== source || image.$destroyed) return;
				if (n + 1 < candidates.length) attempt(n + 1);
				else {
					image.$setImage(null, 3);
					if (error !== undefined) this.report(`Image '${source}'`, error, 0);
				}
			};
			picture.onerror = () => failed();
			const candidate = candidates[n];
			if (typeof fetch === 'function' && /\.svg(?:[?#]|$)/i.test(candidate)) {
				this.svgImageSource(candidate).then((prepared) => {
					if (image.$source === source && !image.$destroyed) picture.src = prepared;
				}, failed);
			} else {
				picture.src = candidate;
			}
		};
		attempt(0);
	}

	loadFont(loader, source) {
		if (source === '') {
			loader.$name = '';
			loader.$status = 'Null';
			return;
		}
		if (this.project === null) {
			loader.$name = '';
			loader.$status = 'Error';
			return;
		}
		// Prefer the path relative to the FontLoader's file; StyleBase hosts the loaders while
		// Style.qml often names fonts next to the Acf module (`../Fonts/...`), so also match by file name.
		const path = resolvePath(source, loader.$directory);
		let family = this.project.fontFamilies.get(path);
		if (!family) {
			const file = basename(String(source).replace(/^qrc:\/*/, ''));
			for (const [registered, name] of this.project.fontFamilies) {
				if (name && basename(registered) === file) {
					family = name;
					break;
				}
			}
		}
		if (family) {
			loader.$name = family;
			loader.$status = 'Ready';
		} else {
			loader.$name = '';
			loader.$status = 'Error';
		}
	}
}


function safeStringify(value) {
	try {
		if (typeof value === 'object' && value !== null && !Array.isArray(value) && value.toString !== Object.prototype.toString) {
			const text = value.toString();
			if (typeof text === 'string') return text;
		}
		return typeof value === 'object' && value !== null ? JSON.stringify(value) : String(value);
	} catch {
		return String(value);
	}
}
