// A text drawn under a scale of the canvas (ctx.scale(), the zoom of a document) is made of the glyphs of the font of the scaled size, drawn
// under the matrix without the scale, not of the stretched glyphs of the nominal size. The renderer here only records the glyphs: the font
// size they come from, the matrix and the place at the time of the call. In a node there is no canvas to measure with, so the font gives
// 55% of its size for the advance of every character and the glyphs are labels.
// `node test/canvas-scaled-text.mjs`
import { CanvasContext, replayCommands } from '../src/canvas.js';
import { fonts } from '../src/fonts.js';

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `\n      got      ${JSON.stringify(actual)}\n      expected ${JSON.stringify(expected)}`}`);
}

fonts.rasterize = (info, ch) => ({ ch, size: info.size });

class Recorder {
	constructor() {
		this.ma = 1;
		this.mb = 0;
		this.mc = 0;
		this.md = 1;
		this.me = 0;
		this.mf = 0;
		this.glyphs = [];
	}

	setMatrix(a, b, c, d, e, f) {
		[this.ma, this.mb, this.mc, this.md, this.me, this.mf] = [a, b, c, d, e, f];
	}

	pack(color) {
		return color;
	}

	// where the glyph lands on the screen: the matrix at the time of the call applied to the place
	glyph(glyph, x, y) {
		this.glyphs.push({
			ch: glyph.ch,
			size: glyph.size,
			x: this.ma * x + this.mc * y + this.me,
			y: this.mb * x + this.md * y + this.mf,
			scale: this.ma,
		});
	}
}

// The glyphs of `text` at (20, 40), drawn under `setup` (a function of the context) with a font of 20 pixels; the recorder after the replay.
function draw(setup, text = 'Ab', font = '20px sans-serif') {
	const commands = [];
	const ctx = new CanvasContext(400, 300, commands);
	ctx.font = font;
	ctx.fillStyle = '#000000';
	setup(ctx);
	ctx.fillText(text, 20, 40);
	const r = new Recorder();
	replayCommands(r, commands, 1, false);
	return r;
}

const round = (v) => Math.round(v * 1000) / 1000;
const advance = (size) => size * 0.55;

{
	const r = draw(() => {});
	check('no scale: the glyphs of the nominal size, as they were', r.glyphs.map((g) => [g.ch, g.size, round(g.x), round(g.y)]), [['A', 20, 20, 40], ['b', 20, round(20 + advance(20)), 40]]);
}

{
	const r = draw((ctx) => ctx.scale(2, 2));
	check('scale 2: the glyphs of the font of twice the size', r.glyphs.map((g) => g.size), [40, 40]);
	// A text laid out at (20, 40) lands at (40, 80); the second glyph is one advance of the nominal font (scaled) after the first.
	check('scale 2: the places of the glyphs on the screen', r.glyphs.map((g) => [round(g.x), round(g.y)]), [[40, 80], [round(40 + 2 * advance(20)), 80]]);
	check('scale 2: the glyphs are drawn under a matrix without the scale', r.glyphs.map((g) => round(g.scale)), [1, 1]);
	check('the matrix of the renderer is what it was after the replay', [r.ma, r.mb, r.mc, r.md, r.me, r.mf], [1, 0, 0, 1, 0, 0]);
}

{
	const r = draw((ctx) => {
		ctx.translate(100, 50);
		ctx.scale(1.5, 1.5);
	});
	check('translate and scale 1.5: the font of 30 pixels at the translated place', r.glyphs.map((g) => [g.size, round(g.x), round(g.y)]), [[30, 130, 110], [30, round(130 + 1.5 * advance(20)), 110]]);
}

{
	const r = draw((ctx) => ctx.scale(1.003, 1.003));
	check('a scale within 1% of one is left alone', r.glyphs.map((g) => [g.size, round(g.scale)]), [[20, 1.003], [20, 1.003]]);
}

{
	const r = draw((ctx) => ctx.scale(0.5, 0.5));
	check('a scale below one is left alone: the nominal glyphs, drawn under the matrix', r.glyphs.map((g) => [g.size, g.scale]), [[20, 0.5], [20, 0.5]]);
}

{
	const r = draw((ctx) => ctx.scale(2, 3));
	check('a scale that is not uniform is left alone', r.glyphs.map((g) => g.size), [20, 20]);
}

{
	const r = draw((ctx) => {
		ctx.rotate(Math.PI / 6);
		ctx.scale(2, 2);
	});
	check('a rotation is left alone', r.glyphs.map((g) => g.size), [20, 20]);
}

{
	const r = draw((ctx) => ctx.scale(100, 100));
	check('the size of the glyphs is at most 256 pixels', r.glyphs.map((g) => g.size), [256, 256]);
	// the layout is the one of the nominal font scaled by the matrix: 256 / 20 of it is the factor of the glyphs
	check('and the pen still moves by the advance of the nominal font', round(r.glyphs[1].x - r.glyphs[0].x), round(100 * advance(20)));
}

{
	const r = draw((ctx) => ctx.scale(1.1, 1.1));
	// 20 * 1.1 = 22: on a quarter of a pixel
	const r2 = draw((ctx) => ctx.scale(1.1004, 1.1004));
	check('a zoom of small steps shares the font: 1.1 and 1.1004 give the same size', [r.glyphs[0].size, r2.glyphs[0].size], [22, 22]);
}

{
	const r = draw((ctx) => ctx.scale(2, 2), 'AbC', 'bold 12px sans-serif');
	check('a bold font stays bold: the font of the scaled size is of the same style', r.glyphs.map((g) => g.size), [24, 24, 24]);
	const font = fonts.get('sans-serif', 24, true, false);
	check('the scaled font is the bold one of the scaled size', font.style.bold, true);
}

{
	// the built-in box font (no family) keeps its glyphs: it has no scaled font
	const commands = [];
	const ctx = new CanvasContext(400, 300, commands);
	ctx.scale(2, 2);
	ctx.fillStyle = '#000000';
	ctx.fillText('A', 10, 20);
	const r = new Recorder();
	replayCommands(r, commands, 1, false);
	const style = commands[0].info.style;
	check('the glyphs of a font without a family are not changed', style === null ? r.glyphs.map((g) => g.size) : 'has a family', style === null ? [commands[0].info.size] : 'has a family');
}

{
	// the text after a scaled one is drawn under its own matrix
	const commands = [];
	const ctx = new CanvasContext(400, 300, commands);
	ctx.font = '20px sans-serif';
	ctx.fillStyle = '#000000';
	ctx.save();
	ctx.scale(2, 2);
	ctx.fillText('A', 10, 20);
	ctx.restore();
	ctx.fillText('B', 10, 20);
	const r = new Recorder();
	replayCommands(r, commands, 1, false);
	check('a text after a scaled one: the places and the sizes', r.glyphs.map((g) => [g.ch, g.size, round(g.x), round(g.y)]), [['A', 40, 20, 40], ['B', 20, 10, 20]]);
}

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
