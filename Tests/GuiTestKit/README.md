# imtcore-gui-testkit

Reusable Playwright GUI-test framework for Imt-based applications (Qt/QML compiled to WASM). Extracted
from ProLife's GUI test suite (`ProLife/Tests/ProLifeGui`), which had grown into a mature,
objectName-driven framework that has nothing to do with ProLife specifically - it only knows about
ImtCore's own UI conventions (MenuPanel, CommandsView, PopupMenuDialog, TableRow_<i>,
AuthorizationPage, ...). Every Imt-based app follows the same `CollectionPage + Editor` structure, so
this is meant to be consumed by each app's own `Tests/<AppName>Gui` project instead of copy-pasted.

## What's in here

- **`lib/`** - `dom.js` (objectName-path locator engine), `actions.js` (click/fill/select/openPage,
  hard-fail-if-missing), `stability.js` (`waitForStable`, a MutationObserver-based settle wait),
  `screenshot.js` (`checkScreenshot`/`expectVisible`/`expectHidden`/`expectCount`/masking), `gui.js`
  (barrel of all of the above).
- **`controls/index.js`** - control wrappers over `lib/actions` (Button, CommandBar, MenuPanel,
  ComboBox, TextInput, FilterPanel, Table, Pagination, Switch, Dialog, TableConfigDialog).
- **`pages/`** - `BasePage` and `CollectionPage` (a standard collection needs no subclass: declare its
  filter ids and its non-deterministic columns), plus page objects for views ImtCore itself owns
  (`AdministrationPage`, `SearchPage`, `OrganizationsPage` - grounded in
  `ImtCore/Qml/imtauthgui/AdministrationView.qml`, `ImtCore/Qml/imtgui/View/SearchPage.qml`,
  `ImtCore/Qml/imtauthgui/TenantCollectionView.qml`).
- **`specs/collectionSpec.js`** - generates a whole standard collection spec from a declaration
  (which page, which filters, which screenshots), and owns the names those screenshots get so tooling
  can ask instead of guessing.
- **`fixtures/defineUsers.js`** - builds an app's whole fixture-user module (`byKey`/`activeUsers`/
  `authFile`/`seededUsers`/`GUEST`) from a plain list of users, and validates it.
- **`fixtures/createTest.js`** - factory for the `test`/`expect`/`gui`/`newUserPage`/`forEachUser`
  fixture bundle.
- **`globalSetup/createGlobalSetup.js`** - factory for the Playwright `globalSetup` function
  (logs in each fixture user, saves storageState, retries a dropped WASM-canvas click, waits for a
  real access token before snapshotting).
- **`playwrightConfig/createConfig.js`** - factory for the whole `playwright.config.js` (per-user
  snapshot dirs, timeouts, reporters, per-phase output dirs, projects), over
  **`buildProjects.js`** - the one-project-per-user + guest `projects` array.

What's **not** here, by design: any app's own business page objects (e.g. ProLife's
`DeviceCollectionPage`), its fixture users/roles, its DB seeding, or its CI script - those are
app-specific and stay in the app's own repo.

## No permission model

These suites do not describe who may do what. A test authenticates, drives the UI and compares
screenshots; the server enforces permissions and the client was already built from the logged-in
user's own set. Where a flow simply is not offered to the current user, ask the running client and
skip - `page.isAvailable()`, `page.commands.isAvailable(id)`, `admin.hasSubPage(id)`. A table of
rules kept in a test suite is a copy of the product's configuration and goes stale silently.

## Adding GUI tests to a new app

Four small files, then specs. Nothing else is required.

```jsonc
// YourApp/Tests/YourAppGui/package.json
{ "devDependencies": {
    "@playwright/test": "^1.52.0",
    "imtcore-gui-testkit": "file:../../../ImtCore/Tests/GuiTestKit" } }
```

```js
// fixtures/users.js - the only file with real content
const { defineUsers } = require('imtcore-gui-testkit/fixtures/defineUsers');
module.exports = defineUsers({
  users: [
    { key: 'su',    title: 'Superuser', login: 'su',    password: '1' },
    { key: 'viewer', title: 'Viewer',   login: 'viewer', password: 'Secret_1', seed: true,
      roleName: 'Viewer', roleId: 'Viewer', permissions: ['ViewOrders'] },
  ],
  defaultUserKeys: ['su'],          // the fast subset; omit to always run everyone
  allUsersEnv: 'YOURAPP_GUI_ALL_USERS',
});
```

```js
// fixtures/test.js
const path = require('path');
const { createGuiTest } = require('imtcore-gui-testkit/fixtures/createTest');
module.exports = createGuiTest(require('./users'), { rootDir: path.resolve(__dirname, '..') });
```

```js
// global-setup.js
const { createGlobalSetup } = require('imtcore-gui-testkit/globalSetup/createGlobalSetup');
const { activeUsers, authFile } = require('./fixtures/users');
module.exports = createGlobalSetup({ activeUsers, authFile, rootDir: __dirname, baseUrl: BASE_URL });
```

```js
// playwright.config.js
const { createGuiConfig } = require('imtcore-gui-testkit/playwrightConfig/createConfig');
module.exports = createGuiConfig({
  rootDir: __dirname,
  baseUrl: process.env.YOURAPP_BASE_URL || 'http://localhost:17778',
  users: require('./fixtures/users'),
  globalSetup: require.resolve('./global-setup.js'),
  mutatingUserKeys: ['su'],         // who runs @mutating specs in the serial phase
});
```

A first spec - a whole standard collection view is a declaration, not code:

```js
// tests/orders.collection.test.js
const fixtures = require('../fixtures/test');
const { defineCollectionSpec } = require('imtcore-gui-testkit/specs/collectionSpec');

defineCollectionSpec({ ...fixtures, defineTest: (title, body) => fixtures.test(title, body) }, {
  title: 'Orders / collection',
  pageId: 'Orders',
  prefix: 'orders',                                   // -> orders-landing, orders-filter-text, ...
  filters: { customers: 'CustomersFilter', creationDate: 'CreationDateFilter' },
  maskColumns: ['added', 'timeStamp'],                // non-deterministic columns, masked everywhere
  scenarios: [
    { name: 'filter-text', search: 'test' },
    { name: 'filter-customer', filter: 'customers', option: 'QUISS', optional: true },
    { name: 'filter-creation-date', dateFilter: 'creationDate', preset: 'Year_Last' },
    { name: 'filter-cleared', clearAll: true, apply: [{ search: 'test' }] },
    { name: 'sort-delivery-id', sort: 'orderId' },    // -> ...-1 and ...-2
    { name: 'pagination', pagination: { size: 50, page: 2 } },
    { name: 'remove-dialog', command: 'Remove' },
  ],
});
```

That generates the landing screenshot on a cold page, a `describe.serial` block sharing one page,
the per-user skip, the filter reset between tests and one screenshot per scenario. `defineTest` has to
be the arrow shown above, declared in the spec file: Playwright files a test under the source location
that called `test()`, and that is what `snapshotPathTemplate` uses to place the baselines.

Anything a declaration cannot express stays an ordinary hand-written spec using the same page objects -
the two mix freely in one suite.

Beyond that, an app adds its own `pages/XxxEditorPage.js` extending `BasePage` for flows that are
genuinely its own, and whatever DB seeding its fixture users need.

## Consuming this from an app

There is no npm registry or workspace set up for this repo family yet, so apps consume this via a
plain npm `file:` dependency (a relative path to a sibling repo - the same "sibling checkout"
assumption `Run-CiTests.ps1`-style scripts already make for Puma/Lisa):

```jsonc
// YourApp/Tests/YourAppGui/package.json
{
  "devDependencies": {
    "imtcore-gui-testkit": "file:../../../ImtCore/Tests/GuiTestKit"
  }
}
```

Also add an `.npmrc` next to that `package.json`:

```
install-links=true
```

Then `npm install`. npm's default for a local `file:` dependency is a **symlink** - which breaks here,
because Node resolves a symlinked package's own `require()`s (e.g. `lib/screenshot.js` requiring
`@playwright/test`) relative to this package's REAL directory, not the consuming app's
`node_modules`. This package has no `node_modules` of its own (deliberately, to avoid two independent
`@playwright/test` instances loaded at once, which Playwright itself detects and rejects), so a
symlinked install can't resolve its own peerDependency at all. `install-links=true` makes npm copy
this package into the app's `node_modules` instead, so its `require()`s resolve through the app's own
`node_modules` like any other dependency.

**Trade-off**: because it's a copy, editing this kit's source is **not** picked up by a plain
`npm install` in the consuming app (npm only reacts to a changed `package.json`). After changing
anything here, refresh every consumer with:

```bash
rm -rf node_modules/imtcore-gui-testkit && npm install
```

See `ProLife/Tests/ProLifeGui` for a complete, working example of all of the above wired together.
