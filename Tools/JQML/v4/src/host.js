// Connects a scene to a canvas of the page: size, pointer and keyboard input, the system clipboard and
// the animation loop. Shared by the viewer page (app.js) and by compiled applications (runtime.js).

// options.afterFrame: () => void, called after every frame (the viewer page updates its error log).
export function attachCanvas(canvas, scene, engine, options = {}) {
	const afterFrame = options.afterFrame || null;

	function updateSize() {
		scene.resize(Math.max(1, canvas.clientWidth), Math.max(1, canvas.clientHeight), window.devicePixelRatio || 1);
	}

	if (typeof ResizeObserver === 'function') new ResizeObserver(updateSize).observe(canvas);
	else window.addEventListener('resize', updateSize);
	updateSize();

	function position(event) {
		const rect = canvas.getBoundingClientRect();
		return [event.clientX - rect.left, event.clientY - rect.top];
	}

	canvas.addEventListener('pointermove', (e) => scene.pointerMove(...position(e)));
	canvas.addEventListener('pointerdown', (e) => {
		if (e.button !== 0 && e.button !== 2) return;
		canvas.setPointerCapture(e.pointerId);
		scene.pointerDown(...position(e), e.button);
	});
	canvas.addEventListener('pointerup', (e) => {
		if (e.button !== 0 && e.button !== 2) return;
		scene.pointerUp(...position(e));
	});
	canvas.addEventListener('dblclick', (e) => {
		if (e.button !== 0 && e.button !== 2) return;
		scene.pointerDoubleClick(...position(e), e.button);
	});
	canvas.addEventListener('contextmenu', (e) => e.preventDefault());
	canvas.addEventListener('pointerleave', () => {
		if (scene.pressed === null) scene.pointerLeave();
	});
	canvas.addEventListener(
		'wheel',
		(e) => {
			e.preventDefault();
			const [x, y] = position(e);
			scene.wheel(x, y, -e.deltaY / (e.deltaMode === 1 ? 3 : 100));
		},
		{ passive: false },
	);

	// ----- keyboard and clipboard -----

	canvas.tabIndex = 0;
	canvas.style.outline = 'none';
	canvas.addEventListener('pointerdown', () => canvas.focus());

	// Keys typed into the page's own controls (the log panel, a file input) are not for the scene.
	function isPageControl(target) {
		return target instanceof HTMLElement && (target.isContentEditable || /^(INPUT|TEXTAREA|SELECT|BUTTON)$/.test(target.tagName));
	}

	const isPaste = (e) => (e.ctrlKey || e.metaKey) && !e.altKey && (e.key === 'v' || e.key === 'V' || e.code === 'KeyV');
	let pasteFallback = 0;

	// The scene copies through the engine; the page mirrors it to the system clipboard when allowed.
	engine.onClipboardWrite = (text, html) => {
		const write = async () => {
			try {
				if (html && typeof ClipboardItem === 'function') {
					await navigator.clipboard.write([
						new ClipboardItem({
							'text/plain': new Blob([text], { type: 'text/plain' }),
							'text/html': new Blob([html], { type: 'text/html' }),
						}),
					]);
				} else {
					await navigator.clipboard.writeText(text);
				}
			} catch {
				// Not allowed (no user gesture, no permission): the engine clipboard still works inside the page.
			}
		};
		write();
	};

	function deliverPaste(text, html) {
		clearTimeout(pasteFallback);
		engine.clipboard = { text, html };
		scene.keyDown({ key: 'v', code: 'KeyV', ctrlKey: true });
		scene.keyUp({ key: 'v', code: 'KeyV', ctrlKey: true });
	}

	window.addEventListener('keydown', (e) => {
		if (isPageControl(e.target) || e.isComposing) return;
		// Ctrl+V: the content of the system clipboard arrives with the `paste` event, so the scene gets the key then.
		if (isPaste(e)) {
			pasteFallback = setTimeout(async () => {
				try {
					deliverPaste(await navigator.clipboard.readText(), '');
				} catch {
					deliverPaste(engine.clipboard.text, engine.clipboard.html);
				}
			}, 250);
			return;
		}
		if (scene.keyDown(e)) e.preventDefault();
		// Tab belongs to the scene: left to the browser it would move the page focus off the canvas and the keys would stop arriving.
		else if (e.key === 'Tab') e.preventDefault();
	});
	window.addEventListener('keyup', (e) => {
		if (isPageControl(e.target) || isPaste(e)) return;
		if (scene.keyUp(e)) e.preventDefault();
	});
	window.addEventListener('paste', (e) => {
		if (isPageControl(e.target) || !e.clipboardData) return;
		e.preventDefault();
		deliverPaste(e.clipboardData.getData('text/plain'), e.clipboardData.getData('text/html'));
	});

	// ----- loop -----

	function loop(milliseconds) {
		// An exception that escapes a frame is reported like any other error; the loop must survive it,
		// otherwise the whole page stops responding.
		try {
			updateSize();
			scene.drawFrame(milliseconds / 1000);
		} catch (error) {
			engine.report('frame', error, 0);
		}
		if (afterFrame !== null) afterFrame();
		requestAnimationFrame(loop);
	}
	requestAnimationFrame(loop);

	return { updateSize };
}
