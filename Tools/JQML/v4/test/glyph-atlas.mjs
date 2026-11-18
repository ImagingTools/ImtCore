// The glyph atlas (src/fonts.js): four pages of 1024 x 1024 pixels; when all are full it is emptied (`generation` goes up) and the glyphs are
// rasterized again. The renderer only queues the draws of a frame (flush) and uploads the pages when it submits them, so a text that is
// queued before the atlas is emptied must still have its glyphs when the frame is submitted: the draws that are queued must be submitted
// before a page is cleared, or the page must stay. Large glyphs (a text under a scale of the canvas has glyphs up to 256 pixels) fill the
// atlas fast, so this is more likely there.
// The last check fails while the atlas clears a page that a queued draw refers to.
// `node test/glyph-atlas.mjs`

// There is no canvas in a node: a page is a stub that records nothing, and a font gives 60% of its size for an advance.
class StubContext {
	constructor() {
		this.font = '10px sans-serif';
	}

	measureText(text) {
		const size = Number(/([\d.]+)px/.exec(this.font)[1]);
		const width = size * 0.6 * String(text).length;
		return { width, actualBoundingBoxLeft: 0, actualBoundingBoxRight: width, actualBoundingBoxAscent: size * 0.8, actualBoundingBoxDescent: size * 0.2, fontBoundingBoxAscent: size * 0.9, fontBoundingBoxDescent: size * 0.25 };
	}

	clearRect() {}

	fillRect() {}

	fillText() {}

	save() {}

	restore() {}
}
globalThis.OffscreenCanvas = class {
	constructor(width, height) {
		this.width = width;
		this.height = height;
		this.context = new StubContext();
	}

	getContext() {
		return this.context;
	}
};

const { fonts } = await import('../src/fonts.js');
const { CanvasContext, replayCommands } = await import('../src/canvas.js');
const { Renderer } = await import('../src/renderer.js');

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `\n      got      ${JSON.stringify(actual)}\n      expected ${JSON.stringify(expected)}`}`);
}

const atlas = fonts.ensureAtlas();
check('the atlas is made when there is a canvas', atlas !== null, true);

// 256 different characters: with a font of 100 pixels they fill the pages of the atlas fast (about 140 glyphs on a page)
const TEXT = Array.from({ length: 256 }, (_, i) => String.fromCharCode(0x400 + i)).join('');

{
	const info = fonts.get('sans-serif', 100, false, false);
	const glyph = info.glyph('A');
	check('a glyph is in a page of the atlas', atlas.pages.includes(glyph.page), true);
	check('the same glyph is given again, not rasterized again', info.glyph('A') === glyph, true);
	check('a character with nothing to draw has no glyph or a blank one', typeof info.glyph(' ') === 'object', true);
}

{
	// fill the atlas with glyphs of sizes that are not used by the tests below
	const before = atlas.generation;
	const sizes = [];
	for (let size = 90; atlas.generation === before && size < 130; size++) {
		const info = fonts.get('sans-serif', size, false, false);
		for (const ch of TEXT) info.glyph(ch);
		sizes.push(size);
	}
	check('the pages of the atlas are at most four', atlas.pages.length <= 4, true);
	check('when the pages are full the atlas is emptied: its generation goes up', atlas.generation, before + 1);
	const first = fonts.get('sans-serif', sizes[0], false, false);
	const glyph = first.glyph(TEXT[0]);
	check('the glyphs of an emptied atlas are rasterized again, into the new generation', glyph !== null && atlas.pages.includes(glyph.page), true);
}

// A frame: the text of a scale is queued by a renderer, then the atlas is emptied by a text of another scale; every draw that is still queued
// must find its glyphs in its page when the frame is submitted. `clears` counts how many times a page was cleared.
{
	for (const page of atlas.pages) page.clears = 0;
	const clear = atlas.clearPage.bind(atlas);
	atlas.clearPage = (page) => {
		page.clears = (page.clears ?? 0) + 1;
		clear(page);
	};
	const newPage = atlas.newPage.bind(atlas);
	atlas.newPage = () => {
		const page = newPage();
		page.clears = 0;
		return page;
	};

	const renderer = new Renderer(null);
	const violations = [];
	// the draws that are queued remember how many times their page was cleared
	const flush = renderer.flush.bind(renderer);
	renderer.flush = () => {
		const before = renderer.cmds.length;
		flush();
		for (let i = before; i < renderer.cmds.length; i++) renderer.cmds[i].clears = renderer.cmds[i].page.clears;
	};
	const submit = renderer.submit.bind(renderer);
	renderer.submit = () => {
		renderer.flush();
		for (const cmd of renderer.cmds) {
			if (cmd.page !== null && cmd.page.clears !== cmd.clears) violations.push(`a draw queued before the page was cleared (${cmd.clears} -> ${cmd.page.clears})`);
		}
		submit();
		renderer.cmds = [];
		renderer.nv = 0;
		renderer.ni = 0;
		renderer.segStart = 0;
	};

	const generation = atlas.generation;
	const commands = [];
	const ctx = new CanvasContext(800, 600, commands);
	ctx.font = '40px sans-serif';
	ctx.fillStyle = '#000000';
	let y = 0;
	for (const scale of [2.5, 2.54, 2.58, 2.62, 2.66]) {
		ctx.save();
		ctx.scale(scale, scale);
		ctx.fillText(TEXT, 0, y += 10);
		ctx.restore();
	}
	replayCommands(renderer, commands, 1, false);
	check('the text of the frame has emptied the atlas', atlas.generation > generation, true);
	renderer.submit();
	check('a draw that is queued does not lose its glyphs when the atlas is emptied before the frame is submitted', violations.slice(0, 3), []);
	check('a renderer with nothing queued is not kept by the atlas', atlas.beforeReset.size, 0);

	// renderers that are made and dropped: the atlas keeps only those with queued geometry, and lets them go after the reset
	for (let i = 0; i < 10; i++) new Renderer(null).fillQuad(0, 0, 10, 10, 0xff000000);
	check('a renderer with queued geometry is submitted at the reset of the atlas', atlas.beforeReset.size, 10);
	atlas.reset();
	check('after the reset the atlas keeps no renderer', atlas.beforeReset.size, 0);
}

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
