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
// Everything it sets is policy every Imt-based app wants the same; any of it can be overridden by
// passing the normal Playwright key through (`use` is merged, the rest replaced).

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

  // A stale or misspelled key matches nobody, every project then gets the grepInvert, and the whole
  // mutating phase vanishes into a green run.
  if (mutatingUserKeys) {
    const known = new Set(users.USERS.map((u) => u.key));
    for (const key of mutatingUserKeys) {
      if (!known.has(key)) {
        throw new Error(`createGuiConfig: mutatingUserKeys names unknown user "${key}" (known: ${[...known].join(', ')})`);
      }
    }
    // Knowing the key is a real user is not enough: if none of them is ACTIVE this run, the phase still
    // disappears.
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
    // A per-test cap, not 0 (unlimited): with no timeout a test waiting on a server that died mid-run
    // hangs forever instead of failing.
    timeout: 60_000,
    expect: { timeout: 15_000 },
    forbidOnly: !!process.env.CI,
    // Every project is one fixture USER, and these apps keep per-user state server-side, so two tests
    // running as the SAME user concurrently share - and can corrupt - one workspace. Drop to 1 for a
    // trustworthy single-pass verification run.
    workers,
    // No retries - a flaky test reports red immediately instead of being masked by a re-run.
    retries: 0,
    // The HTML report is opt-in via PLAYWRIGHT_HTML_REPORT; it duplicates the junit file and failure
    // artifacts. Setting the variable brings it back.
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
    // outputDir at the start of every invocation - without per-phase paths the second phase wipes the
    // first's artifacts.
    outputDir: output.artifacts,
    // No {testFilePath}: the baseline is keyed by user + screenshot name, so screenshot names are
    // global and two specs using one name would share a baseline (prune-orphan-baselines.js reports it).
    snapshotPathTemplate: '{testDir}/__screenshots__/{projectName}/{arg}-{platform}{ext}',
    use: {
      headless: true,
      // Drive a browser that is ALREADY on the machine instead of Playwright's own download, when
      // PLAYWRIGHT_BROWSER_CHANNEL names one ("msedge" / "chrome"). This exists for build agents that
      // can reach the npm registry but not cdn.playwright.dev: the browser download is the only part
      // of the suite that needs the open internet, and every Windows box already ships Edge, whose
      // version tracks the same Chromium release Playwright pins. Unset locally, so nothing changes
      // for a developer with the normal cached browser.
      ...(process.env.PLAYWRIGHT_BROWSER_CHANNEL
        ? { channel: process.env.PLAYWRIGHT_BROWSER_CHANNEL }
        : {}),
      viewport: { width: 1920, height: 1080 },
      baseURL: baseUrl,
      screenshot: 'only-on-failure',
      // Kept for FAILURES only, which is what makes a downloaded CI artifact worth having: a trace
      // replays the whole test - every action, the DOM at each step, console and network - and that is
      // the only way to diagnose a GUI failure you cannot reproduce locally. Not 'on-first-retry',
      // which never fires here because retries are 0 by design. A green run writes none, so the
      // published archive stays a couple of junit files.
      trace: 'retain-on-failure',
      ...use,
    },
    projects: buildProjects({
      users: users.activeUsers(),
      allUsers: users.USERS,
      guest: users.GUEST,
      authFile: users.authFile,
      testDir,
      mutatingUserKeys,
    }),
    ...overrides,
  });
}

module.exports = { createGuiConfig };
