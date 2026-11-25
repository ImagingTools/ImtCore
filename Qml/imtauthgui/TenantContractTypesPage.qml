// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantContractTypesPage
 *
 * The types of contracts of the organization: the system ones, given to every organization and read only,
 * and its own. A type offers a new contract its periods, the roles of its parties, the links and the documents
 * it needs. A row opens the type in ContractTypeDialog. A type in use is archived, not removed.
 */
SimpleCollectionPage {
	id: typesPage

	entityName: qsTr("Type of contracts")
	entityNamePlural: qsTr("Types of contracts")
	createButtonText: qsTr("+ New type")
	descriptionText: qsTr("A type offers a new contract its periods, the roles of the parties, the links between them and the documents it needs.")
	emptyText: qsTr("No types yet.")
	deleteSingleTitle: qsTr("Archive type")
	deleteMultipleTitle: qsTr("Archive types")

	// ContractsApiClient
	property var contractsApiClient: null

	readonly property string __tenantId: typesPage.model && typesPage.model.m_id ? String(typesPage.model.m_id) : ""
	readonly property bool __canEditTypes: typesPage.stateManager ? typesPage.stateManager.canEditContractTypes : false

	createPermissionIds: ["/OrganizationManagement/ContractTypes/Edit"]
	editPermissionIds: ["/OrganizationManagement/Contracts/View", "/OrganizationManagement/ContractTypes/Edit"]
	deletePermissionIds: ["/OrganizationManagement/ContractTypes/Edit"]

	columnIds: ["name", "prefix", "term", "renewal", "origin", "state"]
	columnNames: [qsTr("Name"), qsTr("Prefix"), qsTr("Term"), qsTr("Renewal"), qsTr("Owner"), qsTr("State")]

	listModel: typesPage.__rows

	property var __rows: []

	function updateGui() {
		typesPage.refresh()
	}

	// The archived types are listed too: they are restored in the type. The roles are the names of its slots.
	function refresh() {
		if (!typesPage.contractsApiClient || typesPage.__tenantId === "")
			return
		typesPage.contractsApiClient.fetchContractTypes(typesPage.__tenantId, true)
		typesPage.contractsApiClient.fetchPartyRoles(typesPage.__tenantId, false)
	}

	Component.onCompleted: {
		typesPage.refresh()
	}

	onVisibleChanged: {
		if (typesPage.visible)
			typesPage.refresh()
	}

	function __typeById(typeId) {
		var types = typesPage.contractsApiClient ? typesPage.contractsApiClient.contractTypes : []
		for (var i = 0; i < types.length; i++) {
			if (types[i].typeId === typeId)
				return types[i]
		}
		return null
	}

	function __rowOf(type) {
		return {"id": type.typeId,
				"title": type.name,
				"name": type.name,
				"prefix": type.numberPrefix || "",
				"term": type.termDays ? qsTr("%n day(s)", "", type.termDays) : qsTr("No term"),
				"renewal": type.autoRenew ? qsTr("Automatic") : qsTr("No"),
				"origin": type.isSystem ? qsTr("System") : qsTr("Organization"),
				"state": type.isArchived ? qsTr("Archived") : qsTr("In use")}
	}

	// A system type stays: only the own types of the organization are archived
	function removeItems(ids) {
		if (!typesPage.contractsApiClient)
			return
		for (var i = 0; i < ids.length; i++) {
			var type = typesPage.__typeById(ids[i])
			if (type && !type.isSystem && !type.isArchived)
				typesPage.contractsApiClient.setContractTypeArchived(typesPage.__tenantId, type.typeId, true)
		}
	}

	function confirmRemoveItems(ids, itemTitle) {
		var removable = 0
		for (var i = 0; i < ids.length; i++) {
			var type = typesPage.__typeById(ids[i])
			if (type && !type.isSystem && !type.isArchived)
				removable++
		}
		if (removable === 0) {
			PopupManager.addInfoMessage(qsTr("The system types stay, and archived types are archived already. Only the own types of the organization are archived."), true)
			return
		}
		var message = ids.length === 1 && itemTitle && itemTitle !== ""
			? qsTr("Archive \"%1\"? The contracts of this type keep it; it is not offered for new contracts.").arg(itemTitle)
			: qsTr("Archive %1 selected type(s)? The contracts of these types keep them; they are not offered for new contracts.").arg(ids.length)
		ModalDialogManager.showConfirmationDialog(typesPage.deleteSingleTitle, message, function(result) {
			if (result === Enums.yes) {
				typesPage.removeItems(ids)
				if (typesPage.selectionManager)
					typesPage.selectionManager.clear()
			}
		})
	}

	Connections {
		target: typesPage.contractsApiClient

		function onContractTypesReceived() {
			var rows = []
			var types = typesPage.contractsApiClient.contractTypes || []
			for (var i = 0; i < types.length; i++)
				rows.push(typesPage.__rowOf(types[i]))
			typesPage.__rows = rows
		}

		function onCatalogItemSaved(catalog, itemId) {
			if (catalog !== "ContractTypes")
				return
			PopupManager.addSuccessMessage(qsTr("The type was saved"), true)
			typesPage.refresh()
		}

		function onCommandFailed(command, errorMessage) {
			if (typesPage.visible && command.indexOf("ContractType") >= 0)
				PopupManager.addErrorMessage(errorMessage, true)
		}
	}

	editorDialogComponent: Component {
		ContractTypeDialog {
			tenantId: typesPage.__tenantId
			contractsApiClient: typesPage.contractsApiClient
			canEdit: typesPage.__canEditTypes
		}
	}
}
