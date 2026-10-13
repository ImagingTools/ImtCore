// AdministrationPage - the Admin page (pageId "Administration").
//
// Grounded in imtauthgui/AdministrationView.qml (objectName "AdministrationView"). It hosts
// Users / Roles / Groups sub-pages via a MultiPageView sidebar; each nav item carries
// objectName "Page_" + pageId (e.g. "Page_Roles").

const { BasePage } = require('./BasePage');
const gui = require('../lib/gui');

class AdministrationPage extends BasePage {
  constructor(page) {
    super(page, 'Administration');
  }

  /** Assert the Administration view actually rendered after open() (hard-fails otherwise). */
  async expectLoaded() {
    await gui.expectVisible(this.page, ['AdministrationView'], 'AdministrationView should be visible');
    return this;
  }

  /** Switch to a MultiPageView subpage by its pageId (e.g. "Roles", "Users", "Groups"). */
  openSubPage(pageId) {
    return gui.click(this.page, [`Page_${pageId}`], { what: `Administration subpage "${pageId}"` });
  }

  /**
   * Whether the logged-in user can reach a subpage - each has its own gating. Anchored on the nav's
   * other items, since MultiPageView fills its sidebar in one go once its data arrives.
   */
  hasSubPage(pageId) {
    return gui.dom.isOffered(this.page, [`Page_${pageId}`], { anchorSelector: '[objectName^="Page_"]' });
  }
}

module.exports = { AdministrationPage };
