// The scene: the root item, the frame (layout, bindings, painting) and the mouse input.

import { getFocusItem, Item, setDestroyObserver, setFocusItem, setFrameLoop } from './items.js';

const MAX_PASSES = 10;

// Qt::CursorShape → CSS `cursor` on the host canvas.
const CURSOR_CSS = Object.freeze({
	0: 'default', // ArrowCursor
	1: 'n-resize', // UpArrowCursor
	2: 'crosshair', // CrossCursor
	3: 'wait', // WaitCursor
	4: 'text', // IBeamCursor
	5: 'ns-resize', // SizeVerCursor
	6: 'ew-resize', // SizeHorCursor
	7: 'nesw-resize', // SizeBDiagCursor
	8: 'nwse-resize', // SizeFDiagCursor
	9: 'move', // SizeAllCursor
	10: 'none', // BlankCursor
	11: 'row-resize', // SplitVCursor
	12: 'col-resize', // SplitHCursor
	13: 'pointer', // PointingHandCursor
	14: 'not-allowed', // ForbiddenCursor
	15: 'help', // WhatsThisCursor
	16: 'progress', // BusyCursor
	17: 'grab', // OpenHandCursor
	18: 'grabbing', // ClosedHandCursor
});

function cssCursor(shape) {
	if (typeof shape === 'number' && CURSOR_CSS[shape] !== undefined) return CURSOR_CSS[shape];
	if (typeof shape === 'string') {
		const named = {
			ArrowCursor: 'default',
			PointingHandCursor: 'pointer',
			IBeamCursor: 'text',
			WaitCursor: 'wait',
			BusyCursor: 'progress',
			ForbiddenCursor: 'not-allowed',
			SizeAllCursor: 'move',
			SizeHorCursor: 'ew-resize',
			SizeVerCursor: 'ns-resize',
			SplitHCursor: 'col-resize',
			SplitVCursor: 'row-resize',
			OpenHandCursor: 'grab',
			ClosedHandCursor: 'grabbing',
			CrossCursor: 'crosshair',
			BlankCursor: 'none',
			WhatsThisCursor: 'help',
		};
		if (named[shape] !== undefined) return named[shape];
		if (CURSOR_CSS[shape] !== undefined) return CURSOR_CSS[shape];
	}
	return 'default';
}

export class Scene {
	constructor(renderer, engine) {
		this.renderer = renderer;
		this.engine = engine;
		this.root = new Item();
		this.root.$typeName = 'Scene';
		this.root.$isFocusScope = true;
		this.width = 0;
		this.height = 0;
		this.dpr = 1;
		this.hovered = null;
		this.pressed = null;
		this.pressedButton = 0;
		this.$cursorCss = '';

		this.frameHandler = null; // (seconds) => void, called before every frame
		this.frames = 0; // painted frames since the last reset of the statistics
		this.layoutMs = 0;
		this.paintMs = 0;
		this.skipNextCount = false; // the next painted frame is not counted

		setDestroyObserver((item) => {
			if (this.hovered === item) this.hovered = null;
			if (this.pressed === item) this.pressed = null;
			if (getFocusItem() === item) setFocusItem(null);
			this.syncCursor();
		});
	}

	resize(width, height, dpr) {
		if (width === this.width && height === this.height && dpr === this.dpr) return;
		this.width = width;
		this.height = height;
		this.dpr = dpr;
		this.root.width = width;
		this.root.height = height;
		this.root.$update();
	}

	// Runs one frame; returns true if something was painted.
	drawFrame(seconds) {
		if (this.frameHandler !== null) this.frameHandler(seconds);
		const root = this.root;
		if ((!root.$updateRequested && this.engine.dirty.size === 0) || this.width <= 0 || this.height <= 0) {
			this.layoutMs = 0;
			this.paintMs = 0;
			return false;
		}

		const start = performance.now();
		setFrameLoop(true);
		try {
			const engine = this.engine;
			for (let pass = 0; pass < MAX_PASSES; pass++) {
				root.$updateRequested = false;
				root.$layout();
				if (engine.bindingCount > 0) engine.evaluateBindings();
				if (!root.$updateRequested && engine.dirty.size === 0) break;
			}
			if (engine.dirty.size > 0) root.$updateRequested = true;
			if (engine.auditMode && engine.auditSweep() > 0) root.$updateRequested = true;
		} finally {
			setFrameLoop(false);
		}
		const layoutDone = performance.now();

		const r = this.renderer;
		r.beginFrame(this.width, this.height, this.dpr, 24, 24, 28);
		root.$render(r, 1);
		r.endFrame();
		const end = performance.now();

		// The frame that only redraws the statistics text must not replace the numbers it shows.
		if (this.skipNextCount) {
			this.skipNextCount = false;
		} else {
			this.layoutMs = layoutDone - start;
			this.paintMs = end - layoutDone;
			this.frames++;
		}
		// Bindings may change cursorShape (e.g. containsPress) during the frame.
		this.syncCursor();
		return true;
	}

	// ----- input (CSS pixel coordinates in the scene) -----

	pointerMove(x, y) {
		if (this.pressed !== null) this.pressed.$mouseMoveAt(x, y);
		this.setHovered(this.root.$itemAt(x, y));
		if (this.hovered !== null && this.hovered !== this.pressed) this.hovered.$mouseMoveAt(x, y);
		this.syncCursor();
	}

	pointerDown(x, y, button = 0) {
		const target = this.root.$itemAt(x, y, false, button);
		this.setHovered(button === 0 ? target : this.root.$itemAt(x, y));
		this.pressed = target;
		this.pressedButton = button;
		if (target !== null) {
			target.$pressButton = button;
			target.$pressChanged(true);
			target.$mousePressAt(x, y);
		}
		this.syncCursor();
	}

	pointerUp(x, y) {
		const target = this.pressed;
		this.pressed = null;
		if (target !== null) {
			target.$pressChanged(false);
			if (this.root.$itemAt(x, y, false, this.pressedButton) === target) target.$mouseClicked();
		}
		this.syncCursor();
	}

	pointerLeave() {
		this.setHovered(null);
		this.syncCursor();
	}

	// Applies MouseArea.cursorShape to the host canvas (pressed area wins over hover).
	syncCursor() {
		const el = this.renderer !== null ? this.renderer.canvas : null;
		if (el === null || el.style === undefined) return;
		const target = this.pressed !== null ? this.pressed : this.hovered;
		const shape = target !== null && target.$cursorShape !== undefined ? target.$cursorShape : 0;
		const css = cssCursor(shape);
		if (css === this.$cursorCss) return;
		this.$cursorCss = css;
		el.style.cursor = css;
	}

	wheel(x, y, notches) {
		const target = this.root.$itemAt(x, y, true);
		if (target !== null) target.$mouseWheel(notches);
	}

	// Key events go to the item with the active focus; true if a handler accepted the event.
	keyDown(event) {
		return this.engine.dispatchKey(event, true);
	}

	keyUp(event) {
		return this.engine.dispatchKey(event, false);
	}

	setHovered(target) {
		if (target === this.hovered) return;
		const previous = this.hovered;
		this.hovered = target;
		if (previous !== null) previous.$hoverChanged(false);
		if (target !== null) target.$hoverChanged(true);
	}
}
