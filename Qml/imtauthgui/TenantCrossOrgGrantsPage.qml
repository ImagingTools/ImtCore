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
 * TenantCrossOrgGrantsPage
 *
 * Cross-Org Grants tab of the TenantEditor.
 *
 * Displays grants via TenantSimpleCollectionPage with a document manager
 * for full Create/Edit/Remove workflow.
 */
TenantSimpleCollectionPage {
	id: grantsPage

	entityName: qsTr("Grant")
	entityNamePlural: qsTr("Cross-Org Grants")
	descriptionText: qsTr("Delegate scoped access from this tenant to another tenant.")
	showCreateButton: true
	documentNameFields: ["m_name", "m_targetTenantName"]

	documentManager: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantDocumentManager : null
	objectTypeId: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantObjectTypeId : ""
	dataProvider: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantsListDataProvider : null

	function removeItems(ids) {
		if (grantsPage.apiClient) {
			grantsPage.apiClient.revokeCrossOrgGrant(ids[0])
		}
	}

	Connections {
		target: grantsPage.apiClient
		function onCrossOrgGrantsRemoved() {
			grantsPage.refresh()
		}
	}
}
