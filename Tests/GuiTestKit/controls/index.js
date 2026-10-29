// Control wrappers over lib/actions, each keyed by an objectName path.

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
  /** Whether this command can actually be driven right now. */
  async isAvailable(commandId) {
    // Anchor on the bar's other buttons so a slow GetCommands round-trip isn't misread as "not offered".
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

// Max time the left menu may take to paint before "is this page offered?" is unanswerable.
const MENU_RENDER_TIMEOUT = 30000;

/** The left navigation menu (MenuPanel). Pages are addressed by PageId -> <PageId>Button. */
class MenuPanel {
  constructor(page) {
    this.page = page;
  }
  open(pageId) {
    return gui.openPage(this.page, pageId);
  }
  /** Whether this page is reachable for the logged-in user (the client put it in the menu). */
  async hasPage(pageId) {
    const menuIsUp = await gui.dom.isVisible(this.page, ['MenuPanel'], MENU_RENDER_TIMEOUT);
    if (!menuIsUp) {
      throw new Error(
        `the left menu never appeared within ${MENU_RENDER_TIMEOUT}ms, so whether "${pageId}" is ` +
          'offered to this user cannot be answered - treating that as "not offered" would skip tests green'
      );
    }
    // The menu frame paints before its page buttons, so anchor on the buttons themselves.
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
   * Whether this combo currently offers an option with the given text.
   * @param {string} itemText
   */
  /**
   * How many options this combo currently offers - 0 when it offers none.
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
    // A combo whose option list is empty for this user may not open a popup - treat that as "no option".
    try {
      await gui.openComboPopup(this.page, this.path);
    } catch (_) {
      return false;
    }
    const present = (await gui.dom.popupItem(this.page, itemText).count()) > 0;
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
   * Use after a command that reverts/sets this field as a side effect (Undo/Redo), whose value-commit
   * can lag behind the command's own settle under load.
   * @param {string} [expected] defaults to '' (the field reverted to empty)
   */
  waitForValue(expected = '') {
    return gui.waitForTextInputValue(this.page, this.path, (v) => v === expected);
  }
}

// SearchTextInput holds typed text for 500ms before emitting searchChanged and reloading the collection.
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
   * Click the "Clear all filters" button (registered filters + built-in search + sorting). Waits for
   * the search box to actually read empty afterward, since its value-commit can lag the click's settle.
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
 * A collection table. Rows are addressable by index via objectName "TableRow_<i>"; columns by their
 * header id. "Table" / "TableHeaders" wrap the whole table.
 *
 * "TableRow_<i>" numbering is NOT globally unique: every Table numbers its own rows from 0, so when a
 * second Table is on screen at once, pass `scope` (an objectName path prefix, e.g. `['Dialog']`) to
 * disambiguate; leave it empty for the one-table-on-screen default.
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
   * Toggle a row's checkbox by zero-based index - only present when the table is `checkable`. This is
   * a separate control ("RowCheckBox") from the row's own selection click.
   */
  toggleRowCheck(index) {
    return gui.click(this.page, [...this.scope, `TableRow_${index}`, 'RowCheckBox'], { what: `row ${index} checkbox` });
  }
  /**
   * Screenshot masks ({x,y,width,height}) covering the given columns - use for columns whose value
   * changes across runs/edits (e.g. Added/Last Modified timestamps) so screenshots stay deterministic.
   * Silently yields no mask for a headerId not currently rendered. Waits for the row set to settle
   * first so the rects are sized against the final row extent, not an in-flight reload.
   */
  async columnMasks(headerIds) {
    await gui.waitForStable(this.page);
    await this.waitForRowExtent();
    const rects = await gui.dom.columnRects(this.page, Array.isArray(headerIds) ? headerIds : [headerIds]);
    // Pad a little so a stray antialiased edge pixel doesn't fail under maxDiffPixels 0.
    return rects.map((r) => ({ ...r, padding: 3 }));
  }
  /**
   * Block until the visible row count holds steady. columnRects() measures the rows rendered AT THAT
   * MOMENT, so a mask taken mid-refill spans fewer rows than the screenshot that follows and leaves the
   * timestamps below it uncovered; waitForStable() does not catch this, a refill being DOM-quiet between
   * batches. 0 is never accepted as settled, since a view still loading looks exactly like an empty one
   * - so only ask a page that HAS a collection on it for column masks, or this costs the full timeout.
   */
  async waitForRowExtent(settleTimeout = 6000, interval = 150) {
    const deadline = Date.now() + settleTimeout;
    let previous = -1;
    for (;;) {
      const count = await this.visibleRowCount();
      if (count > 0 && count === previous) return count;
      if (Date.now() >= deadline) return count;
      previous = count;
      await this.page.waitForTimeout(interval);
    }
  }
  /** Number of rows currently in the DOM (regardless of visibility). */
  rowCount() {
    const prefix = this.scope.length ? `${gui.dom.selectorForPath(this.scope)} ` : '';
    return this.page.locator(`${prefix}[objectName^="TableRow_"]`).count();
  }
  /**
   * Number of currently VISIBLE rows. An empty collection is NOT detectable via rowCount(): the table
   * keeps a hidden TableRow_0 placeholder, so rowCount() returns >=1 for an empty table. The [visible]
   * attribute is cleared for that placeholder, so filtering on it gives the true data-row count.
   */
  visibleRowCount() {
    const prefix = this.scope.length ? `${gui.dom.selectorForPath(this.scope)} ` : '';
    return this.page.locator(`${prefix}[objectName^="TableRow_"][visible]`).count();
  }
  /**
   * Whether this collection has data rows for the current user, tolerating slow first-render. Polls for
   * a visible row up to `timeout`; only after the timeout with none does it conclude the collection is
   * genuinely empty and return false.
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
   * The visible column header ids, left to right. Use this to assert a reorder instead of comparing one
   * header's x, which needs the column on screen and survives a narrower table.
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
   * The rendered values of one column, top to bottom, addressed by header id. Use it to assert a sort
   * actually sorted, instead of pixel-matching one arrangement (tied values have no defined order).
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
   * Sort by a column, addressed by its header field id (the *Page.acc HeaderIds entry, NOT the visible
   * HeaderNames caption - e.g. "Name" may map to id "licenseName"). Confirm the real id before use.
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
   * Whether a given 1-based page button is present (i.e. the collection has that many pages at the
   * current page size). Use to make a pagination test data-adaptive.
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
 * The "Table configuration" dialog, opened by right-clicking any sortable column header. Lets a user
 * toggle column visibility and reorder columns, or reset to defaults. Its column list is a ListView
 * whose rows carry "ColumnRow_<i>".
 */
class TableConfigDialog {
  constructor(page) {
    this.page = page;
  }
  /** How many columns the dialog lists. Throws on zero, which means the list did not render. */
  async rowCount() {
    const count = await this.page.locator('[objectName^="ColumnRow_"]').count();
    if (count === 0) {
      throw new Error('GUI table-configuration dialog lists no columns - it did not render, or its rows are not named');
    }
    return count;
  }
  /**
   * The column names the dialog lists, top to bottom. Reading the dialog rather than the table lets a
   * reorder be asserted without applying it (an Apply persists per-user and leaks to other tests).
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
   * clickSelf, not click: the row itself is the clickable surface, and click() would target the inner
   * checkbox's MouseArea instead.
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
 * A TreeExplorerView: the breadcrumb + command row + search + row list used wherever a nested
 * collection is edited in place.
 *
 * Its rows carry "ExplorerRow_<i>", not "TableRow_<i>", so they never collide with a collection table.
 * Everything is addressed relative to the explorer's own objectName. Commands fold into a "···"
 * overflow when the pane is narrow, which is why run() looks there too.
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
   * Type into the explorer's own filter box and wait for the filtering to happen. Not `search.fill(...)`:
   * SearchTextInput debounces SEARCH_DEBOUNCE_MS, so waitForStable alone would act on the old rows.
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
   * Select a row. Here selecting and ticking are one set (a selected row is already a command target
   * and is already ticked).
   */
  selectRow(index) {
    return gui.click(this.page, this.path(`ExplorerRow_${index}`), { what: `explorer row ${index}` });
  }

  /**
   * TOGGLE a row's box, to build a multi-row selection. It toggles the same set selectRow() writes, so
   * calling it on an already-selected row unticks it. Use selectRow() for the first row and checkRow()
   * for each additional one. The click target is the sibling hit area, since the box itself is inert.
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
   * Run one of the explorer's own commands, by objectName: Create, OpenLevel, EditRow, RenameRow,
   * MoveRow, RemoveRows, CommitRow, CancelRow, MoveHere, CancelMove.
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
    // The overflow spells the same commands without the row/rows suffix (OverflowEditButton, etc.).
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
 * A CheckableListPanel - the searchable, tick-per-row list placed beside a TreeExplorerView and inside
 * pickers. Addressed by its own objectName.
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

  /** Type into the panel's filter box and wait for filtering - see TreeExplorer.filter for why. */
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

  /** Tick/untick a row. The whole row is the hit area; the box itself is inert. */
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
   * Whether the panel is offering its header command at all - it hides the command when there is
   * nothing to act on. Ask this before runAction().
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
