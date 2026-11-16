// The page's own input elements for the text editors of the scene (a prototype).
//
// The scene is one canvas, so the browser knows nothing about its text fields: a phone shows no on-screen keyboard (an `input` of the page
// has to have the focus, in the handler of a touch), and a password manager neither fills nor saves what it cannot see in a `form`.
// This module keeps real `input` / `textarea` elements over the fields, transparent and not hit by the pointer (the canvas still gets
// every click), and mirrors them with the editing model of the engine, which stays the one that draws the text:
//
//  - the editor that has the focus has an element that has the focus of the page; typing, a virtual keyboard, a composition (IME), a
//    paste, a drop and the autofill of the browser change the element, and `TextEditor.$fromDom` brings the model after it;
//  - what a script or a key of the engine changes in the model (text, cursor, selection) is written to the element;
//  - the keys that the editor keeps for editing (see `$shortcutOverride`) are left to the element, the others (Return, Escape, Tab,
//    shortcuts) go to the engine;
//  - a visible password editor, with the single-line editor above it (the user name), gets a `form` with the attributes a password manager
//    looks for (`autocomplete="username"` / `"current-password"`, `type="password"`), as long as it is visible; when the fields go away
//    after the login the browser offers to save the password. Return in the password field submits the form.

import { getFocusItem, textEditors } from './items.js';
import { qtKeyEvent } from './keys.js';

// Qt.InputMethodHints
const IMH = { hidden: 0x1, sensitive: 0x2, noUppercase: 0x4, noPredictive: 0x40, digits: 0x10000, numbers: 0x20000, dialable: 0x100000, email: 0x200000, url: 0x400000 };

const STYLE = `
.jqml-proxy { position: absolute; box-sizing: border-box; margin: 0; padding: 0; border: 0; outline: 0; resize: none; overflow: hidden;
	background: transparent !important; color: transparent !important; -webkit-text-fill-color: transparent; caret-color: transparent;
	font: 16px sans-serif; opacity: 0.01; pointer-events: none; touch-action: none; box-shadow: none; }
.jqml-proxy::selection { background: transparent; }
.jqml-proxy:-webkit-autofill { transition: background-color 600000s 0s, color 600000s 0s; }
`;

const isPassword = (item) => item.$echo === 2 || item.$echo === 3;

// Is the editor shown (the item and every item above it)?
function shown(item) {
	for (let it = item; it !== null; it = it.$parent) if (!it.$visible) return false;
	return item.$aw > 0 && item.$ah > 0;
}

export function attachTextProxy(canvas, scene, engine) {
	if (typeof document === 'undefined') return null;
	const style = document.createElement('style');
	style.textContent = STYLE;
	document.head.appendChild(style);
	const layer = document.createElement('div');
	layer.style.cssText = 'position: fixed; left: 0; top: 0; width: 0; height: 0; overflow: visible; pointer-events: none;';
	(canvas.parentNode || document.body).appendChild(layer);

	const forms = new Map(); // password editor -> { form, user, password, userItem }
	let free = null; // { item, el } the element of the focused editor that is not in a login form
	const byElement = new Map(); // element -> editor
	let composing = false;

	// ----- elements -----

	function configure(el, item, role) {
		el.className = 'jqml-proxy';
		const hints = item.$hints | 0;
		const multi = item.$multi;
		if (!multi) {
			el.type = role === 'password' || isPassword(item) ? 'password' : 'text';
		}
		el.autocomplete = role === 'username' ? 'username' : role === 'password' ? 'current-password' : 'off';
		const name = item.$objectName || role || 'text';
		el.name = name;
		el.id = `jqml-${name}`;
		el.spellcheck = false;
		el.setAttribute('autocorrect', 'off');
		if (hints & (IMH.noUppercase | IMH.email | IMH.url) || role === 'username') el.setAttribute('autocapitalize', 'none');
		if (hints & IMH.digits) el.inputMode = 'numeric';
		else if (hints & IMH.numbers) el.inputMode = 'decimal';
		else if (hints & IMH.dialable) el.inputMode = 'tel';
		else if (hints & IMH.email) el.inputMode = 'email';
		else if (hints & IMH.url) el.inputMode = 'url';
		if (hints & IMH.noPredictive || hints & IMH.sensitive) el.setAttribute('autocomplete', role ? el.autocomplete : 'off');
		if (item.$maxLength < 32767) el.maxLength = item.$maxLength;
		el.readOnly = !item.$editable;
		// The fields of a login form take the clicks themselves: the list of the saved logins of the browser opens only on a click in the
		// element (not on a focus from a script). Their caret and selection come to the model through `selectionchange`.
		if (role !== '') el.style.pointerEvents = 'auto';
		byElement.set(el, item);
		listen(el);
		return el;
	}

	function place(el, item) {
		const rect = canvas.getBoundingClientRect();
		const s = el.style;
		s.left = `${rect.left + item.$sx}px`;
		s.top = `${rect.top + item.$sy}px`;
		s.width = `${Math.max(1, item.$aw)}px`;
		s.height = `${Math.max(1, item.$ah)}px`;
	}

	// Writes the model of the editor to its element (not while the user composes a word: that would end the composition).
	function push(el, item) {
		if (composing && document.activeElement === el) return;
		const state = item.$domState();
		if (el.value !== state.text) el.value = state.text;
		if (document.activeElement === el && (el.selectionStart !== state.start || el.selectionEnd !== state.end)) {
			try {
				el.setSelectionRange(state.start, state.end, state.backward ? 'backward' : 'forward');
			} catch {
				// a type that has no selection
			}
		}
	}

	// ----- from the element to the model -----

	function pull(el) {
		const item = byElement.get(el);
		if (item === undefined) return;
		item.$fromDom(el.value, el.selectionStart ?? el.value.length, el.selectionEnd ?? el.value.length, el.selectionDirection === 'backward');
		// the model may have taken less (maxLength): the element follows it
		if (item.$text !== el.value) push(el, item);
		scene.root.$updateRequested = true;
	}

	function listen(el) {
		el.addEventListener('input', () => pull(el));
		el.addEventListener('compositionstart', () => (composing = true));
		el.addEventListener('compositionend', () => {
			composing = false;
			pull(el);
		});
		el.addEventListener('keydown', (e) => keyEvent(e, true));
		el.addEventListener('keyup', (e) => keyEvent(e, false));
		el.addEventListener('focus', () => engineFocus(el));
	}

	// The editor of an element that got the focus (the user tapped it in the autofill menu, or moved with Tab of the browser).
	function engineFocus(el) {
		const item = byElement.get(el);
		if (item !== undefined && getFocusItem() !== item) item.forceActiveFocus();
	}

	// The keys of the element: what the editor keeps for editing stays with the element, the rest is for the scene.
	function keyEvent(e, pressed) {
		if (e.isComposing || e.keyCode === 229) return;
		const item = byElement.get(e.currentTarget);
		const event = qtKeyEvent(e);
		const native = item !== undefined && item.$shortcutOverride(event);
		const taken = pressed ? scene.keyDown(e, native) : scene.keyUp(e, native);
		if (!native && (taken || (item !== undefined && !item.$multi && e.key === 'Enter'))) e.preventDefault();
		if (pressed) {
			update();
			focusCurrent();
		}
	}

	document.addEventListener('selectionchange', () => {
		const el = document.activeElement;
		if (el !== null && byElement.has(el) && !composing) pull(el);
	});

	// ----- the elements of the editors, per frame -----

	function userNameOf(password) {
		let best = null;
		for (const c of textEditors) {
			if (c === password || c.$multi || isPassword(c) || !c.$editable || !shown(c)) continue;
			const gap = password.$sy - c.$sy;
			if (gap < -1 || gap > 250 || Math.abs(c.$sx - password.$sx) > 60) continue;
			if (best === null || c.$sy > best.$sy) best = c;
		}
		return best;
	}

	function scanForms() {
		const live = new Set();
		for (const p of textEditors) {
			if (!isPassword(p) || !p.$editable || !shown(p)) continue;
			live.add(p);
			if (forms.has(p)) continue;
			const userItem = userNameOf(p);
			const form = document.createElement('form');
			form.style.display = 'contents';
			form.autocomplete = 'on';
			form.addEventListener('submit', (e) => e.preventDefault());
			const user = userItem === null ? null : form.appendChild(configure(document.createElement('input'), userItem, 'username'));
			const password = form.appendChild(configure(document.createElement('input'), p, 'password'));
			const submit = form.appendChild(document.createElement('button'));
			submit.type = 'submit';
			submit.tabIndex = -1;
			submit.style.cssText = 'position: absolute; width: 1px; height: 1px; opacity: 0; pointer-events: none;';
			layer.appendChild(form);
			forms.set(p, { form, user, password, userItem });
			p.$connect('accepted', () => {
				if (typeof form.requestSubmit === 'function') form.requestSubmit();
			});
		}
		for (const [p, entry] of [...forms]) {
			if (live.has(p)) continue;
			for (const el of [entry.user, entry.password]) if (el !== null) byElement.delete(el);
			entry.form.remove();
			forms.delete(p);
		}
	}

	function update() {
		for (const [p, entry] of forms) {
			place(entry.password, p);
			push(entry.password, p);
			if (entry.user !== null) {
				place(entry.user, entry.userItem);
				push(entry.user, entry.userItem);
			}
		}
		const focus = getFocusItem();
		const editor = focus !== null && focus !== undefined && typeof focus.$domState === 'function' && focus.$editable && shown(focus) ? focus : null;
		let inForm = false;
		for (const [p, entry] of forms) if (p === editor || entry.userItem === editor) inForm = true;
		if (free !== null && (editor === null || free.item !== editor || inForm)) {
			byElement.delete(free.el);
			free.el.remove();
			free = null;
		}
		if (editor !== null && !inForm) {
			if (free === null) {
				const el = configure(document.createElement(editor.$multi ? 'textarea' : 'input'), editor, '');
				layer.appendChild(el);
				free = { item: editor, el };
			}
			place(free.el, editor);
			// the second click in the focused field is native too (the list of the browser, the caret, a selection with the mouse)
			free.el.style.pointerEvents = 'auto';
			push(free.el, editor);
		}
	}

	// The element of the focused editor gets the focus of the page. A phone shows its keyboard only when this is done in the handler
	// of a touch, so it is called from the pointer handlers (and from the keys, for Tab).
	function focusCurrent() {
		const focus = getFocusItem();
		let el = null;
		for (const [p, entry] of forms) {
			if (focus === p) el = entry.password;
			else if (focus === entry.userItem && entry.user !== null) el = entry.user;
		}
		if (el === null && free !== null && free.item === focus) el = free.el;
		if (el !== null && document.activeElement !== el) el.focus({ preventScroll: true });
	}

	function afterPointer() {
		update();
		focusCurrent();
	}
	// after the handlers of the scene and of the canvas (host.js registers them first)
	canvas.addEventListener('pointerdown', afterPointer);
	canvas.addEventListener('pointerup', afterPointer);
	canvas.addEventListener('click', afterPointer);

	let lastScan = 0;
	function frame(time) {
		if (time - lastScan > 250) {
			lastScan = time;
			scanForms();
		}
		update();
		requestAnimationFrame(frame);
	}
	requestAnimationFrame(frame);

	return { update, focusCurrent, scanForms, forms: () => forms };
}
