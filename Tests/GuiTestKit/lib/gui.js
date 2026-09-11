// Single convenient entry point for tests and page objects (the "utils.js replacement").
// Internals are split across dom/actions/stability/screenshot for cleanliness; this barrel gives a
// test one import for everything:  const gui = require('../lib/gui');

const dom = require('./dom');
const actions = require('./actions');
const stability = require('./stability');
const screenshot = require('./screenshot');
const graphql = require('./graphql');

module.exports = {
  // locator engine
  ...dom,
  // actions (click / fill / select / navigate ...)
  ...actions,
  // waits
  ...stability,
  // screenshot + structural assertions
  ...screenshot,

  // keep sub-namespaces available too, for readability where preferred
  dom,
  actions,
  stability,
  screenshot,
  graphql,
  // Also exposed as `gui.gql` - shorter, and mirrors the `gql.spyOn(...)` usage shown in graphql.js.
  gql: graphql,
};
