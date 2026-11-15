// A small software rasterizer for the commands that a CanvasContext records, so the tests can read pixels (the engine draws with WebGL,
// and a Context2D of the engine has no getImageData). It knows what the pattern and image data tests draw: rect, poly, gpoly (a
// gradient), tpoly (a pattern) and bitmap (putImageData), each with the transform the command was recorded with. A pixel is the colour
// at its centre, as the GPU gives it; `over` is source-over on the colours of the canvas, like Qt.

export function rasterize(commands, width, height, background = [0, 0, 0, 0]) {
	const pixels = Array.from({ length: width * height }, () => background.slice());
	const put = (x, y, rgb, alpha) => {
		const a = alpha;
		if (a <= 0) return;
		const dst = pixels[y * width + x];
		const da = dst[3] / 255;
		const oa = a + da * (1 - a);
		for (let k = 0; k < 3; k++) dst[k] = oa === 0 ? 0 : (rgb[k] * a + dst[k] * da * (1 - a)) / oa;
		dst[3] = oa * 255;
	};
	for (const c of commands) {
		if (c.k === 'unclip' || c.k === 'clip' || c.k === 'text') continue;
		const m = c.m;
		const det = m[0] * m[3] - m[1] * m[2];
		for (let y = 0; y < height; y++) {
			for (let x = 0; x < width; x++) {
				const px = x + 0.5;
				const py = y + 0.5;
				// the point in the space the command was recorded in
				const lx = (m[3] * (px - m[4]) - m[2] * (py - m[5])) / det;
				const ly = (-m[1] * (px - m[4]) + m[0] * (py - m[5])) / det;
				if (c.k === 'rect') {
					if (lx >= c.x && lx < c.x + c.w && ly >= c.y && ly < c.y + c.h) put(x, y, [c.color.R, c.color.G, c.color.B], c.color.a);
				} else if (c.k === 'bitmap') {
					const u = Math.floor(lx - c.x);
					const v = Math.floor(ly - c.y);
					if (lx >= c.x && ly >= c.y && u < c.w && v < c.h) {
						const at = (v * c.w + u) * 4;
						put(x, y, [c.data[at], c.data[at + 1], c.data[at + 2]], (c.data[at + 3] / 255) * c.alpha);
					}
				} else if (c.k === 'tpoly') {
					const n = c.points.length / 2;
					for (let i = 1; i + 1 < n; i++) {
						const hit = barycentric(c.points, i, lx, ly);
						if (hit === null) continue;
						const u = hit[0] * c.uvs[0] + hit[1] * c.uvs[2 * i] + hit[2] * c.uvs[2 * i + 2];
						const v = hit[0] * c.uvs[1] + hit[1] * c.uvs[2 * i + 1] + hit[2] * c.uvs[2 * i + 3];
						const tx = Math.min(c.tile.w - 1, Math.max(0, Math.floor(u * c.tile.w)));
						const ty = Math.min(c.tile.h - 1, Math.max(0, Math.floor(v * c.tile.h)));
						const at = (ty * c.tile.w + tx) * 4;
						put(x, y, [c.tile.pixels[at], c.tile.pixels[at + 1], c.tile.pixels[at + 2]], (c.tile.pixels[at + 3] / 255) * c.alpha);
						break;
					}
				} else if (c.k === 'poly') {
					for (let i = 0; i + 2 < c.indices.length; i += 3) {
						const tri = [];
						for (let k = 0; k < 3; k++) tri.push(c.points[2 * c.indices[i + k]], c.points[2 * c.indices[i + k] + 1]);
						if (barycentric(tri, 1, lx, ly) !== null) {
							put(x, y, [c.color.R, c.color.G, c.color.B], c.color.a);
							break;
						}
					}
				} else if (c.k === 'gpoly') {
					const n = c.points.length / 2;
					for (let i = 1; i + 1 < n; i++) {
						const hit = barycentric(c.points, i, lx, ly);
						if (hit === null) continue;
						const [ca, cb, cc] = [c.colors[0], c.colors[i], c.colors[i + 1]];
						put(x, y, ['R', 'G', 'B'].map((k) => hit[0] * ca[k] + hit[1] * cb[k] + hit[2] * cc[k]), hit[0] * ca.a + hit[1] * cb.a + hit[2] * cc.a);
						break;
					}
				}
			}
		}
	}
	return {
		width,
		height,
		at: (x, y) => pixels[y * width + x].map((v) => Math.round(v)),
	};
}

// The weights of the triangle (point 0, point i, point i + 1) at (x, y), or null outside of it.
function barycentric(points, i, x, y) {
	const [ax, ay, bx, by, cx, cy] = [points[0], points[1], points[2 * i], points[2 * i + 1], points[2 * i + 2], points[2 * i + 3]];
	const d = (by - cy) * (ax - cx) + (cx - bx) * (ay - cy);
	if (d === 0) return null;
	const u = ((by - cy) * (x - cx) + (cx - bx) * (y - cy)) / d;
	const v = ((cy - ay) * (x - cx) + (ax - cx) * (y - cy)) / d;
	const w = 1 - u - v;
	const eps = -1e-9;
	return u >= eps && v >= eps && w >= eps ? [u, v, w] : null;
}
