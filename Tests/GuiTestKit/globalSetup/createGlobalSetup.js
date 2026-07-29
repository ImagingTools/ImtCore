// Generic Playwright globalSetup factory for any ImtCore-based app's GUI test suite.
//
// A consuming app's own global-setup.js (which lives at the project root, next to
// playwright.config.js) is a thin shim:
//
//   const { createGlobalSetup } = require('imtcore-gui-testkit/globalSetup/createGlobalSetup');
//   const { activeUsers, authFile } = require('./fixtures/users');
//   module.exports = createGlobalSetup({
//     activeUsers,
//     authFile,
//     rootDir: __dirname,
//     baseUrl: process.env.MYAPP_BASE_URL || 'http://localhost:7776',
//   });
//
// Produces one storageState per active fixture user by logging in through the UI - nothing gets
// CREATED here (fixture users/roles must already exist in the app's seeded test database).

const fs = require('fs');
const path = require('path');
const { chromium } = require('@playwright/test');
const { login } = require('../lib/actions');
const { waitForStable } = require('../lib/stability');

const VIEWPORT = { width: 1920, height: 1080 };

/**
 * Pull every `--project=<name>` / `--project <name>` value out of a CLI argv snapshot. Playwright
 * doesn't parse its own CLI args for globalSetup - `config.argv` (or process.argv, same process) is
 * handed over raw - so this does the minimal parsing needed for one specific flag, not a general CLI
 * parser.
 */
function parseRequestedProjectKeys(argv) {
  const keys = [];
  for (let i = 0; i < argv.length; i++) {
    const arg = argv[i];
    if (arg.startsWith('--project=')) {
      keys.push(arg.slice('--project='.length));
    } else if (arg === '--project' && argv[i + 1] !== undefined) {
      keys.push(argv[i + 1]);
      i++;
    }
  }
  return keys;
}

/**
 * @param {object} opts
 * @param {() => object[]} opts.activeUsers        returns the fixture users to log in this run
 * @param {(key: string) => string} opts.authFile   storageState path for a user key, relative to rootDir
 * @param {string} opts.rootDir                     the app's project root (pass __dirname)
 * @param {string} opts.baseUrl                     the app's base URL
 * @param {string} [opts.tokenStorageKey]           localStorage key proving a real session exists -
 *   defaults to ImtCore's standard AuthorizationController key. A DOM-quiet login can resolve *before*
 *   the auth round-trip actually wrote the session, so waiting for this - rather than just "login()
 *   settled" - is what makes the saved storageState actually authenticated.
 * @param {string} [opts.suUserKey]                 fixture key that MUST be present in activeUsers()
 *   (default: 'su') - sanity-checked so a misconfigured users module fails loudly instead of silently
 *   producing an all-guest test run.
 * @param {number} [opts.maxLoginAttempts]          retries for a dropped WASM-canvas login click (default 3)
 * @param {number} [opts.loginConcurrency]          how many non-su users to log in at once (default 3) -
 *   each gets its own browser context/page, so this is real network+WASM-boot parallelism, not just
 *   Promise scheduling. Kept modest (not "all at once"): the app/server is still cold right after
 *   ProLifeServerTest.exe opened its port (see loginAndWaitForToken's own comment on generous first-load
 *   waits), and every login is itself a fresh WASM boot - so unbounded concurrency here just recreates
 *   the same "boot storm" problem the per-worker test stagger (fixtures/createTest.js) exists to avoid,
 *   just moved earlier into global-setup.
 * @param {boolean} [opts.reuseExistingAuth]        skip logging in a user whose storageState file
 *   already exists (default false - always log in fresh). A CI script that invokes "npx playwright
 *   test" more than once against the SAME still-running server in the SAME run (e.g. a read-only phase
 *   followed by a serial @mutating phase - see ProLife's Run-CiTests.ps1) pays a full UI login per
 *   active user on EVERY invocation, since globalSetup has no memory of the previous one. Measured
 *   live: this was a large share of a second phase's wall-clock for a run with many active users.
 *   The caller is trusted to only pass true when it knows the existing files are fresh (produced
 *   moments ago by an earlier phase of the SAME run against the SAME server/DB state) - this function
 *   has no way to verify that itself, so passing true against stale/foreign storageState files will
 *   silently skip re-authenticating them.
 */
function createGlobalSetup({
  activeUsers,
  authFile,
  rootDir,
  baseUrl,
  tokenStorageKey = 'AuthorizationController/accessToken',
  suUserKey = 'su',
  maxLoginAttempts = 5,
  loginConcurrency = 3,
  reuseExistingAuth = false,
}) {
  async function loginAndWaitForToken(context, user) {
    // Own page per attempt, closed in finally: a failed attempt (below) otherwise leaks its page into
    // the shared context, and each retry piled another one on.
    const page = await context.newPage();
    try {
      await page.goto(baseUrl);
      // Generous first-load waits: global-setup runs immediately after ProLifeServerTest.exe opened its
      // port, while it is still warming up (JIT-compiling the WASM app it serves, priming caches, first
      // GraphQL/auth round-trips cold). The token-wait in particular was seen timing out at 20s for a
      // later user in the login sequence and failing the WHOLE run at setup; 45s absorbs that cold-start
      // latency. These are ceilings on a fast path, so raising them doesn't slow a warm login.
      await waitForStable(page, { timeout: 30000, quietMs: 600 });
      await login(page, user.login, user.password);
      await page.waitForFunction(
        (key) => {
          try {
            const raw = localStorage.getItem(key);
            return !!raw && JSON.parse(raw).length > 0;
          } catch {
            return false;
          }
        },
        tokenStorageKey,
        { timeout: 45000 }
      );
    } finally {
      await page.close().catch(() => {});
    }
  }

  async function saveStateForUser(browser, user) {
    const dest = path.resolve(rootDir, authFile(user.key));
    if (reuseExistingAuth && fs.existsSync(dest)) {
      return dest;
    }

    const context = await browser.newContext({ viewport: VIEWPORT });

    // The QML/WASM login click is occasionally lost (a canvas click that lands mid-repaint never
    // fires), leaving no accessToken. That is a genuine flaky-input reality of driving a WASM canvas,
    // so retry the whole login a couple of times rather than failing the entire suite on one dropped
    // click.
    let lastErr;
    for (let attempt = 1; attempt <= maxLoginAttempts; attempt++) {
      try {
        await loginAndWaitForToken(context, user);
        lastErr = null;
        break;
      } catch (err) {
        lastErr = err;
        // eslint-disable-next-line no-console
        console.warn(
          `global-setup: login attempt ${attempt}/${maxLoginAttempts} for "${user.key}" did not produce a session (${err.message}); retrying`
        );
        // Brief backoff before retrying: if the miss was a transient server hiccup (a GC pause / a slow
        // cold GraphQL round-trip during warm-up), an immediate re-goto tends to hit the same stall.
        if (attempt < maxLoginAttempts) {
          await new Promise((resolve) => setTimeout(resolve, 3000));
        }
      }
    }
    if (lastErr) {
      await context.close();
      throw lastErr;
    }

    fs.mkdirSync(path.dirname(dest), { recursive: true });
    await context.storageState({ path: dest });
    await context.close();
    return dest;
  }

  return async (config) => {
    const users = activeUsers();
    const su = users.find((u) => u.key === suUserKey);
    if (!su) throw new Error(`global-setup: "${suUserKey}" must be in activeUsers() (check your users module)`);

    // A run scoped to specific projects (e.g. "npx playwright test --project=guest") only needs
    // storageState for the user(s) those projects actually map to - logging in every OTHER active user
    // regardless was pure waste (measured live: scoping to the guest project, which needs NO
    // storageState at all, still paid for logging in all 9 default users first). Playwright doesn't
    // parse its own --project flag for globalSetup, so this reads it from the raw argv snapshot
    // Playwright hands over via config.argv (falls back to process.argv - same process - if an older
    // Playwright version doesn't expose it). No --project flag at all (the default full-suite run)
    // keeps today's behavior: everyone logs in.
    const requestedKeys = parseRequestedProjectKeys((config && config.argv) || process.argv);
    const activeForThisRun = requestedKeys.length > 0 ? users.filter((u) => requestedKeys.includes(u.key)) : users;

    const browser = await chromium.launch();
    try {
      // su first and alone: seeded users are typically created/verified via su-authenticated GraphQL
      // calls elsewhere in the pipeline (Generate-Backups.ps1 / seed-fixture-users.js), and su logging in
      // first also means a genuinely broken server fails fast on ONE login instead of after burning
      // through a whole concurrent batch. Skipped entirely if this run's scope doesn't even include su
      // (e.g. --project=guest, which needs no authenticated user at all).
      if (activeForThisRun.some((u) => u.key === suUserKey)) {
        await saveStateForUser(browser, su);
      }

      const rest = activeForThisRun.filter((u) => u.key !== suUserKey && u.seed);
      // Chunked concurrency (see loginConcurrency's own comment) - process.all per chunk, chunks
      // sequential, so at most `loginConcurrency` WASM boots are ever in flight at once.
      for (let i = 0; i < rest.length; i += loginConcurrency) {
        const chunk = rest.slice(i, i + loginConcurrency);
        await Promise.all(chunk.map((user) => saveStateForUser(browser, user)));
      }
    } finally {
      await browser.close();
    }
  };
}

module.exports = { createGlobalSetup };
