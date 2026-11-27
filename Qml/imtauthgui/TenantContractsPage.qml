// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantContractsPage
 *
 * The contracts the organization takes part in. A row opens the card of the contract in ContractDialog,
 * the create command opens an empty one. The contracts that wait for an answer of the organization come first.
 */
SimpleCollectionPage {
	id: contractsPage

	entityName: qsTr("Contract")
	entityNamePlural: qsTr("Contracts")
	createButtonText: qsTr("+ New contract")
	descriptionText: qsTr("The agreements of the organization with other organizations: the parties, their roles, the permissions they give each other and the documents.")
	emptyText: qsTr("No contracts yet. Press \"+ New contract\", add a partner as a party, agree on the terms and sign. A partner is added on the page Address book.")

	// The transport of the contracts (ContractsApiClient)
	property var contractsApiClient: null
	property string contractToOpenId: ""
	signal contractOpenHandled()

	readonly property string __tenantId: contractsPage.model && contractsPage.model.m_id ? String(contractsPage.model.m_id) : ""

	createPermissionIds: ["/OrganizationManagement/Contracts/Edit"]
	editPermissionIds: ["/OrganizationManagement/Contracts/View"]
	deletePermissionIds: ["/OrganizationManagement/Contracts/Edit"]

	// The product is shown only when the list has contracts of other products, read only
	property bool hasOtherProducts: false

	columnIds: contractsPage.hasOtherProducts ? ["number", "name", "productId", "partyNames", "stateText", "period"] : ["number", "name", "partyNames", "stateText", "period"]
	columnNames: contractsPage.hasOtherProducts ? [qsTr("Number"), qsTr("Name"), qsTr("Product"), qsTr("Parties"), qsTr("State"), qsTr("Period")] : [qsTr("Number"), qsTr("Name"), qsTr("Parties"), qsTr("State"), qsTr("Period")]

	listModel: contractsPage.__rows

	property var __rows: []

	function updateGui() {
		contractsPage.refresh()
	}

	function refresh() {
		if (contractsPage.contractsApiClient && contractsPage.__tenantId !== "")
			refreshTimer.restart()
	}

	onContractToOpenIdChanged: {
		if (contractsPage.visible && contractsPage.contractToOpenId !== "")
			contractsPage.refresh()
	}

	Timer {
		id: refreshTimer
		interval: 50
		repeat: false

		onTriggered: {
			if (contractsPage.contractsApiClient && contractsPage.__tenantId !== "")
				contractsPage.contractsApiClient.fetchContracts(contractsPage.__tenantId)
		}
	}

	Component.onCompleted: {
		contractsPage.refresh()
	}

	// A draft never sent is removed, the other contracts stay in the history of the parties
	property bool __isRemoving: false

	function removeItems(ids) {
		if (!contractsPage.contractsApiClient)
			return
		contractsPage.__isRemoving = true
		for (var i = 0; i < ids.length; i++)
			contractsPage.contractsApiClient.runCommand("RemoveContract", contractsPage.__tenantId, ids[i], 0, "")
	}

	function openItem(itemId) {
		var item = contractsPage.__findItemById(itemId)
		if (item && contractsPage.canEdit)
			contractsPage.openEdit(itemId, item.title, "")
	}

	function __rowOf(contract) {
		var row = {}
		row.id = contract.id
		row.productId = contract.productId || ""
		row.isReadOnly = contract.isReadOnly === true
		row.number = contract.number || qsTr("Draft")
		row.name = contract.name || ""
		row.title = contract.number ? contract.number + " " + (contract.name || "") : (contract.name || qsTr("Draft"))
		row.partyNames = (contract.partyNames || []).join(", ")
		row.stateText = ContractTexts.contractStateText(contract.state)
		if (contract.isMyTurnToSign)
			row.stateText = row.stateText + " · " + qsTr("your signature is awaited")
		else if (contract.partyState === "Reviewing" && contract.initiatorTenantId !== contractsPage.__tenantId
				 && (contract.state === "Negotiation" || contract.state === "Signing"))
			row.stateText = row.stateText + " · " + qsTr("your answer is awaited")
		if (contract.isTerminating)
			row.stateText = row.stateText + " · " + qsTr("terminating")
		else if (contract.isExpiring)
			row.stateText = row.stateText + " · " + qsTr("expiring")
		// A closed contract shows the day it ended
		row.period = ContractTexts.periodText(contract.effectiveStart, contract.effectiveStart ? (contract.endedAt || contract.periodEnd) : contract.periodEnd)
		row.isWaiting = contract.isMyTurnToSign === true || contract.isChangesRequested === true
		return row
	}

	Connections {
		target: contractsPage.contractsApiClient

		function onContractsReceived() {
			var contracts = contractsPage.contractsApiClient.contracts || []
			var waiting = []
			var others = []
			var hasOtherProducts = false
			for (var i = 0; i < contracts.length; i++) {
				var row = contractsPage.__rowOf(contracts[i])
				if (row.isReadOnly)
					hasOtherProducts = true
				if (row.isWaiting)
					waiting.push(row)
				else
					others.push(row)
			}
			contractsPage.hasOtherProducts = hasOtherProducts
			contractsPage.__rows = waiting.concat(others)
			if (contractsPage.visible && contractsPage.contractToOpenId !== "") {
				var contractId = contractsPage.contractToOpenId
				contractsPage.contractOpenHandled()
				contractsPage.openItem(contractId)
			}
		}

		function onCommandSucceeded(command, contractId, revision) {
			if (command === "RemoveContract")
				contractsPage.__isRemoving = false
			contractsPage.refresh()
		}

		// The card shows its own errors, the page those of removing from the list
		function onCommandFailed(command, errorMessage) {
			if (command === "RemoveContract" && contractsPage.__isRemoving) {
				contractsPage.__isRemoving = false
				PopupManager.addErrorMessage(errorMessage, true)
			}
		}

		function onContractNotified(contractId, tenantId) {
			if (contractsPage.visible && (tenantId === "" || tenantId === contractsPage.__tenantId))
				contractsPage.refresh()
		}
	}

	onVisibleChanged: {
		if (contractsPage.visible)
			contractsPage.refresh()
	}

	editorDialogComponent: Component {
		ContractDialog {
			tenantId: contractsPage.__tenantId
			tenantName: contractsPage.model && contractsPage.model.m_name ? String(contractsPage.model.m_name) : ""
			contractsApiClient: contractsPage.contractsApiClient
			tenantApiClient: contractsPage.apiClient
			stateManager: contractsPage.stateManager
		}
	}
}
