# Making QML testable: the objectName conventions this kit relies on

This kit drives the app entirely through DOM attributes the WASM bridge exposes for each QML
element's `objectName` (see `lib/dom.js`'s header comment). If a QML element you're adding or
touching is meant to be clickable/fillable/assertable from a test, it needs an `objectName` that
follows one of the conventions below - otherwise a test can only reach it by guessing, or not at
all. This file exists so that guessing stops being necessary, on both sides: the person writing the
QML and the person writing the test.

## The rules

1. **Every interactive element a test needs to address must have an explicit `objectName`.**
   Some base controls auto-derive one (see below) - when that auto-derivation doesn't produce a
   *unique* name (most commonly: an icon-only `ToolButton`/`Button` with no `text`, which falls back
   to the shared generic `"Button"`), set one explicitly.

2. **`Button`/`ToolButton` auto-derive their objectName from their own `text` property**, with
   whitespace stripped (`"Sign in"` -> `"Signin"`). This is why `OKButton`/`CancelButton`/
   `YesButton`/`NoButton` work without a single explicit `objectName:` anywhere near them - they're
   base-Dialog buttons whose `text` already IS "OK"/"Cancel"/"Yes"/"No", with `"Button"` appended by
   convention at each concrete usage site. If your button has no `text` (icon-only) or its `text` is
   dynamic/localizable-only, it will collide with every other untexted button on the page unless
   you set `objectName:` explicitly. Comment WHY when you do this - see convention #6.

3. **Every element inside an open `PopupMenuDialog` is addressed by its own display TEXT**, spaces
   stripped (`dom.popupItem`/`gui.select`) - e.g. a menu item captioned "Close document" resolves to
   `objectName="Closedocument"`. This falls out of the same base-delegate auto-naming as rule #2, not
   anything popup-specific. Only use position-based addressing (`gui.clickPopupItemByIndex`,
   `gui.selectIndex`) when the item's display text is itself dynamic/seeded data with nothing stable
   to assert on - the codebase already reserves ID-only alternatives for exactly the same call sites
   (`select` vs `selectIndex`, `openComboPopup` vs `openComboPopup`+index).

4. **Table rows are `objectName="TableRow_<i>"`, one Table instance at a time.** This numbering is
   NOT globally unique across the page - two Table instances open simultaneously (e.g. a "select
   from list" dialog layered over the page's own collection table behind it) each count their own
   rows from 0. If your feature can have more than one Table visible at once, scope the query to a
   parent container (`['GroupsTable', 'TableRow_0']`) rather than querying `TableRow_0` bare.

5. **A control repeated per-row (Edit/Remove inside a list delegate) cannot carry a globally unique
   objectName** - every row's own instance shares the same generic fallback name (e.g. every row's
   Remove button is just `"Button"`/`"RemoveButton"`, indistinguishable from every OTHER row's).
   Address these with `gui.clickWithin(page, rowLocator, objectName)`, scoping the click to one
   already-located row `Locator` (see `OrderEditorPage.js`'s `productRow(index)` +
   `removeProductRow`, or `SearchResultsView`'s per-row "go to content" button) - do NOT try to make
   the row delegate's inner controls individually unique; that fights the framework instead of using
   it.

6. **Comment every `objectName:` you add purely for testability**, one line, explaining what would
   otherwise happen (the generic fallback it avoids, or the ambiguity it resolves) and that it's
   inert. This isn't busywork - it's what tells the next person touching this file that the property
   is load-bearing for tests and shouldn't be deleted as "unused", and it's what makes a rebuild-
   pending PR reviewable without re-deriving the reasoning. Two real examples already in this
   codebase:
   ```qml
   ToolButton {
       id: expandButton;
       // Test instrumentation: ToolButton derives its objectName from `text`, which this button
       // never sets (icon-only), so it would otherwise resolve to the generic, non-unique "Button"
       // shared by every untexted ToolButton on the page. Inert - no runtime/visual effect.
       objectName: "ExpandProductsButton";
       ...
   }
   ```
   ```qml
   ListView {
       id: productsView;
       // Test instrumentation: scopes row-count / row-lookup queries to just this list (each row's
       // own objectName is the product's own name, which collides across rows sharing a product
       // name). Inert - no runtime/visual effect.
       objectName: "OrderProductsListView";
       ...
   }
   ```

7. **A reusable component used more than once per page/document needs a per-usage-site distinct
   name**, not one baked into the component itself - see `ItemSelectElementView.qml`'s add-button,
   which derives `objectName: "Add" + label.replace(...)` from its own `label` property so the SAME
   component used for both "Roles" and "Groups" on one document produces `"AddRoles"`/`"AddGroups"`
   instead of two colliding `"Button"`s.

8. **Generic modal dialogs (confirm/error/info) all resolve to `objectName="Dialog"`** - the base
   `Dialog` component's own name, not something set per usage. There is normally only one open at a
   time, so `gui.expectVisible(page, ['Dialog'])` / `gui.dismissDialog(page)` (Escape) work without
   knowing which specific dialog it is. Don't invent a more specific name for a one-off confirm/error
   dialog just because you can - screenshot the state instead; a specific name is only worth adding
   when a test needs to distinguish it from ANOTHER simultaneously-open dialog (rare).

9. **`MultiPageView`'s sidebar nav items are `objectName="Page_" + pageId`** (the exact string passed
   to `addPage(pageId, ...)`), already wired into the shared component itself - any NEW consumer of
   `MultiPageView` (Administration's Roles/Users/Groups, Profile's General/Organizations/
   AccessTokens/Access, and whatever comes next) gets this addressing for free, with zero
   per-consumer QML changes.

10. **Tab strips (`TabPanel`/`TabDelegate`) are `objectName="Tab" + model.index`**, positional and
    reused by EVERY `TabPanel` instance on the page (document tabs, collection-vs-document tabs,
    search-result category tabs, ...) - the same "Tab0"/"Tab1" names are NOT unique across multiple
    simultaneously-rendered tab strips. A test that needs to assert "we left tab strip A" cannot
    safely check "Tab0 is hidden" if tab strip B might legitimately have its OWN "Tab0" visible right
    now - check something specific to A's content instead (see search.multiuser.test.js's
    'clicking a result row navigates to that entity', which deliberately does NOT assert on Tab0
    after navigating away, for exactly this reason).

## Before you add a new `objectName`

- Grep the page/component tree for the name you're about to use - collisions are silent (the
  locator engine's `.first()` just picks whichever element happens to come first in DOM order) and
  will not show up as an error, only as a flaky or subtly-wrong test months later.
- Prefer deriving the name from something already stable and unique at that usage site (a `label`/
  `pageId`/`commandId` the component already receives) over inventing a new literal - it keeps the
  name in sync automatically if the component is ever copy-pasted to a third usage site.
- Every `objectName:` addition here is **inert** by construction (a string property with no runtime
  effect on layout/behavior) - if what you're adding does anything visible, it isn't test
  instrumentation, it's a real change, and belongs in the PR description, not this file's spirit.
- All of these additions require a WASM rebuild before a test can use them - batch several into one
  PR/rebuild cycle rather than rebuilding once per test file when you're adding coverage for a whole
  feature area.
