// WebGL2 renderer. All geometry (rectangles, anti-aliased polygons, glyphs) goes through one shader
// and one batch of triangles; the glyph atlas page that is bound also provides a white texel for
// untextured shapes. Items draw in their local coordinates through a 2D matrix stack
// (x' = a*x + c*y + e, y' = b*x + d*y + f; units are CSS pixels).
//
// Without a canvas (tests under Node) the renderer builds the same batches and drops them.

import { fonts } from './fonts.js';

const MAX_VERTICES = 1 << 16;
const WORDS = 5; // x, y, u, v, color
const FRINGE_MITER_LIMIT = 3;
const DRAW = 0;
const STATE = 1;

const VERTEX_SHADER = `#version 300 es
in vec2 aPos;
in vec2 aUv;
in vec4 aColor;
uniform vec2 uView;
out vec2 vUv;
out vec4 vColor;
void main() {
	vec2 p = aPos / uView * 2.0 - 1.0;
	gl_Position = vec4(p.x, -p.y, 0.0, 1.0);
	vUv = aUv;
	vColor = aColor;
}`;

const FRAGMENT_SHADER = `#version 300 es
precision mediump float;
in vec2 vUv;
in vec4 vColor;
uniform sampler2D uTex;
out vec4 outColor;
void main() {
	vec4 c = vColor;
	c.rgb *= c.a;
	outColor = texture(uTex, vUv) * c;
}`;

export class Renderer {
	// canvas: HTMLCanvasElement/OffscreenCanvas, or null for a renderer that draws nothing.
	constructor(canvas) {
		this.canvas = canvas;
		this.gl = null;
		this.dpr = 1;
		this.width = 0; // CSS pixels
		this.height = 0;

		this.ma = 1;
		this.mb = 0;
		this.mc = 0;
		this.md = 1;
		this.me = 0;
		this.mf = 0;

		const buffer = new ArrayBuffer(MAX_VERTICES * WORDS * 4);
		this.f32 = new Float32Array(buffer);
		this.u32 = new Uint32Array(buffer);
		this.indices = new Uint32Array(MAX_VERTICES * 3);
		this.nv = 0;
		this.ni = 0;
		this.segStart = 0; // first index of the draw range being recorded
		this.cmds = []; // recorded draw ranges and state changes, executed by submit()

		this.sx = new Float64Array(64);
		this.sy = new Float64Array(64);
		this.nx = new Float64Array(64);
		this.ny = new Float64Array(64);
		this.quadPoints = new Float64Array(8);
		this.quadIndices = new Uint32Array([0, 1, 2, 0, 2, 3]);

		this.page = null;
		this.whiteUv = 2 / 1024;
		this.clipX0 = 0; // device pixels, top-left origin
		this.clipY0 = 0;
		this.clipX1 = 0;
		this.clipY1 = 0;
		this.stencilLevel = 0;

		this.drawCalls = 0;
		this.vertexCount = 0;

		const atlas = fonts.ensureAtlas();
		if (atlas !== null) {
			this.page = atlas.pages[0];
			this.whiteUv = 2 / this.page.size;
		}

		if (canvas !== null) this.initGl();
	}

	initGl() {
		const gl = this.canvas.getContext('webgl2', {
			alpha: false,
			antialias: false,
			stencil: true,
			depth: false,
			preserveDrawingBuffer: false,
			powerPreference: 'high-performance',
		});
		if (gl === null) throw new Error('WebGL2 is not available');
		this.gl = gl;

		const compile = (type, source) => {
			const shader = gl.createShader(type);
			gl.shaderSource(shader, source);
			gl.compileShader(shader);
			if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
				throw new Error(`Shader error: ${gl.getShaderInfoLog(shader)}`);
			}
			return shader;
		};
		const program = gl.createProgram();
		gl.attachShader(program, compile(gl.VERTEX_SHADER, VERTEX_SHADER));
		gl.attachShader(program, compile(gl.FRAGMENT_SHADER, FRAGMENT_SHADER));
		gl.bindAttribLocation(program, 0, 'aPos');
		gl.bindAttribLocation(program, 1, 'aUv');
		gl.bindAttribLocation(program, 2, 'aColor');
		gl.linkProgram(program);
		if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
			throw new Error(`Program error: ${gl.getProgramInfoLog(program)}`);
		}
		gl.useProgram(program);
		this.uView = gl.getUniformLocation(program, 'uView');
		gl.uniform1i(gl.getUniformLocation(program, 'uTex'), 0);

		this.vao = gl.createVertexArray();
		gl.bindVertexArray(this.vao);
		this.vertexBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, this.f32.byteLength, gl.STREAM_DRAW);
		const stride = WORDS * 4;
		gl.enableVertexAttribArray(0);
		gl.vertexAttribPointer(0, 2, gl.FLOAT, false, stride, 0);
		gl.enableVertexAttribArray(1);
		gl.vertexAttribPointer(1, 2, gl.FLOAT, false, stride, 8);
		gl.enableVertexAttribArray(2);
		gl.vertexAttribPointer(2, 4, gl.UNSIGNED_BYTE, true, stride, 16);
		this.indexBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.indexBuffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, this.indices.byteLength, gl.STREAM_DRAW);

		gl.enable(gl.BLEND);
		gl.blendFuncSeparate(gl.ONE, gl.ONE_MINUS_SRC_ALPHA, gl.ONE, gl.ONE_MINUS_SRC_ALPHA);
		gl.disable(gl.DEPTH_TEST);
		gl.disable(gl.CULL_FACE);
		gl.enable(gl.SCISSOR_TEST);
		gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, true);
		gl.activeTexture(gl.TEXTURE0);
	}

	// ----- frame -----

	// Prepares a frame of a width x height CSS pixels, with a background color in 0..255.
	beginFrame(width, height, dpr, r, g, b) {
		if (dpr !== this.dpr) {
			this.dpr = dpr;
			fonts.setDevicePixelRatio(dpr);
			this.page = fonts.ensureAtlas()?.pages[0] ?? null;
		}
		this.width = width;
		this.height = height;
		const pw = Math.max(1, Math.round(width * dpr));
		const ph = Math.max(1, Math.round(height * dpr));
		this.deviceWidth = pw;
		this.deviceHeight = ph;
		this.drawCalls = 0;
		this.vertexCount = 0;
		this.nv = 0;
		this.ni = 0;
		this.segStart = 0;
		this.cmds.length = 0;
		this.stencilLevel = 0;
		this.clipX0 = 0;
		this.clipY0 = 0;
		this.clipX1 = pw;
		this.clipY1 = ph;
		this.setMatrix(1, 0, 0, 1, 0, 0);

		const gl = this.gl;
		if (gl === null) return;
		if (this.canvas.width !== pw || this.canvas.height !== ph) {
			this.canvas.width = pw;
			this.canvas.height = ph;
		}
		gl.viewport(0, 0, pw, ph);
		gl.disable(gl.STENCIL_TEST);
		gl.stencilMask(0xff);
		gl.scissor(0, 0, pw, ph);
		gl.clearColor(r / 255, g / 255, b / 255, 1);
		gl.clearStencil(0);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.STENCIL_BUFFER_BIT);
		gl.uniform2f(this.uView, width, height);
	}

	endFrame() {
		this.submit();
	}

	// Switches the texture page of the following geometry.
	bindPage(page) {
		if (page === this.page) return;
		this.flush();
		this.page = page;
	}

	// Closes the draw range recorded so far. Nothing is sent to the GPU yet: state changes (scissor,
	// stencil) and draw ranges are queued in order, and submit() uploads the geometry once.
	flush() {
		if (this.ni === this.segStart) return;
		this.cmds.push({ op: DRAW, start: this.segStart, count: this.ni - this.segStart, page: this.page });
		this.segStart = this.ni;
		this.drawCalls++;
	}

	// Queues a GL state change that has to happen between the draw ranges.
	queueState(fn) {
		if (this.gl === null) return;
		this.flush();
		this.cmds.push({ op: STATE, fn });
	}

	// Uploads the batched geometry with two buffer updates and executes the queued commands.
	submit() {
		this.flush();
		const gl = this.gl;
		if (gl !== null && this.cmds.length > 0) {
			gl.bufferSubData(gl.ARRAY_BUFFER, 0, this.f32, 0, this.nv * WORDS);
			gl.bufferSubData(gl.ELEMENT_ARRAY_BUFFER, 0, this.indices, 0, this.ni);
			let bound = null;
			for (const cmd of this.cmds) {
				if (cmd.op === STATE) {
					cmd.fn(gl);
					continue;
				}
				const page = cmd.page;
				if (page !== null && page !== bound) {
					this.uploadPage(page);
					bound = page;
				}
				gl.drawElements(gl.TRIANGLES, cmd.count, gl.UNSIGNED_INT, cmd.start * 4);
			}
		}
		this.cmds.length = 0;
		this.vertexCount += this.nv;
		this.nv = 0;
		this.ni = 0;
		this.segStart = 0;
	}

	// Binds the texture of a page, creating it and uploading the canvas when needed.
	// `page.nearest`: sharp UI images (1:1). Glyph atlas pages leave it unset and keep LINEAR.
	uploadPage(page) {
		const gl = this.gl;
		if (page.texture === null) {
			page.texture = gl.createTexture();
			gl.bindTexture(gl.TEXTURE_2D, page.texture);
			const filter = page.nearest ? gl.NEAREST : gl.LINEAR;
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, filter);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, filter);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
			page.dirty = true;
		} else {
			gl.bindTexture(gl.TEXTURE_2D, page.texture);
		}
		if (page.dirty) {
			gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, page.canvas);
			page.dirty = false;
		}
	}

	// ----- matrix -----

	setMatrix(a, b, c, d, e, f) {
		this.ma = a;
		this.mb = b;
		this.mc = c;
		this.md = d;
		this.me = e;
		this.mf = f;
	}

	translate(x, y) {
		this.me += this.ma * x + this.mc * y;
		this.mf += this.mb * x + this.md * y;
	}

	// Rotation by (cos, sin) clockwise and uniform scale around the point (ox, oy) of the local space.
	rotateScaleAround(ox, oy, cos, sin, s) {
		this.translate(ox, oy);
		const la = cos * s;
		const lb = sin * s;
		const lc = -sin * s;
		const ld = cos * s;
		const a = this.ma * la + this.mc * lb;
		const b = this.mb * la + this.md * lb;
		const c = this.ma * lc + this.mc * ld;
		const d = this.mb * lc + this.md * ld;
		this.ma = a;
		this.mb = b;
		this.mc = c;
		this.md = d;
		this.translate(-ox, -oy);
	}

	// ----- colors -----

	// Packs a color object (see util.js) with an opacity as ABGR bytes.
	pack(color, opacity) {
		let a = Math.round(color.a * opacity * 255);
		a = a < 0 ? 0 : a > 255 ? 255 : a;
		return ((a << 24) | (color.B << 16) | (color.G << 8) | color.R) >>> 0;
	}

	packRgba(r, g, b, alpha) {
		let a = Math.round(alpha * 255);
		a = a < 0 ? 0 : a > 255 ? 255 : a;
		return ((a << 24) | (b << 16) | (g << 8) | r) >>> 0;
	}

	// ----- batching -----

	reserve(vertices, indices) {
		if (this.nv + vertices > MAX_VERTICES || this.ni + indices > this.indices.length) this.submit();
		return vertices <= MAX_VERTICES && indices <= this.indices.length;
	}

	vertex(x, y, u, v, color) {
		const o = this.nv * WORDS;
		this.f32[o] = x;
		this.f32[o + 1] = y;
		this.f32[o + 2] = u;
		this.f32[o + 3] = v;
		this.u32[o + 4] = color;
		return this.nv++;
	}

	// A filled rectangle of the local space.
	fillQuad(x, y, w, h, color) {
		if (w <= 0 || h <= 0 || color >>> 24 === 0 || !this.reserve(4, 6)) return;
		const { ma, mb, mc, md, me, mf } = this;
		const u = this.whiteUv;
		const x1 = x + w;
		const y1 = y + h;
		const base = this.nv;
		this.vertex(ma * x + mc * y + me, mb * x + md * y + mf, u, u, color);
		this.vertex(ma * x1 + mc * y + me, mb * x1 + md * y + mf, u, u, color);
		this.vertex(ma * x1 + mc * y1 + me, mb * x1 + md * y1 + mf, u, u, color);
		this.vertex(ma * x + mc * y1 + me, mb * x + md * y1 + mf, u, u, color);
		const idx = this.indices;
		let i = this.ni;
		idx[i++] = base;
		idx[i++] = base + 1;
		idx[i++] = base + 2;
		idx[i++] = base;
		idx[i++] = base + 2;
		idx[i++] = base + 3;
		this.ni = i;
	}

	// A picture (a texture page of its own) in a rectangle of the local space; the atlas page is bound again after it.
	image(page, x, y, w, h, u0, v0, u1, v1, opacity) {
		const previous = this.page;
		this.bindPage(page);
		if (this.reserve(4, 6)) {
			const { ma, mb, mc, md, me, mf } = this;
			const color = this.packRgba(255, 255, 255, opacity);
			const base = this.nv;
			// Sub-pixel quads + LINEAR smear icon edges; snap when the transform is upright.
			if (mb === 0 && mc === 0) {
				const dpr = this.dpr;
				const x0 = Math.round((ma * x + me) * dpr) / dpr;
				const y0 = Math.round((md * y + mf) * dpr) / dpr;
				const x1 = Math.round((ma * (x + w) + me) * dpr) / dpr;
				const y1 = Math.round((md * (y + h) + mf) * dpr) / dpr;
				this.vertex(x0, y0, u0, v0, color);
				this.vertex(x1, y0, u1, v0, color);
				this.vertex(x1, y1, u1, v1, color);
				this.vertex(x0, y1, u0, v1, color);
			} else {
				const x1 = x + w;
				const y1 = y + h;
				this.vertex(ma * x + mc * y + me, mb * x + md * y + mf, u0, v0, color);
				this.vertex(ma * x1 + mc * y + me, mb * x1 + md * y + mf, u1, v0, color);
				this.vertex(ma * x1 + mc * y1 + me, mb * x1 + md * y1 + mf, u1, v1, color);
				this.vertex(ma * x + mc * y1 + me, mb * x + md * y1 + mf, u0, v1, color);
			}
			const idx = this.indices;
			let i = this.ni;
			idx[i++] = base;
			idx[i++] = base + 1;
			idx[i++] = base + 2;
			idx[i++] = base;
			idx[i++] = base + 2;
			idx[i++] = base + 3;
			this.ni = i;
		}
		this.bindPage(previous);
	}

	dropPage(page) {
		if (this.gl !== null && page.texture !== null) this.gl.deleteTexture(page.texture);
		page.texture = null;
	}

	// A convex polygon of the local space as a triangle fan, with one packed color per vertex.
	fillFan(points, colors) {
		const n = colors.length;
		if (n < 3 || !this.reserve(n, (n - 2) * 3)) return;
		const { ma, mb, mc, md, me, mf } = this;
		const u = this.whiteUv;
		const base = this.nv;
		for (let i = 0; i < n; i++) {
			const x = points[2 * i];
			const y = points[2 * i + 1];
			this.vertex(ma * x + mc * y + me, mb * x + md * y + mf, u, u, colors[i]);
		}
		const idx = this.indices;
		let k = this.ni;
		for (let i = 1; i < n - 1; i++) {
			idx[k++] = base;
			idx[k++] = base + i;
			idx[k++] = base + i + 1;
		}
		this.ni = k;
	}

	fillQuadAA(x, y, w, h, color) {
		if (w <= 0 || h <= 0) return;
		const p = this.quadPoints;
		p[0] = x;
		p[1] = y;
		p[2] = x + w;
		p[3] = y;
		p[4] = x + w;
		p[5] = y + h;
		p[6] = x;
		p[7] = y + h;
		this.shape(p, 4, this.quadIndices, color, true);
	}

	// A convex polygon given as flat local coordinates.
	fillPolygon(points, color, aa) {
		this.shape(points, points.length >> 1, null, color, aa);
	}

	// An arbitrary polygon of flat local coordinates with its triangulation.
	fillTriangles(points, indices, color, aa) {
		this.shape(points, points.length >> 1, indices, color, aa);
	}

	// Transforms the points, then emits the fill (a fan, or the given triangle indices) and
	// optionally the anti-aliasing fringe.
	shape(points, n, tri, color, aa) {
		if (n < 3 || color >>> 24 === 0) return;
		if (this.sx.length < n) {
			const size = Math.max(n, this.sx.length * 2);
			this.sx = new Float64Array(size);
			this.sy = new Float64Array(size);
			this.nx = new Float64Array(size);
			this.ny = new Float64Array(size);
		}
		const { ma, mb, mc, md, me, mf, sx, sy } = this;
		for (let i = 0; i < n; i++) {
			const x = points[2 * i];
			const y = points[2 * i + 1];
			sx[i] = ma * x + mc * y + me;
			sy[i] = mb * x + md * y + mf;
		}
		const triCount = tri === null ? (n - 2) * 3 : tri.length;
		const vertexCount = aa ? 2 * n : n;
		const indexCount = triCount + (aa ? 6 * n : 0);
		if (!this.reserve(vertexCount, indexCount)) return;

		const u = this.whiteUv;
		const base = this.nv;
		const idx = this.indices;
		let ii = this.ni;

		if (!aa) {
			for (let i = 0; i < n; i++) this.vertex(sx[i], sy[i], u, u, color);
		} else {
			const { nx, ny } = this;
			let area = 0;
			for (let i = 0; i < n; i++) {
				const j = i + 1 === n ? 0 : i + 1;
				area += sx[i] * sy[j] - sx[j] * sy[i];
			}
			const side = area > 0 ? 1 : -1;
			for (let i = 0; i < n; i++) {
				const j = i + 1 === n ? 0 : i + 1;
				const dx = sx[j] - sx[i];
				const dy = sy[j] - sy[i];
				const len = Math.hypot(dx, dy);
				if (len > 1e-6) {
					nx[i] = (side * dy) / len;
					ny[i] = (-side * dx) / len;
				} else {
					nx[i] = 0;
					ny[i] = 0;
				}
			}
			const fringe = 1 / this.dpr;
			const transparent = color & 0x00ffffff;
			for (let i = 0; i < n; i++) {
				const prev = i === 0 ? n - 1 : i - 1;
				const hasBoth = (nx[prev] !== 0 || ny[prev] !== 0) && (nx[i] !== 0 || ny[i] !== 0);
				const mx = nx[prev] + nx[i];
				const my = ny[prev] + ny[i];
				const len = Math.hypot(mx, my);
				let ox = 0;
				let oy = 0;
				if (len > 1e-6) {
					const cosHalf = hasBoth ? len / 2 : 1;
					const miter = Math.min(1 / Math.max(cosHalf, 1e-3), FRINGE_MITER_LIMIT);
					const k = (0.5 * fringe * miter) / len;
					ox = mx * k;
					oy = my * k;
				}
				this.vertex(sx[i] - ox, sy[i] - oy, u, u, color);
				this.u32[(base + n + i) * WORDS + 4] = transparent;
				const o = (base + n + i) * WORDS;
				this.f32[o] = sx[i] + ox;
				this.f32[o + 1] = sy[i] + oy;
				this.f32[o + 2] = u;
				this.f32[o + 3] = u;
			}
			this.nv += n; // the outer ring was written in place
		}

		if (tri === null) {
			for (let i = 1; i + 1 < n; i++) {
				idx[ii++] = base;
				idx[ii++] = base + i;
				idx[ii++] = base + i + 1;
			}
		} else {
			for (let i = 0; i < tri.length; i++) idx[ii++] = base + tri[i];
		}
		if (aa) {
			for (let i = 0; i < n; i++) {
				const j = i + 1 === n ? 0 : i + 1;
				idx[ii++] = base + i;
				idx[ii++] = base + j;
				idx[ii++] = base + n + j;
				idx[ii++] = base + i;
				idx[ii++] = base + n + j;
				idx[ii++] = base + n + i;
			}
		}
		this.ni = ii;
	}

	// A glyph of the atlas; (penX, baseY) is the pen position on the baseline in the local space.
	glyph(g, penX, baseY, color) {
		if (color >>> 24 === 0) return;
		if (g.page !== this.page) this.bindPage(g.page);
		if (!this.reserve(4, 6)) return;
		const { ma, mb, mc, md, me, mf } = this;
		const base = this.nv;
		if (mb === 0 && mc === 0) {
			const dpr = this.dpr;
			const x0 = Math.round((ma * (penX + g.x0) + me) * dpr) / dpr;
			const y0 = Math.round((md * (baseY + g.y0) + mf) * dpr) / dpr;
			const x1 = x0 + g.w * ma;
			const y1 = y0 + g.h * md;
			this.vertex(x0, y0, g.u0, g.v0, color);
			this.vertex(x1, y0, g.u1, g.v0, color);
			this.vertex(x1, y1, g.u1, g.v1, color);
			this.vertex(x0, y1, g.u0, g.v1, color);
		} else {
			const lx0 = penX + g.x0;
			const ly0 = baseY + g.y0;
			const lx1 = lx0 + g.w;
			const ly1 = ly0 + g.h;
			this.vertex(ma * lx0 + mc * ly0 + me, mb * lx0 + md * ly0 + mf, g.u0, g.v0, color);
			this.vertex(ma * lx1 + mc * ly0 + me, mb * lx1 + md * ly0 + mf, g.u1, g.v0, color);
			this.vertex(ma * lx1 + mc * ly1 + me, mb * lx1 + md * ly1 + mf, g.u1, g.v1, color);
			this.vertex(ma * lx0 + mc * ly1 + me, mb * lx0 + md * ly1 + mf, g.u0, g.v1, color);
		}
		const idx = this.indices;
		let i = this.ni;
		idx[i++] = base;
		idx[i++] = base + 1;
		idx[i++] = base + 2;
		idx[i++] = base;
		idx[i++] = base + 2;
		idx[i++] = base + 3;
		this.ni = i;
	}

	// ----- clipping -----

	// Clips the following drawing to the rectangle (0, 0, w, h) of the local space.
	// Returns a token for endClip, or null if nothing can be visible any more.
	beginClip(w, h) {
		if (w <= 0 || h <= 0) return null;
		const { ma, mb, mc, md, me, mf, dpr } = this;
		if (Math.abs(mb) < 1e-9 && Math.abs(mc) < 1e-9) {
			// An axis-aligned clip becomes a scissor rectangle of whole device pixels.
			const x0 = Math.floor(me * dpr + 1e-6);
			const y0 = Math.floor(mf * dpr + 1e-6);
			const x1 = x0 + Math.ceil(w * ma * dpr - 1e-6);
			const y1 = y0 + Math.ceil(h * md * dpr - 1e-6);
			const token = { stencil: false, x0: this.clipX0, y0: this.clipY0, x1: this.clipX1, y1: this.clipY1 };
			const nx0 = Math.max(x0, this.clipX0);
			const ny0 = Math.max(y0, this.clipY0);
			const nx1 = Math.min(x1, this.clipX1);
			const ny1 = Math.min(y1, this.clipY1);
			if (nx1 <= nx0 || ny1 <= ny0) return null;
			this.flush();
			this.setScissor(nx0, ny0, nx1, ny1);
			return token;
		}

		// A rotated clip is written into the stencil buffer.
		const level = this.stencilLevel;
		this.queueState((gl) => {
			gl.enable(gl.STENCIL_TEST);
			gl.colorMask(false, false, false, false);
			gl.stencilFunc(gl.EQUAL, level, 0xff);
			gl.stencilOp(gl.KEEP, gl.KEEP, gl.INCR);
		});
		this.fillQuad(0, 0, w, h, 0xffffffff);
		this.flush();
		this.stencilLevel++;
		const inner = this.stencilLevel;
		this.queueState((gl) => {
			gl.colorMask(true, true, true, true);
			gl.stencilFunc(gl.EQUAL, inner, 0xff);
			gl.stencilOp(gl.KEEP, gl.KEEP, gl.KEEP);
		});
		return { stencil: true, w, h };
	}

	endClip(token) {
		this.flush();
		if (!token.stencil) {
			this.setScissor(token.x0, token.y0, token.x1, token.y1);
			return;
		}
		const level = this.stencilLevel;
		this.queueState((gl) => {
			gl.colorMask(false, false, false, false);
			gl.stencilFunc(gl.EQUAL, level, 0xff);
			gl.stencilOp(gl.KEEP, gl.KEEP, gl.DECR);
		});
		this.fillQuad(0, 0, token.w, token.h, 0xffffffff);
		this.flush();
		this.stencilLevel--;
		const outer = this.stencilLevel;
		this.queueState((gl) => {
			gl.colorMask(true, true, true, true);
			gl.stencilOp(gl.KEEP, gl.KEEP, gl.KEEP);
			if (outer === 0) gl.disable(gl.STENCIL_TEST);
			else gl.stencilFunc(gl.EQUAL, outer, 0xff);
		});
	}

	setScissor(x0, y0, x1, y1) {
		this.clipX0 = x0;
		this.clipY0 = y0;
		this.clipX1 = x1;
		this.clipY1 = y1;
		const h = this.deviceHeight;
		this.queueState((gl) => gl.scissor(x0, h - y1, x1 - x0, y1 - y0));
	}
}
