// AdministrationPage - the Admin page (pageId "Administration").
//
// Grounded in ImtCore/Qml/imtauthgui/AdministrationView.qml, whose root carries
// objectName: "AdministrationView". The view hosts Users / Roles / Groups sub-pages via a generic
// MultiPageView (imtgui/View/MultiPageView.qml) sidebar; each nav item now carries
// objectName: "Page_" + pageId (e.g. "Page_Roles"/"Page_Users"/"Page_Groups" - the exact pageId
// strings AdministrationView.qml passes to addPage()).

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
   * Whether the logged-in user can reach a subpage - each has its own independent gating.
   *
   * Anchored on the nav's other items: MultiPageView fills its sidebar in one go once the view's data
   * has arrived, so a look taken before that answered "not offered" for a superuser and took the
   * Roles/Users/Groups blocks down with it.
   */
  hasSubPage(pageId) {
    return gui.dom.isOffered(this.page, [`Page_${pageId}`], { anchorSelector: '[objectName^="Page_"]' });
  }
}

module.exports = { AdministrationPage };
