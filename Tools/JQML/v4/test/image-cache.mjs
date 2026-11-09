// A picture that many items show (the arrows of a tree) is requested from the server once: the items that ask while it is
// loading wait for it, the items that ask later get it at once, a failed request is asked again.
// `node test/image-cache.mjs`
import { Engine } from '../src/engine.js';

const requests = [];
globalThis.document = {};
globalThis.Image = class {
	set src(value) {
		requests.push(value);
		setTimeout(() => (value.includes('missing') ? this.onerror() : this.onload()), 0);
	}

	get naturalWidth() {
		return 8;
	}
};

let failures = 0;
function check(name, actual, expected) {
	const same = JSON.stringify(actual) === JSON.stringify(expected);
	if (!same) failures++;
	console.log(`${same ? '[+]' : '[-]'} ${name}${same ? '' : `: got ${JSON.stringify(actual)}, expected ${JSON.stringify(expected)}`}`);
}

const engine = new Engine();
engine.lenient = true;

function makeImage(source) {
	return {
		$source: source,
		$directory: '',
		$destroyed: false,
		status: 0,
		picture: null,
		$setImage(picture, status) {
			this.picture = picture;
			this.status = status;
		},
	};
}
const wait = () => new Promise((resolve) => setTimeout(resolve, 20));

const first = [makeImage('arrow.png'), makeImage('arrow.png'), makeImage('arrow.png')];
for (const image of first) engine.loadImage(image, image.$source);
check('items that ask together send one request', requests.length, 1);
await wait();
check('and all of them get the picture', first.map((image) => image.status), [1, 1, 1]);
check('the same picture', first[0].picture !== null && first[0].picture === first[2].picture, true);

const late = makeImage('arrow.png');
engine.loadImage(late, 'arrow.png');
engine.flushImages();
check('an item that asks later gets it at once, without a request', [requests.length, late.status, late.picture === first[0].picture], [1, 1, true]);

const other = makeImage('other.png');
engine.loadImage(other, 'other.png');
await wait();
check('another picture is requested', requests.length, 2);

const changed = makeImage('arrow.png');
engine.loadImage(changed, 'arrow.png');
changed.$source = 'other.png';
engine.flushImages();
check('an item whose source changed meanwhile does not get the old picture', changed.status, 0);

const gone = makeImage('wait.png');
const alive = makeImage('wait.png');
engine.loadImage(gone, 'wait.png');
engine.loadImage(alive, 'wait.png');
gone.$destroyed = true;
await wait();
check('a destroyed item is skipped, the others still get the picture', [gone.status, alive.status], [0, 1]);

const missing = [makeImage('missing.png'), makeImage('missing.png')];
for (const image of missing) engine.loadImage(image, image.$source);
const before = requests.length;
await wait();
check('a missing picture is an error for all that waited', missing.map((image) => image.status), [3, 3]);
const again = makeImage('missing.png');
engine.loadImage(again, 'missing.png');
check('a failed request is not kept: the next item asks again', requests.length, before + 1);
await wait();

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
