// Global teardown wired in by createGuiConfig: runs once after every worker has finished, when each
// test's output folder is final, and leaves only the folders that hold something (a failure's
// screenshot, trace, diff).

const { pruneEmptyDirs } = require('./output');

module.exports = async (config) => {
  for (const project of config.projects) {
    pruneEmptyDirs(project.outputDir);
  }
};
