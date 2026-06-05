// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import QtQuick.Controls
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthgui 1.0
import imtauthTenantsSdl 1.0
import imtauthTenantCollectionDocumentServiceSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtbaseUndoManagerSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import imtauthTenantMembershipsSdl 1.0

RemoteCollectionView {
	id: container

	collectionId: "Tenants"
	gqlGetListCommandId: ImtauthTenantsSdlCommandIds.s_getTenantList
	documentCollectionFilter: null
	additionalFieldIds: ["id", "name", TenantItemDataTypeMetaInfo.s_tenantRelationScope, TenantItemDataTypeMetaInfo.s_invitationId, TenantItemDataTypeMetaInfo.s_invitedByName]

	Component.onCompleted: {
		table.setSortingInfo(TenantItemDataTypeMetaInfo.s_createdAt, "DESC")
		table.nonSortableColumns = [TenantItemDataTypeMetaInfo.s_ownerId, TenantItemDataTypeMetaInfo.s_isActive, TenantItemDataTypeMetaInfo.s_membersCount, TenantItemDataTypeMetaInfo.s_tenantRelationScope, TenantItemDataTypeMetaInfo.s_invitationId, TenantItemDataTypeMetaInfo.s_invitedByName]
		registerFieldFilterDelegate("isActiveFilter", isActiveDelegateFilterComp)
		registerFieldFilterDelegate("tenantRelationFilter", tenantRelationDelegateFilterComp)
	}

	onCommandActivated: {
		if (commandId === "Switch") {
			let indexes = table.getSelectedIndexes()
			if (indexes.length === 1) {
				let tenantId = table.elements.getData("id", indexes[0])
				container.switchToTenant(tenantId)
			}
		} else if (commandId === "Leave") {
			let indexes = table.getSelectedIndexes()
			if (indexes.length === 1) {
				let tenantId = table.elements.getData("id", indexes[0])
				let tenantName = table.elements.getData("name", indexes[0])
				ModalDialogManager.openDialog(leaveConfirmDialogComp, {"tenantId": tenantId, "tenantName": tenantName || tenantId})
			}
		}
	}

	Component {
		id: leaveConfirmDialogComp
		MessageDialog {
			property string tenantId: ""
			property string tenantName: ""
			width: Style.sizeHintM
			title: qsTr("Leave organization")
			message: qsTr("Are you sure you want to leave \"%1\"?").arg(tenantName)
			onFinished: {
				if (buttonId == Enums.yes) {
					container.leaveTenant(tenantId)
				}
			}
		}
	}

	// --- Real-time invitation notifications (handled by tenantManagementApiClient) ---
	property GqlBasedTenantManagementApiClient tenantManagementApiClient: GqlBasedTenantManagementApiClient {}

	Connections {
		target: container.tenantManagementApiClient
		function onSubscriptionInvitationReceived(notification) {
			container.doUpdateGui()
		}
		function onSubscriptionInvitationAccepted(notification) {
			container.doUpdateGui()
		}
		function onSubscriptionInvitationRejected(notification) {
			container.doUpdateGui()
		}
		function onSubscriptionOwnershipTransferred(notification) {
			container.doUpdateGui()
		}
	}

	onHeadersChanged: {
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_membersCount, membersCountCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_ownerId, ownerIdCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_createdAt, createdAtCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_updatedAt, updatedAtCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_isActive, isActiveCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_tenantRelationScope, tenantRelationScopeCellDelegateComp)
	}

	function acceptInvitation(invitationId) {
		acceptInvitationInput.m_invitationId = invitationId
		// Capture the tenantId of the accepted invitation so we can broadcast
		// it locally on success (the server doesn't re-notify the local actor
		// via subscription, so any open TenantEditor for that tenant has to
		// be refreshed explicitly here).
		container.__pendingAcceptTenantId = ""
		var rows = container.table ? container.table.elementsCount : 0
		for (var r = 0; r < rows; r++) {
			if (container.table.elements.getData(TenantItemDataTypeMetaInfo.s_invitationId, r) === invitationId) {
				container.__pendingAcceptTenantId = container.table.elements.getData("id", r) || ""
				break
			}
		}
		acceptInvitationSender.send(acceptInvitationInput)
	}

	function rejectInvitation(invitationId) {
		rejectInvitationInput.m_invitationId = invitationId
		container.__pendingRejectTenantId = ""
		var rows = container.table ? container.table.elementsCount : 0
		for (var r = 0; r < rows; r++) {
			if (container.table.elements.getData(TenantItemDataTypeMetaInfo.s_invitationId, r) === invitationId) {
				container.__pendingRejectTenantId = container.table.elements.getData("id", r) || ""
				break
			}
		}
		rejectInvitationSender.send(rejectInvitationInput)
	}

	property string __pendingAcceptTenantId: ""
	property string __pendingRejectTenantId: ""

	property AcceptTenantInvitationInput acceptInvitationInput: AcceptTenantInvitationInput {}
	property GqlSdlRequestSender acceptInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_acceptTenantInvitation
		sdlObjectComp: Component {
			AcceptTenantInvitationPayload {
				onFinished: {
					if (m_success) {
						container.doUpdateGui()
						// Fan out so any TenantEditor open on this tenant reloads.
						AuthorizationController.tenantInvitationAccepted({
							"membershipId": "",
							"userId": AuthorizationController.userTokenProvider ? AuthorizationController.userTokenProvider.userId : "",
							"tenantId": container.__pendingAcceptTenantId,
							"tenantName": "",
							"role": ""
						})
					} else if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					}
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showInfoDialog(message)
		}
	}

	property RejectTenantInvitationInput rejectInvitationInput: RejectTenantInvitationInput {}
	property GqlSdlRequestSender rejectInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_rejectTenantInvitation
		sdlObjectComp: Component {
			RejectTenantInvitationPayload {
				onFinished: {
					if (m_success) {
						container.doUpdateGui()
						AuthorizationController.tenantInvitationRejected({
							"membershipId": "",
							"userId": AuthorizationController.userTokenProvider ? AuthorizationController.userTokenProvider.userId : "",
							"tenantId": container.__pendingRejectTenantId,
							"tenantName": "",
							"role": ""
						})
					} else if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					}
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showInfoDialog(message)
		}
	}

	Component {
		id: tenantRelationScopeCellDelegateComp
		TableCellDelegateBase {
			id: tenantRelationScopeDelegate

			property string scopeValue: ""
			property string invitationIdValue: ""
			property string invitedByNameValue: ""

			onReused: {
				if (rowIndex >= 0 && tenantRelationScopeDelegate.rowDelegate && tenantRelationScopeDelegate.rowDelegate.tableItem){
					scopeValue = tenantRelationScopeDelegate.rowDelegate.tableItem.elements.getData(TenantItemDataTypeMetaInfo.s_tenantRelationScope, rowIndex);
					invitationIdValue = tenantRelationScopeDelegate.rowDelegate.tableItem.elements.getData(TenantItemDataTypeMetaInfo.s_invitationId, rowIndex);
					invitedByNameValue = tenantRelationScopeDelegate.rowDelegate.tableItem.elements.getData(TenantItemDataTypeMetaInfo.s_invitedByName, rowIndex);
				}
			}

			Text {
				id: scopeLabel
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				font.pixelSize: Style.fontSizeM
				color: tenantRelationScopeDelegate.scopeValue === "Invited" ? Style.imaginToolsAccentColor : Style.textColor
				text: tenantRelationScopeDelegate.scopeValue
				font.underline: tenantRelationScopeDelegate.scopeValue === "Invited"

				MouseArea {
					id: scopeMouseArea
					anchors.fill: parent
					hoverEnabled: tenantRelationScopeDelegate.scopeValue === "Invited"
					cursorShape: tenantRelationScopeDelegate.scopeValue === "Invited" ? Qt.PointingHandCursor : Qt.ArrowCursor
					onEntered: {
						if (tenantRelationScopeDelegate.scopeValue === "Invited") {
							invitationPopup.open()
						}
					}
				}
			}

			Popup {
				id: invitationPopup
				x: scopeLabel.x
				y: scopeLabel.y + scopeLabel.height + Style.spacingS
				width: invitationPopupContent.width + 2 * Style.marginL
				height: invitationPopupContent.height + 2 * Style.marginL
				closePolicy: Enums.popupCloseOnEscape | Enums.popupCloseOnPressOutside

				background: Rectangle {
					color: Style.baseColor
					border.color: Style.borderColor
					border.width: 1
					radius: Style.radiusM
				}

				Column {
					id: invitationPopupContent
					anchors.centerIn: parent
					spacing: Style.spacingM

					Text {
						text: qsTr("Invited by: %1").arg(tenantRelationScopeDelegate.invitedByNameValue || qsTr("Unknown"))
						font.pixelSize: Style.fontSizeM
						color: Style.textColor
					}

					Row {
						spacing: Style.spacingM
						anchors.horizontalCenter: parent.horizontalCenter

						Rectangle {
							width: acceptBtnLabel.contentWidth + 2 * Style.marginL
							height: Style.controlHeightS
							radius: Style.radiusM
							color: "#3FB950"

							Text {
								id: acceptBtnLabel
								anchors.centerIn: parent
								text: qsTr("Accept")
								font.pixelSize: Style.fontSizeS
								color: "#FFFFFF"
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								onClicked: {
									if (tenantRelationScopeDelegate.invitationIdValue !== "") {
										container.acceptInvitation(tenantRelationScopeDelegate.invitationIdValue)
										invitationPopup.close()
									}
								}
							}
						}

						Rectangle {
							width: rejectBtnLabel.contentWidth + 2 * Style.marginL
							height: Style.controlHeightS
							radius: Style.radiusM
							color: "#DA3633"

							Text {
								id: rejectBtnLabel
								anchors.centerIn: parent
								text: qsTr("Reject")
								font.pixelSize: Style.fontSizeS
								color: "#FFFFFF"
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								onClicked: {
									if (tenantRelationScopeDelegate.invitationIdValue !== "") {
										container.rejectInvitation(tenantRelationScopeDelegate.invitationIdValue)
										invitationPopup.close()
									}
								}
							}
						}
					}
				}
			}
		}
	}

	Component {
		id: membersCountCellDelegateComp
		TableCellDelegateBase {
			id: membersCountDelegate

			onReused: {
				var val = membersCountDelegate.getValue()
				membersCountLabel.text = val !== undefined && val !== null ? val.toString() : "0"
			}

			Text {
				id: membersCountLabel
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				font.pixelSize: Style.fontSizeM
				color: Style.textColor
			}
		}
	}

	Component {
		id: ownerIdCellDelegateComp
		TableCellDelegateBase {
			id: ownerIdDelegate

			onReused: {
				var val = ownerIdDelegate.getValue()
				ownerIdLabel.text = val !== undefined && val !== null ? val.toString() : ""
			}

			Text {
				id: ownerIdLabel
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				width: parent.width - 2 * Style.marginM
				font.pixelSize: Style.fontSizeM
				color: Style.textColor
				elide: Text.ElideRight
			}
		}
	}

	Component {
		id: createdAtCellDelegateComp
		TableCellDateDelegate {}
	}

	Component {
		id: updatedAtCellDelegateComp
		TableCellDateDelegate {}
	}

	Component {
		id: isActiveCellDelegateComp
		TableCellDelegateBase {
			id: isActiveDelegate
			onReused: {
				var val = isActiveDelegate.getValue()
				isActiveLabel.text = val ? qsTr("Active") : qsTr("Inactive")
				isActiveIndicator.color = val ? "#3FB950" : "#8C95A6"
			}

			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				spacing: Style.spacingS

				Rectangle {
					id: isActiveIndicator
					anchors.verticalCenter: parent.verticalCenter
					width: 10
					height: 10
					radius: width / 2
					color: "#8C95A6"
				}

				Text {
					id: isActiveLabel
					anchors.verticalCenter: parent.verticalCenter
					font.pixelSize: Style.fontSizeM
					color: Style.textColor
				}
			}
		}
	}

	Component {
		id: isActiveDelegateFilterComp
		FieldFilterDelegate {
			name: qsTr("Status")
			defaultFieldFilter.m_fieldId: "IsActive"
			defaultFieldFilter.m_filterValueType: "Integer"

			Component.onCompleted: {
				createAndAddOption("1", qsTr("Active"), "", true)
				createAndAddOption("0", qsTr("Inactive"), "", true)
			}
		}
	}

	Component {
		id: tenantRelationDelegateFilterComp
		FieldFilterDelegate {
			name: qsTr("My tenants")

			Component.onCompleted: {
				createAndAddOption("owner", qsTr("I am owner"), "", true)
				createAndAddOption("member", qsTr("I am member"), "", true)
				createAndAddOption("invited", qsTr("I am invited"), "", true)
				setFieldFilterForOption("owner", ownerFieldFilterComp.createObject(this))
				setFieldFilterForOption("member", memberFieldFilterComp.createObject(this))
				setFieldFilterForOption("invited", invitedFieldFilterComp.createObject(this))
			}

			Component {
				id: ownerFieldFilterComp
				FieldFilter {
					m_fieldId: "TenantRelationScope"
					m_filterValueType: "String"
					m_filterOperations: ["Equal"]
					m_filterValue: "Owner"
				}
			}

			Component {
				id: memberFieldFilterComp
				FieldFilter {
					m_fieldId: "TenantRelationScope"
					m_filterValueType: "String"
					m_filterOperations: ["Equal"]
					m_filterValue: "Member"
				}
			}

			Component {
				id: invitedFieldFilterComp
				FieldFilter {
					m_fieldId: "TenantRelationScope"
					m_filterValueType: "String"
					m_filterOperations: ["Equal"]
					m_filterValue: "Invited"
				}
			}
		}
	}

	// --- Switch to organization ---
	function switchToTenant(tenantId) {
		if (tenantId && tenantId !== AuthorizationController.currentTenantId) {
			AuthorizationController.selectTenant(tenantId)
		}
	}

	Component {
		id: switchOnDoubleClickDialogComp
		MessageDialog {
			property string tenantId: ""
			property string tenantName: ""
			width: Style.sizeHintM
			title: qsTr("Switch organization")
			message: qsTr("Do you want to switch to \"%1\"?").arg(tenantName)
			onFinished: {
				if (buttonId == Enums.yes) {
					container.switchToTenant(tenantId)
				}
			}
		}
	}

	Component {
		id: switchToNewTenantDialogComp
		MessageDialog {
			property string tenantId: ""
			property string tenantName: ""
			width: Style.sizeHintM
			title: qsTr("Switch to new organization")
			message: qsTr("Organization \"%1\" has been created. Do you want to switch to it?").arg(tenantName)
			onFinished: {
				if (buttonId == Enums.yes) {
					container.switchToTenant(tenantId)
				}
			}
		}
	}

	// --- Leave organization ---
	function leaveTenant(tenantId) {
		if (!tenantId) return
		__leaveTenantId = tenantId
		__findMembershipForLeaveInput.m_userId = AuthorizationController.userTokenProvider.userId
		__findMembershipForLeaveInput.m_tenantId = tenantId
		__findMembershipForLeaveSender.send(__findMembershipForLeaveInput)
	}

	property string __leaveTenantId: ""

	property FindMembershipInput __findMembershipForLeaveInput: FindMembershipInput {}
	property GqlSdlRequestSender __findMembershipForLeaveSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_findMembership

		sdlObjectComp: Component {
			FindMembershipPayload {
				onFinished: {
					if (m_membership && m_membership.m_id && m_membership.m_id !== "") {
						container.__removeMembershipForLeaveInput.m_membershipId = m_membership.m_id
						container.__removeMembershipForLeaveSender.send(container.__removeMembershipForLeaveInput)
					} else if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					}
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showInfoDialog(message)
		}
	}

	property RemoveMembershipInput __removeMembershipForLeaveInput: RemoveMembershipInput {}
	property GqlSdlRequestSender __removeMembershipForLeaveSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_removeMembership

		sdlObjectComp: Component {
			RemoveMembershipPayload {
				onFinished: {
					if (m_success) {
						// If we left the currently selected tenant, deselect it
						if (container.__leaveTenantId === AuthorizationController.currentTenantId) {
							AuthorizationController.selectTenant("")
						}
						container.doUpdateGui()
					} else if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					}
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showInfoDialog(message)
		}
	}

	commandsDelegateComp: Component {
		DocCollectionViewDelegate {
			id: tenantCommandsDelegate
			collectionView: container

			function updateStateCustomCommands(selection, commandsController, elementsModel){
				let singleSelection = selection && selection.length === 1
				let switchEnabled = false
				let leaveEnabled = false
				if (singleSelection) {
					let row = selection[0]

					let tenantId = elementsModel.getData("id", row)
					let scope = elementsModel.getData(TenantItemDataTypeMetaInfo.s_tenantRelationScope, row)

					switchEnabled =  tenantId !== AuthorizationController.currentTenantId
					leaveEnabled =  scope === "Member"
				}
				commandsController.setCommandIsEnabled("Switch", switchEnabled)
				commandsController.setCommandIsEnabled("Leave", leaveEnabled)
			}

			Component.onCompleted: {
				registerDocumentType("Tenant", qsTr("Tenant"))
				addDocumentView("Tenant", "TenantEditor", tenantEditorComp, tenantDataControllerFactory)
			}

			Component {
				id: tenantEditorComp

				TenantEditor {
					id: tenantEditor
					apiClient: container.tenantManagementApiClient
					commandsControllerComp: Component {
						GqlBasedCommandsController {
							typeId: "Tenant"
						}
					}
					commandsPanelVisible: isNewTenant
					onDocumentSaved: {
						if (isNewTenant){
							if (tenantEditor.representationController){
								tenantEditor.representationController.updateRepresentationFromDocument()
							}

							var newTenantId = tenantEditor.tenantData ? tenantEditor.tenantData.m_id : ""
							var newTenantName = (tenantEditor.tenantData && tenantEditor.tenantData.m_name) || newTenantId
							if (newTenantId && newTenantId !== AuthorizationController.currentTenantId){
								ModalDialogManager.openDialog(switchToNewTenantDialogComp,
									{"tenantId": newTenantId, "tenantName": newTenantName})
							}
						}
					}
				}
			}

			Component {
				id: tenantDataControllerFactory

				DocumentRepresentationController {
					id: root

					representationModel: TenantData {
						m_isActive: true
					}

					function updateRepresentationFromDocument(){
						startUpdateRepresentation(documentId, representationModel)

						documentIdInput.m_id = documentId
						documentIdInput.m_collectionId = container.collectionId

						getTenantRequest.send(documentIdInput)
					}

					function updateDocumentFromRepresentation(){
						startUpdateDocument(documentId)

						updateTenantInput.m_documentId = documentId
						updateTenantInput.m_tenant = representationModel

						updateTenantRequest.send(updateTenantInput)
					}

					property DocumentId documentIdInput: DocumentId {}
					property GqlSdlRequestSender getTenantRequest: GqlSdlRequestSender {
						gqlCommandId: ImtauthTenantCollectionDocumentServiceSdlCommandIds.s_getTenantRepresentation
						sdlObjectComp: Component {
							TenantData {
								onFinished: {
									root.representationModel.copyFrom(this)
									root.representationUpdated(root.documentId, root.representationModel)
								}
							}
						}

						function onError(message, type){
							root.updateRepresentationFailed(root.documentId, message)
						}
					}

					property UpdateTenantFromRepresentationInput updateTenantInput: UpdateTenantFromRepresentationInput {}
					property GqlSdlRequestSender updateTenantRequest: GqlSdlRequestSender {
						gqlCommandId: ImtauthTenantCollectionDocumentServiceSdlCommandIds.s_updateTenantFromRepresentation
						requestType: 1
						sdlObjectComp: Component {
							DocumentOperationStatus {
								onFinished: {
									if (m_status === "Success"){
										root.documentUpdated(root.documentId)
									}
								}
							}
						}

						function onError(message, type){
							root.updateDocumentFailed(root.documentId, message)
						}
					}
				}
			}
		}
	}
}
