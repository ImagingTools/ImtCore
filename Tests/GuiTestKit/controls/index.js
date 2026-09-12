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
   * Whether this command can actually be driven right now: its button is visible, so a click reaches
   * it. Deliberately does not try to say WHY it isn't - a command the user lacks stays in the DOM as a
   * hidden button (verified live: accounts "New" for a user without AddAccount), and a command the user
   * holds can also be hidden because it overflowed into the "..." menu (verified live: fullAccess's
   * ResetTransferCounter). Both mean the same thing to a test: this flow is not drivable for this user,
   * so skip it rather than fail.
   */
  isAvailable(commandId) {
    return gui.dom.isVisible(this.page, ['CommandsView', `${commandId}Button`]);
  }
}

/** The left navigation menu (MenuPanel). Pages are addressed by PageId -> <PageId>Button. */
class MenuPanel {
  constructor(page) {
    this.page = page;
  }
  open(pageId) {
    return gui.openPage(this.page, pageId);
  }
  /** Whether this page is reachable for the logged-in user - i.e. the client put it in the menu. */
  hasPage(pageId) {
    return gui.dom.isVisible(this.page, ['MenuPanel', `${pageId}Button`]);
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

/** The collection filter panel (FilterPanel). */
class FilterPanel {
  constructor(page) {
    this.page = page;
  }
  search(text) {
    return gui.fill(this.page, ['FilterPanel', 'SearchTextInput'], text);
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
    return rects.map((r) => ({ ...r, padding: 1 }));
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
  /** Select a column's row (needed before moveUp()/moveDown(), which act on the current selection). */
  selectColumn(rowIndex) {
    return gui.click(this.page, [`ColumnRow_${rowIndex}`], { what: `column row ${rowIndex}` });
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
};
