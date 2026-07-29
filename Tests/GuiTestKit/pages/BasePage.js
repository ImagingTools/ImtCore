// Base for every page object. Owns concerns shared by all ProLife pages: navigating to the page via
// the MenuPanel, running commands from the CommandsView command bar, and closing document tabs.
//
// Page objects expose ACTIONS and LOCATORS only. Assertions (checkScreenshot, expect*) belong in the
// tests, so the same page object can be reused by screenshot tests and by permission-matrix checks.

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
