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
	if (name.length === 1 || [...name].length === 1) {
		// With Ctrl the physical key decides, so that shortcuts work on non-Latin layouts too.
		const physical = shortcut && typeof e.code === 'string' ? /^(?:Key([A-Z])|Digit([0-9]))$/.exec(e.code) : null;
		key = physical ? (physical[1] || physical[2]).charCodeAt(0) : name.toUpperCase().codePointAt(0);
		if (!shortcut) text = name;
	} else if (name in NAMED) {
		key = NAMED[name];
		text = TEXT_OF[name] || '';
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
