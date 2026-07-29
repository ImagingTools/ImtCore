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
 * @returns {object[]}
 */
function buildProjects({ users, guest, authFile, testDir = './tests', guestTestMatch = /.*\.guest\.test\.js/ }) {
  // Every isolated spec across the active users - regular projects must exclude ALL of them, not just
  // their own, so an isolated editor spec never runs under a general matrix user.
  const isolatedMatchers = users.filter((u) => u.isolatedSpec).map((u) => specToRegExp(u.isolatedSpec));

  const userProjects = users.map((u) => {
    if (u.isolatedSpec) {
      // Dedicated project: runs ONLY its one isolated spec (still excluding guest-only specs).
      return {
        name: u.key,
        testDir,
        testMatch: specToRegExp(u.isolatedSpec),
        use: { storageState: authFile(u.key) },
      };
    }
    // Regular authenticated project: everything that is neither a guest-only spec nor any isolated spec.
    return {
      name: u.key,
      testDir,
      testIgnore: [guestTestMatch, ...isolatedMatchers],
      use: { storageState: authFile(u.key) },
    };
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
