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
const { resolveOutputPaths } = require('./output');

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
  if (!users || typeof users.activeUsers !== 'function' || !Array.isArray(users.USERS)) {
    throw new Error('createGuiConfig: `users` must be a users module (see fixtures/defineUsers)');
  }

  const output = resolveOutputPaths(rootDir);

  // A stale or misspelled key matches nobody, every project then gets the grepInvert, and the entire
  // mutating phase vanishes into a green run with nothing to show it went missing.
  if (mutatingUserKeys) {
    const known = new Set(users.USERS.map((u) => u.key));
    for (const key of mutatingUserKeys) {
      if (!known.has(key)) {
        throw new Error(`createGuiConfig: mutatingUserKeys names unknown user "${key}" (known: ${[...known].join(', ')})`);
      }
    }
    // Knowing the key is a real user is not enough: if none of them is ACTIVE this run, every active
    // project still gets the grepInvert and the phase still disappears - the very outcome this guards.
    const active = new Set(users.activeUsers().map((u) => u.key));
    if (!mutatingUserKeys.some((key) => active.has(key))) {
      throw new Error(
        `createGuiConfig: none of mutatingUserKeys [${mutatingUserKeys.join(', ')}] is active this run ` +
          `(active: ${[...active].join(', ')}), so no project would run @mutating tests at all`
      );
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
    // The HTML report is opt-in, via PLAYWRIGHT_HTML_REPORT. It duplicates what the junit file and
    // the failure artifacts already carry, and costs a directory per phase to say it - so a suite that
    // reads its results from the junit XML and the diff PNGs (all of them, so far) should not have to
    // keep sweeping those folders up. Setting the variable brings it back.
    reporter: process.env.CI
      ? [
          ['list'],
          ['junit', { outputFile: output.junit }],
          ...(process.env.PLAYWRIGHT_HTML_REPORT
            ? [['html', { outputFolder: output.html, open: 'never' }]]
            : []),
        ]
      : 'list',
    ...(globalSetup ? { globalSetup } : {}),
    // A two-phase CI run invokes Playwright twice against this ONE config, and Playwright clears
    // outputDir at the start of every invocation - without per-phase paths the second phase silently
    // wipes the first's screenshots/diffs/traces before anyone can look at them.
    // Everything a run leaves behind goes under one directory, so a suite root stays readable: an ad
    // hoc `npx playwright test` lands here too, not in a test-results/ of its own.
    outputDir: output.artifacts,
    // No {testFilePath}: the baseline is keyed by user + screenshot name, so it no longer matters which
    // FILE registered the test. That is what let the spec generators drop the `defineTest` shim every
    // spec had to repeat purely so Playwright would file generated tests under the right path.
    // Screenshot names are therefore global: two specs using one name would share a baseline, which
    // prune-orphan-baselines.js reports.
    snapshotPathTemplate: '{testDir}/__screenshots__/{projectName}/{arg}-{platform}{ext}',
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
