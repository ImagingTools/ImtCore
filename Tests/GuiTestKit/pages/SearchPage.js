// SearchPage - the universal Search page (pageId "Search", visible to everyone).
//
// open()/the landing screenshot navigate via the menu button (SearchButton); everything below drives
// the top-bar global search box instead (type and the app navigates here for you).

const { BasePage } = require('./BasePage');
const gui = require('../lib/gui');
const { waitForBusyIndicatorGone, waitForNetworkIdle } = require('../lib/stability');

// SearchTextInput's debounce, before which no query has been sent yet.
const SEARCH_DEBOUNCE_MS = 500;

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
   * Wait for the debounced auto-navigation to land, whether or not it matched anything.
   *
   * Zero results is a valid outcome the caller reads via tabCount(), so this must not be expressed as
   * "wait for a result tab": that spent the full assertion timeout on a tab that was never going to
   * appear and then swallowed it, which is why a nonsense search cost ten seconds. Each wait below
   * finishes as soon as it is genuinely done instead.
   */
  async waitForResults() {
    // Nothing has been sent yet while the debounce is running, so neither the network nor the DOM
    // means anything until it has elapsed.
    await this.page.waitForTimeout(SEARCH_DEBOUNCE_MS);
    await waitForNetworkIdle(this.page, { timeout: 15_000 });
    await waitForBusyIndicatorGone(this.page, { timeout: 15_000 });
    // The response has landed; a result tab may still be painting. Its absence is an answer here, not
    // a failure, so this is a short look rather than an assertion.
    await gui.dom.isVisible(this.page, ['Tab0'], 1000);
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
