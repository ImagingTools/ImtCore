// SearchPage - the universal Search page (pageId "Search", visible to everyone).
//
// open()/the landing screenshot navigate via the menu button (SearchButton); everything below drives
// the top-bar global search box instead (type and the app navigates here for you).

const { BasePage } = require('./BasePage');
const gui = require('../lib/gui');
const { waitForBusyIndicatorGone } = require('../lib/stability');

class SearchPage extends BasePage {
  constructor(page) {
    super(page, 'Search');
  }

  /**
   * Type into the GLOBAL search box. SearchTextInput debounces 500ms, then fires `searchChanged`, and
   * the app runs the query and lands on the Search page automatically - no Enter or button click.
   *
   * Addressed by "GlobalSearchInput" (a bare "SearchTextInput" is ambiguous wherever a FilterPanel is
   * on screen, since that box carries the same objectName).
   * @param {string} text
   */
  async search(text) {
    // Falls back to the old bare path for a client built before that objectName existed. The fallback
    // is ambiguous where a FilterPanel is on screen, so it is a bridge, not a second supported path.
    const named = (await gui.dom.countVisible(this.page, ['GlobalSearchInput'])) > 0;
    await gui.fill(this.page, [named ? 'GlobalSearchInput' : 'SearchTextInput'], text);
    return this;
  }

  /**
   * Wait for the debounced auto-navigation to land with results (a first result tab rendered). Polls
   * rather than sleeping, and swallows its own timeout - a search returning zero results is a valid
   * outcome the caller detects via tabCount() === 0, not a hang.
   */
  async waitForResults(options = {}) {
    if (options.allowEmpty) {
      await this.page.waitForTimeout(700);
      await waitForBusyIndicatorGone(this.page, { timeout: 1500 });
      return this;
    }

    try {
      await gui.expectVisible(this.page, ['Tab0'], 'search results tab should appear');
      await waitForBusyIndicatorGone(this.page, { timeout: 15_000 });
    } catch (_) {
      // no results for this query - tabCount() will correctly report 0
    }
    return this;
  }

  /** Number of result tabs currently rendered (Tab0, Tab1, ...); stops at the first missing index. */
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
