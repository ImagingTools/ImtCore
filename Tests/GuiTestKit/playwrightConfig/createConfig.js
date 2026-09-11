// Builds an app's playwright.config.js from the few things that actually differ between apps.
//
//   const { createGuiConfig } = require('imtcore-gui-testkit/playwrightConfig/createConfig');
//   const users = require('./fixtures/users');
//
//   module.exports = createGuiConfig({
//     rootDir: __dirname,
//     baseUrl: process.env.MYAPP_BASE_URL || 'http://localhost:17778',
//     users,
//     mutatingUserKeys: ['su'],
//   });
//
// Everything it sets is policy this suite learned the hard way and that every Imt-based app wants the
// same: per-user snapshot directories, a per-test timeout that bounds a dead-server hang, no retries,
// per-phase output/report directories for the two-phase CI run, and the CI reporter set. Any of it can
// be overridden by passing the normal Playwright key through - `use` is merged, the rest replaced.

const { defineConfig } = require('@playwright/test');
const { buildProjects } = require('./buildProjects');

/**
 * @param {object} opts
 * @param {string} opts.rootDir          The directory playwright.config.js lives in (__dirname).
 * @param {string} opts.baseUrl
 * @param {{activeUsers: Function, GUEST: object, authFile: Function}} opts.users  the app's users
 *   module (see fixtures/defineUsers).
 * @param {string[]} [opts.mutatingUserKeys]  users allowed to run @mutating specs (see buildProjects).
 *   Validated against the full user list here, where that list is known.
 * @param {string} [opts.globalSetup]    path to the app's global-setup.js
 * @param {string} [opts.testDir]
 * @param {number} [opts.workers]
 * @param {object} [opts.use]            merged over the defaults
 */
function createGuiConfig({
  rootDir,
  baseUrl,
  users,
  mutatingUserKeys,
  globalSetup,
  testDir = './tests',
  workers = 10,
  use = {},
  ...overrides
}) {
  if (!rootDir) throw new Error('createGuiConfig: `rootDir` is required (pass __dirname)');
  if (!baseUrl) throw new Error('createGuiConfig: `baseUrl` is required');
  if (!users || typeof users.activeUsers !== 'function') {
    throw new Error('createGuiConfig: `users` must be a users module (see fixtures/defineUsers)');
  }

  // A stale or misspelled key matches nobody, every project then gets the grepInvert, and the entire
  // mutating phase vanishes into a green run with nothing to show it went missing. buildProjects cannot
  // catch this (it only sees the ACTIVE subset); here the full list is known.
  if (mutatingUserKeys) {
    const known = new Set(users.USERS.map((u) => u.key));
    for (const key of mutatingUserKeys) {
      if (!known.has(key)) {
        throw new Error(`createGuiConfig: mutatingUserKeys names unknown user "${key}" (known: ${[...known].join(', ')})`);
      }
    }
  }

  return defineConfig({
    testDir,
    // A per-test cap, not 0 (unlimited): with an unlimited timeout a test waiting on a server that died
    // mid-run hangs forever instead of failing, stalling the whole run with no further output.
    timeout: 60_000,
    expect: { timeout: 15_000 },
    forbidOnly: !!process.env.CI,
    // Every project is one fixture USER, and these apps keep per-user view/document state server-side,
    // so two tests running as the SAME user concurrently share - and can corrupt - one workspace.
    // Different projects/users are independent, so the risk is confined to same-user files running at
    // once. Drop to 1 for a trustworthy single-pass verification run.
    workers,
    // No retries - a flaky test reports red immediately instead of being masked by a re-run.
    retries: 0,
    reporter: process.env.CI
      ? [
          ['list'],
          ['junit', { outputFile: process.env.PLAYWRIGHT_JUNIT_OUTPUT || 'junit-report.xml' }],
          ['html', { outputFolder: process.env.PLAYWRIGHT_HTML_OUTPUT_DIR || 'playwright-report', open: 'never' }],
        ]
      : 'list',
    ...(globalSetup ? { globalSetup } : {}),
    // A two-phase CI run invokes Playwright twice against this ONE config, and Playwright clears
    // outputDir at the start of every invocation - without per-phase paths the second phase silently
    // wipes the first's screenshots/diffs/traces before anyone can look at them.
    outputDir: process.env.PLAYWRIGHT_OUTPUT_DIR || 'test-results',
    // Per-user baselines: __screenshots__/<userKey>/<specPath>/<name>-<platform>.png
    snapshotPathTemplate: '{testDir}/__screenshots__/{projectName}/{testFilePath}/{arg}-{platform}{ext}',
    use: {
      headless: true,
      viewport: { width: 1920, height: 1080 },
      baseURL: baseUrl,
      screenshot: 'only-on-failure',
      trace: 'off',
      ...use,
    },
    projects: buildProjects({
      users: users.activeUsers(),
      guest: users.GUEST,
      authFile: users.authFile,
      testDir,
      mutatingUserKeys,
    }),
    ...overrides,
  });
}

module.exports = { createGuiConfig };
