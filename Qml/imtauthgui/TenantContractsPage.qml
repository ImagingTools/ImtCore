// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtauthgui 1.0

/**
 * TenantContractsPage
 *
 * Cooperation Contracts tab of the TenantEditor.
 *
 * Displays contracts via SimpleCollectionPage. Create opens a separate
 * ContractView editor (ViewBase) with GqlBasedCommandsController.
 * Status changes (Activate / Terminate) are per-item in the list delegate.
 */
SimpleCollectionPage {
	id: contractsPage
	
	entityName: qsTr("Contract")
	entityNamePlural: qsTr("Cooperation Contracts")
	descriptionText: qsTr("Manage bilateral cooperation agreements with partner tenants.")
	
	listModel: apiClient ? apiClient.contractsModel : null
	
	customEditorComponent: createContractComp
	
	headerButtonsComponent: headerBtnsComp
	
	readonly property bool __canManageContracts: contractsPage.stateManager
												 ? contractsPage.stateManager.canManageOrganizationConnections
												 : false
	
	function updateGui() {
		if (contractsPage.apiClient && contractsPage.model && contractsPage.model.m_id) {
			contractsPage.apiClient.fetchContracts(contractsPage.model.m_id)
		}
	}
	
	Component.onCompleted: {
		if (contractsPage.apiClient && contractsPage.model && contractsPage.model.m_id) {
			contractsPage.apiClient.fetchContracts(contractsPage.model.m_id)
		}
	}
	
	onVisibleChanged: {
		if (contractsPage.visible && contractsPage.apiClient && contractsPage.model && contractsPage.model.m_id) {
			contractsPage.apiClient.fetchContracts(contractsPage.model.m_id)
		}
	}
	
	Connections {
		target: contractsPage.apiClient
		
		function onContractCreated(contractId) {
			PopupManager.addSuccessMessage(qsTr("Contract created successfully"), true)
			contractsPage.popEditor()
			if (contractsPage.apiClient && contractsPage.model && contractsPage.model.m_id) {
				contractsPage.apiClient.fetchContracts(contractsPage.model.m_id)
			}
		}
		
		function onContractStatusUpdated(contractId) {
			PopupManager.addSuccessMessage(qsTr("Contract status updated"), true)
			if (contractsPage.apiClient && contractsPage.model && contractsPage.model.m_id) {
				contractsPage.apiClient.fetchContracts(contractsPage.model.m_id)
			}
		}
		
		function onContractTerminated(contractId) {
			PopupManager.addSuccessMessage(qsTr("Contract terminated"), true)
			if (contractsPage.apiClient && contractsPage.model && contractsPage.model.m_id) {
				contractsPage.apiClient.fetchContracts(contractsPage.model.m_id)
			}
		}
	}
	
	// --- Custom header ---
	Component {
		id: headerBtnsComp
		
		Text {
			text: qsTr("+ Create Contract")
			font.pixelSize: Style.fontSizeM
			font.bold: true
			color: contractsPage.__canManageContracts ? Style.linkColor : Style.inactiveTextColor
			
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				cursorShape: contractsPage.__canManageContracts ? Qt.PointingHandCursor : Qt.ArrowCursor
				enabled: contractsPage.__canManageContracts
				onClicked: {
					contractsPage.openCreate()
				}
			}
		}
	}
	
	// --- Custom list delegate ---
	delegateComponent: Component {
		Rectangle {
			id: contractDelegate
			width: contractDelegate.parent ? contractDelegate.parent.width : 0
			height: contractDelegateContent.height + 2 * Style.marginM
			color: Style.alternateBaseColor
			radius: Style.radiusS
			border.color: Style.borderColor
			border.width: 1
			
			// contractsModel is a role-based ListModel, so rows arrive as `model` and
			// `modelData` is undefined - resolve both, as the shared delegate base does.
			readonly property var __contract: (typeof modelData !== "undefined" && modelData !== null) ? modelData : model
			readonly property bool __canManage: contractsPage.stateManager
												&& contractsPage.__canManageContracts
			
			Column {
				id: contractDelegateContent
				anchors.left: contractDelegate.left
				anchors.right: contractDelegate.right
				anchors.top: contractDelegate.top
				anchors.margins: Style.marginM
				spacing: Style.marginXS
				
				BaseText {
					width: contractDelegateContent.width
					elide: Text.ElideRight
					text: qsTr("Target: %1").arg(contractDelegate.__contract.targetTenantId || "")
					font.pixelSize: Style.fontSizeM
					color: Style.textColor
				}
				
				BaseText {
					width: contractDelegateContent.width
					elide: Text.ElideRight
					text: qsTr("Status: %1   Scope: %2")
					.arg(contractDelegate.__contract.status || qsTr("Draft"))
					.arg((contractDelegate.__contract.scope && contractDelegate.__contract.scope !== "") ? contractDelegate.__contract.scope : qsTr("All"))
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
				
				BaseText {
					width: contractDelegateContent.width
					visible: (contractDelegate.__contract.validFrom && contractDelegate.__contract.validFrom !== "")
							 || (contractDelegate.__contract.validUntil && contractDelegate.__contract.validUntil !== "")
					elide: Text.ElideRight
					text: qsTr("Valid: %1 – %2")
					.arg((contractDelegate.__contract.validFrom && contractDelegate.__contract.validFrom !== "") ? contractDelegate.__contract.validFrom : qsTr("now"))
					.arg((contractDelegate.__contract.validUntil && contractDelegate.__contract.validUntil !== "") ? contractDelegate.__contract.validUntil : qsTr("open"))
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
				
				BaseText {
					width: contractDelegateContent.width
					visible: contractDelegate.__contract.description && contractDelegate.__contract.description !== ""
					elide: Text.ElideRight
					text: contractDelegate.__contract.description || ""
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
				
				// --- Action buttons ---
				Row {
					spacing: Style.marginM
					visible: contractDelegate.__canManage
					
					Button {
						visible: contractDelegate.__contract.status === "Draft"
						text: qsTr("Activate")
						onClicked: {
							if (contractsPage.apiClient) {
								contractsPage.apiClient.updateContractStatus(contractDelegate.__contract.contractId || "", "Active")
							}
						}
					}
					
					Button {
						visible: contractDelegate.__contract.status !== "Terminated"
						text: qsTr("Terminate")
						onClicked: {
							if (contractsPage.apiClient) {
								contractsPage.apiClient.terminateContract(contractDelegate.__contract.contractId || "")
							}
						}
					}
				}
			}
		}
	}
	
	// --- Create contract editor (ViewBase with GqlBasedCommandsController) ---
	Component {
		id: createContractComp
		
		ContractView {
			apiClient: contractsPage.apiClient
			tenantData: contractsPage.model
			commandsControllerComp: Component {
				GqlBasedCommandsController {
					typeId: contractsPage.apiClient ? contractsPage.apiClient.contractObjectTypeId : ""
				}
			}
			
			onCommandActivated: {
				if (commandId === "save" || commandId === "create") {
					submitContract()
				}
			}
		}
	}
}
