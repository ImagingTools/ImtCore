// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantContractsPage
 *
 * Cooperation Contracts tab of the TenantEditor.
 *
 * A contract is the bilateral terms/governance layer that sits between a
 * tenant relationship (identity/trust) and the concrete cross-org grants.
 * It references a relationship (mandatory) and has its own lifecycle
 * (Draft/Active/Expired/Terminated/Renewed). Contracts are listed, created,
 * status-updated and terminated through the abstract TenantManagementApiClient
 * contract (no direct SDL dependency here).
 */
ViewBase {
	id: contractsPage

	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: contractsPage.model
	property var stateManager: null
	property var apiClient: null

	readonly property bool __canManage: contractsPage.stateManager
		&& (contractsPage.stateManager.isCreator || contractsPage.stateManager.isOwner)

	// Status options. Index maps to the SDL ContractStatus tokens.
	readonly property var __statusTokens: ["Draft", "Active", "Expired", "Terminated", "Renewed"]

	function updateGui() {
		contractsPage.__refreshContracts()
	}

	function __refreshContracts() {
		if (contractsPage.apiClient && contractsPage.tenantData && contractsPage.tenantData.m_id)
			contractsPage.apiClient.fetchContracts(contractsPage.tenantData.m_id)
	}

	function __clearForm() {
		relationshipInput.text = ""
		targetTenantInput.text = ""
		scopeInput.text = ""
		validFromInput.text = ""
		validUntilInput.text = ""
		descriptionInput.text = ""
		termsInput.text = ""
	}

	function __createContract() {
		if (!contractsPage.apiClient || !contractsPage.tenantData)
			return
		var relationshipId = relationshipInput.text.trim()
		var targetTenantId = targetTenantInput.text.trim()
		if (relationshipId === "" || targetTenantId === "") {
			ModalDialogManager.showInfoDialog(qsTr("Relationship and target tenant are required."))
			return
		}
		contractsPage.apiClient.createContract(
			relationshipId,
			contractsPage.tenantData.m_id,
			targetTenantId,
			scopeInput.text.trim(),
			validFromInput.text.trim(),
			validUntilInput.text.trim(),
			descriptionInput.text.trim(),
			termsInput.text.trim())
	}

	onVisibleChanged: {
		if (contractsPage.visible)
			contractsPage.__refreshContracts()
	}

	Component.onCompleted: contractsPage.__refreshContracts()

	Connections {
		target: contractsPage.apiClient

		function onContractCreated(contractId) {
			PopupManager.addSuccessMessage(qsTr("Contract created successfully"), true)
			contractsPage.__clearForm()
			contractsPage.__refreshContracts()
		}

		function onContractStatusUpdated(contractId) {
			PopupManager.addSuccessMessage(qsTr("Contract status updated"), true)
			contractsPage.__refreshContracts()
		}

		function onContractTerminated(contractId) {
			PopupManager.addSuccessMessage(qsTr("Contract terminated"), true)
			contractsPage.__refreshContracts()
		}
	}

	CustomScrollbar {
		id: contractsScrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: contractsFlickable.top
		anchors.bottom: contractsFlickable.bottom
		secondSize: Style.marginM
		targetItem: contractsFlickable
	}

	Flickable {
		id: contractsFlickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		contentHeight: contractsColumn.height + 2 * Style.marginXL
		clip: true

		Column {
			id: contractsColumn
			width: Style.sizeHintXXL
			spacing: Style.marginXL

			Column {
				width: parent.width
				spacing: Style.marginXS

				BaseText {
					text: qsTr("Cooperation Contracts")
					font.pixelSize: Style.fontSizeXL
					font.bold: true
					color: Style.textColor
				}

				BaseText {
					width: parent.width
					wrapMode: Text.WordWrap
					text: qsTr("A contract captures the agreed terms of a cooperation between two tenants. It references a tenant relationship and governs the cross-org grants issued under it.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			// --- Create contract form ---
			GroupElementView {
				id: createGroup
				width: parent.width
				visible: contractsPage.__canManage

				TextInputElementView {
					id: relationshipInput
					name: qsTr("Relationship ID")
					placeHolderText: qsTr("Associated tenant relationship")
				}

				TextInputElementView {
					id: targetTenantInput
					name: qsTr("Target Tenant ID")
					placeHolderText: qsTr("Counterparty tenant")
				}

				TextInputElementView {
					id: scopeInput
					name: qsTr("Scope")
					placeHolderText: qsTr("Optional — agreed scope of cooperation")
				}

				TextInputElementView {
					id: validFromInput
					name: qsTr("Valid From")
					placeHolderText: qsTr("Optional ISO timestamp")
				}

				TextInputElementView {
					id: validUntilInput
					name: qsTr("Valid Until")
					placeHolderText: qsTr("Optional ISO timestamp — empty for open-ended")
				}

				TextInputElementView {
					id: descriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Optional description")
				}

				TextInputElementView {
					id: termsInput
					name: qsTr("Terms")
					placeHolderText: qsTr("Optional free-form terms metadata")
				}

				Button {
					text: qsTr("Create Contract")
					onClicked: contractsPage.__createContract()
				}
			}

			// --- Existing contracts ---
			BaseText {
				text: qsTr("Contracts")
				font.pixelSize: Style.fontSizeL
				font.bold: true
				color: Style.textColor
			}

			BaseText {
				width: parent.width
				visible: !contractsList.count
				text: qsTr("No contracts for this tenant.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Column {
				id: contractsList
				width: parent.width
				spacing: Style.marginM

				property int count: contractsPage.apiClient && contractsPage.apiClient.contractsModel
					? contractsPage.apiClient.contractsModel.count
					: 0

				Repeater {
					model: contractsPage.apiClient ? contractsPage.apiClient.contractsModel : null

					delegate: Rectangle {
						width: contractsList.width
						height: contractColumn.height + 2 * Style.marginM
						color: Style.alternateBaseColor
						radius: Style.radiusS
						border.color: Style.borderColor
						border.width: 1

						Row {
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							anchors.leftMargin: Style.marginM
							anchors.rightMargin: Style.marginM
							spacing: Style.marginM

							Column {
								id: contractColumn
								width: parent.width - contractActions.width - Style.marginM
								spacing: Style.marginXS

								BaseText {
									width: parent.width
									elide: Text.ElideRight
									text: qsTr("Target: %1").arg(model.targetTenantId || "")
									font.pixelSize: Style.fontSizeM
									color: Style.textColor
								}

								BaseText {
									width: parent.width
									elide: Text.ElideRight
									text: qsTr("Status: %1   Scope: %2")
										.arg(model.status || qsTr("Draft"))
										.arg((model.scope && model.scope !== "") ? model.scope : qsTr("All"))
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}

								BaseText {
									width: parent.width
									visible: (model.validFrom && model.validFrom !== "") || (model.validUntil && model.validUntil !== "")
									elide: Text.ElideRight
									text: qsTr("Valid: %1 – %2")
										.arg((model.validFrom && model.validFrom !== "") ? model.validFrom : qsTr("now"))
										.arg((model.validUntil && model.validUntil !== "") ? model.validUntil : qsTr("open"))
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}

								BaseText {
									width: parent.width
									visible: model.description && model.description !== ""
									elide: Text.ElideRight
									text: model.description || ""
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}
							}

							Column {
								id: contractActions
								anchors.verticalCenter: parent.verticalCenter
								spacing: Style.marginXS

								Button {
									visible: contractsPage.__canManage && model.status === "Draft"
									text: qsTr("Activate")
									onClicked: {
										if (contractsPage.apiClient)
											contractsPage.apiClient.updateContractStatus(model.contractId || "", "Active")
									}
								}

								Button {
									visible: contractsPage.__canManage && model.status !== "Terminated"
									text: qsTr("Terminate")
									onClicked: {
										if (contractsPage.apiClient)
											contractsPage.apiClient.terminateContract(model.contractId || "")
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
