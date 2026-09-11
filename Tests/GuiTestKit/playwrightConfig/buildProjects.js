// Generic Playwright `projects` array builder for the "one project per fixture user, storageState
// reused across all its tests" model, plus a guest project for unauthenticated specs.
//
// A consuming app's own playwright.config.js:
//
//   const { buildProjects } = require('imtcore-gui-testkit/playwrightConfig/buildProjects');
//   const { GUEST, authFile, activeUsers } = require('./fixtures/users');
//   ...
//   projects: buildProjects({ users: activeUsers(), guest: GUEST, authFile }),
//
// Spec isolation: a user may carry an `isolatedSpec` (a spec filename or RegExp). That user's project
// then runs ONLY that spec, and every OTHER authenticated project testIgnores it. Use this to pin a
// spec that must not share server-side per-user state with any other spec (e.g. a document-editor spec
// whose open-document workspace is keyed per user server-side) to its own dedicated fixture user.

// Turn an isolatedSpec value (filename string or RegExp) into a RegExp suitable for testMatch/testIgnore.
function specToRegExp(spec) {
  if (spec instanceof RegExp) return spec;
  // Match the filename anywhere in the path; escape regex metacharacters (notably the dots).
  return new RegExp(String(spec).replace(/[.*+?^${}()|[\]\\]/g, '\\$&'));
}

/**
 * @param {object} opts
 * @param {object[]} opts.users               activeUsers() - fixture users to build a project for
 * @param {{key: string}} opts.guest          the guest pseudo-user (no storageState)
 * @param {(key: string) => string} opts.authFile
 * @param {string} [opts.testDir]             default './tests'
 * @param {RegExp} [opts.guestTestMatch]      which spec files are guest-only (default *.guest.test.js)
 * @param {Object<string, string>} [opts.specPages]  spec filename -> the page id it exercises. A user
 *   who cannot see that page has the spec removed from its project entirely, so Playwright never
 *   schedules it: no browser context, no app boot, no worker time. Without this the spec is scheduled,
 *   boots the app in its beforeAll/beforeEach, and only then hits the test body's own
 *   `test.skip(!canSeePage(...))` - paying the full cold-boot cost to discover it had nothing to do.
 *   Requires `canSeePage`. Specs not listed here are always scheduled.
 * @param {(user: object, pageId: string) => boolean} [opts.canSeePage]
 * @param {string[]} [opts.mutatingUserKeys]  users allowed to run @mutating specs. Mutating tests run
 *   serially (one shared database), so their cost is linear in the number of users AND unaffected by
 *   worker count - and running the same mutation again as a different user re-proves the flow, not the
 *   permission (which the structural checks already cover). Users outside this list get a project-level
 *   grepInvert so the mutating phase stays the same size however wide the matrix gets. Omit to let
 *   every user run them, as before.
 * @returns {object[]}
 */
function buildProjects({
  users,
  guest,
  authFile,
  testDir = './tests',
  guestTestMatch = /.*\.guest\.test\.js/,
  specPages,
  canSeePage,
  mutatingUserKeys,
}) {
  // Every isolated spec across the active users - regular projects must exclude ALL of them, not just
  // their own, so an isolated editor spec never runs under a general matrix user.
  const isolatedMatchers = users.filter((u) => u.isolatedSpec).map((u) => specToRegExp(u.isolatedSpec));

  // Specs this user provably has nothing to do in, decided from static permission data alone.
  function unreachableSpecs(user) {
    if (!specPages || !canSeePage) return [];
    return Object.entries(specPages)
      .filter(([, pageId]) => pageId && !canSeePage(user, pageId))
      .map(([spec]) => specToRegExp(spec));
  }

  // An isolated user must keep its mutating tests: its spec runs under that user and no other, so
  // excluding them there would drop the coverage entirely rather than deduplicate it.
  function runsMutating(user) {
    return !mutatingUserKeys || !!user.isolatedSpec || mutatingUserKeys.includes(user.key);
  }

  // A misspelled or stale key matches nobody, every project then gets grepInvert, and the whole
  // mutating phase disappears into a green run with nothing to show it went missing. Naming a user who
  // is simply outside this run's scope is legitimate (the fast default subset is narrower than the full
  // matrix), so what is checked is the dangerous outcome itself: that SOMETHING still runs them.
  if (mutatingUserKeys && users.length && !users.some(runsMutating)) {
    throw new Error(
      `buildProjects: mutatingUserKeys [${mutatingUserKeys.join(', ')}] matches none of the active ` +
        `users [${users.map((u) => u.key).join(', ')}], so no project would run @mutating tests at all.`
    );
  }

  const userProjects = users.map((u) => {
    const project = u.isolatedSpec
      ? {
          // Dedicated project: runs ONLY its one isolated spec (still excluding guest-only specs).
          name: u.key,
          testDir,
          testMatch: specToRegExp(u.isolatedSpec),
          use: { storageState: authFile(u.key) },
        }
      : {
          // Regular authenticated project: everything that is neither a guest-only spec, nor any
          // isolated spec, nor a spec for a page this user cannot open.
          name: u.key,
          testDir,
          testIgnore: [guestTestMatch, ...isolatedMatchers, ...unreachableSpecs(u)],
          use: { storageState: authFile(u.key) },
        };

    if (!runsMutating(u)) project.grepInvert = /@mutating/;
    return project;
  });

  const guestProject = {
    name: guest.key,
    testDir,
    testMatch: guestTestMatch, // guest-only specs opt in via the .guest.test.js suffix
    use: {},
  };

  return [...userProjects, guestProject];
}

module.exports = { buildProjects };
