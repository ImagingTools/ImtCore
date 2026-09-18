const fs = require('fs');
const path = require('path');

const DEFAULT_OUTPUT_ROOT = 'test-output';

function resolveOutputPaths(rootDir, env = process.env) {
  const outputRoot = path.resolve(env.PLAYWRIGHT_OUTPUT_ROOT || path.join(rootDir, DEFAULT_OUTPUT_ROOT));
  const phase = env.PLAYWRIGHT_OUTPUT_PHASE || '';
  if (phase && !/^[a-z0-9][a-z0-9-]*$/i.test(phase)) {
    throw new Error(`Invalid PLAYWRIGHT_OUTPUT_PHASE "${phase}"`);
  }

  const phaseRoot = phase ? path.join(outputRoot, phase) : outputRoot;
  return {
    outputRoot,
    phaseRoot,
    artifacts: path.join(phaseRoot, 'artifacts'),
    junit: path.join(phaseRoot, 'junit.xml'),
    html: path.join(phaseRoot, 'html'),
  };
}

function prepareOutputRoot(outputRoot) {
  const resolved = path.resolve(outputRoot);
  const parsed = path.parse(resolved);
  if (resolved === parsed.root || !path.basename(resolved).toLowerCase().includes(DEFAULT_OUTPUT_ROOT)) {
    throw new Error(`Refusing to clear unsafe GUI test output root "${resolved}"`);
  }

  fs.rmSync(resolved, { recursive: true, force: true });
  fs.mkdirSync(resolved, { recursive: true });
  return resolved;
}

module.exports = { DEFAULT_OUTPUT_ROOT, prepareOutputRoot, resolveOutputPaths };