// Convenience barrel - deep imports (require('imtcore-gui-testkit/lib/gui'), .../controls,
// .../pages, .../fixtures/createTest, .../globalSetup/createGlobalSetup,
// .../playwrightConfig/buildProjects) are the primary, documented way to consume this package and
// mirror the internal require() style already used throughout (require('../lib/gui') etc.) - this
// root export just gathers the most common entry points for a one-import convenience option.

const gui = require('./lib/gui');
const controls = require('./controls');
const pages = require('./pages');
const { createGuiTest } = require('./fixtures/createTest');
const { createGlobalSetup } = require('./globalSetup/createGlobalSetup');
const { buildProjects } = require('./playwrightConfig/buildProjects');

module.exports = { gui, controls, pages, createGuiTest, createGlobalSetup, buildProjects };
