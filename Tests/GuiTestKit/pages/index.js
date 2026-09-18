// Generic ImtCore page objects - each wraps a view owned by ImtCore itself (not a consuming app's
// business logic), so any Imt-based app gets them for free instead of reimplementing.

const { BasePage } = require('./BasePage');
const { CollectionPage } = require('./CollectionPage');
const { AdministrationPage } = require('./AdministrationPage');
const { SearchPage } = require('./SearchPage');
const { OrganizationsPage } = require('./OrganizationsPage');
const { RoleCollectionPage } = require('./RoleCollectionPage');
const { RoleEditorPage } = require('./RoleEditorPage');
const { UserCollectionPage } = require('./UserCollectionPage');
const { UserEditorPage } = require('./UserEditorPage');
const { GroupCollectionPage } = require('./GroupCollectionPage');
const { GroupEditorPage } = require('./GroupEditorPage');

module.exports = {
  BasePage,
  CollectionPage,
  AdministrationPage,
  SearchPage,
  OrganizationsPage,
  RoleCollectionPage,
  RoleEditorPage,
  UserCollectionPage,
  UserEditorPage,
  GroupCollectionPage,
  GroupEditorPage,
};
