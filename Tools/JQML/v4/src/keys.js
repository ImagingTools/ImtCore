// Keyboard: Qt key codes, modifiers and the conversion of a DOM KeyboardEvent into a Qt-like KeyEvent.

export const KEY = {
	Key_Escape: 0x01000000, Key_Tab: 0x01000001, Key_Backtab: 0x01000002, Key_Backspace: 0x01000003,
	Key_Return: 0x01000004, Key_Enter: 0x01000005, Key_Insert: 0x01000006, Key_Delete: 0x01000007,
	Key_Pause: 0x01000008, Key_Print: 0x01000009,
	Key_Home: 0x01000010, Key_End: 0x01000011, Key_Left: 0x01000012, Key_Up: 0x01000013, Key_Right: 0x01000014,
	Key_Down: 0x01000015, Key_PageUp: 0x01000016, Key_PageDown: 0x01000017,
	Key_Shift: 0x01000020, Key_Control: 0x01000021, Key_Meta: 0x01000022, Key_Alt: 0x01000023, Key_CapsLock: 0x01000024,
	Key_F1: 0x01000030, Key_F2: 0x01000031, Key_F3: 0x01000032, Key_F4: 0x01000033, Key_F5: 0x01000034, Key_F6: 0x01000035,
	Key_F7: 0x01000036, Key_F8: 0x01000037, Key_F9: 0x01000038, Key_F10: 0x01000039, Key_F11: 0x0100003a, Key_F12: 0x0100003b,
	Key_Space: 0x20, Key_Plus: 0x2b, Key_Minus: 0x2d, Key_Period: 0x2e, Key_Comma: 0x2c, Key_Slash: 0x2f,
};
for (let c = 0x41; c <= 0x5a; c++) KEY[`Key_${String.fromCharCode(c)}`] = c;
for (let c = 0x30; c <= 0x39; c++) KEY[`Key_${String.fromCharCode(c)}`] = c;

export const MODIFIERS = {
	NoModifier: 0,
	ShiftModifier: 0x02000000,
	ControlModifier: 0x04000000,
	AltModifier: 0x08000000,
	MetaModifier: 0x10000000,
	KeypadModifier: 0x20000000,
};

const NAMED = {
	Escape: KEY.Key_Escape, Tab: KEY.Key_Tab, Backspace: KEY.Key_Backspace, Enter: KEY.Key_Return, Insert: KEY.Key_Insert,
	Delete: KEY.Key_Delete, Home: KEY.Key_Home, End: KEY.Key_End, ArrowLeft: KEY.Key_Left, ArrowUp: KEY.Key_Up,
	ArrowRight: KEY.Key_Right, ArrowDown: KEY.Key_Down, PageUp: KEY.Key_PageUp, PageDown: KEY.Key_PageDown,
	Shift: KEY.Key_Shift, Control: KEY.Key_Control, Alt: KEY.Key_Alt, Meta: KEY.Key_Meta, CapsLock: KEY.Key_CapsLock,
	Pause: KEY.Key_Pause, PrintScreen: KEY.Key_Print,
};
for (let i = 1; i <= 12; i++) NAMED[`F${i}`] = KEY[`Key_F${i}`];

const TEXT_OF = { Enter: '\r', Tab: '\t', Backspace: '\b', Escape: '\x1b' };

// A Qt KeyEvent for a DOM KeyboardEvent (or a plain object with the same fields).
export function qtKeyEvent(e) {
	// AltGr reports Ctrl+Alt on Windows; what it types is text, not a shortcut.
	const altGraph = typeof e.getModifierState === 'function' && e.getModifierState('AltGraph');
	const ctrl = !altGraph && Boolean(e.ctrlKey || e.metaKey);
	let modifiers = 0;
	if (e.shiftKey) modifiers |= MODIFIERS.ShiftModifier;
	if (ctrl) modifiers |= MODIFIERS.ControlModifier;
	if (e.altKey && !altGraph) modifiers |= MODIFIERS.AltModifier;
	const shortcut = ctrl;
	let key = 0;
	let text = '';
	const name = String(e.key);
	const numpadEnter = e.code === 'NumpadEnter';
	if (name.length === 1 || [...name].length === 1) {
		// With Ctrl the physical key decides, so that shortcuts work on non-Latin layouts too.
		const physical = shortcut && typeof e.code === 'string' ? /^(?:Key([A-Z])|Digit([0-9]))$/.exec(e.code) : null;
		key = physical ? (physical[1] || physical[2]).charCodeAt(0) : name.toUpperCase().codePointAt(0);
		if (!shortcut) text = name;
	} else if (name in NAMED) {
		key = NAMED[name];
		text = TEXT_OF[name] || '';
		// The Enter of the number pad is Key_Enter, the one of the main block Key_Return (Qt).
		if (numpadEnter) key = KEY.Key_Enter;
	}
	return {
		key,
		text,
		modifiers,
		accepted: false,
		isAutoRepeat: Boolean(e.repeat),
		count: 1,
		nativeScanCode: 0,
	};
}

// `Keys.onReturnPressed` and the like: the handler name -> the key it reacts to.
export const NAMED_KEY_HANDLERS = new Map(
	[
		'Escape', 'Tab', 'Backtab', 'Backspace', 'Return', 'Enter', 'Insert', 'Delete', 'Home', 'End', 'Left', 'Up', 'Right',
		'Down', 'PageUp', 'PageDown', 'Space', 'Plus', 'Minus', 'Period', 'Comma', 'Slash',
		'F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8', 'F9', 'F10', 'F11', 'F12',
	].map((name) => [`Keys.on${name}Pressed`, KEY[`Key_${name}`]]),
);

// ----- key sequences (`Shortcut`) -----

const SEQUENCE_MODIFIERS = { ctrl: MODIFIERS.ControlModifier, shift: MODIFIERS.ShiftModifier, alt: MODIFIERS.AltModifier, meta: MODIFIERS.MetaModifier };
// The names a key of a sequence can have (Qt reads them without regard to case); `Plus` is no name, the key is written `+`.
const SEQUENCE_KEYS = {
	esc: KEY.Key_Escape, escape: KEY.Key_Escape, return: KEY.Key_Return, enter: KEY.Key_Enter, tab: KEY.Key_Tab, backtab: KEY.Key_Backtab,
	backspace: KEY.Key_Backspace, del: KEY.Key_Delete, delete: KEY.Key_Delete, ins: KEY.Key_Insert, insert: KEY.Key_Insert,
	home: KEY.Key_Home, end: KEY.Key_End, pgup: KEY.Key_PageUp, pageup: KEY.Key_PageUp, pgdown: KEY.Key_PageDown, pagedown: KEY.Key_PageDown,
	left: KEY.Key_Left, right: KEY.Key_Right, up: KEY.Key_Up, down: KEY.Key_Down, space: KEY.Key_Space,
};
for (let n = 1; n <= 35; n++) SEQUENCE_KEYS[`f${n}`] = 0x01000030 + n - 1;

// The names of the keys in the text of a sequence (QKeySequence::PortableText).
const PORTABLE_NAMES = new Map([
	[KEY.Key_Escape, 'Esc'], [KEY.Key_Return, 'Return'], [KEY.Key_Enter, 'Enter'], [KEY.Key_Tab, 'Tab'], [KEY.Key_Backtab, 'Backtab'],
	[KEY.Key_Backspace, 'Backspace'], [KEY.Key_Delete, 'Del'], [KEY.Key_Insert, 'Ins'], [KEY.Key_Home, 'Home'], [KEY.Key_End, 'End'],
	[KEY.Key_PageUp, 'PgUp'], [KEY.Key_PageDown, 'PgDown'], [KEY.Key_Left, 'Left'], [KEY.Key_Right, 'Right'], [KEY.Key_Up, 'Up'],
	[KEY.Key_Down, 'Down'], [KEY.Key_Space, 'Space'],
]);
for (let n = 1; n <= 35; n++) PORTABLE_NAMES.set(0x01000030 + n - 1, `F${n}`);

// One step of a sequence ("Ctrl+Shift+Z", "Esc", "Ctrl++") as { key, modifiers, text }, or null when it is not a key.
function parseStep(text) {
	let modifiers = 0;
	let rest = text.trim();
	if (rest === '') return null;
	for (;;) {
		const plus = rest.indexOf('+');
		// the last key may be the `+` itself: "Ctrl++", "+"
		if (plus <= 0) break;
		const modifier = SEQUENCE_MODIFIERS[rest.slice(0, plus).trim().toLowerCase()];
		if (modifier === undefined) return null;
		modifiers |= modifier;
		rest = rest.slice(plus + 1).trim();
	}
	let key;
	if ([...rest].length === 1) key = rest.toUpperCase().codePointAt(0);
	else key = SEQUENCE_KEYS[rest.toLowerCase()];
	if (key === undefined) return null;
	const parts = [];
	for (const [name, value] of [['Meta', MODIFIERS.MetaModifier], ['Ctrl', MODIFIERS.ControlModifier], ['Alt', MODIFIERS.AltModifier], ['Shift', MODIFIERS.ShiftModifier]]) {
		if (modifiers & value) parts.push(name);
	}
	parts.push(PORTABLE_NAMES.get(key) ?? String.fromCodePoint(key));
	return { key, modifiers, text: parts.join('+') };
}

// A key sequence as text: `{ steps: [{ key, modifiers }], text }`, or null when it is not valid (such a shortcut never activates). A
// sequence of several steps ("Ctrl+K, Ctrl+C") is read, but only the first step is waited for: the engine does not go on with it.
export function parseKeySequence(text) {
	if (typeof text !== 'string') return null;
	const steps = [];
	for (const part of text.split(', ')) {
		const step = parseStep(part);
		if (step === null) return null;
		steps.push(step);
	}
	return steps.length === 0 ? null : { steps, text: steps.map((step) => step.text).join(', ') };
}

// QKeySequence::StandardKey: the number Qt gives and the sequence of Windows (the first one when there are several).
export const STANDARD_KEYS = [
	['HelpContents', 1, 'F1'], ['WhatsThis', 2, 'Shift+F1'], ['Open', 3, 'Ctrl+O'], ['Close', 4, 'Ctrl+F4'], ['Save', 5, 'Ctrl+S'], ['New', 6, 'Ctrl+N'],
	['Delete', 7, 'Del'], ['Cut', 8, 'Ctrl+X'], ['Copy', 9, 'Ctrl+C'], ['Paste', 10, 'Ctrl+V'], ['Undo', 11, 'Ctrl+Z'], ['Redo', 12, 'Ctrl+Y'],
	['Back', 13, 'Alt+Left'], ['Forward', 14, 'Alt+Right'], ['Refresh', 15, 'F5'], ['ZoomIn', 16, 'Ctrl++'], ['ZoomOut', 17, 'Ctrl+-'],
	['Print', 18, 'Ctrl+P'], ['AddTab', 19, 'Ctrl+T'], ['NextChild', 20, 'Ctrl+Tab'], ['PreviousChild', 21, 'Ctrl+Shift+Backtab'],
	['Find', 22, 'Ctrl+F'], ['FindNext', 23, 'F3'], ['FindPrevious', 24, 'Shift+F3'], ['Replace', 25, 'Ctrl+H'], ['SelectAll', 26, 'Ctrl+A'],
];

// The DOM KeyboardEvent fields for a Qt key and its modifiers, as the keyboard of a test gives them (see TestCase).
export function qtKeyToDom(key, modifiers) {
	const event = {
		key: '',
		code: '',
		shiftKey: (modifiers & MODIFIERS.ShiftModifier) !== 0,
		ctrlKey: (modifiers & MODIFIERS.ControlModifier) !== 0,
		altKey: (modifiers & MODIFIERS.AltModifier) !== 0,
		metaKey: (modifiers & MODIFIERS.MetaModifier) !== 0,
		repeat: false,
		getModifierState: () => false,
	};
	for (const [name, value] of Object.entries(NAMED)) {
		if (value !== key) continue;
		event.key = name;
		return event;
	}
	if (key === KEY.Key_Enter) {
		event.key = 'Enter';
		event.code = 'NumpadEnter';
	} else if (key === KEY.Key_Backtab) {
		event.key = 'Tab';
		event.shiftKey = true;
	} else if (key >= 0x01000030 && key < 0x01000030 + 35) {
		event.key = `F${key - 0x01000030 + 1}`;
	} else if (key < 0x01000000) {
		const char = String.fromCodePoint(key);
		event.key = /[A-Z]/.test(char) && !event.shiftKey ? char.toLowerCase() : char;
		if (/[A-Z]/.test(char)) event.code = `Key${char}`;
		else if (/[0-9]/.test(char)) event.code = `Digit${char}`;
	}
	return event;
}
