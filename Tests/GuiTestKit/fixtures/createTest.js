// Generic Playwright test/fixture factory for any ImtCore-based app's GUI test suite.
//
// A consuming app's own fixtures/test.js is a thin shim:
//
//   const { createGuiTest } = require('imtcore-gui-testkit/fixtures/createTest');
//   const users = require('./users');
//   module.exports = createGuiTest(users, { rootDir: path.resolve(__dirname, '..') });
//
// `users` must expose byKey(key) and authFile(key); fixtures/defineUsers.js builds that from a user list.

const fs = require('fs');
const path = require('path');
const base = require('@playwright/test');
const gui = require('../lib/gui');
const { captureConsoleErrors } = require('../lib/consoleErrors');

const PERMISSIONS_STORAGE_KEY = 'AuthorizationController/permissions';

/**
 * The permission codes the server granted this user, from the storageState saved after login. Returns
 * null when they cannot be determined at all - which is NOT the same as "none".
 * @param {string} authFilePath
 * @returns {Set<string>|null}
 */
function readGrantedPermissions(authFilePath) {
  let state;
  try {
    state = JSON.parse(fs.readFileSync(authFilePath, 'utf8'));
  } catch (_) {
    return null;
  }
  for (const origin of state.origins || []) {
    for (const entry of origin.localStorage || []) {
      if (entry.name !== PERMISSIONS_STORAGE_KEY) continue;
      // Stored as a JSON string holding a ';'-separated list (an empty one for a user with none).
      let raw = entry.value;
      try {
        raw = JSON.parse(raw);
      } catch (_) {
        /* already a bare string */
      }
      return new Set(String(raw).split(';').filter(Boolean));
    }
  }
  return null;
}

/**
 * @param {{byKey: Function, authFile: Function}} users
 * @param {{rootDir: string}} opts  rootDir: the app's PROJECT ROOT (the directory playwright.config.js
 *   lives in) - NOT __dirname of the calling file if that file sits in a subfolder like `fixtures/`.
 *   authFile()'s return value is resolved relative to this.
 */
function createGuiTest(users, { rootDir }) {
  const { byKey, authFile } = users;

  /**
   * The user, plus `can(permission)` answered from the permissions the server granted at login.
   *
   * A user declared with '*' is the superuser - the server sends it an empty list because it bypasses
   * permission checks, so an empty list there means "everything", not "nothing". `can()` returns
   * undefined when the permissions are genuinely unknown; callers must not treat that as "no".
   */
  function decorate(user) {
    const granted = readGrantedPermissions(path.resolve(rootDir, authFile(user.key)));
    const isSuperuser = Array.isArray(user.permissions) && user.permissions.includes('*');
    const can = (permission) => {
      if (isSuperuser) return true;
      if (!granted) return undefined;
      return granted.has(permission);
    };
    return { ...user, grantedPermissions: granted, can };
  }

  const test = base.test.extend({
    // Worker-startup stagger (worker-scoped, once per worker before its first test). The app is a
    // Qt/QML->WASM canvas that is expensive to cold-boot, so offset each worker's start to spread the
    // one-time boot storm; parallelIndex caps the max delay.
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
      await use(decorate(user));
    },

    // The GUI helper barrel, handed to tests so they don't each require it.
    gui: async ({}, use) => {
      await use(gui);
    },

    // Overrides Playwright's `page` fixture to also watch for browser-side errors (see
    // lib/consoleErrors.js): an uncaught pageerror hard-fails the test, while console.error/warn text
    // is attached to the report as diagnostic context. Only covers the default per-test `page`.
    page: async ({ page }, use, testInfo) => {
      const finish = captureConsoleErrors(page);
      await use(page);
      await finish(testInfo);
    },
  });

  const expect = base.expect;

  /**
   * Open one browser page for a whole `test.describe.serial(...)` block, logged in as the current
   * project's user, instead of the default per-test `page`. Every test in the block works against the
   * same app instance (no reboot/re-login between steps).
   *
   * Trade-off: test.describe.serial stops after the first failure, so you lose per-step failure signal;
   * reserve this for a deliberate narrative sequence. Clean up in afterAll with `page.context().close()`
   * (this creates one BrowserContext per block), not `page.close()`.
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
    const decorated = decorate(user);
    const context = await browser.newContext({ storageState: path.resolve(rootDir, authFile(user.key)) });
    const page = await context.newPage();
    // Same browser-error watching as the default `page` fixture, but this page is shared across a
    // describe.serial block, so a pageerror can't be attributed to one test - log it instead of throwing.
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
   * Prefer the per-project model for the common case; use this only for genuine multi-user comparisons.
   * @param {object[]} userList
   * @param {(user: object) => void} defineTests
   */
  function forEachUser(userList, defineTests) {
    for (const user of userList) {
      test.describe(`[user: ${user.title}]`, () => defineTests(user));
    }
  }

  /**
   * Skip this test unless the server granted the user this permission. Call it as the first line of a
   * test body. Uses `can(x) === false`, never `!user.can(x)`: can() returns undefined when permissions
   * could not be read, and the `!` form would wrongly skip on that.
   * @param {{can: Function, key: string}} user
   * @param {string} permission
   */
  function requires(user, permission) {
    test.skip(user.can(permission) === false, `${user.key} was not granted ${permission}`);
  }

  return { test, expect, gui, newUserPage, forEachUser, requires };
}

module.exports = { createGuiTest };
