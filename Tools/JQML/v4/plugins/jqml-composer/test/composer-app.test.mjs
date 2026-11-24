import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { test } from 'node:test';
import { fileURLToPath } from 'node:url';
import { generateApplication, sanitizeName, writeSources } from '../src/composer/app.mjs';
import { normalizeSchema } from '../src/composer/schema.mjs';

const here = path.dirname(fileURLToPath(import.meta.url));
const enginePath = path.resolve(here, '..', '..', '..');

const node = (uid, type, id, properties = {}, children = []) => ({ uid, type, source: '', id, properties, children });
const instance = (uid, source, id, properties = {}) => ({ uid, type: '', source, id, properties, children: [] });

const FILES = {
	'Card.jqschema': { root: node('r', 'Rectangle', 'card', { width: '200', height: '80', color: '"white"' }, [node('t', 'Text', 'label', { text: '"Title"' })]), exports: [{ name: 'title', uid: 't', property: 'text' }, { name: 'fill', uid: 'r', property: 'color' }] },
	'BigCard.jqschema': { extends: 'Card.jqschema', root: node('br', '', 'big', { title: '"Big"', width: '320' }), exports: [] },
	'Page.jqschema': { root: node('pr', 'Item', 'page', { width: '600', height: '400' }, [instance('i1', 'Card.jqschema', 'first', { title: '"One"', x: '10' }), instance('i2', 'BigCard.jqschema', 'second', { fill: '"red"', y: '100' })]) },
};

function world() {
	const store = new Map(Object.entries(FILES).map(([key, schema]) => [key, normalizeSchema(schema)]));
	const resolve = (ref) => (store.has(ref) ? { key: ref, schema: store.get(ref) } : null);
	return { entry: { key: 'Page.jqschema', schema: store.get('Page.jqschema') }, resolve };
}

function temp() {
	return fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-composer-'));
}

test('the sources and the config are written', () => {
	const { entry, resolve } = world();
	const out = temp();
	try {
		const written = writeSources(entry, resolve, out, ['imt.json']);
		assert.deepEqual(written.files.sort(), ['BigCard.qml', 'Card.qml', 'Page.qml']);
		assert.ok(fs.existsSync(path.join(out, 'src', 'Page.qml')));
		assert.deepEqual(JSON.parse(fs.readFileSync(written.configPath, 'utf8')).includes, [path.resolve('imt.json')]);
	} finally {
		fs.rmSync(out, { recursive: true, force: true });
	}
});

test('the application is compiled by the JQML 4 compiler', () => {
	const { entry, resolve } = world();
	const out = temp();
	try {
		const result = generateApplication({ entry, resolve, outDir: out, name: 'My App!', enginePath });
		assert.equal(result.compiled, true);
		assert.equal(result.name, 'MyApp');
		assert.ok(fs.statSync(result.jsPath).size > 1000);
		assert.ok(fs.existsSync(result.htmlPath));
	} finally {
		fs.rmSync(out, { recursive: true, force: true });
	}
});

test('without an engine only the sources are written', () => {
	const { entry, resolve } = world();
	const out = temp();
	try {
		assert.equal(generateApplication({ entry, resolve, outDir: out, name: 'x', enginePath: '' }).compiled, false);
	} finally {
		fs.rmSync(out, { recursive: true, force: true });
	}
});

test('a schema with a problem is not generated', () => {
	const { resolve } = world();
	const broken = normalizeSchema({ root: node('r', 'Item', 'a', {}, [instance('q', 'Card.jqschema', 'c', { radius: '1' })]) });
	const out = temp();
	try {
		assert.throws(() => generateApplication({ entry: { key: 'Broken.jqschema', schema: broken }, resolve, outDir: out, name: 'x', enginePath }), /radius is not exported/);
		assert.equal(fs.existsSync(path.join(out, 'src')), false);
	} finally {
		fs.rmSync(out, { recursive: true, force: true });
	}
});

test('the name of the application is cleaned', () => {
	assert.equal(sanitizeName(' a b/c '), 'abc');
	assert.equal(sanitizeName(''), 'app');
});
