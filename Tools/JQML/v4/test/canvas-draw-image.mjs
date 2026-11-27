// Context2D.drawImage(): the three forms of the arguments, and an address that was not loaded with Canvas.loadImage() is asked
// for and drawn by the next paint (desktop Qt shows it without a loadImage() call).
// `node test/canvas-draw-image.mjs`
import { CanvasContext } from '../src/canvas.js';

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}

const picture = { width: 32, height: 16 };
const pictures = new Map([['a.svg', picture]]);
const asked = [];
const output = [];
const ctx = new CanvasContext(100, 100, output, pictures, (url) => asked.push(url));
const drawn = () => output.splice(0).map((c) => [c.k, c.sx, c.sy, c.sw, c.sh, c.x, c.y, c.w, c.h]);

ctx.drawImage('a.svg', 5, 6);
check('at a point: the size of the picture', drawn(), [['image', 0, 0, 32, 16, 5, 6, 32, 16]]);

ctx.drawImage('a.svg', 5, 6, 20, 10);
check('into a rectangle', drawn(), [['image', 0, 0, 32, 16, 5, 6, 20, 10]]);

ctx.drawImage('a.svg', 1, 2, 3, 4, 5, 6, 7, 8);
check('a part into a rectangle', drawn(), [['image', 1, 2, 3, 4, 5, 6, 7, 8]]);

ctx.drawImage({ $img: picture }, 0, 0);
check('an Image item', drawn(), [['image', 0, 0, 32, 16, 0, 0, 32, 16]]);

ctx.drawImage('b.svg', 0, 0);
check('an address that is not loaded draws nothing', drawn(), []);
check('and is asked for', asked, ['b.svg']);

ctx.globalAlpha = 0.5;
ctx.drawImage('a.svg', 0, 0);
check('globalAlpha goes with the picture', output.splice(0)[0].alpha, 0.5);

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
