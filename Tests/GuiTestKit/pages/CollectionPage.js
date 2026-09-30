// Base for the table+filter+CRUD collection pages (Customers, Devices/Hardware, Orders, Licenses).
//
// A standard collection needs no subclass - declare the two things that differ between apps:
//
//   new CollectionPage(page, 'Orders', {
//     filters: { customers: 'CustomersFilter', creationDate: 'CreationDateFilter' },
//     maskColumns: ['added', 'timeStamp'],
//   })
//
// Subclass only to add genuinely page-specific flows.

const gui = require('../lib/gui');
const { BasePage } = require('./BasePage');
const { FilterPanel, Table, Pagination, TableConfigDialog } = require('../controls');

class CollectionPage extends BasePage {
  /**
   * @param {import('@playwright/test').Page} page
   * @param {string} pageId   MenuPanel PageId (e.g. 'Accounts', 'Devices')
   * @param {{filters?: Object<string, string>, maskColumns?: string[]}} [declaration]
   *   `filters` maps a short key to the filter's objectName. `maskColumns` lists header ids whose
   *   values are not deterministic across runs and must be masked out of every screenshot.
   */
  constructor(page, pageId, { filters = {}, maskColumns = [] } = {}) {
    super(page, pageId);
    this.filters = new FilterPanel(page);
    this.table = new Table(page);
    this.pagination = new Pagination(page);
    this.filterIds = filters;
    this.maskColumns = maskColumns;
  }

  /** The declared filter's objectName; an undeclared key is taken to be an objectName already. */
  filterId(key) {
    return this.filterIds[key] || key;
  }

  /**
   * Assert this collection is actually on screen. Cheap, and it closes a failure class: navigation
   * that quietly didn't happen leaves the previous page up, and its screenshot gets committed as this
   * page's baseline. Every collection view has a table, and no non-collection page does.
   */
  expectOpen() {
    return gui.expectVisible(this.page, ['Table'], `${this.pageId}: expected a collection table on screen`);
  }

  /** Screenshot masks for this collection's non-deterministic columns (see `maskColumns`). */
  masks() {
    return this.maskColumns.length ? this.columnMasks(this.maskColumns) : [];
  }

  selectFilterOption(key, optionText) {
    return this.filters.combo(this.filterId(key)).select(optionText);
  }

  selectFilterOptionByIndex(key, index) {
    return this.filters.combo(this.filterId(key)).selectIndex(index);
  }

  /** Whether this filter currently offers an option - data-driven filters legitimately may not. */
  hasFilterOption(key, optionText) {
    return this.filters.combo(this.filterId(key)).hasOption(optionText);
  }

  setDateFilter(key, preset) {
    return this.filters.dateFilter(this.filterId(key), preset);
  }

  clearFilter(key) {
    return this.filters.clearFilter(this.filterId(key));
  }

  clearAllFilters() {
    return this.filters.clearAllFilters();
  }

  /** Text-search the collection. */
  async search(text) {
    await this.filters.search(text);
    return this;
  }

  /** Select the Nth row (0-based). */
  async selectRow(index = 0) {
    await this.table.selectRow(index);
    return this;
  }

  /**
   * Open this collection with no view state left over from an earlier test.
   *
   * Filters, search and sorting are persisted per user session on the SERVER, not in the page, so they
   * outlive a reload, leak into the next spec, and survive into the second CI phase - which is how a
   * spec that never touched a filter ends up looking at a filtered (or differently ordered) collection.
   */
  async openClean() {
    await this.open();
    await this.clearAllFilters();
    return this;
  }

  /**
   * Select the one row matching `text`, and fail if it is not exactly one.
   *
   * Prefer this over selectRow(index) whenever a specific RECORD is meant. Row order is the server's,
   * and a header click TOGGLES the sort it then persists for the session
   * (imtcontrols/Views/TableHeaderDelegate.qml), so the same index addresses a different record
   * depending on what ran before it.
   */
  async selectRecord(text) {
    await this.clearAllFilters();
    await this.search(text);
    // Wait for the search to have been APPLIED, not merely for the table to hold still: the unfiltered
    // table is perfectly stable too, so settling alone happily counts the rows the search is about to
    // drop. Seen under load as "Automatic must match exactly one row, but matched 25" - the whole
    // collection - which blames the fixture for what is really timing. The round-trip is what to wait
    // on, so waitForStable (which tracks the in-flight request) comes first.
    await gui.waitForStable(this.page);
    await this.table.waitForRowExtent();
    const matches = await this.table.visibleRowCount();
    if (matches !== 1) {
      throw new Error(
        `${this.pageId}: "${text}" must match exactly one row to address a record, but matched ${matches}`
      );
    }
    await this.selectRow(0);
    return this;
  }

  /**
   * Screenshot masks ({x,y,width,height}[]) for one or more columns, addressed by header id (the
   * HeaderIds entry from the collection's *Page.acc, NOT the visible caption). Use for any column whose
   * value isn't deterministic across runs. Thin forwarder to `this.table.columnMasks`.
   * @param {string|string[]} headerIds
   */
  columnMasks(headerIds) {
    return this.table.columnMasks(headerIds);
  }

  /**
   * Open the "Table configuration" dialog by right-clicking a sortable column header. Generic to every
   * collection, so it lives here rather than in a subclass.
   * @param {string} headerId the header/field id to right-click - any sortable column will do
   * @returns {Promise<TableConfigDialog>}
   */
  async openColumnConfig(headerId) {
    const dialog = new TableConfigDialog(this.page);
    await dialog.openViaHeader(headerId);
    return dialog;
  }

  // New / Edit / Remove / Revision via the command bar (present on every collection page).
  newItem() {
    return this.runCommand('New');
  }
  editItem() {
    return this.runCommand('Edit');
  }
  removeItem() {
    return this.runCommand('Remove');
  }
  revision() {
    return this.runCommand('Revision');
  }
}

module.exports = { CollectionPage };
