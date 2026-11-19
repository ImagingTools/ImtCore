// The API a test uses to find and drive the items of a running application (Playwright, Puppeteer, the devtools, or a Node test).
//
// A canvas has no DOM tree to look into, so the page publishes `window.qmlTest` (runtime.js, in every build, release ones too: it
// needs no flag). Everything it takes and gives is plain JSON, so one `page.evaluate` is one call. Items are named by handles
// (`{ id: 'e12', ... }`): a handle stays the same while its item lives, and a call with the handle of a destroyed item fails with a
// message that starts with `STALE:` (the Playwright client looks the element up again).
//
// Finding. `find(scope, selector)` looks for the items under `scope` (a handle, or null for the whole application); the scope is
// where the search starts, so an item that was found by its objectName is the root of the next search, and an objectName that several
// items have does not make a conflict. The selector is a small CSS-like text:
//
//   saveButton               the objectName (`*` and `?` are wildcards: `row*`, `*Button`)
//   #saveButton              the same, written as an id
//   "Save as"                an objectName with spaces
//   @Rectangle  @ImtButton   the type (the QML type of the item, or the name it has where it is used)
//   @ImtButton#ok            a type and an objectName
//   [text="OK"]  [width>0]   a property: =, !=, *= (contains), ^= (begins), $= (ends), and  >, <, >=, <=  for numbers
//   :visible  :enabled  :disabled
//   a b                      b anywhere under a
//   a > b                    b directly under a
//   > b                      b directly under the scope
//
// The items come in the order of the tree (parents before their children). Items that are hidden or have no size are found as well:
// what an action needs of an item is checked by `actionability`.
//
// Items that exist are found: a row of a ListView that the view has not built is not (scroll to it first).

const HIDDEN_FRAME_MS = 100;

// ----- selectors -----

const selectorCache = new Map();

function fail(selector, at, why) {
	throw new Error(`Invalid selector '${selector}' at ${at}: ${why}`);
}

function globToRegExp(pattern) {
	const text = pattern.replace(/[.+^${}()|[\]\\]/g, '\\$&').replace(/\*/g, '.*').replace(/\?/g, '.');
	return new RegExp(`^${text}$`);
}

// `[prop op value]` from the text between the brackets.
function parseAttribute(selector, inner, at) {
	const match = /^\s*([A-Za-z_$][\w$.]*)\s*(?:(\*=|\^=|\$=|!=|>=|<=|=|>|<)\s*(.*?))?\s*$/s.exec(inner);
	if (match === null) fail(selector, at, `cannot read the property test [${inner}]`);
	const [, name, op, raw] = match;
	if (op === undefined) return { name, op: 'has', value: undefined };
	let value;
	if (/^(["']).*\1$/s.test(raw)) value = raw.slice(1, -1);
	else if (raw === 'true') value = true;
	else if (raw === 'false') value = false;
	else if (raw === 'null') value = null;
	else if (raw !== '' && !Number.isNaN(Number(raw))) value = Number(raw);
	else value = raw;
	return { name, op, value };
}

function parseCompound(selector, text, start) {
	const compound = { name: null, nameRe: null, type: null, attrs: [], visible: null, enabled: null, any: false };
	let i = 0;
	// The characters of a name: letters of any alphabet (the names of a translated client: `ПользовательElementView`), digits and _ - . : $ * ?
	const identity = /[\p{L}\p{N}_\-.:$*?]/u;
	// A `:visible`, `:enabled` or `:disabled` at `at` that is not the start of a longer word ends a name: `Name:visible` is the name `Name` and a state.
	const stateAt = (at) => /^:(visible|enabled|disabled)(?![\p{L}\p{N}_\-.$*?])/u.test(text.slice(at, at + 10));
	while (i < text.length) {
		const c = text[i];
		if (c === '"' || c === "'") {
			const end = text.indexOf(c, i + 1);
			if (end < 0) fail(selector, start + i, 'a quote is not closed');
			compound.name = text.slice(i + 1, end);
			i = end + 1;
		} else if (c === '#') {
			let j = i + 1;
			while (j < text.length && identity.test(text[j]) && !stateAt(j)) j++;
			if (j === i + 1) fail(selector, start + i, 'a name is expected after #');
			compound.name = text.slice(i + 1, j);
			i = j;
		} else if (c === '@') {
			let j = i + 1;
			while (j < text.length && /[A-Za-z0-9_.]/.test(text[j])) j++;
			if (j === i + 1) fail(selector, start + i, 'a type is expected after @');
			compound.type = text.slice(i + 1, j);
			i = j;
		} else if (c === '[') {
			const end = text.indexOf(']', i);
			if (end < 0) fail(selector, start + i, 'a bracket is not closed');
			compound.attrs.push(parseAttribute(selector, text.slice(i + 1, end), start + i));
			i = end + 1;
		} else if (c === ':') {
			const match = /^:(visible|enabled|disabled)/.exec(text.slice(i));
			if (match === null) fail(selector, start + i, 'a state is expected after : (visible, enabled or disabled)');
			if (match[1] === 'visible') compound.visible = true;
			else compound.enabled = match[1] === 'enabled';
			i += match[0].length;
		} else if (c === '*' && (!identity.test(text[i + 1] || ' ') || stateAt(i + 1))) {
			compound.any = true;
			i++;
		} else if (identity.test(c)) {
			let j = i;
			while (j < text.length && identity.test(text[j]) && !stateAt(j)) j++;
			compound.name = text.slice(i, j);
			i = j;
		} else {
			fail(selector, start + i, `unexpected '${c}'`);
		}
	}
	if (compound.name !== null && /[*?]/.test(compound.name)) {
		compound.nameRe = globToRegExp(compound.name);
		compound.name = null;
	}
	return compound;
}

// A selector text as a list of steps `{ combinator: 'descendant' | 'child', compound }`.
export function parseSelector(selector) {
	if (typeof selector !== 'string') throw new Error('A selector is a text');
	const hit = selectorCache.get(selector);
	if (hit !== undefined) return hit;
	const steps = [];
	let combinator = 'descendant';
	let i = 0;
	const n = selector.length;
	while (i < n) {
		const c = selector[i];
		if (/\s/.test(c)) {
			i++;
			continue;
		}
		if (c === '>') {
			if (steps.length > 0 && combinator === 'child') fail(selector, i, 'two > in a row');
			combinator = 'child';
			i++;
			continue;
		}
		// a compound ends at a space or > that is outside of quotes and brackets
		let j = i;
		let quote = '';
		let depth = 0;
		while (j < n) {
			const d = selector[j];
			if (quote !== '') {
				if (d === quote) quote = '';
			} else if (d === '"' || d === "'") quote = d;
			else if (d === '[') depth++;
			else if (d === ']') depth--;
			else if (depth === 0 && (/\s/.test(d) || d === '>')) break;
			j++;
		}
		steps.push({ combinator, compound: parseCompound(selector, selector.slice(i, j), i) });
		combinator = 'descendant';
		i = j;
	}
	if (steps.length === 0) fail(selector, 0, 'it is empty');
	if (combinator === 'child') fail(selector, n, 'a selector cannot end with >');
	selectorCache.set(selector, steps);
	return steps;
}

// ----- values -----

// A value as JSON: items become handles, colors and other objects their text or their plain fields.
function plain(value, handleOf, depth = 0) {
	if (value === undefined || value === null) return null;
	const kind = typeof value;
	if (kind === 'string' || kind === 'boolean') return value;
	if (kind === 'number') return Number.isFinite(value) ? value : null;
	if (kind === 'function') return null;
	if (value !== null && value.$children !== undefined && value.$typeName !== undefined) return handleOf(value);
	if (value instanceof Date) return value.toISOString();
	if (depth >= 4) return null;
	if (Array.isArray(value)) return value.slice(0, 1000).map((entry) => plain(entry, handleOf, depth + 1));
	if (kind === 'object') {
		// a color of the engine ({ r, g, b, a, ... }) as the text a script gets
		if (typeof value.A === 'number' && typeof value.R === 'number') return `#${[value.A, value.R, value.G, value.B].map((v) => v.toString(16).padStart(2, '0')).join('')}`;
		const out = {};
		for (const key of Object.keys(value).slice(0, 200)) {
			if (key.startsWith('$')) continue;
			out[key] = plain(value[key], handleOf, depth + 1);
		}
		return out;
	}
	return String(value);
}

function compare(actual, op, expected) {
	if (op === '=') return actual === expected || (actual !== undefined && actual !== null && String(actual) === String(expected));
	if (op === '!=') return !compare(actual, '=', expected);
	if (op === '*=') return typeof actual === 'string' && actual.includes(String(expected));
	if (op === '^=') return typeof actual === 'string' && actual.startsWith(String(expected));
	if (op === '$=') return typeof actual === 'string' && actual.endsWith(String(expected));
	const a = Number(actual);
	const b = Number(expected);
	if (Number.isNaN(a) || Number.isNaN(b)) return false;
	if (op === '>') return a > b;
	if (op === '<') return a < b;
	if (op === '>=') return a >= b;
	return a <= b;
}

function readProperty(item, name) {
	try {
		return name.split('.').reduce((target, part) => (target === undefined || target === null ? undefined : target[part]), item);
	} catch {
		return undefined;
	}
}

// ----- keys -----

const NAMED_KEYS = {
	enter: 'Enter', return: 'Enter', esc: 'Escape', escape: 'Escape', tab: 'Tab', backspace: 'Backspace', delete: 'Delete', del: 'Delete',
	insert: 'Insert', home: 'Home', end: 'End', pageup: 'PageUp', pagedown: 'PageDown', space: ' ', up: 'ArrowUp', down: 'ArrowDown',
	left: 'ArrowLeft', right: 'ArrowRight', arrowup: 'ArrowUp', arrowdown: 'ArrowDown', arrowleft: 'ArrowLeft', arrowright: 'ArrowRight',
	control: 'Control', ctrl: 'Control', shift: 'Shift', alt: 'Alt', meta: 'Meta',
};

// The fields of a DOM KeyboardEvent for a key text such as 'a', 'Enter', 'Control+a', 'Shift+Tab'.
export function keyEventFor(text) {
	const parts = text.length > 1 && text.includes('+') && !text.endsWith('++') ? text.split('+') : [text];
	let key = parts.pop();
	const mods = { ctrlKey: false, shiftKey: false, altKey: false, metaKey: false };
	for (const part of parts) {
		const name = NAMED_KEYS[part.toLowerCase()];
		if (name === 'Control') mods.ctrlKey = true;
		else if (name === 'Shift') mods.shiftKey = true;
		else if (name === 'Alt') mods.altKey = true;
		else if (name === 'Meta') mods.metaKey = true;
		else throw new Error(`'${part}' is not a modifier key`);
	}
	const named = NAMED_KEYS[key.toLowerCase()];
	if (key.length > 1 && named !== undefined) key = named;
	else if (/^F([1-9]|1[0-2])$/i.test(key)) key = key.toUpperCase();
	else if ([...key].length !== 1) throw new Error(`Unknown key '${key}'`);
	let code = '';
	if (/^[a-z]$/i.test(key)) code = `Key${key.toUpperCase()}`;
	else if (/^[0-9]$/.test(key)) code = `Digit${key}`;
	if (/^[A-Z]$/.test(key)) mods.shiftKey = true;
	return { key, code, repeat: false, getModifierState: () => false, ...mods };
}

// ----- the API -----

// engine, scene: of the application; canvas: its canvas (null in Node); nextFrame: resolves when a frame was drawn;
// network: { count } of the requests in flight (see trackNetwork); logs: the lines the application printed.
export function createTestApi({ engine, scene, canvas = null, nextFrame = null, network = null, classes }) {
	const { Flickable, ListView } = classes;
	const ids = new WeakMap();
	const byId = new Map();
	let seq = 0;
	let started = false;
	const logs = [];
	const frame =
		nextFrame ||
		(() =>
			new Promise((resolve) => {
				// A page that is in the background gets no animation frames: the timer lets the test go on.
				setTimeout(resolve, HIDDEN_FRAME_MS);
				if (typeof requestAnimationFrame === 'function') requestAnimationFrame(() => resolve());
			}));

	const previousLog = engine.onLog;
	engine.onLog = (line) => {
		logs.push(String(line));
		if (logs.length > 2000) logs.splice(0, logs.length - 2000);
		if (typeof previousLog === 'function') previousLog(line);
	};

	function idOf(item) {
		let id = ids.get(item);
		if (id === undefined) {
			id = `e${++seq}`;
			ids.set(item, id);
			byId.set(id, new WeakRef(item));
		}
		return id;
	}

	function handleOf(item) {
		return { id: idOf(item), objectName: item.$objectName || '', type: item.$typeName, qmlType: item.$qmlType || item.$typeName };
	}

	function itemOf(id) {
		if (id === null || id === undefined) return scene.root;
		const ref = byId.get(id);
		const item = ref === undefined ? undefined : ref.deref();
		if (item === undefined || item.$destroyed === true) throw new Error(`STALE: the item ${id} does not exist any more`);
		return item;
	}

	function matches(item, c) {
		if (c.name !== null && (item.$objectName || '') !== c.name) return false;
		if (c.nameRe !== null && !c.nameRe.test(item.$objectName || '')) return false;
		if (c.type !== null && item.$typeName !== c.type && item.$qmlType !== c.type) return false;
		if (c.visible === true && !isVisible(item)) return false;
		if (c.enabled !== null && Boolean(readProperty(item, 'enabled')) !== c.enabled) return false;
		for (const attr of c.attrs) {
			const value = readProperty(item, attr.name);
			if (attr.op === 'has') {
				if (value === undefined) return false;
			} else if (!compare(value, attr.op, attr.value)) return false;
		}
		return true;
	}

	// `item` matches steps[index] and the steps before it match its ancestors, below `scope`.
	function matchesFrom(item, steps, index, scope) {
		if (!matches(item, steps[index].compound)) return false;
		if (index === 0) {
			// the first step is a child of the scope, or anywhere under it
			if (steps[0].combinator === 'child') return item.$parent === scope;
			return true;
		}
		const child = steps[index].combinator === 'child';
		for (let up = item.$parent; up !== null && up !== undefined; up = up.$parent) {
			if (matchesFrom(up, steps, index - 1, scope)) return true;
			if (up === scope || child) break;
		}
		return false;
	}

	function search(scope, selector, limit) {
		const steps = typeof selector === 'string' ? parseSelector(selector) : parseSelector(selectorText(selector));
		const found = [];
		const visit = (item) => {
			for (const child of item.$children) {
				if (child.$destroyed === true) continue;
				if (matchesFrom(child, steps, steps.length - 1, scope)) {
					found.push(child);
					if (found.length >= limit) return true;
				}
				if (visit(child)) return true;
			}
			return false;
		};
		visit(scope);
		return found;
	}

	// The text form of a structured selector { objectName, type, props, visible, enabled, text }.
	function selectorText(sel) {
		let text = '';
		if (sel.objectName !== undefined) text += /^[A-Za-z0-9_\-.:$*?]+$/.test(sel.objectName) ? `#${sel.objectName}` : JSON.stringify(sel.objectName);
		if (sel.type !== undefined) text += `@${sel.type}`;
		for (const [name, value] of Object.entries(sel.props || {})) text += `[${name}=${JSON.stringify(value)}]`;
		if (sel.text !== undefined) text += `[text=${JSON.stringify(sel.text)}]`;
		if (sel.visible === true) text += ':visible';
		if (sel.enabled === true) text += ':enabled';
		if (sel.enabled === false) text += ':disabled';
		return text === '' ? '*' : text;
	}

	// ----- geometry -----

	function canvasOrigin() {
		if (canvas === null || typeof canvas.getBoundingClientRect !== 'function') return { x: 0, y: 0 };
		const box = canvas.getBoundingClientRect();
		return { x: box.left, y: box.top };
	}

	function sceneBox(item) {
		item.$ensureGeometry();
		return { x: item.$sx, y: item.$sy, width: item.$aw, height: item.$ah };
	}

	function intersect(a, b) {
		const x0 = Math.max(a.x, b.x);
		const y0 = Math.max(a.y, b.y);
		const x1 = Math.min(a.x + a.width, b.x + b.width);
		const y1 = Math.min(a.y + a.height, b.y + b.height);
		return x1 > x0 && y1 > y0 ? { x: x0, y: y0, width: x1 - x0, height: y1 - y0 } : null;
	}

	function isVisible(item) {
		try {
			return item.visible === true;
		} catch {
			return false;
		}
	}

	function effectiveOpacity(item) {
		let opacity = 1;
		for (let i = item; i !== null && i !== undefined; i = i.$parent) if (typeof i.$opacity === 'number') opacity *= i.$opacity;
		return opacity;
	}

	// The part of the item that can be seen: its box cut by the boxes of the ancestors that clip, and by the scene.
	function visibleBox(item) {
		let box = sceneBox(item);
		for (let up = item.$parent; up !== null && up !== undefined && box !== null; up = up.$parent) {
			if (up.$clip === true) box = intersect(box, sceneBox(up));
		}
		if (box === null) return null;
		return intersect(box, { x: 0, y: 0, width: scene.width || 0, height: scene.height || 0 });
	}

	function toPage(box) {
		if (box === null) return null;
		const origin = canvasOrigin();
		return { x: box.x + origin.x, y: box.y + origin.y, width: box.width, height: box.height };
	}

	function center(box) {
		return { x: box.x + box.width / 2, y: box.y + box.height / 2 };
	}

	function textOf(item) {
		const text = readProperty(item, 'text');
		return typeof text === 'string' ? text : null;
	}

	function relationTo(item, hit) {
		if (hit === null) return 'none';
		if (hit === item) return 'self';
		for (let up = hit.$parent; up !== null && up !== undefined; up = up.$parent) if (up === item) return 'descendant';
		return 'other';
	}

	function describe(item) {
		const full = sceneBox(item);
		const visible = visibleBox(item);
		const opacity = effectiveOpacity(item);
		const isShown = isVisible(item) && opacity > 0 && visible !== null;
		const parent = item.$parent;
		return {
			...handleOf(item),
			text: textOf(item),
			visible: isShown,
			enabled: Boolean(readProperty(item, 'enabled')),
			opacity,
			x: Number(readProperty(item, 'x')) || 0,
			y: Number(readProperty(item, 'y')) || 0,
			width: full.width,
			height: full.height,
			box: toPage(full),
			visibleBox: toPage(visible),
			parent: parent !== null && parent !== undefined && parent !== scene.root ? handleOf(parent) : null,
			childCount: item.$children.length,
			focus: Boolean(readProperty(item, 'activeFocus')),
		};
	}

	// What stands between a click and the item (see relationTo): the item at the middle of its visible part that takes the pointer.
	function hitTest(item) {
		const visible = visibleBox(item);
		if (visible === null) return { relation: 'hidden', at: null, by: null };
		const point = center(visible);
		const hit = scene.root.$itemAt(point.x, point.y, false, 0);
		const relation = relationTo(item, hit);
		return { relation, at: toPage({ x: point.x, y: point.y, width: 0, height: 0 }), by: relation === 'other' ? handleOf(hit) : null };
	}

	// Whether a click can reach the item: why not, when it cannot.
	function actionability(item) {
		const reasons = [];
		const opacity = effectiveOpacity(item);
		if (!isVisible(item)) reasons.push('not visible');
		else {
			const box = sceneBox(item);
			if (box.width <= 0 || box.height <= 0) reasons.push('has no size');
			else if (visibleBox(item) === null) reasons.push('is outside of the visible area');
			else if (opacity <= 0) reasons.push('is transparent');
		}
		if (!readProperty(item, 'enabled')) reasons.push('is disabled');
		let hit = null;
		if (reasons.length === 0) {
			hit = hitTest(item);
			if (hit.relation === 'other') reasons.push(`is covered by ${hit.by.qmlType}${hit.by.objectName === '' ? '' : ` '${hit.by.objectName}'`}`);
		}
		return { ok: reasons.length === 0, reasons, hit };
	}

	// ----- scrolling -----

	function isView(item) {
		return item instanceof Flickable || item instanceof ListView;
	}

	// Moves the views that hold the item so that it is in them; true if something was moved (the layout needs a frame after it).
	function scrollIntoViewNow(item) {
		let moved = false;
		for (let view = item.$parent; view !== null && view !== undefined; view = view.$parent) {
			if (!isView(view)) continue;
			const target = sceneBox(item);
			const area = sceneBox(view);
			const maxX = Math.max(0, view.$maxContentX || 0);
			const maxY = Math.max(0, view.$maxContentY || 0);
			let dx = 0;
			let dy = 0;
			if (target.x < area.x) dx = target.x - area.x;
			else if (target.x + target.width > area.x + area.width) dx = Math.min(target.x - area.x, target.x + target.width - (area.x + area.width));
			if (target.y < area.y) dy = target.y - area.y;
			else if (target.y + target.height > area.y + area.height) dy = Math.min(target.y - area.y, target.y + target.height - (area.y + area.height));
			if (dx !== 0 && maxX > 0) {
				const next = Math.min(maxX, Math.max(0, view.contentX + dx));
				if (next !== view.contentX) {
					view.contentX = next;
					moved = true;
				}
			}
			if (dy !== 0 && maxY > 0) {
				const next = Math.min(maxY, Math.max(0, view.contentY + dy));
				if (next !== view.contentY) {
					view.contentY = next;
					moved = true;
				}
			}
		}
		return moved;
	}

	async function scrollIntoView(item) {
		if (scrollIntoViewNow(item)) {
			await frame();
			await frame();
		}
	}

	// ----- input -----

	async function pointerAt(item, options) {
		if (options.force !== true) {
			await scrollIntoView(item);
			const check = actionability(item);
			if (!check.ok) throw new Error(`NOT_ACTIONABLE: the item ${item.$objectName || item.$typeName} ${check.reasons.join(', ')}`);
		}
		const visible = visibleBox(item) || sceneBox(item);
		const box = sceneBox(item);
		if (options.position !== undefined) return { x: box.x + options.position.x, y: box.y + options.position.y };
		return center(visible);
	}

	async function press(keyText, count = 1) {
		const event = keyEventFor(keyText);
		for (let i = 0; i < count; i++) {
			engine.dispatchKey(event, true);
			engine.dispatchKey(event, false);
		}
		await frame();
	}

	const api = {
		version: 1,
		get started() {
			return started;
		},
		markStarted() {
			started = true;
		},

		parseSelector: (selector) => parseSelector(selector).map((step) => ({ combinator: step.combinator, ...step.compound, nameRe: step.compound.nameRe === null ? null : step.compound.nameRe.source })),

		// The items under `scope` that match the selector, as handles, in the order of the tree.
		find(scope, selector, options = {}) {
			const limit = options.limit === undefined ? Infinity : options.limit;
			return search(itemOf(scope), selector, limit).map(handleOf);
		},

		count(scope, selector) {
			return search(itemOf(scope), selector, Infinity).length;
		},

		// What a handle stands for now: its text, state and boxes (in CSS pixels of the page, as `getBoundingClientRect` gives them).
		info: (id) => describe(itemOf(id)),

		// The information of several items at once (one call for a whole list of handles).
		infos: (list) => list.map((id) => describe(itemOf(id))),

		exists(id) {
			const ref = byId.get(id);
			const item = ref === undefined ? undefined : ref.deref();
			return item !== undefined && item.$destroyed !== true;
		},

		parent(id) {
			const parent = itemOf(id).$parent;
			return parent === null || parent === undefined || parent === scene.root ? null : handleOf(parent);
		},

		children(id) {
			return itemOf(id).$children.filter((child) => child.$destroyed !== true).map(handleOf);
		},

		// The nearest ancestor that matches the selector (searching upwards from the item), or null.
		closest(id, selector) {
			const item = itemOf(id);
			const steps = parseSelector(selector);
			for (let up = item.$parent; up !== null && up !== undefined && up !== scene.root; up = up.$parent) {
				if (matches(up, steps[steps.length - 1].compound)) return handleOf(up);
			}
			return null;
		},

		actionability(id) {
			const check = actionability(itemOf(id));
			return { ok: check.ok, reasons: check.reasons, hit: check.hit };
		},

		get(id, name) {
			return plain(readProperty(itemOf(id), name), handleOf);
		},

		set(id, name, value) {
			const item = itemOf(id);
			if (!(name.split('.')[0] in item)) throw new Error(`The item has no property '${name}'`);
			const parts = name.split('.');
			const last = parts.pop();
			const target = parts.reduce((object, part) => object[part], item);
			target[last] = value;
			return true;
		},

		call(id, name, args = []) {
			const item = itemOf(id);
			if (typeof item[name] !== 'function') throw new Error(`The item has no function '${name}'`);
			return plain(item[name](...args), handleOf);
		},

		// The tree under an item: { id, objectName, type, children }; `depth` levels, and only the named items with `named`.
		tree(id = null, options = {}) {
			const depth = options.depth === undefined ? 6 : options.depth;
			const build = (item, level) => {
				const node = { ...handleOf(item) };
				if (options.boxes === true) node.box = toPage(sceneBox(item));
				if (level < depth) {
					const kids = [];
					for (const child of item.$children) {
						if (child.$destroyed === true) continue;
						const sub = build(child, level + 1);
						// without `named` every item is there; with it only the items with a name, and what is below the others is lifted
						if (options.named === true && sub.objectName === '') kids.push(...(sub.children || []));
						else kids.push(sub);
					}
					node.children = kids;
				}
				return node;
			};
			return build(itemOf(id), 0);
		},

		// The tree as indented text, for a test that fails and a person who has to see why.
		dump(id = null, options = {}) {
			const lines = [];
			const walk = (node, level) => {
				const name = node.objectName === '' ? '' : ` #${node.objectName}`;
				const type = node.qmlType === node.type ? node.type : `${node.qmlType} (${node.type})`;
				lines.push(`${'  '.repeat(level)}${type}${name}`);
				for (const child of node.children || []) walk(child, level + 1);
			};
			walk(api.tree(id, options), 0);
			return lines.join('\n');
		},

		// ----- input (a synthetic pointer and keyboard that go through the scene) -----

		async scrollIntoView(id) {
			await scrollIntoView(itemOf(id));
			return true;
		},

		async hover(id, options = {}) {
			const item = itemOf(id);
			const point = await pointerAt(item, options);
			scene.pointerMove(point.x, point.y);
			await frame();
			return true;
		},

		// options: { button: 0 | 2, clickCount: 1 | 2, position: { x, y } (from the top left of the item), force: true (no checks) }
		async click(id, options = {}) {
			const item = itemOf(id);
			const point = await pointerAt(item, options);
			const button = options.button === 'right' ? 2 : options.button || 0;
			scene.pointerMove(point.x, point.y);
			await frame();
			const times = options.clickCount || 1;
			for (let i = 0; i < times; i++) {
				scene.pointerDown(point.x, point.y, button);
				await frame();
				scene.pointerUp(point.x, point.y);
				await frame();
			}
			if (times > 1) scene.pointerDoubleClick(point.x, point.y, button);
			await frame();
			return { x: point.x + canvasOrigin().x, y: point.y + canvasOrigin().y };
		},

		async focus(id) {
			const item = itemOf(id);
			if (typeof item.forceActiveFocus !== 'function') throw new Error('The item cannot take the focus');
			item.forceActiveFocus();
			await frame();
			return true;
		},

		// Types a text with the keys of the keyboard into the item that has the focus.
		async type(text, options = {}) {
			for (const ch of [...text]) {
				const event = keyEventFor(ch);
				engine.dispatchKey(event, true);
				engine.dispatchKey(event, false);
				if (options.delay > 0) await new Promise((resolve) => setTimeout(resolve, options.delay));
			}
			await frame();
			return true;
		},

		async press(key, options = {}) {
			await press(key, options.count || 1);
			return true;
		},

		// Replaces the text of an editor: the focus, select all, the keys of the text (so that the handlers of the editor run).
		async fill(id, text) {
			const item = itemOf(id);
			if (typeof item.selectAll !== 'function') throw new Error(`The item ${item.$objectName || item.$typeName} is not a text editor`);
			if (typeof item.forceActiveFocus === 'function') item.forceActiveFocus();
			await frame();
			item.selectAll();
			if (text === '') await press('Delete');
			else await api.type(text);
			return true;
		},

		async wheel(id, deltaX = 0, deltaY = 0) {
			const item = itemOf(id);
			const point = await pointerAt(item, {});
			scene.wheel(point.x, point.y, -deltaY / 100, -deltaX / 100);
			await frame();
			return true;
		},

		// ----- waiting -----

		// Whether the application is doing something: a frame is wanted, a change is not applied, an animation runs, a picture or a
		// request is loading.
		busy() {
			return (
				scene.root.$updateRequested === true ||
				engine.dirty.size > 0 ||
				engine.animations.size > 0 ||
				(engine.pendingImages !== undefined && engine.pendingImages.size > 0) ||
				(network !== null && network.count > 0)
			);
		},

		// Resolves true when `quietFrames` frames in a row had nothing to do, false after `timeout` ms.
		async whenIdle(options = {}) {
			const quiet = options.quietFrames === undefined ? 3 : options.quietFrames;
			const deadline = Date.now() + (options.timeout === undefined ? 10000 : options.timeout);
			let still = 0;
			while (Date.now() < deadline) {
				await frame();
				still = api.busy() ? 0 : still + 1;
				if (still >= quiet) return true;
			}
			return false;
		},

		errors: () => engine.errors.map(String),
		logs: () => logs.slice(),
		clearLogs() {
			logs.length = 0;
		},
		size: () => ({ width: scene.width, height: scene.height }),
	};
	return api;
}

// Counts the requests of the page that are in flight (XMLHttpRequest and fetch), for `whenIdle`.
export function trackNetwork(win) {
	const network = { count: 0 };
	const done = (request) => {
		if (request.$counted === true) {
			request.$counted = false;
			network.count--;
		}
	};
	if (win.XMLHttpRequest !== undefined) {
		const send = win.XMLHttpRequest.prototype.send;
		win.XMLHttpRequest.prototype.send = function (...args) {
			if (this.$counted !== true) {
				this.$counted = true;
				network.count++;
				this.addEventListener('loadend', () => done(this));
			}
			return send.apply(this, args);
		};
	}
	if (typeof win.fetch === 'function') {
		const original = win.fetch.bind(win);
		win.fetch = (...args) => {
			network.count++;
			return original(...args).finally(() => {
				network.count--;
			});
		};
	}
	return network;
}
