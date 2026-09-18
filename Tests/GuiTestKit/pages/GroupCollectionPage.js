// GroupCollectionPage - the "Groups" subpage under Administration
// (imtauthgui/UserGroupCollectionView.qml, collectionId "Groups"). Reached via
// AdministrationPage.openSubPage("Groups"), NOT its own MenuPanel button, so open()/reload() are not
// meaningful here - navigate through AdministrationPage instead. Reuses CollectionPage's standard
// command bar (New/Edit/Remove) + Table wiring for free.

const { CollectionPage } = require('./CollectionPage');

class GroupCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'Groups');
  }
}

module.exports = { GroupCollectionPage };
