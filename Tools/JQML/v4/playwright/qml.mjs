// The client of window.qmlTest for Playwright (or any object that has the same `page.evaluate` and `page.waitForFunction`).
//
//   import { qml } from '<path>/qml.mjs';
//   const app = qml(page);                                  // the whole application
//   await app.locator('loginPanel').locator('okButton').click();
//   const row = app.locator('listPanel').locator('row').nth(2);
//   await row.locator('title').waitForText('Pump 3');
//
// A locator is a description (the scope it starts from and a selector), not an item: it is resolved at every action, so it keeps
// working when the item is rebuilt, and an action waits until the item exists and can be clicked (like the locators of Playwright).
// `locator(selector)` on a locator looks only below the item that the locator found, so the objectName that many items have is no
// conflict when the search starts from their parents. The selector language is in src/testapi.js.

const DEFAULT_TIMEOUT = 5000;
const POLL_MS = 50;

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

// Runs `method` of window.qmlTest in the page.
function call(page, method, args) {
	return page.evaluate(([name, list]) => window.qmlTest[name](...list), [method, args]);
}

export class QmlError extends Error {}

export class QmlLocator {
	constructor(page, parent, selector, index, options) {
		this.page = page;
		this.parent = parent; // QmlLocator or null (the application)
		this.selector = selector;
		this.index = index; // null: exactly one item must match; a number: the item at the place (-1 is the last)
		this.options = options;
	}

	toString() {
		const base = this.parent === null ? '' : `${this.parent} >> `;
		const index = this.index === null ? '' : ` [${this.index}]`;
		return `${base}${typeof this.selector === 'string' ? this.selector : JSON.stringify(this.selector)}${index}`;
	}

	// The items below the one that this locator finds.
	locator(selector) {
		return new QmlLocator(this.page, this, selector, null, this.options);
	}

	first() {
		return this.nth(0);
	}

	last() {
		return this.nth(-1);
	}

	nth(index) {
		return new QmlLocator(this.page, this.parent, this.selector, index, this.options);
	}

	// ----- resolving -----

	// The handles of the items now: [] if there are none.
	async handles() {
		let scope = null;
		if (this.parent !== null) {
			scope = await this.parent.resolve();
			if (scope === null) return [];
		}
		return call(this.page, 'find', [scope === null ? null : scope.id, this.selector]);
	}

	// The handle of the item, or null: it fails when the locator has no index and several items match.
	async resolve() {
		const list = await this.handles();
		if (this.index === null) {
			if (list.length > 1) {
				throw new QmlError(`'${this}' found ${list.length} items; use nth(), first() or a more exact parent: ${list.map((h) => h.objectName || h.qmlType).join(', ')}`);
			}
			return list.length === 1 ? list[0] : null;
		}
		const at = this.index < 0 ? list.length + this.index : this.index;
		return at >= 0 && at < list.length ? list[at] : null;
	}

	async count() {
		return (await this.handles()).length;
	}

	// All the items that match, as locators of their own (with the index at the time).
	async all() {
		const count = await this.count();
		return Array.from({ length: count }, (_, i) => this.nth(i));
	}

	// ----- waiting -----

	timeout(options) {
		return options && options.timeout !== undefined ? options.timeout : this.options.timeout;
	}

	// Calls `step()` until it gives a value that is not undefined, for `timeout` ms. step() may throw STALE or NOT_ACTIONABLE: that is a
	// reason to try again, and the last one is what the failure says.
	async retry(what, step, options) {
		const deadline = Date.now() + this.timeout(options);
		let last = '';
		for (;;) {
			try {
				const result = await step();
				if (result !== undefined) return result;
				last = 'the item does not exist';
			} catch (error) {
				const message = String(error.message);
				if (!/STALE:|NOT_ACTIONABLE:/.test(message)) throw error;
				last = message.replace(/^.*?(STALE|NOT_ACTIONABLE): /, '');
			}
			if (Date.now() >= deadline) throw new QmlError(`${what} '${this}': ${last} (timeout ${this.timeout(options)} ms)`);
			await sleep(POLL_MS);
		}
	}

	// The handle of the item, waiting until it exists.
	async wait(what, options) {
		return this.retry(what, async () => {
			const handle = await this.resolve();
			return handle === null ? undefined : handle;
		}, options);
	}

	async waitFor(options = {}) {
		const state = options.state || 'visible';
		if (state === 'attached') return void (await this.wait('waiting for', options));
		if (state === 'detached') {
			await this.retry('waiting for the removal of', async () => ((await this.resolve()) === null ? true : undefined), options);
			return;
		}
		await this.retry(`waiting until it is ${state}`, async () => {
			const handle = await this.resolve();
			if (handle === null) return state === 'hidden' ? true : undefined;
			const info = await call(this.page, 'info', [handle.id]);
			return (state === 'visible') === info.visible ? true : undefined;
		}, options);
	}

	// ----- reading -----

	async info(options) {
		const handle = await this.wait('reading', options);
		return call(this.page, 'info', [handle.id]);
	}

	async get(name, options) {
		const handle = await this.wait('reading', options);
		return call(this.page, 'get', [handle.id, name]);
	}

	async text(options) {
		return (await this.info(options)).text;
	}

	async isVisible() {
		const handle = await this.resolve();
		return handle !== null && (await call(this.page, 'info', [handle.id])).visible;
	}

	async isEnabled() {
		const handle = await this.resolve();
		return handle !== null && (await call(this.page, 'info', [handle.id])).enabled;
	}

	// The box in the coordinates of the page, like boundingBox() of Playwright.
	async boundingBox(options) {
		return (await this.info(options)).box;
	}

	// Waits until `read(info)` gives the expected value (a value, a RegExp or a function), like expect(...).toHaveText().
	async waitForValue(what, read, expected, options) {
		const test = typeof expected === 'function' ? expected : expected instanceof RegExp ? (v) => typeof v === 'string' && expected.test(v) : (v) => v === expected;
		let seen;
		try {
			await this.retry(what, async () => {
				const handle = await this.resolve();
				if (handle === null) return undefined;
				seen = await read(handle);
				return test(seen) ? true : undefined;
			}, options);
		} catch (error) {
			if (error instanceof QmlError) throw new QmlError(`${error.message}; the last value was ${JSON.stringify(seen)}`);
			throw error;
		}
	}

	waitForText(expected, options) {
		return this.waitForValue('waiting for the text of', async (h) => (await call(this.page, 'info', [h.id])).text, expected, options);
	}

	waitForProperty(name, expected, options) {
		return this.waitForValue(`waiting for '${name}' of`, (h) => call(this.page, 'get', [h.id, name]), expected, options);
	}

	// ----- actions -----

	async act(what, method, args, options) {
		return this.retry(what, async () => {
			const handle = await this.resolve();
			return handle === null ? undefined : call(this.page, method, [handle.id, ...args]);
		}, options);
	}

	click(options = {}) {
		return this.act('clicking', 'click', [options], options);
	}

	dblclick(options = {}) {
		return this.act('double clicking', 'click', [{ ...options, clickCount: 2 }], options);
	}

	hover(options = {}) {
		return this.act('hovering', 'hover', [options], options);
	}

	focus(options) {
		return this.act('focusing', 'focus', [], options);
	}

	scrollIntoViewIfNeeded(options) {
		return this.act('scrolling to', 'scrollIntoView', [], options);
	}

	fill(text, options) {
		return this.act('filling', 'fill', [text], options);
	}

	// Types into the item: the focus first, then the keys.
	async type(text, options = {}) {
		await this.focus(options);
		await call(this.page, 'type', [text, options]);
	}

	async press(key, options = {}) {
		await this.focus(options);
		await call(this.page, 'press', [key, options]);
	}

	wheel(deltaX, deltaY, options) {
		return this.act('scrolling', 'wheel', [deltaX, deltaY], options);
	}

	set(name, value, options) {
		return this.act(`setting '${name}' of`, 'set', [name, value], options);
	}

	callMethod(name, args = [], options) {
		return this.act(`calling '${name}' of`, 'call', [name, args], options);
	}

	// The text tree below the item, for the message of a test that fails.
	async dump(options = {}) {
		const handle = await this.wait('dumping', options);
		return call(this.page, 'dump', [handle.id, options]);
	}
}

// The locator of the whole application. Waits until the application has started (`started` of window.qmlTest).
export function qml(page, options = {}) {
	const settings = { timeout: options.timeout === undefined ? DEFAULT_TIMEOUT : options.timeout };
	const root = new QmlLocator(page, null, '*', 0, settings);
	return new QmlApplication(page, settings, root);
}

export class QmlApplication {
	constructor(page, options, root) {
		this.page = page;
		this.options = options;
		this.root = root;
	}

	locator(selector) {
		return new QmlLocator(this.page, null, selector, null, this.options);
	}

	// Resolves when window.qmlTest exists, the application has loaded its first document and has nothing to do.
	async ready(options = {}) {
		await this.page.waitForFunction(() => window.qmlTest !== undefined && window.qmlTest.started === true, undefined, { timeout: options.timeout === undefined ? 30000 : options.timeout });
		await this.idle(options);
	}

	// Resolves when the application is not busy: no frame is wanted, no animation, picture or request is in progress.
	async idle(options = {}) {
		const ok = await call(this.page, 'whenIdle', [{ timeout: options.timeout === undefined ? 10000 : options.timeout }]);
		if (!ok) throw new QmlError('the application is still busy after the timeout');
	}

	async press(key, options = {}) {
		await call(this.page, 'press', [key, options]);
	}

	async type(text, options = {}) {
		await call(this.page, 'type', [text, options]);
	}

	// What the QML engine reported as errors, and what the application printed.
	errors() {
		return call(this.page, 'errors', []);
	}

	logs() {
		return call(this.page, 'logs', []);
	}

	async dump(options = {}) {
		return call(this.page, 'dump', [null, options]);
	}
}
