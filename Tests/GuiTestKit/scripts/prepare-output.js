const { prepareOutputRoot } = require('../playwrightConfig/output');

const outputRoot = process.argv[2];
if (!outputRoot) {
  throw new Error('Usage: node prepare-output.js <output-root>');
}

console.log(`Prepared GUI test output: ${prepareOutputRoot(outputRoot)}`);