// Generic Playwright test/fixture factory for any ImtCore-based app's GUI test suite.
//
// A consuming app's own fixtures/test.js (which lives in a `fixtures/` subfolder of the project, one
// level below playwright.config.js) is a thin shim:
//
//   const path = require('path');
//   const { createGuiTest } = require('imtcore-gui-testkit/fixtures/createTest');
//   const users = require('./users');
//   module.exports = createGuiTest(users, { rootDir: path.resolve(__dirname, '..') });
//
// `users` must expose:
//   byKey(key)                 -> the fixture user for a Playwright project name, or undefined
//   can(user, permission)      -> boolean
//   authFile(key)              -> storageState path, relative to the app's project root (see rootDir)

const path = require('path');
const base = require('@playwright/test');
const gui = require('../lib/gui');
const { captureConsoleErrors } = require('../lib/consoleErrors');

/**
 * @param {{byKey: Function, can: Function, authFile: Function}} users
 * @param {{rootDir: string}} opts  rootDir: the app's PROJECT ROOT (the directory playwright.config.js
 *   lives in) - NOT __dirname of the calling file if that file sits in a subfolder like `fixtures/`.
 *   authFile()'s return value is resolved relative to this.
 */
function createGuiTest(users, { rootDir }) {
  const { byKey, can, authFile } = users;

  const test = base.test.extend({
    // Worker-startup stagger (worker-scoped, runs once per worker before its first test, auto).
    // The app is a Qt/QML->WASM canvas that is expensive to cold-boot. At workers:10 every worker
    // otherwise boots its first app instance at t=0, and 10 simultaneous WASM boots saturate the one
    // Debug server + CPU so badly that the earliest tests' first render / menu never reaches the
    // "visible" state within even a generous locator timeout (seen live as "[MenuPanel > XButton] -
    // 2 element(s) exist but none became visible", only ever on the very first tests of the run).
    // Offsetting each worker's start by parallelIndex * STAGGER spreads that one-time boot storm over
    // ~15s instead of all-at-once. It fires once per worker, NOT per test, so steady-state throughput
    // is unchanged; parallelIndex (0..workers-1, reused on worker replacement) caps the max delay.
    _workerStartupStagger: [
      async ({}, use, workerInfo) => {
        const STAGGER_MS = 1500;
        const delay = workerInfo.parallelIndex * STAGGER_MS;
        if (delay > 0) await new Promise((resolve) => setTimeout(resolve, delay));
        await use();
      },
      { scope: 'worker', auto: true },
    ],

    // The current test user, derived from the Playwright project name.
    user: async ({}, use, testInfo) => {
      const user = byKey(testInfo.project.name);
      if (!user) {
        throw new Error(
          `No test user maps to project "${testInfo.project.name}". Project names must match your users module's keys.`
        );
      }
      // Attach a convenience predicate.
      const decorated = { ...user, can: (perm) => can(user, perm) };
      await use(decorated);
    },

    // The GUI helper barrel, handed to tests so they don't each require it.
    gui: async ({}, use) => {
      await use(gui);
    },

    // Overrides Playwright's built-in `page` fixture to also watch for browser-side errors (see
    // lib/consoleErrors.js) - an uncaught pageerror hard-fails the test (unambiguous - nothing
    // legitimate throws one), while console.error/warn text is attached to the report as diagnostic
    // context rather than failing the test outright (this codebase has legitimate expected
    // warn/error calls for handled edge cases - see consoleErrors.js's own header comment). Only
    // covers the default per-test `page` fixture; a describe.serial block's newUserPage()-created page
    // isn't wrapped by this (see newUserPage's own comment on why it can't hard-fail the same way).
    page: async ({ page }, use, testInfo) => {
      const finish = captureConsoleErrors(page);
      await use(page);
      await finish(testInfo);
    },
  });

  const expect = base.expect;

  /**
   * Open one browser page for a whole `test.describe.serial(...)` block, logged in as the current
   * project's user, instead of the default per-test `page` fixture. Use in a `test.beforeAll` and
   * keep the returned `page`/`user` in a describe-scoped closure variable - every test in that block
   * then continues working against the SAME app instance (no full reboot/re-login between steps),
   * which is both much faster and a more honest model of most Imt-based apps' reality: document
   * tabs/filters/sort are typically server-side session state, i.e. already "one continuous session"
   * in production, not independent page loads.
   *
   * Trade-off (why this isn't the default for every test): test.describe.serial stops running the
   * rest of the block after the first failure (reported as "skipped", not "failed") - you lose the
   * per-step-independent failure signal you get from a fresh page per test. Reserve this for tests
   * that are already a deliberate narrative sequence (fill field A, then B, then save), not for
   * unrelated checks that happen to share a page.
   * @param {import('@playwright/test').Browser} browser
   * @param {import('@playwright/test').TestInfo} testInfo
   */
  async function newUserPage(browser, testInfo) {
    const user = byKey(testInfo.project.name);
    if (!user) {
      throw new Error(
        `newUserPage: no test user maps to project "${testInfo.project.name}". Project names must match your users module's keys.`
      );
    }
    const decorated = { ...user, can: (perm) => can(user, perm) };
    const context = await browser.newContext({ storageState: path.resolve(rootDir, authFile(user.key)) });
    const page = await context.newPage();
    // Same browser-error watching as the default `page` fixture (see lib/consoleErrors.js), but this
    // page is shared across every test in a describe.serial block, so a pageerror can't be attributed to
    // one specific test the way the per-test `page` fixture does - log it to the Node/CI console instead
    // of throwing into whichever test happens to be running when it fires. Nothing calls the returned
    // finish() here (there's no single natural "end" short of the block's own afterAll closing the
    // page); the listeners are harmless to leave attached until the page/context closes.
    captureConsoleErrors(page, {
      onPageError: (err) => {
        // eslint-disable-next-line no-console
        console.error(`[browser pageerror] project="${testInfo.project.name}": ${err.message}`);
      },
    });
    return { context, page, user: decorated };
  }

  /**
   * Run the same body for several users inside ONE spec, for side-by-side permission comparisons.
   * Prefer the per-project model (one project per user) for the common case; use this only when a
   * test genuinely needs to compare multiple users in a single body.
   * @param {object[]} userList
   * @param {(user: object) => void} defineTests
   */
  function forEachUser(userList, defineTests) {
    for (const user of userList) {
      test.describe(`[user: ${user.title}]`, () => defineTests(user));
    }
  }

  return { test, expect, gui, newUserPage, forEachUser };
}

module.exports = { createGuiTest };
