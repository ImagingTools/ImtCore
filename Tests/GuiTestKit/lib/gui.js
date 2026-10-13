// Single convenient entry point for tests and page objects. Internals are split across
// dom/actions/stability/screenshot; this barrel gives a test one import for everything.

const dom = require('./dom');
const actions = require('./actions');
const stability = require('./stability');
const screenshot = require('./screenshot');
const graphql = require('./graphql');

module.exports = {
  ...dom,
  ...actions,
  ...stability,
  ...screenshot,

  dom,
  actions,
  stability,
  screenshot,
  graphql,
  gql: graphql,
};
