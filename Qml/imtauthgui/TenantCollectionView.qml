// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
// TEMPORARY: removed QtQuick.Controls import (was used for Popup, now using PopupView)
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
	additionalFieldIds: [
		TenantItemDataTypeMetaInfo.s_id,
		TenantItemDataTypeMetaInfo.s_name,
		TenantItemDataTypeMetaInfo.s_tenantRelationScope,
		TenantItemDataTypeMetaInfo.s_invitationId,
		TenantItemDataTypeMetaInfo.s_invitedByName
	]

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
				let scope = table.elements.getData(TenantItemDataTypeMetaInfo.s_tenantRelationScope, indexes[0])
				if (scope === "Invited") return
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
				if (rowIndex >= 0 && tenantRelationScopeDelegate && tenantRelationScopeDelegate.rowDelegate && tenantRelationScopeDelegate.rowDelegate.tableItem){
					let scope = tenantRelationScopeDelegate.rowDelegate.tableItem.elements.getData(TenantItemDataTypeMetaInfo.s_tenantRelationScope, rowIndex);
					if (scope){
						scopeValue = scope
					}

					let invitationId = tenantRelationScopeDelegate.rowDelegate.tableItem.elements.getData(TenantItemDataTypeMetaInfo.s_invitationId, rowIndex);
					if (invitationId){
						invitationIdValue = invitationId
					}

					let invitedByName = tenantRelationScopeDelegate.rowDelegate.tableItem.elements.getData(TenantItemDataTypeMetaInfo.s_invitedByName, rowIndex);
					if (invitedByName){
						invitedByNameValue = invitedByName
					}
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
							var point = tenantRelationScopeDelegate.mapToItem(null, 0, tenantRelationScopeDelegate.height)
							ModalDialogManager.openDialog(invitationPopupComp, {
								"x": point.x,
								"y": point.y
							})
						}
					}
				}
			}

			Component {
				id: invitationPopupComp
			// TEMPORARY: PopupView-based solution (replaces QtQuick.Controls Popup)
			PopupView {
				id: invitationPopup
				x: scopeLabel.x
				y: scopeLabel.y + scopeLabel.height + Style.spacingS
				z: tenantRelationScopeDelegate.z + 1
				width: invitationPopupContent.width + 2 * Style.marginL
				height: invitationPopupContent.height + 2 * Style.marginL
				forceFocus: true

				Rectangle {
					anchors.fill: parent
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
										invitationPopup.visible = false
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
										invitationPopup.visible = false
									}
								}
							}
						}
					}
				}
			}
			// END TEMPORARY: PopupView-based solution
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
		if (tenantId === AuthorizationController.currentTenantId) {
			return
		}

		AuthorizationController.selectTenant(tenantId || "")
	}

	property string __pendingOpenTenantAfterSwitchId: ""
	property bool __pendingCreateNewTenantDocumentAfterSwitch: false

	function openTenantDocument(tenantId) {
		if (!tenantId) return
		var tenantDocumentService = MainDocumentService.getDocumentService(container.collectionId)
		if (!tenantDocumentService) {
			console.error("Unable to open tenant document. Error: Document manager is invalid")
			return
		}
		tenantDocumentService.openDocument("Tenant", tenantId)
	}

	function requestOpenTenantDocument(tenantId, tenantName) {
		if (!tenantId) return
		if (tenantId !== AuthorizationController.currentTenantId) {
			ModalDialogManager.openDialog(switchOnDoubleClickDialogComp,
				{"tenantId": tenantId, "tenantName": tenantName || tenantId})
		} else {
			openTenantDocument(tenantId)
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
					if (tenantId && tenantId !== AuthorizationController.currentTenantId) {
						container.__pendingOpenTenantAfterSwitchId = tenantId
						container.switchToTenant(tenantId)
					} else {
						container.openTenantDocument(tenantId)
					}
				}
			}
		}
	}

	Connections {
		target: AuthorizationController

		function onTenantSelected(tenantId) {
			if (container.__pendingCreateNewTenantDocumentAfterSwitch && tenantId === "") {
				container.__pendingCreateNewTenantDocumentAfterSwitch = false
				if (container.commandsDelegate) {
					container.commandsDelegate.createNewTenantDocument()
				}
				return
			}

			if (container.__pendingOpenTenantAfterSwitchId !== ""
					&& tenantId === container.__pendingOpenTenantAfterSwitchId) {
				var pendingTenantId = container.__pendingOpenTenantAfterSwitchId
				container.__pendingOpenTenantAfterSwitchId = ""
				container.openTenantDocument(pendingTenantId)
			}
		}

		function onTenantSelectionFailed(errorMessage) {
			if (container.__pendingCreateNewTenantDocumentAfterSwitch) {
				container.__pendingCreateNewTenantDocumentAfterSwitch = false
				if (errorMessage && errorMessage !== "") {
					ModalDialogManager.showInfoDialog(errorMessage)
				}
				return
			}

			if (container.__pendingOpenTenantAfterSwitchId !== "") {
				container.__pendingOpenTenantAfterSwitchId = ""
				if (errorMessage && errorMessage !== "") {
					ModalDialogManager.showInfoDialog(errorMessage)
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

	Component {
		id: leaveCurrentTenantForNewDialogComp
		MessageDialog {
			width: Style.sizeHintM
			title: qsTr("Switch current organization")
			message: qsTr("To create a new organization, switch from the current organization?")
			onFinished: {
				if (buttonId == Enums.yes) {
					container.__pendingCreateNewTenantDocumentAfterSwitch = true
					container.switchToTenant("")
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

			function createNewTenantDocument(){
				if (!documentManager){
					console.error("Unable to create object. Error: Document manager is invalid")
					return
				}

				documentManager.createDocument("Tenant")
			}

			function onNew(){
				if (AuthorizationController.currentTenantId && AuthorizationController.currentTenantId !== "") {
					ModalDialogManager.openDialog(leaveCurrentTenantForNewDialogComp)
					return
				}

				createNewTenantDocument()
			}

			function onEdit(){
				if (!collectionView){
					console.error("Unable to edit element. Error: Collection view is invalid")
					return
				}

				if (!documentManager){
					console.error("Unable to edit elements. Error: Document manager is invalid")
					return
				}

				let elementsModel = collectionView.table.elements
				if (!elementsModel){
					console.error("Unable to edit document. Error: Elements for collection view is invalid")
					return
				}

				let indexes = collectionView.table.getSelectedIndexes()
				for (let i = 0; i < indexes.length; ++i){
					let index = indexes[i]
					if (!elementsModel.containsKey("id", index)){
						console.error("Unable to edit element. Field: 'id' does not exists in the table model")
						return
					}

					if (!elementsModel.containsKey("typeId", index)){
						console.error("Unable to edit element. Field: 'typeId' does not exists in the table model")
						return
					}

					let tenantId = elementsModel.getData("id", index)
					let typeId = elementsModel.getData("typeId", index)
					let scope = elementsModel.getData(TenantItemDataTypeMetaInfo.s_tenantRelationScope, index)
					if (scope === "Invited") {
						return
					}
					if (tenantId && tenantId !== AuthorizationController.currentTenantId){
						let tenantName = elementsModel.containsKey("name", index)
								? elementsModel.getData("name", index)
								: tenantId
						container.requestOpenTenantDocument(tenantId, tenantName)
						return
					}

					documentManager.openDocument(typeId, tenantId)
				}
			}

			function updateStateCustomCommands(selection, commandsController, elementsModel){
				let singleSelection = selection && selection.length === 1
				let switchEnabled = false
				let leaveEnabled = false
				if (singleSelection) {
					let row = selection[0]

					let tenantId = elementsModel.getData("id", row)
					let scope = elementsModel.getData(TenantItemDataTypeMetaInfo.s_tenantRelationScope, row)

					switchEnabled = tenantId !== AuthorizationController.currentTenantId && scope !== "Invited"
					leaveEnabled = scope === "Member"
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
						}
					}
					onIsNewTenantChanged: {
						if (!isNewTenant){
							var tenantId = tenantEditor.tenantData ? tenantEditor.tenantData.m_id : ""
							if (tenantId && tenantId !== AuthorizationController.currentTenantId){
								container.switchToTenant(tenantId)
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
