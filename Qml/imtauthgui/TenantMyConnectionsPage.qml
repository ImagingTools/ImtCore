// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantMyRelationshipsPage
 *
 * Displays confirmed relationships for the tenant.
 * Uses standard TenantSimpleCollectionPage with FilterableSelectGqlDataProvider
 * (same mechanism as TenantRolesPage). Supports Remove/Edit/Create.
 * Create opens the RelationshipView which creates a relationship proposal
 * (bilateral approval required).
 */
TenantSimpleCollectionPage {
	id: relationshipsPage

	entityName: qsTr("Relationship")
	entityNamePlural: qsTr("My Relationships")
	descriptionText: qsTr("Business relationships with other organizations. Creating or modifying a relationship requires approval from both parties.")
	showCreateButton: true
	documentNameFields: ["m_name", "m_targetTenantName"]

	documentManager: relationshipsPage.apiClient ? relationshipsPage.apiClient.relationshipDocumentManager : null
	objectTypeId: relationshipsPage.apiClient ? relationshipsPage.apiClient.relationshipObjectTypeId : ""
	dataProvider: relationshipsPage.apiClient ? relationshipsPage.apiClient.tenantRelationshipsListDataProvider : null

	function removeItems(ids) {
		if (relationshipsPage.apiClient && relationshipsPage.tenantData) {
			relationshipsPage.apiClient.removeTenantRelationship(relationshipsPage.tenantData.m_id, ids[0])
		}
	}

	Connections {
		target: relationshipsPage.apiClient
		function onTenantRelationshipRemoved() {
			relationshipsPage.refresh()
		}
	}
}
