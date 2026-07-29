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
- **`pages/`** - `BasePage` and `CollectionPage` (the generic base classes every app's own
  `XxxCollectionPage`/`XxxEditorPage` extend), plus three page objects for views ImtCore itself owns
  (`AdministrationPage`, `SearchPage`, `OrganizationsPage` - grounded in
  `ImtCore/Qml/imtauthgui/AdministrationView.qml`, `ImtCore/Qml/imtgui/View/SearchPage.qml`,
  `ImtCore/Qml/imtauthgui/TenantCollectionView.qml`).
- **`fixtures/createTest.js`** - factory for the `test`/`expect`/`gui`/`newUserPage`/`forEachUser`
  fixture bundle.
- **`globalSetup/createGlobalSetup.js`** - factory for the Playwright `globalSetup` function
  (logs in each fixture user, saves storageState, retries a dropped WASM-canvas click, waits for a
  real access token before snapshotting).
- **`playwrightConfig/buildProjects.js`** - factory for the one-project-per-user + guest
  `projects` array.

What's **not** here, by design: any app's own business page objects (e.g. ProLife's
`DeviceCollectionPage`), its permission matrix, its fixture users/roles, its DB seeding, or its CI
script - those are app-specific and stay in the app's own repo.

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

A consuming app needs, at minimum:

- **`fixtures/users.js`** exposing `byKey(key)`, `can(user, permission)`, `authFile(key)`,
  `activeUsers()`, and a `GUEST` pseudo-user - see ProLife's `fixtures/users.js` for the shape.
- Thin shims wiring the three factories above to that `users.js` (see each factory's own header
  comment for the exact snippet) for `fixtures/test.js`, `global-setup.js`, and `playwright.config.js`.
- Its own `pages/XxxCollectionPage.js` / `XxxEditorPage.js` extending `imtcore-gui-testkit/pages`'
  `CollectionPage`/`BasePage`, and a `matrix/permissions.js` describing which permission gates which
  page/command/field (see ProLife's for the pattern - it's app-specific data, not shared code).

See `ProLife/Tests/ProLifeGui` for a complete, working example of all of the above wired together.
