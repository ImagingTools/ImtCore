// Builds an app's fixture-user module from just its user list.
//
//   const { defineUsers } = require('imtcore-gui-testkit/fixtures/defineUsers');
//   module.exports = defineUsers({
//     users: [ { key: 'su', title: 'Superuser', login: 'su', password: '1' }, ... ],
//     defaultUserKeys: ['su', 'fullAccess'],
//     allUsersEnv: 'MYAPP_GUI_ALL_USERS',
//   });
//
// It owns the plumbing (byKey / activeUsers / authFile / seededUsers) every app would otherwise copy.

/**
 * @typedef {Object} TestUser
 * @property {string} key            Stable id -> Playwright project name + .auth/<key>.json
 * @property {string} title          Human label
 * @property {string} login          Username used at the login screen
 * @property {string} password       Password used at the login screen
 * @property {boolean} [seed]        If true, the app's seeding step creates this user
 * @property {string[]} [permissions] Permission codes GRANTED when seeding this user's role.
 * @property {string|RegExp} [isolatedSpec]  Pin this user to exactly one spec file, which then runs
 *   under this user and no other (see buildProjects).
 */

/**
 * @param {object} opts
 * @param {TestUser[]} opts.users
 * @param {string[]} [opts.defaultUserKeys]  The fast subset used when the all-users env var is unset.
 *   Omit to make every user active by default.
 * @param {string} [opts.allUsersEnv]        Env var that switches on the full matrix ('1'/'true').
 * @param {TestUser} [opts.guest]            Override the unauthenticated pseudo-user.
 * @param {(key: string) => string} [opts.authFile]
 * @param {string} [opts.suKey]  the bootstrap superuser global-setup logs in even without `seed` (default "su")
 */
function defineUsers({ users, defaultUserKeys, allUsersEnv, guest, authFile, suKey = 'su' }) {
  const GUEST = guest || { key: 'guest', title: 'Guest', login: null, password: null, seed: false, permissions: [] };

  // Fail loudly here at config-load time: a duplicate key silently shadows a project, and a
  // defaultUserKeys entry matching nobody quietly shrinks the default run.
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
  // The superuser holds no permissions in the server's own list (it bypasses the checks), so its
  // granted set comes back empty. `permissions: ['*']` is the only thing distinguishing it from a user
  // granted nothing, without which `requires()` would skip it out of every permission-gated test.
  const su = users.find((u) => u.key === suKey);
  if (su && !(Array.isArray(su.permissions) && su.permissions.includes('*'))) {
    throw new Error(
      `defineUsers: the superuser "${suKey}" must declare permissions: ['*']. The server sends it an ` +
        'empty permission list (it bypasses the checks), so without that marker every permission-gated ' +
        'test would skip for it instead of running.'
    );
  }

  // global-setup logs in su plus every SEEDED user. A user without `seed` still gets a project pointed
  // at a storageState path that is never written, so its tests die at context creation. Say it here.
  for (const user of users) {
    if (!user.seed && user.key !== suKey) {
      throw new Error(
        `defineUsers: user "${user.key}" has no \`seed: true\`, so global-setup never logs it in and its ` +
          'storageState is never written - every test in its project would fail at context creation'
      );
    }
  }

  const byKey = (key) => users.find((u) => u.key === key) || (key === GUEST.key ? GUEST : undefined);

  const useAllUsers = () => {
    if (!allUsersEnv) return !defaultUserKeys;
    const value = process.env[allUsersEnv];
    return value === '1' || value === 'true';
  };

  /**
   * The users that get a Playwright project / storageState this run. `isolatedSpec` users are always
   * active (their dedicated spec runs under them and no other, so leaving one out runs it under nobody).
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
