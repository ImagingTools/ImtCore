// UserCollectionPage - the "Users" subpage under Administration (imtauthgui/UserCollectionView.qml,
// collectionId "Users"). Reached via AdministrationPage.openSubPage("Users"), NOT its own MenuPanel
// button, so open()/reload() are not meaningful here - navigate through AdministrationPage instead.
// Reuses CollectionPage's standard command bar (New/Edit/Remove) + Table wiring for free.

const { CollectionPage } = require('./CollectionPage');

class UserCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'Users');
  }
}

module.exports = { UserCollectionPage };
