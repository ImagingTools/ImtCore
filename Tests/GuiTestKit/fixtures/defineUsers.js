// Builds an app's fixture-user module from just its user list.
//
// Every consumer of this kit - createGuiTest, createGlobalSetup and buildProjects - needs the same
// four things from an app's users module (byKey / activeUsers / authFile / seededUsers). Each app used
// to hand-write them, which made the contract implicit: a renamed export or a key typo surfaced much
// later as a confusing failure inside a fixture, or as a project that silently ran nothing. Declare the
// users, get the module:
//
//   const { defineUsers } = require('imtcore-gui-testkit/fixtures/defineUsers');
//   module.exports = defineUsers({
//     users: [ { key: 'su', title: 'Superuser', login: 'su', password: '1' }, ... ],
//     defaultUserKeys: ['su', 'fullAccess'],
//     allUsersEnv: 'MYAPP_GUI_ALL_USERS',
//   });
//
// Anything extra an app needs (its own permission bundles, a shared PASSWORD constant) stays in its
// own file - this only owns the plumbing every app would otherwise copy.

/**
 * @typedef {Object} TestUser
 * @property {string} key            Stable id -> Playwright project name + .auth/<key>.json
 * @property {string} title          Human label
 * @property {string} login          Username used at the login screen
 * @property {string} password       Password used at the login screen
 * @property {boolean} [seed]        If true, the app's seeding step creates this user
 * @property {string[]} [permissions] Permission codes GRANTED when seeding this user's role. Not a
 *   claim about what the product requires anywhere - tests never branch on these.
 * @property {string|RegExp} [isolatedSpec]  Pin this user to exactly one spec file, which then runs
 *   under this user and no other (see buildProjects). For specs whose server-side per-user state
 *   cannot be shared with a concurrently-running spec.
 */

/**
 * @param {object} opts
 * @param {TestUser[]} opts.users
 * @param {string[]} [opts.defaultUserKeys]  The fast subset used when the all-users env var is unset.
 *   Omit to make every user active by default.
 * @param {string} [opts.allUsersEnv]        Env var that switches on the full matrix ('1'/'true').
 * @param {TestUser} [opts.guest]            Override the unauthenticated pseudo-user.
 * @param {(key: string) => string} [opts.authFile]
 */
function defineUsers({ users, defaultUserKeys, allUsersEnv, guest, authFile }) {
  const GUEST = guest || { key: 'guest', title: 'Guest', login: null, password: null, seed: false, permissions: [] };

  // Fail loudly here, at config-load time, rather than somewhere far away at run time: a duplicate key
  // silently shadows a project, and a defaultUserKeys entry that matches nobody quietly shrinks the
  // default run to less than it claims to cover.
  if (!Array.isArray(users) || users.length === 0) {
    throw new Error('defineUsers: `users` must be a non-empty array');
  }
  const seen = new Set();
  for (const user of users) {
    if (!user.key) throw new Error(`defineUsers: every user needs a \`key\` (offending entry: ${JSON.stringify(user)})`);
    if (seen.has(user.key)) throw new Error(`defineUsers: duplicate user key "${user.key}"`);
    seen.add(user.key);
    if (!user.login) throw new Error(`defineUsers: user "${user.key}" has no \`login\` to sign in with`);
  }
  if (GUEST.key && seen.has(GUEST.key)) {
    throw new Error(`defineUsers: "${GUEST.key}" is the guest pseudo-user's key and cannot also be a real user`);
  }
  for (const key of defaultUserKeys || []) {
    if (!seen.has(key)) {
      throw new Error(`defineUsers: defaultUserKeys names unknown user "${key}" (known: ${[...seen].join(', ')})`);
    }
  }

  const byKey = (key) => users.find((u) => u.key === key) || (key === GUEST.key ? GUEST : undefined);

  const useAllUsers = () => {
    if (!allUsersEnv) return !defaultUserKeys;
    const value = process.env[allUsersEnv];
    return value === '1' || value === 'true';
  };

  /**
   * The users that get a Playwright project / storageState this run.
   *
   * `isolatedSpec` users are ALWAYS active, in both the fast subset and the full matrix: their whole
   * point is that their dedicated spec runs under them and no other user, so leaving one out would mean
   * that spec runs under nobody at all (every other project testIgnores it). They add no matrix breadth
   * - one user, one spec - so including them costs one extra project, not a full re-run of everything.
   */
  const activeUsers = () => {
    const matrix = useAllUsers() || !defaultUserKeys ? users : users.filter((u) => defaultUserKeys.includes(u.key));
    const isolated = users.filter((u) => u.isolatedSpec && !matrix.includes(u));
    return [...matrix, ...isolated];
  };

  return {
    USERS: users,
    GUEST,
    DEFAULT_USER_KEYS: defaultUserKeys,
    byKey,
    activeUsers,
    authFile: authFile || ((key) => `.auth/${key}.json`),
    seededUsers: () => users.filter((u) => u.seed),
  };
}

module.exports = { defineUsers };
