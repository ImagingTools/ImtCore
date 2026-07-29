// OrganizationsPage - the Organizations page (pageId "Tenants", menu label "Organizations").
//
// Grounded in Partitura/ProLifeQmlVoce.arp/Pages.acc (TenantsPage, PageId=Tenants, IsVisible=true,
// PagePermissions=[ViewOrganizations]) and ImtCore/Qml/imtauthgui/TenantCollectionView.qml.
//
// TenantCollectionView is not objectName-instrumented internally yet, so this page object only
// navigates via the menu button (TenantsButton) - open() hard-fails if that button is missing - and
// coverage is the per-user landing screenshot.

const { BasePage } = require('./BasePage');

class OrganizationsPage extends BasePage {
  constructor(page) {
    super(page, 'Tenants');
  }
}

module.exports = { OrganizationsPage };
