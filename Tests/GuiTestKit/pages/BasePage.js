// Base for every page object. Owns concerns shared by all pages: navigating via the MenuPanel, running
// CommandsView commands, and closing document tabs.
//
// Page objects expose ACTIONS and LOCATORS only. Assertions belong in the tests.

const gui = require('../lib/gui');
const { CommandBar, MenuPanel } = require('../controls');

class BasePage {
  /**
   * @param {import('@playwright/test').Page} page
   * @param {string} pageId  The QML PageId used by the MenuPanel button (e.g. 'Workspace').
   */
  constructor(page, pageId) {
    this.page = page;
    this.pageId = pageId;
    this.menu = new MenuPanel(page);
    this.commands = new CommandBar(page);
  }

  /** Reload the app shell (fresh session state already applied by the project's storageState). */
  async reload() {
    await gui.reload(this.page);
    return this;
  }

  /**
   * Whether the logged-in user can reach this page at all. Asks the menu the client rendered from that
   * user's own permissions, instead of a table kept in the test suite.
   */
  isAvailable() {
    return this.menu.hasPage(this.pageId);
  }

  /** Navigate to this page via the left menu. */
  async open() {
    await this.menu.open(this.pageId);
    return this;
  }

  /** Run a command bar command by id (e.g. 'New', 'Edit', 'Save'). */
  async runCommand(commandId) {
    await this.commands.run(commandId);
    return this;
  }

  /** One Save click, with no expectation that it commits - for the "Save is refused" tests. */
  async clickSave() {
    await this.runCommand('Save');
    return this;
  }

  /**
   * Save the open document and confirm the server actually took it.
   *
   * Saving takes more than one Save click (one click pushes the representation, the next commits it),
   * and a dirty-marker check isn't usable, so the check is the server round-trip: click until a click
   * stops producing a mutation, then require that at least one went through with no errors.
   */
  async save(attempts = 2) {
    const errors = [];
    let mutations = 0;
    const onResponse = async (response) => {
      const request = response.request();
      if (request.method() !== 'POST' || !/\bmutation\s/.test(request.postData() || '')) return;
      mutations += 1;
      const body = await response.json().catch(() => null);
      if (!body || !body.errors) return;
      // Warning-level entries are not refusals (e.g. creating a support ticket succeeds while still
      // reporting a generic "Failed to save document"). Everything else is a refusal and must be loud.
      const refusals = body.errors.filter((e) => !(e && e.extensions && e.extensions.type === 'Warning'));
      if (refusals.length) errors.push(JSON.stringify(refusals).slice(0, 400));
    };
    this.page.on('response', onResponse);
    try {
      for (let i = 0; i < attempts; i++) {
        const before = mutations;
        try {
          await this.runCommand('Save');
        } catch (err) {
          // Some editors hide the command bar once the document is no longer new. Having already
          // committed, that is the end of the flow, not a failure.
          if (mutations > 0) break;
          throw err;
        }
        await gui.waitForStable(this.page);
        if (errors.length) throw new Error(`Save was rejected by the server: ${errors.join('; ')}`);
        // A click that asked the server for nothing means nothing was left to commit - but only once
        // something already went through, since the first click is often the silent one.
        if (mutations === before && mutations > 0) break;
      }
      if (mutations === 0) {
        throw new Error(
          `Save never reached the server: ${attempts} Save clicks produced no mutation at all ` +
            '(a validator refusing the document looks like this too - use clickSave() where that is the point)'
        );
      }
    } finally {
      this.page.off('response', onResponse);
    }
    return this;
  }

  /** Close the currently open document tab. */
  async closeDocument() {
    await gui.clickButton(this.page, ['CloseButton']);
    return this;
  }

  /**
   * Switch to another open tab by its position (objectName "Tab" + index; 0 is the pinned collection
   * tab, 1+ are document tabs in open order). A plain left click selects a tab.
   * @param {number} index
   */
  async switchToTab(index) {
    await gui.clickButton(this.page, [`Tab${index}`]);
    return this;
  }
}

module.exports = { BasePage };
