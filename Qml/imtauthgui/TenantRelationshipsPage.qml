// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtauthgui 1.0

/**
 * TenantRelationshipsPage
 *
 * Relationships tab of the TenantEditor.
 *
 * Displays tenant relationships using the standard TenantSimpleCollectionPage
 * mechanism (FilterableSelectGqlDataProvider + default list delegate), following
 * the same pattern as TenantRolesPage.
 *
 * Relationships are established automatically when connections are accepted;
 * therefore the Create button is hidden.  Remove and Edit are available via
 * the standard header buttons and the per-item context menu.
 */
TenantSimpleCollectionPage {
	id: relationshipsPage

	entityName: qsTr("Relationship")
	entityNamePlural: qsTr("Tenant Relationships")
	descriptionText: qsTr("Relationships are established automatically when connections are accepted.")
	emptyText: qsTr("No relationships found. Establish a connection to create a relationship.")

	dataProvider: apiClient ? apiClient.tenantRelationshipsListDataProvider : null
	showCreateButton: false

	function removeItem(id) {
		if (relationshipsPage.apiClient && relationshipsPage.tenantData)
			relationshipsPage.apiClient.removeTenantRelationship(
				relationshipsPage.tenantData.m_id, id)
	}

	// Populate the full-data model used by RelationshipView for edit pre-population
	function updateGui() {
		if (relationshipsPage.apiClient && relationshipsPage.tenantData
				&& relationshipsPage.tenantData.m_id) {
			relationshipsPage.apiClient.fetchTenantRelationships(
				relationshipsPage.tenantData.m_id)
		}
	}

	onVisibleChanged: {
		if (relationshipsPage.visible && relationshipsPage.apiClient
				&& relationshipsPage.tenantData && relationshipsPage.tenantData.m_id) {
			relationshipsPage.apiClient.fetchTenantRelationships(
				relationshipsPage.tenantData.m_id)
		}
	}

	Connections {
		target: relationshipsPage.apiClient

		function onTenantRelationshipRemoved(relationshipId) {
			PopupManager.addSuccessMessage(qsTr("Relationship removed"), true)
			relationshipsPage.refresh()
			if (relationshipsPage.apiClient && relationshipsPage.tenantData
					&& relationshipsPage.tenantData.m_id) {
				relationshipsPage.apiClient.fetchTenantRelationships(
					relationshipsPage.tenantData.m_id)
			}
		}

		function onTenantRelationshipAdded(relationshipId) {
			relationshipsPage.refresh()
			if (relationshipsPage.apiClient && relationshipsPage.tenantData
					&& relationshipsPage.tenantData.m_id) {
				relationshipsPage.apiClient.fetchTenantRelationships(
					relationshipsPage.tenantData.m_id)
			}
		}
	}

	customEditorComponent: Component {
		RelationshipView {
			apiClient: relationshipsPage.apiClient
			tenantData: relationshipsPage.tenantData
			editRelationshipId: relationshipsPage.__editItemId

			onRelationshipCreated: {
				relationshipsPage.popEditor()
				relationshipsPage.refresh()
			}
		}
	}
}
