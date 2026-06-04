// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantRelationshipsPage
 *
 * Relationships tab — list / edit / remove tenant relationships via the
 * abstract apiClient. Uses standard TenantSimpleCollectionPage with
 * FilterableSelectGqlDataProvider (same mechanism as TenantRolesPage).
 * Create is hidden because relationships are created via proposal flow.
 */
TenantSimpleCollectionPage {
	id: relationshipsPage

	entityName: qsTr("Relationship")
	entityNamePlural: qsTr("Relationships")
	descriptionText: qsTr("Business relationships with connected organizations. Creating or modifying a relationship requires approval from both parties.")
	showCreateButton: false

	documentManager: apiClient ? apiClient.relationshipDocumentManager : null
	objectTypeId: apiClient ? apiClient.relationshipObjectTypeId : ""
	dataProvider: apiClient ? apiClient.tenantRelationshipsListDataProvider : null

	function removeItem(id) {
		if (relationshipsPage.apiClient && relationshipsPage.tenantData) {
			relationshipsPage.apiClient.removeTenantRelationship(
				relationshipsPage.tenantData.m_id, id)
		}
	}

	Connections {
		target: relationshipsPage.apiClient
		function onTenantRelationshipRemoved() {
			relationshipsPage.refresh()
		}
	}
}

