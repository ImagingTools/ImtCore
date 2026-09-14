// Base for every page object. Owns concerns shared by all ProLife pages: navigating to the page via
// the MenuPanel, running commands from the CommandsView command bar, and closing document tabs.
//
// Page objects expose ACTIONS and LOCATORS only. Assertions (checkScreenshot, expect*) belong in the
// tests, so the same page object can be reused wherever the flow is needed.

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
   * Whether the logged-in user can reach this page at all. Asks the menu the CLIENT rendered - it was
   * built from that user's own permissions - instead of consulting a table kept in the test suite.
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
   * Save the open document, and confirm the server actually took it.
   *
   * Saving takes more than one Save click. Watching the POSTs live: on Hardware the first click pushes
   * the representation (UpdateDeviceFromRepresentation) and only the next commits it (SaveDocument); on
   * Orders the first click produces no request at all and the second posts OrderUpdate. One click
   * therefore wrote NOTHING from any editor in this suite - verified against the database - while the
   * tests read the still-open document's own in-memory state and called it saved.
   *
   * The check is the server round-trip, not anything on screen. The dirty marker looks like the obvious
   * signal and is not usable: the bridge mirrors the tab's plain name, so a tab rendered "* 22188" reads
   * back as "22188" and any assertion on it silently passes. So: click until a click stops producing a
   * mutation, then require that at least one went through and that none came back with errors - a
   * rejected save ("Unable to update order ...") otherwise just left a toast nobody looked at.
   *
   * Two clicks is what every flow observed here needs - the push and the commit - and each one costs a
   * full settle, so a third only ever confirmed there was nothing left to do.
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
      // Warning-level entries are not refusals: creating a support ticket succeeds (the row is in the
      // database) while still reporting "Failed to save document" from the generic SaveDocument step it
      // does not use. Everything else is a refusal and must be loud.
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
          // Some editors hide the command bar once the document is no longer new (the Support ticket
          // does). Having already committed, that is the end of the flow, not a failure.
          if (mutations > 0) break;
          throw err;
        }
        await gui.waitForStable(this.page);
        if (errors.length) throw new Error(`Save was rejected by the server: ${errors.join('; ')}`);
        // A click that asked the server for nothing means there was nothing left to commit - but only
        // once something has already gone through, since the FIRST click is often the silent one.
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
   * Switch to another open tab by its position (TabDelegate.qml's objectName is "Tab" + model.index,
   * a stable per-page-instance sequence - 0 is always the pinned collection tab, 1+ are document tabs
   * in the order they were opened). Every tab's MouseArea reacts to a plain left click for selection
   * (right-click is reserved for the tab's own context menu - see MultiDocumentCollectionView.qml's
   * onTabClicked), so this is just a regular click.
   * @param {number} index
   */
  async switchToTab(index) {
    await gui.clickButton(this.page, [`Tab${index}`]);
    return this;
  }
}

module.exports = { BasePage };
