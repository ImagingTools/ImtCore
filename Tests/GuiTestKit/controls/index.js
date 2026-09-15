// Control wrappers: thin, stateless factories over lib/actions, each keyed by an objectName path.
// They give page objects a readable vocabulary (combo.select('QUISS'), commandBar.run('Save'),
// table.sortBy('Name')) instead of raw path arrays, and they inherit the hard-fail-on-missing
// behaviour from lib/actions - a control method never silently does nothing.

const gui = require('../lib/gui');

/** A generic clickable button addressed by an objectName path (or single name). */
class Button {
  constructor(page, pathOrName) {
    this.page = page;
    this.path = Array.isArray(pathOrName) ? pathOrName : [pathOrName];
  }
  click() {
    return gui.clickButton(this.page, this.path);
  }
  expectVisible() {
    return gui.expectVisible(this.page, this.path);
  }
  expectHidden() {
    return gui.expectHidden(this.page, this.path);
  }
}

/** The top command bar (CommandsView). Commands are addressed by their element id -> <id>Button. */
class CommandBar {
  constructor(page) {
    this.page = page;
  }
  run(commandId) {
    return gui.clickCommand(this.page, commandId);
  }
  /**
   * Whether this command can actually be driven right now.
   *
   * A command the user lacks stays in the DOM as a hidden button (verified live: accounts "New" for a
   * user without AddAccount) - that is a genuine "no". Being off the bar is NOT: a command declared
   * with priority -1 never gets a bar button, and a narrow bar pushes others off, both into the "..."
   * overflow - and run() drives those, so they count as available.
   */
  async isAvailable(commandId) {
    // Anchored on the bar's OTHER buttons: they arrive together, on a GetCommands round-trip that
    // regularly outlasts a two-second look under four workers. Asking before any of them is on screen
    // answered "not offered" for a superuser and skipped whole blocks green - see dom.isOffered.
    const offered = await gui.dom.isOffered(this.page, ['CommandsView', `${commandId}Button`], {
      anchorSelector: '[objectName$="Button"]',
      anchorScope: ['CommandsView'],
    });
    if (offered) return true;
    if (!(await gui.dom.isVisible(this.page, ['MoreCommandsButton'], 1000))) return false;
    await gui.click(this.page, ['MoreCommandsButton'], { what: 'the "..." command overflow menu' });
    const present = (await gui.dom.countVisible(this.page, ['PopupMenuDialog', `PopupItem_${commandId}`])) > 0;
    await this.page.keyboard.press('Escape');
    await gui.waitForStable(this.page);
    return present;
  }
}

// How long the left menu itself may take to paint before "is this page offered?" becomes unanswerable.
// Generous on purpose: this is a cold app boot competing with every other worker, and the cost is only
// ever paid when something is genuinely wrong.
const MENU_RENDER_TIMEOUT = 30000;

/** The left navigation menu (MenuPanel). Pages are addressed by PageId -> <PageId>Button. */
class MenuPanel {
  constructor(page) {
    this.page = page;
  }
  open(pageId) {
    return gui.openPage(this.page, pageId);
  }
  /**
   * Whether this page is reachable for the logged-in user - i.e. the client put it in the menu.
   *
   * Two stages, because a single short probe cannot tell "not offered" from "not painted yet", and
   * under worker contention it silently answers the wrong one: seen live, `su` - a superuser - was
   * reported as having no Administration page four times in one run, and four tests skipped
   * themselves green. So first wait GENEROUSLY for the menu itself, then ask about the button with a
   * short timeout. Once the menu is up its buttons are up with it, so a missing button at that point
   * really is a button the client chose not to offer.
   */
  async hasPage(pageId) {
    const menuIsUp = await gui.dom.isVisible(this.page, ['MenuPanel'], MENU_RENDER_TIMEOUT);
    if (!menuIsUp) {
      throw new Error(
        `the left menu never appeared within ${MENU_RENDER_TIMEOUT}ms, so whether "${pageId}" is ` +
          'offered to this user cannot be answered - treating that as "not offered" would skip tests green'
      );
    }
    // The menu frame paints before the pages inside it do, so "MenuPanel is up" is not yet an answer -
    // anchor on the page buttons themselves.
    return gui.dom.isOffered(this.page, ['MenuPanel', `${pageId}Button`], {
      anchorSelector: '[objectName$="Button"]',
      anchorScope: ['MenuPanel'],
    });
  }
}

/** A ComboBox / dropdown. */
class ComboBox {
  constructor(page, path) {
    this.page = page;
    this.path = path;
  }
  open() {
    return gui.click(this.page, this.path, { what: 'combo box' });
  }
  select(itemText) {
    return gui.select(this.page, this.path, itemText);
  }
  /** Select the Nth item by position - use when the real catalogue text isn't a stable constant. */
  selectIndex(index) {
    return gui.selectIndex(this.page, this.path, index);
  }
  /**
   * Whether this combo currently offers an option with the given text - opens the popup, checks, and
   * closes it again (leaving the combo unselected). Use to make a data-dependent selection adaptive:
   * a filter whose option list is populated from data the current user cannot see (e.g. the Customers
   * filter for a user whose org resolves to zero customers) legitimately won't contain a given entry,
   * and forcing selectFilterOption('customers','QUISS') there is a data mismatch, not a real failure.
   * @param {string} itemText
   */
  /**
   * How many options this combo currently offers - 0 when it offers none.
   *
   * A combo whose list is built from data the current user can see is legitimately empty for some
   * users (the Customers combo for an org-scoped user resolves to zero), and ComboBox.qml's
   * onMouseAreaClicked returns early on an empty model, so such a combo never opens a popup at all.
   * Selecting from it then fails as "did not open its popup after 3 attempts", which reads like a
   * broken control rather than a user who simply has nothing to pick. Ask this first and skip on 0.
   * @returns {Promise<number>}
   */
  async optionCount() {
    try {
      await gui.openComboPopup(this.page, this.path);
    } catch (_) {
      return 0;
    }
    const count = await this.page.locator('[objectName="PopupMenuDialog"] [objectName="MouseArea"][visible]').count();
    await this.page.keyboard.press('Escape');
    await gui.waitForStable(this.page);
    return count;
  }

  async hasOption(itemText) {
    // A filter combo whose option list is empty for this user (e.g. the Customers filter for an
    // org-scoped user with zero visible customers) may not open a popup at all - treat "couldn't open"
    // as "no such option" rather than letting openComboPopup throw, since the caller uses this to
    // DECIDE whether to skip.
    try {
      await gui.openComboPopup(this.page, this.path);
    } catch (_) {
      return false;
    }
    const present = (await gui.dom.popupItem(this.page, itemText).count()) > 0;
    // Close the popup so the caller starts from a clean state (Escape dismisses PopupMenuDialog).
    await this.page.keyboard.press('Escape');
    await gui.waitForStable(this.page);
    return present;
  }
  clear() {
    return gui.clickButton(this.page, [...this.path, 'ClearButton']);
  }
}

/** A text input (wraps an inner [objectName="TextInput"]). */
class TextInput {
  constructor(page, path) {
    this.page = page;
    this.path = path;
  }
  fill(text, opts) {
    return gui.fill(this.page, this.path, text, opts);
  }
  clear() {
    return gui.clickButton(this.page, [...this.path, 'ClearText']);
  }
  /** Current value, or null when this control exposes none. */
  value() {
    return gui.textInputValue(this.page, this.path);
  }
  /**
   * Poll this field's DOM value until it satisfies `predicate` (default: matches `expected` exactly).
   * Use after a command that reverts/sets this field as a SIDE EFFECT (Undo/Redo) instead of trusting
   * generic DOM-quiet: confirmed live, an editor field's value-commit after Undo can lag behind the
   * command's own settle under concurrent-worker load, so a screenshot taken right after undo() can
   * stably (reproducibly, not a one-off flicker) still show the pre-undo value. See
   * waitForTextInputValue's own comment for the full story.
   * @param {string} [expected] defaults to '' (the field reverted to empty)
   */
  waitForValue(expected = '') {
    return gui.waitForTextInputValue(this.page, this.path, (v) => v === expected);
  }
}

// SearchTextInput (imtcontrols/Inputs/SearchTextInput.qml) holds the typed text for 500ms before it
// emits searchChanged and the collection reloads. Typing itself stops mutating the DOM immediately, so
// waitForStable's quiet window expires INSIDE that gap and reports settled while the reload has not even
// started - and a click issued in that window is dropped by the app, silently and reproducibly (the
// "Clear all filters" click right after a search was swallowed on every run; clicking again worked).
const SEARCH_DEBOUNCE_MS = 500;

/** The collection filter panel (FilterPanel). */
class FilterPanel {
  constructor(page) {
    this.page = page;
  }
  async search(text) {
    await gui.fill(this.page, ['FilterPanel', 'SearchTextInput'], text);
    await this.page.waitForTimeout(SEARCH_DEBOUNCE_MS);
    await gui.waitForStable(this.page);
  }
  clearSearch() {
    return gui.clickButton(this.page, ['SearchTextInput', 'ClearText']);
  }
  /**
   * Click the "Clear all filters" button (registered filters + built-in search + sorting - see
   * CollectionViewBase.qml's onClearAllFilters). Explicitly waits for the search box to actually READ
   * empty afterward rather than trusting generic DOM-quiet: confirmed live, under concurrent-worker
   * load the search input's value-commit can lag behind the click's own settle, so a screenshot taken
   * right after the click can stably (not flickering - reproducibly) still show the pre-clear search
   * text. See waitForTextInputValue's own comment for the full story.
   */
  async clearAllFilters() {
    await gui.clickButton(this.page, ['FilterPanel', 'ClearAllFilters']);
    await gui.waitForTextInputValue(this.page, ['FilterPanel', 'SearchTextInput']);
  }
  combo(filterObjectName) {
    return new ComboBox(this.page, ['FilterPanel', filterObjectName]);
  }
  clearFilter(filterObjectName) {
    return gui.clickButton(this.page, ['FilterPanel', filterObjectName, 'ClearButton']);
  }
  /** Open a date-range filter and pick a preset (e.g. 'Month_Current', 'Year_Last'). */
  async dateFilter(dateFilterObjectName, preset) {
    await gui.clickButton(this.page, ['FilterPanel', dateFilterObjectName]);
    await gui.clickButton(this.page, ['TimeFilterParamView', preset]);
  }
}

/**
 * A collection table. Rows are addressable by index thanks to the objectName "TableRow_<i>" added to
 * TableRowDelegateBase.qml; columns are addressable by their header id (TableHeaderDelegate sets
 * objectName: headerId). "Table" / "TableHeaders" wrap the whole table.
 *
 * "TableRow_<i>" numbering is NOT globally unique across the page - every Table instance numbers its
 * own rows from 0, so if a second Table is on screen at the same time (e.g. TableHeaderParamComp.qml's
 * column-visibility list, opened as a dialog over the collection's own table), a bare
 * `[objectName="TableRow_i"]` matches BOTH and `.first()` picks whichever the browser happens to
 * return first - not necessarily the one you meant. Pass `scope` (an objectName path prefix, e.g.
 * `['Dialog']`) to disambiguate; leave it empty for the one-table-on-screen default case (the main
 * collection page).
 */
class Table {
  constructor(page, scope = []) {
    this.page = page;
    this.scope = scope;
  }
  /** Click a row by zero-based index (selects it). */
  selectRow(index) {
    return gui.click(this.page, [...this.scope, `TableRow_${index}`], { what: `table row ${index}` });
  }
  /**
   * Toggle a row's checkbox by zero-based index - only present when the table is `checkable`
   * (e.g. TableHeaderParamComp.qml's column-visibility list). This is a SEPARATE control from the
   * row's own selection click: the checkbox is a distinct "RowCheckBox" overlay
   * (TableRowDelegateBase.qml), not toggled by selectRow().
   */
  toggleRowCheck(index) {
    return gui.click(this.page, [...this.scope, `TableRow_${index}`, 'RowCheckBox'], { what: `row ${index} checkbox` });
  }
  /**
   * Screenshot masks ({x,y,width,height}) covering the given columns (matched via
   * TableCellDelegateBase's objectName == headerId) - use for columns whose value changes across
   * runs/edits (e.g. Added/Last Modified timestamps) so screenshots stay deterministic. One rect per
   * column, clamped to the table's actual rendered row extent (see dom.columnRects). Silently yields
   * no mask for a headerId that isn't currently rendered/visible (e.g. scrolled out of view) - this is
   * a noise-reducer, not a structural assertion.
   *
   * Waits for the DOM to settle FIRST: the typical call site is
   * `checkScreenshot(page, name, await page.timestampColumnMasks())`, where this runs right after some
   * triggering action (a filter clear, a reload, ...) whose OWN click() already waited once - but a
   * reload that re-fetches a much bigger page (e.g. clearing a filter back to a large unfiltered
   * collection) can still be mid-flight a moment later. Computing the rects against that in-between
   * state bakes in a mask sized to whatever ROW COUNT happened to be rendered at that instant - too
   * small once the rest of the page's rows arrive, leaving them unmasked and genuinely flaky (caught
   * live: a 25-row Orders reload after clearAllFilters() left ~23 rows' worth of real, differing
   * timestamps outside a mask sized for only the first 2). An extra wait here, right before measuring,
   * costs nothing when the DOM was already quiet and closes this window when it wasn't.
   */
  async columnMasks(headerIds) {
    await gui.waitForStable(this.page);
    const rects = await gui.dom.columnRects(this.page, Array.isArray(headerIds) ? headerIds : [headerIds]);
    // A little more than a hairline: under maxDiffPixels 0 a single stray antialiased pixel at the
    // edge of a masked cell is a failure.
    return rects.map((r) => ({ ...r, padding: 3 }));
  }
  /** Number of rows currently in the DOM (regardless of visibility). */
  rowCount() {
    const prefix = this.scope.length ? `${gui.dom.selectorForPath(this.scope)} ` : '';
    return this.page.locator(`${prefix}[objectName^="TableRow_"]`).count();
  }
  /**
   * Number of currently VISIBLE rows. An "empty" collection is NOT reliably detectable via rowCount():
   * the table keeps a hidden TableRow_0 placeholder in the DOM even when it holds no data, so
   * rowCount() returns >=1 for an empty table (this silently defeated an emptiness guard). The [visible]
   * attribute the QML bridge sets IS cleared for that placeholder, so filtering on it gives the true
   * data-row count.
   */
  visibleRowCount() {
    const prefix = this.scope.length ? `${gui.dom.selectorForPath(this.scope)} ` : '';
    return this.page.locator(`${prefix}[objectName^="TableRow_"][visible]`).count();
  }
  /**
   * Resolve whether this collection actually has data rows for the current user, tolerating slow
   * first-render under concurrent load. Polls for a visible row up to `timeout`, returning true as soon
   * as one appears; only after the timeout with still no visible row does it conclude the collection is
   * genuinely empty (e.g. an org-scoped account list that resolves to zero rows) and return false. This
   * distinction matters: a row-interaction test should SKIP on a genuinely-empty collection (there is
   * no row to act on - not a bug) but must NOT skip just because rows were slow to paint.
   * @param {number} [timeout]
   */
  async hasRows(timeout = 6000) {
    const deadline = Date.now() + timeout;
    for (;;) {
      if ((await this.visibleRowCount()) > 0) return true;
      if (Date.now() >= deadline) return false;
      await this.page.waitForTimeout(200);
    }
  }
  /**
   * The visible column header ids, left to right.
   *
   * Use this to assert a REORDER instead of comparing one header's x before and after. A pixel
   * comparison needs its column to be on screen, and the rightmost ones are not always: selecting a row
   * opens a details panel beside the table, which narrows it enough to push the last column out of
   * view - so the measurement hung until the test timed out, on a table that was in fact perfectly fine.
   * Order is what the test actually means, and it survives a narrower table.
   * @returns {Promise<string[]>}
   */
  headerOrder() {
    return this.page.evaluate(() => {
      const headers = document.querySelector('[objectName="TableHeaders"]');
      if (!headers) return [];
      return Array.from(headers.querySelectorAll('[objectName][visible]'))
        .map((el) => el.getAttribute('objectName'))
        .filter((name) => name && name !== 'MouseArea');
    });
  }

  /**
   * The rendered values of one column, top to bottom - addressed by header id, the same way
   * sortBy()/columnMasks() address a column.
   *
   * Use it to assert that a sort actually SORTED, instead of pixel-matching one particular
   * arrangement. On a column whose values repeat (a category with two options, a product id shared by
   * dozens of licenses) the rows that tie have no defined order, so the screenshot differs run to run
   * while the sort is perfectly correct - caught live as a 19537-pixel diff on a licence list ordered
   * by product.
   * @param {string} headerId
   * @returns {Promise<string[]>}
   */
  columnValues(headerId) {
    const prefix = this.scope.length ? `${gui.dom.selectorForPath(this.scope)} ` : '';
    return this.page.evaluate(
      ({ prefix, headerId }) =>
        Array.from(document.querySelectorAll(`${prefix}[objectName^="TableRow_"][visible]`))
          .map((row) => row.querySelector(`[objectName="${headerId}"]`))
          .filter(Boolean)
          .map((cell) => (cell.textContent || '').trim()),
      { prefix, headerId }
    );
  }

  /**
   * Sort by a column, addressed by its header field id - this is the page's HeaderIds entry, NOT the
   * visible HeaderNames caption (the two lists are independently ordered per *Page.acc, e.g. Devices/
   * SoftwareProducts's "Name" caption maps to id "licenseName", not "name"). Confirm the real id in
   * the collection's *Page.acc before using a new one here.
   * @example sortBy('status') // Devices/Orders/SoftwareProducts "Status" column
   * @example sortBy('macAddress') // Devices "MAC Address" column
   */
  sortBy(headerId) {
    return gui.click(this.page, ['TableHeaders', headerId], { what: `column "${headerId}"` });
  }
  /** Assert the table has at least `n` rows (row 0..n-1 visible). */
  async expectAtLeastRows(n) {
    await gui.expectVisible(this.page, [`TableRow_${n - 1}`], `expected at least ${n} table rows`);
  }
  expectRow(index) {
    return gui.expectVisible(this.page, [`TableRow_${index}`]);
  }
}

/** The collection pagination bar (Pagination.qml). */
class Pagination {
  constructor(page) {
    this.page = page;
  }
  /** Choose page size from the per-page combo (values: 25/50/100/250/500). */
  setPageSize(size) {
    return gui.select(this.page, ['Pagination', 'PageSizeCombo'], String(size));
  }
  /** Go to a page by its 1-based number (page buttons are auto-named "<n>Button"). */
  goToPage(pageNumber) {
    return gui.click(this.page, ['Pagination', `${pageNumber}Button`], { what: `page ${pageNumber}` });
  }
  /**
   * Whether a given 1-based page button is present (i.e. the collection actually has that many pages
   * at the current page size). Use to make a pagination test data-adaptive: a collection smaller than
   * one page (e.g. 14 accounts at page size 50) legitimately has no page 2, and forcing goToPage(2)
   * there is a test/data mismatch, not a real failure. Counts elements without the visibility filter
   * and without waiting, so it reflects the settled pagination bar the caller just rendered.
   * @param {number} pageNumber
   */
  async hasPage(pageNumber) {
    return (await gui.dom.countVisible(this.page, ['Pagination', `${pageNumber}Button`])) > 0;
  }
  next() {
    return gui.click(this.page, ['Pagination', 'NextPageButton'], { what: 'next page' });
  }
  prev() {
    return gui.click(this.page, ['Pagination', 'PrevPageButton'], { what: 'prev page' });
  }
}

/** A labelled on/off switch (SwitchElementView -> inner SwitchButton). */
class Switch {
  constructor(page, path) {
    this.page = page;
    this.path = path;
  }
  toggle() {
    return gui.click(this.page, [...this.path, 'SwitchButton'], { what: 'switch' });
  }
}

/** A modal dialog with confirm/cancel style buttons. */
class Dialog {
  constructor(page) {
    this.page = page;
  }
  clickButtonByText(text) {
    // imtcontrols Button auto-names as "<text w/o spaces>Button".
    return gui.clickButton(this.page, [`${text.replace(/\s/g, '')}Button`]);
  }
  confirm() {
    return this.clickButtonByText('Yes');
  }
  cancel() {
    return this.clickButtonByText('No');
  }
  ok() {
    return this.clickButtonByText('OK');
  }
  close() {
    return gui.clickButton(this.page, ['CloseButton']);
  }
}

/**
 * The "Table configuration" dialog (imtcontrols/Views/TableHeaderParamComp.qml), opened by
 * right-clicking any sortable column header (CollectionViewBase.qml's headerRightClickEnabled).
 * Lets a user toggle column visibility (checkbox per row) and reorder columns (Up/Down), or reset
 * to defaults. Its column list used to be a Table, whose rows the bridge named "TableRow_<i>"; it is
 * now a ListView whose delegate carries its own "ColumnRow_<i>" (added for these tests - a plain
 * delegate has no name of its own). Addressing by that name also retires the old ambiguity with the
 * collection's own table behind the dialog, which numbers its rows from 0 as well.
 */
class TableConfigDialog {
  constructor(page) {
    this.page = page;
  }
  /**
   * How many columns the dialog lists. Throws on zero rather than returning it: every caller derives an
   * index from this ("the last row" is `rowCount() - 1`), so a zero silently becomes -1 and the failure
   * surfaces far away as a missing "ColumnRow_-1". Zero here means the list did not render, which is
   * worth saying where it happens.
   */
  async rowCount() {
    const count = await this.page.locator('[objectName^="ColumnRow_"]').count();
    if (count === 0) {
      throw new Error('GUI table-configuration dialog lists no columns - it did not render, or its rows are not named');
    }
    return count;
  }
  /**
   * The column names the dialog lists, top to bottom.
   *
   * Reading the DIALOG rather than the table is what lets a reorder be asserted without applying it:
   * the per-user column layout lives on the server, so an Apply is visible to every other test signed
   * in as the same user - measured as a 28987-pixel diff in an unrelated screenshot.
   * @returns {Promise<string[]>}
   */
  columnNames() {
    return this.page.evaluate(() =>
      Array.from(document.querySelectorAll('[objectName^="ColumnRow_"]'))
        .sort((a, b) => {
          const index = (el) => parseInt(el.getAttribute('objectName').split('_')[1], 10);
          return index(a) - index(b);
        })
        .map((row) => {
          const title = row.querySelector('[objectName="ColumnRowTitle"]');
          return title ? (title.textContent || '').trim() : '';
        })
    );
  }

  /** Right-click a column header (by its header/field id) to open this dialog. */
  async openViaHeader(headerId) {
    const header = gui.dom.byPath(this.page, ['TableHeaders', headerId]);
    await header.waitFor({ state: 'visible', timeout: gui.DEFAULT_TIMEOUT });
    const box = await header.boundingBox();
    if (!box) throw new Error(`GUI table header "${headerId}" has no bounding box`);
    await this.page.mouse.click(box.x + box.width / 2, box.y + box.height / 2, { button: 'right' });
    await gui.waitForStable(this.page);
    return this;
  }
  /** Toggle a column's visibility checkbox by its zero-based row position in the column list. */
  toggleColumn(rowIndex) {
    return gui.click(this.page, [`ColumnRow_${rowIndex}`, 'ColumnCheckBox'], {
      what: `column ${rowIndex} visibility checkbox`,
    });
  }
  /**
   * Select a column's row (needed before moveUp()/moveDown(), which act on the current selection).
   *
   * clickSelf, not click: the row IS the clickable surface (its delegate is filled by a bare MouseArea
   * that sets currentRow on press), while click() targets the first inner [objectName="MouseArea"] -
   * and the row contains a checkbox that has one of those. Selecting a row must not be able to land on
   * a control inside it.
   */
  selectColumn(rowIndex) {
    return gui.clickSelf(this.page, [`ColumnRow_${rowIndex}`], { what: `column row ${rowIndex}` });
  }
  moveUp() {
    return gui.clickButton(this.page, ['MoveColumnUpButton']);
  }
  moveDown() {
    return gui.clickButton(this.page, ['MoveColumnDownButton']);
  }
  /** Opens the "Reset header settings to default?" confirm - use confirmReset()/cancelReset() next. */
  reset() {
    return gui.clickButton(this.page, ['ResetColumnsButton']);
  }
  confirmReset() {
    return gui.clickButton(this.page, ['YesButton']);
  }
  cancelReset() {
    return gui.clickButton(this.page, ['NoButton']);
  }
  fitToWidth() {
    return gui.clickButton(this.page, ['FitToWidthButton']);
  }
  apply() {
    return gui.clickButton(this.page, ['ApplyButton']);
  }
  cancel() {
    return gui.clickButton(this.page, ['CancelButton']);
  }
}

/**
 * A TreeExplorerView (imtcontrols/Views/TreeExplorerView.qml): the breadcrumb + command row + search +
 * row list ImtCore uses wherever a nested collection is edited in place (Lisa's sub-features, a
 * product's features, a license's features).
 *
 * It is NOT a Table: its rows carry "ExplorerRow_<i>", not "TableRow_<i>", so the two never collide
 * when a collection table sits behind an editor. Everything is addressed relative to the explorer's
 * own objectName, which the embedding view sets (e.g. 'SubfeaturesExplorer'), so two explorers on one
 * page stay apart.
 *
 * Commands fold into a "···" overflow when the pane is narrow (the view's own `compact` breakpoint),
 * which is why run() looks there instead of failing on a button that simply is not on the bar now.
 */
class TreeExplorer {
  /**
   * @param {import('@playwright/test').Page} page
   * @param {string} objectName  the explorer's own objectName
   */
  constructor(page, objectName) {
    this.page = page;
    this.root = [objectName];
    this.search = new TextInput(page, [...this.root, 'ExplorerSearchInput']);
  }

  path(...segments) {
    return [...this.root, ...segments];
  }

  /**
   * Type into the explorer's own filter box and wait for the filtering to actually happen.
   *
   * Not `search.fill(...)`: SearchTextInput holds the typed text for SEARCH_DEBOUNCE_MS before it
   * emits searchChanged, and typing stops mutating the DOM immediately - so waitForStable reports
   * settled while the list is still unfiltered, and whatever the test does next acts on the OLD rows
   * (seen live: a row ticked right after a search was the first row of the UNFILTERED list).
   */
  async filter(text) {
    await this.search.fill(text);
    await this.page.waitForTimeout(SEARCH_DEBOUNCE_MS);
    await gui.waitForStable(this.page);
    return this;
  }

  /** Number of rows currently rendered at this level. */
  rowCount() {
    return this.page.locator(`${gui.dom.selectorForPath(this.root)} [objectName^="ExplorerRow_"][visible]`).count();
  }

  /** Whether this level has rows, tolerating a slow first render (same contract as Table.hasRows). */
  async hasRows(timeout = 6000) {
    const deadline = Date.now() + timeout;
    for (;;) {
      if ((await this.rowCount()) > 0) return true;
      if (Date.now() >= deadline) return false;
      await this.page.waitForTimeout(200);
    }
  }

  /**
   * Select a row. In this control selecting and ticking are ONE set (TreeExplorerView's selectNode,
   * toggleChecked and commandTargets all read `checkedNodes`) - deliberately, so "the commands are
   * lit but nothing is highlighted" cannot happen. A selected row is therefore already a command
   * target, and is already ticked.
   */
  selectRow(index) {
    return gui.click(this.page, this.path(`ExplorerRow_${index}`), { what: `explorer row ${index}` });
  }

  /**
   * TOGGLE a row's box, to build a selection of more than one row. It is a toggle over the same set
   * selectRow() writes, so calling it on a row that is already selected UNTICKS that row and leaves
   * the explorer with no command target at all (seen live: a Remove that then silently did nothing).
   * Use selectRow() for the first row and checkRow() for each additional one.
   *
   * The box itself is inert (`mouseArea.enabled: false`, so its tick keeps following checkState); the
   * click target is the sibling hit area beside it.
   */
  checkRow(index) {
    return gui.click(this.page, this.path(`ExplorerRow_${index}`, 'RowCheckBoxHit'), {
      what: `explorer row ${index} checkbox`,
    });
  }

  /** Tick/clear every row at this level, via the column header's tri-state box. */
  checkAll() {
    return gui.click(this.page, this.path('SelectAllCheckBoxHit'), { what: 'the select-all checkbox' });
  }

  /**
   * Run one of the explorer's own commands, by the objectName TreeExplorerView.qml gives it: Create,
   * OpenLevel, EditRow, RenameRow, MoveRow, RemoveRows, CommitRow, CancelRow, MoveHere, CancelMove.
   */
  async run(commandId) {
    const onBar = this.path(`${commandId}Button`);
    if ((await gui.dom.countVisible(this.page, onBar)) > 0) {
      await gui.click(this.page, onBar, { what: `explorer command "${commandId}"` });
      return;
    }
    const more = this.path('ExplorerMoreButton');
    if ((await gui.dom.countVisible(this.page, more)) === 0) {
      // Not on the bar and no overflow to look in - let the direct click report the real error.
      await gui.click(this.page, onBar, { what: `explorer command "${commandId}"` });
      return;
    }
    await gui.click(this.page, more, { what: 'the explorer "···" overflow menu' });
    // The overflow spells the same commands without the row/rows suffix (OverflowEditButton for
    // EditRow, OverflowRemoveButton for RemoveRows - see TreeExplorerView.qml's overflowColumn).
    await gui.click(this.page, this.path(`Overflow${commandId.replace(/Rows?$/, '')}Button`), {
      what: `explorer command "${commandId}" in the overflow menu`,
    });
  }

  create() {
    return this.run('Create');
  }
  editRow() {
    return this.run('EditRow');
  }
  commitRow() {
    return this.run('CommitRow');
  }
  cancelRow() {
    return this.run('CancelRow');
  }
  removeRows() {
    return this.run('RemoveRows');
  }
  openLevel() {
    return this.run('OpenLevel');
  }

  /** Go back up one level (the button shown only below the root). */
  navigateUp() {
    return gui.click(this.page, this.path('NavigateUpButton'), { what: 'the explorer up button' });
  }

  /** Jump to a breadcrumb segment by depth (0 is the root). */
  navigateToDepth(depth) {
    return gui.click(this.page, this.path(`Breadcrumb_${depth}`), { what: `breadcrumb segment ${depth}` });
  }

  /** The breadcrumb trail, root first. */
  breadcrumbs() {
    return this.page.evaluate((selector) => {
      const scope = document.querySelector(selector);
      if (!scope) return [];
      return Array.from(scope.querySelectorAll('[objectName^="Breadcrumb_"][visible]')).map((el) =>
        (el.textContent || '').trim()
      );
    }, gui.dom.selectorForPath(this.root));
  }

  /** The side panel this explorer hosts, addressed by the panel's own objectName. */
  panel(objectName) {
    return new CheckableListPanel(this.page, objectName);
  }
}

/**
 * A CheckableListPanel (imtcontrols/Views/CheckableListPanel.qml) - the searchable, tick-per-row list
 * ImtCore puts beside a TreeExplorerView (Lisa's Dependencies / Feature content / Inherited licenses)
 * and inside pickers such as Lisa's "Select features" dialog. Addressed by its own objectName, so a
 * dialog's panel over a page's panel stays unambiguous.
 */
class CheckableListPanel {
  constructor(page, objectName) {
    this.page = page;
    this.root = [objectName];
    this.search = new TextInput(page, [...this.root, 'PanelSearchInput']);
  }

  path(...segments) {
    return [...this.root, ...segments];
  }

  /**
   * Type into the panel's filter box and wait for the filtering to actually happen - see
   * TreeExplorer.filter for why `search.fill(...)` alone is not enough.
   */
  async filter(text) {
    await this.search.fill(text);
    await this.page.waitForTimeout(SEARCH_DEBOUNCE_MS);
    await gui.waitForStable(this.page);
    return this;
  }

  rowCount() {
    return this.page.locator(`${gui.dom.selectorForPath(this.root)} [objectName^="PanelRow_"][visible]`).count();
  }

  /** Whether the panel lists anything, tolerating a slow first render. */
  async hasRows(timeout = 6000) {
    const deadline = Date.now() + timeout;
    for (;;) {
      if ((await this.rowCount()) > 0) return true;
      if (Date.now() >= deadline) return false;
      await this.page.waitForTimeout(200);
    }
  }

  /**
   * Tick/untick a row. The whole row is the hit area (the panel's entryMouse); the box itself is
   * inert, exactly as in TreeExplorerView.
   */
  toggleRow(index) {
    return gui.click(this.page, this.path(`PanelRow_${index}`), { what: `panel row ${index}` });
  }

  /** The visible row titles, top to bottom. */
  rowTitles() {
    return this.page.evaluate((selector) => {
      const scope = document.querySelector(selector);
      if (!scope) return [];
      return Array.from(scope.querySelectorAll('[objectName="PanelRowTitle"][visible]')).map((el) =>
        (el.textContent || '').trim()
      );
    }, gui.dom.selectorForPath(this.root));
  }

  /** The header command - "Clear" / "Take all" / "Select all", whatever it currently says. */
  runAction() {
    return gui.click(this.page, this.path('PanelActionButton'), { what: 'the panel action command' });
  }

  /**
   * Whether the panel is offering its header command at all. The owner decides: CheckableListPanel
   * hides it whenever `actionText` is empty, which is how a panel says there is nothing to act on
   * (Lisa's Feature content panel does exactly that for a feature with no OPTIONAL parts - it still
   * lists the mandatory ones, so a row count is not the same question). Ask this before runAction()
   * rather than treating an absent command as a broken control.
   */
  hasAction() {
    return gui.dom.isVisible(this.page, this.path('PanelActionButton'), 1000);
  }

  /** Whether the panel shows its placeholder instead of a list (nothing selected / nothing to show). */
  isPlaceholderShown() {
    return gui.dom.isVisible(this.page, this.path('PanelPlaceholder'), 1000);
  }
}

module.exports = {
  Button,
  CommandBar,
  MenuPanel,
  ComboBox,
  TextInput,
  FilterPanel,
  Table,
  Pagination,
  Switch,
  Dialog,
  TableConfigDialog,
  TreeExplorer,
  CheckableListPanel,
};
