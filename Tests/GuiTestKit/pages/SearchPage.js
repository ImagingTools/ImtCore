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
   * Type into the GLOBAL search box (TopCenterPanelDecorator.qml). SearchTextInput.qml debounces 500ms
   * after the last keystroke, then fires `searchChanged` -> the app sends "GlobalSearchActivated" ->
   * SearchPage.updateSearch() runs the query and (per NavigationController) lands on the Search page
   * automatically - no Enter key or button click needed.
   *
   * Addressed by "GlobalSearchInput", its own name. A bare `SearchTextInput` used to be the path here,
   * and it is ambiguous wherever a collection's FilterPanel is on screen - that box carries the same
   * objectName, so the click landed on whichever the DOM happened to list first and the query went
   * into the page's own filter instead of the global search. Confirmed live on Lisa, whose Workspace
   * has a FilterPanel too, so "call it from a page without one" was no longer an escape.
   * @param {string} text
   */
  async search(text) {
    // Falls back to the old bare path for a client built before that objectName existed: the name
    // lives in ImtCore's QML, but each app ships its own compiled copy of it, so a sibling suite
    // running against a binary built earlier would otherwise fail here for a reason that has nothing
    // to do with its own code. The fallback is ambiguous where a FilterPanel is on screen - which is
    // the whole point of the new name - so it is a bridge, not a second supported path.
    const named = (await gui.dom.countVisible(this.page, ['GlobalSearchInput'])) > 0;
    await gui.fill(this.page, [named ? 'GlobalSearchInput' : 'SearchTextInput'], text);
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
