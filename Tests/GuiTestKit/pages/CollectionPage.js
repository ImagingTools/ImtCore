// Base for the table+filter+CRUD collection pages (Customers, Devices/Hardware, Orders, Licenses).
//
// A standard collection needs no subclass at all - declare the two things that differ between apps:
//
//   new CollectionPage(page, 'Orders', {
//     filters: { customers: 'CustomersFilter', creationDate: 'CreationDateFilter' },
//     maskColumns: ['added', 'timeStamp'],
//   })
//
// Subclass only to add genuinely page-specific flows (a Bind dialog, a license-file command).

const gui = require('../lib/gui');
const { BasePage } = require('./BasePage');
const { FilterPanel, Table, Pagination } = require('../controls');

class CollectionPage extends BasePage {
  /**
   * @param {import('@playwright/test').Page} page
   * @param {string} pageId   MenuPanel PageId (e.g. 'Accounts', 'Devices')
   * @param {{filters?: Object<string, string>, maskColumns?: string[]}} [declaration]
   *   `filters` maps a short key to the filter's objectName, so a test names a filter by what it means
   *   rather than by an id out of the app's own configuration. `maskColumns` lists header ids whose
   *   values are not deterministic across runs (timestamps, generated ids) and must be masked out of
   *   every screenshot of this collection.
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
   * Assert this collection is actually the thing on screen. Cheap, and it closes a whole failure class:
   * navigation that quietly did not happen leaves the PREVIOUS page up, and a screenshot taken then is
   * committed as this page's baseline - which is how a Support landing baseline came to be byte-identical
   * to the Workspace one. Every collection view has a table, and no non-collection page does.
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
   * Screenshot masks ({x,y,width,height}[]) for one or more columns, addressed by header id (the
   * HeaderIds entry from the collection's own *Page.acc, NOT the visible HeaderNames caption - e.g.
   * Orders/Devices/SoftwareProducts all use "added"/"timeStamp" for their Added/Last Modified
   * columns). Use for any column whose value isn't deterministic across runs (timestamps, generated
   * ids, ...) so `checkScreenshot` stays reproducible instead of asserting on it. Thin forwarder to
   * `this.table.columnMasks` - exists so a concrete page can call `this.columnMasks([...])` directly
   * without reaching through `.table`, and so it reads the same regardless of which collection it's
   * called on.
   * @param {string|string[]} headerIds
   */
  columnMasks(headerIds) {
    return this.table.columnMasks(headerIds);
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
