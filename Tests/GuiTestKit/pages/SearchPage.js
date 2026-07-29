// SearchPage - the universal Search page (pageId "Search", PagePermissions=["*"], visible to everyone).
//
// Grounded in Partitura/ImtGraphQlVoce.arp/PagesController.acc (SearchPage, PageId=Search),
// ImtCore/Qml/imtgui/View/SearchPage.qml, SearchResultsView.qml and the GLOBAL search box in
// imtgui/Panels/TopCenterPanelDecorator.qml. open()/the landing screenshot navigate via the menu
// button (SearchButton); everything below drives the top-bar search box instead, which is a
// different, faster path real users take constantly (type and the app navigates here for you).

const { BasePage } = require('./BasePage');
const gui = require('../lib/gui');

class SearchPage extends BasePage {
  constructor(page) {
    super(page, 'Search');
  }

  /**
   * Type into the GLOBAL search box (TopCenterPanelDecorator.qml's bare `SearchTextInput`, NOT the
   * FilterPanel's own same-objectName search box used on collection pages). SearchTextInput.qml
   * debounces 500ms after the last keystroke, then fires `searchChanged` -> the app sends
   * "GlobalSearchActivated" -> SearchPage.updateSearch() runs the query and (per NavigationController)
   * lands on the Search page automatically - no Enter key or button click needed.
   *
   * Call this from a page WITHOUT its own FilterPanel (e.g. Workspace) - on a collection page, TWO
   * elements would match the bare `SearchTextInput` path (the global one AND the FilterPanel's), and
   * this targets whichever the DOM happens to list first.
   * @param {string} text
   */
  async search(text) {
    await gui.fill(this.page, ['SearchTextInput'], text);
    return this;
  }

  /**
   * Wait for the debounced auto-navigation to actually land with results (a first result tab
   * rendered). The 500ms debounce plus the search round-trip both vary under load, so this polls
   * (expectVisible's own ASSERT_TIMEOUT window) rather than sleeping a fixed amount. Swallows its own
   * timeout (matching waitForBusyIndicatorGone's "noise reducer, not a structural assertion" stance) -
   * a search that genuinely returns zero results is a valid outcome the caller should detect via
   * tabCount() === 0, not a hang.
   */
  async waitForResults() {
    try {
      await gui.expectVisible(this.page, ['Tab0'], 'search results tab should appear');
    } catch (_) {
      // no results for this query - tabCount() will correctly report 0
    }
    return this;
  }

  /** Number of result tabs currently rendered (SearchResultsView's TabPanel: Tab0, Tab1, ... one per
   * result group) - stops at the first missing index rather than assuming a fixed count. */
  async tabCount() {
    let n = 0;
    // eslint-disable-next-line no-await-in-loop
    while (await gui.countVisible(this.page, [`Tab${n}`])) n++;
    return n;
  }

  /** Click a result tab by its zero-based index (Tab0, Tab1, ...). */
  clickTab(index) {
    return gui.click(this.page, [`Tab${index}`], { what: `search result tab ${index}` });
  }
}

module.exports = { SearchPage };
