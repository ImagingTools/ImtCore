// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantMyConnectionsPage
 *
 * Displays confirmed connections and relationships for the tenant.
 * Uses standard TenantSimpleCollectionPage with FilterableSelectGqlDataProvider
 * (same mechanism as TenantRolesPage). Supports Remove/Edit.
 * Create is hidden because relationships are created via proposal flow.
 */
TenantSimpleCollectionPage {
	id: connectionsPage

	entityName: qsTr("Relationship")
	entityNamePlural: qsTr("My Connections")
	descriptionText: qsTr("Confirmed connections and business relationships with other organizations. Removing a relationship requires approval from both parties.")
	showCreateButton: false

	documentManager: connectionsPage.apiClient ? connectionsPage.apiClient.relationshipDocumentManager : null
	objectTypeId: connectionsPage.apiClient ? connectionsPage.apiClient.relationshipObjectTypeId : ""
	dataProvider: connectionsPage.apiClient ? connectionsPage.apiClient.tenantRelationshipsListDataProvider : null

	function removeItem(id) {
		if (connectionsPage.apiClient && connectionsPage.tenantData) {
			connectionsPage.apiClient.removeTenantRelationship(
				connectionsPage.tenantData.m_id, id)
		}
	}

	Connections {
		target: connectionsPage.apiClient
		function onTenantRelationshipRemoved() {
			connectionsPage.refresh()
		}
	}
}
