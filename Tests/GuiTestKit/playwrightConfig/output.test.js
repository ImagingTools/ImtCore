const assert = require('node:assert/strict');
const fs = require('fs');
const os = require('os');
const path = require('path');
const test = require('node:test');

const { prepareOutputRoot, resolveOutputPaths } = require('./output');

test('resolves the universal phased output layout', () => {
  const rootDir = path.join('C:', 'suite');
  const outputRoot = path.join('C:', 'reports');
  const paths = resolveOutputPaths(rootDir, {
    PLAYWRIGHT_OUTPUT_ROOT: outputRoot,
    PLAYWRIGHT_OUTPUT_PHASE: 'phase1-readonly',
  });

  assert.equal(paths.phaseRoot, path.resolve(outputRoot, 'phase1-readonly'));
  assert.equal(paths.artifacts, path.resolve(outputRoot, 'phase1-readonly', 'artifacts'));
  assert.equal(paths.junit, path.resolve(outputRoot, 'phase1-readonly', 'junit.xml'));
  assert.equal(paths.html, path.resolve(outputRoot, 'phase1-readonly', 'html'));
});

test('clears stale output and recreates the root', () => {
  const outputRoot = fs.mkdtempSync(path.join(os.tmpdir(), 'gui-test-output-'));
  fs.writeFileSync(path.join(outputRoot, 'stale.txt'), 'stale');

  prepareOutputRoot(outputRoot);

  assert.deepEqual(fs.readdirSync(outputRoot), []);
  fs.rmSync(outputRoot, { recursive: true, force: true });
});

test('rejects phase names that could escape the output root', () => {
  assert.throws(
    () => resolveOutputPaths('suite', { PLAYWRIGHT_OUTPUT_PHASE: '../outside' }),
    /Invalid PLAYWRIGHT_OUTPUT_PHASE/
  );
});

test('refuses to clear a directory that is not a test-output root', () => {
  assert.throws(() => prepareOutputRoot(os.tmpdir()), /unsafe GUI test output root/);
});