// Convenience barrel gathering the most common entry points; deep imports remain the primary way to
// consume this package.

const gui = require('./lib/gui');
const controls = require('./controls');
const pages = require('./pages');
const { defineUsers } = require('./fixtures/defineUsers');
const { createGuiTest } = require('./fixtures/createTest');
const { createGlobalSetup } = require('./globalSetup/createGlobalSetup');
const { createGuiConfig } = require('./playwrightConfig/createConfig');
const { buildProjects } = require('./playwrightConfig/buildProjects');

module.exports = { gui, controls, pages, defineUsers, createGuiTest, createGlobalSetup, createGuiConfig, buildProjects };
