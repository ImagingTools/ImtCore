// `Qt.md5(string)`: the MD5 hash of the UTF-8 bytes of the text as 32 lower case hexadecimal digits (what QCryptographicHash gives in Qt).
const SHIFTS = [7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21];
const SINES = Array.from({ length: 64 }, (_, i) => Math.floor(Math.abs(Math.sin(i + 1)) * 4294967296) >>> 0);

export function md5(value) {
	const bytes = new TextEncoder().encode(String(value));
	const total = ((bytes.length + 8) >> 6) + 1;
	const words = new Int32Array(total * 16);
	for (let i = 0; i < bytes.length; i++) words[i >> 2] |= bytes[i] << ((i % 4) * 8);
	words[bytes.length >> 2] |= 0x80 << ((bytes.length % 4) * 8);
	words[total * 16 - 2] = (bytes.length * 8) | 0;
	words[total * 16 - 1] = Math.floor((bytes.length * 8) / 4294967296);
	let a0 = 0x67452301;
	let b0 = 0xefcdab89 | 0;
	let c0 = 0x98badcfe | 0;
	let d0 = 0x10325476;
	for (let block = 0; block < total; block++) {
		let a = a0;
		let b = b0;
		let c = c0;
		let d = d0;
		for (let i = 0; i < 64; i++) {
			let f;
			let g;
			if (i < 16) {
				f = (b & c) | (~b & d);
				g = i;
			} else if (i < 32) {
				f = (d & b) | (~d & c);
				g = (5 * i + 1) % 16;
			} else if (i < 48) {
				f = b ^ c ^ d;
				g = (3 * i + 5) % 16;
			} else {
				f = c ^ (b | ~d);
				g = (7 * i) % 16;
			}
			const sum = (a + f + SINES[i] + words[block * 16 + g]) | 0;
			a = d;
			d = c;
			c = b;
			const shift = SHIFTS[(i >> 4) * 4 + (i % 4)];
			b = (b + ((sum << shift) | (sum >>> (32 - shift)))) | 0;
		}
		a0 = (a0 + a) | 0;
		b0 = (b0 + b) | 0;
		c0 = (c0 + c) | 0;
		d0 = (d0 + d) | 0;
	}
	let hex = '';
	for (const word of [a0, b0, c0, d0]) for (let i = 0; i < 4; i++) hex += ((word >>> (i * 8)) & 0xff).toString(16).padStart(2, '0');
	return hex;
}
