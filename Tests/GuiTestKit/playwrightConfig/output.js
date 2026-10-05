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

// Remove every empty directory below `dir` (not `dir` itself). Playwright gives each test its own
// output folder, and trace 'retain-on-failure' deletes a passing test's trace but leaves the folder.
function pruneEmptyDirs(dir) {
  if (!fs.existsSync(dir)) return;
  for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
    if (!entry.isDirectory()) continue;
    const child = path.join(dir, entry.name);
    pruneEmptyDirs(child);
    if (fs.readdirSync(child).length === 0) fs.rmdirSync(child);
  }
}

module.exports = { DEFAULT_OUTPUT_ROOT, prepareOutputRoot, pruneEmptyDirs, resolveOutputPaths };