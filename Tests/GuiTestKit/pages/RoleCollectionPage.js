// RoleCollectionPage - the "Roles" subpage under Administration (imtauthgui/RoleCollectionView.qml,
// collectionId "Roles"). Reached via AdministrationPage.openSubPage("Roles"), NOT its own MenuPanel
// button, so open()/reload() are not meaningful here - navigate through AdministrationPage instead.
// Reuses CollectionPage's standard command bar (New/Edit/Remove) + Table wiring for free.

const { CollectionPage } = require('./CollectionPage');

class RoleCollectionPage extends CollectionPage {
  constructor(page) {
    super(page, 'Roles');
  }
}

module.exports = { RoleCollectionPage };
