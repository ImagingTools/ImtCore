// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantMembersPage
 *
 * Members tab — list / create / edit / exclude tenant members.
 * Extends SimpleCollectionPage with custom header buttons,
 * a custom delegate (TenantMemberDelegate), and member-specific
 * commands (invite, exclude, role management).
 */
SimpleCollectionPage {
	id: membersPage

	entityName: qsTr("User")
	entityNamePlural: qsTr("Members")
	descriptionText: qsTr("Manage tenant members. Users created here automatically become members.")

	documentManager: apiClient ? apiClient.userDocumentManager : null
	objectTypeId: apiClient ? apiClient.userObjectTypeId : ""

	// Members use stateManager-driven model, not a data provider
	listModel: __combinedModel
	managePermissionIds: ["EditOrganizationMember", "InviteOrganizationMember", "ExcludeOrganizationMember", "ChangeOrganizationMemberRole"]
	createPermissionIds: ["EditOrganizationMember"]
	editPermissionIds: ["ChangeOrganizationMemberRole", "EditOrganizationMember"]
	deletePermissionIds: ["ExcludeOrganizationMember", "EditOrganizationMember"]

	readonly property bool __canCreateMember: stateManager
		? stateManager.hasAnyPermission(createPermissionIds)
		: false
	readonly property bool __canInviteMember: stateManager
		? stateManager.canInviteOrganizationMember
		: false
	readonly property bool __canEditMember: stateManager
		? stateManager.hasAnyPermission(editPermissionIds)
		: false
	readonly property bool __canExcludeMember: stateManager
		? stateManager.canExcludeOrganizationMember
		: false

	delegateComponent: memberDelegateComp

	headerButtonsComponent: memberHeaderButtonsComp

	function removeItem(id) {
		if (membersPage.apiClient) {
			var tenantId = membersPage.model ? membersPage.model.m_id : ""
			if (id.indexOf("inv_") === 0)
				membersPage.apiClient.revokeInvitation(id.substring(4))
			else
				membersPage.apiClient.removeMember(tenantId, id)
		}
	}

	function confirmTransferOwnership() {
		ModalDialogManager.showConfirmationDialog(
			qsTr("Transfer Ownership"),
			qsTr("Are you sure you want to transfer ownership to \"%1\"?").arg(memberActionMenu.targetUserName),
			function(result) {
				if (result === Enums.yes && membersPage.apiClient) {
					var tid = membersPage.model ? membersPage.model.m_id : ""
					membersPage.apiClient.transferOwnership(tid, memberActionMenu.targetUserId)
				}
			}
		)
	}

	function showMemberOrgPermissionsPopup(memberData) {
		orgPermsPopup.currentMember = memberData
		orgPermsPopup.__checkedCount = 0
		permsLoading.start()
		orgPermsPopup.open()
		var tid = membersPage.model ? membersPage.model.m_id : ""
		var uid = memberData ? (memberData.id || "") : ""
		if (membersPage.apiClient && tid) {
			membersPage.apiClient.fetchOrganizationPermissions(tid, uid)
		}
	}

	Popup {
		id: orgPermsPopup
		parent: ModalDialogManager.activeView
		modal: true
		dim: true
		width: Math.min(Style.sizeHintXXXL, Math.round(parent.width * 0.8))
		height: Math.min(Style.sizeHintXXXL, Math.round(parent.height * 0.8))
		x: Math.round((parent.width - width) / 2)
		y: Math.round((parent.height - height) / 2)
		padding: 0
		closePolicy: Enums.popupCloseOnEscape | Enums.popupCloseOnPressOutside

		property var currentMember: null
		property int __checkedCount: 0

		background: Rectangle {
			color: Style.baseColor
			border.color: Style.borderColor
			border.width: 1
			radius: Style.radiusM
		}

		Item {
			anchors.fill: parent
			anchors.margins: 1

			// ─── Header ─────────────────────────────────────────────
			Rectangle {
				id: permsHeader
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
				height: 62
				color: Style.alternateBaseColor
				radius: Style.radiusM

				// Square off bottom corners so the color fills edge-to-edge
				Rectangle {
					anchors.bottom: parent.bottom
					anchors.left: parent.left
					anchors.right: parent.right
					height: Style.radiusM
					color: parent.color
				}

				Row {
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.leftMargin: Style.marginM
					anchors.rightMargin: Style.marginM
					spacing: Style.marginM

					Rectangle {
						width: 36
						height: 36
						radius: 18
						color: Style.selectedColor
						anchors.verticalCenter: parent.verticalCenter

						BaseText {
							anchors.centerIn: parent
							text: orgPermsPopup.currentMember
								? ((orgPermsPopup.currentMember.name || orgPermsPopup.currentMember.id || "").charAt(0).toUpperCase() || "?")
								: "?"
							font.bold: true
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
						}
					}

					Column {
						anchors.verticalCenter: parent.verticalCenter
						spacing: 2

						BaseText {
							text: qsTr("Organization Permissions")
							font.pixelSize: Style.fontSizeL
							font.bold: true
							color: Style.textColor
						}

						BaseText {
							text: orgPermsPopup.currentMember
								? (orgPermsPopup.currentMember.name || orgPermsPopup.currentMember.id || "")
								: ""
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}
					}
				}
			}

			// ─── Permissions tree ────────────────────────────────────
			// Always visible — delegates build while Loading covers the area,
			// so when Loading stops the tree is already fully rendered.
			PermissionsTableView {
				id: orgPermsTable
				anchors.top: permsHeader.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.bottom: permsFooter.top
				anchors.leftMargin: Style.marginM
				anchors.rightMargin: Style.marginM
				showControlPanel: true
				controlPanelTopMargin: Style.marginM
				treeTopMargin: Style.marginS
				treeBottomMargin: Style.marginXS

				onSelectionChanged: {
					orgPermsPopup.__checkedCount = getCheckedIds().length
				}
			}

			// ─── Loading overlay ─────────────────────────────────────
			Loading {
				id: permsLoading
				anchors.top: permsHeader.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.bottom: permsFooter.top
				visible: false
				background.color: Style.baseColor
			}

			// ─── Footer ──────────────────────────────────────────────
			Rectangle {
				id: permsFooter
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				height: 54
				color: Style.alternateBaseColor
				radius: Style.radiusM

				// Square off top corners
				Rectangle {
					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right
					height: Style.radiusM
					color: parent.color
				}

				BaseText {
					anchors.left: parent.left
					anchors.leftMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					text: orgPermsPopup.__checkedCount > 0
						? qsTr("%1 permission(s) selected").arg(orgPermsPopup.__checkedCount)
						: qsTr("No permissions selected")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Row {
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginL

					Text {
						id: cancelBtn
						text: qsTr("Cancel")
						font.pixelSize: Style.fontSizeM
						color: Style.linkColor
						opacity: cancelMouse.containsMouse ? 0.65 : 1.0
						anchors.verticalCenter: parent.verticalCenter

						Behavior on opacity { NumberAnimation { duration: 100 } }

						MouseArea {
							id: cancelMouse
							anchors.fill: parent
							anchors.margins: -4
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onClicked: orgPermsPopup.close()
						}
					}

					Rectangle {
						id: applyBtnRect
						width: applyBtnLabel.implicitWidth + Style.marginXL
						height: 32
						radius: 4
						color: Style.linkColor
						anchors.verticalCenter: parent.verticalCenter
						opacity: applyBtnMouse.containsMouse ? 0.75 : 1.0

						Behavior on opacity { NumberAnimation { duration: 100 } }

						BaseText {
							id: applyBtnLabel
							anchors.centerIn: parent
							text: qsTr("Apply")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.baseColor
						}

						MouseArea {
							id: applyBtnMouse
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								var selected = orgPermsTable.getCheckedIds ? orgPermsTable.getCheckedIds() : []
								var tid = membersPage.model ? membersPage.model.m_id : ""
								var uid = orgPermsPopup.currentMember ? orgPermsPopup.currentMember.id : ""
								if (uid && tid && membersPage.apiClient) {
									membersPage.apiClient.setMemberOrganizationPermissionsByUser(tid, uid, selected)
								}
								orgPermsPopup.close()
								if (membersPage.stateManager) {
									membersPage.stateManager.loadMembersFromModel()
								}
							}
						}
					}
				}
			}
		}
	}

	Connections {
		target: membersPage.apiClient
		function onOrganizationPermissionsReceived() {
			if (orgPermsPopup.opened && orgPermsPopup.currentMember && orgPermsTable) {
				var tree = membersPage.apiClient ? membersPage.apiClient.organizationPermissions : []
				orgPermsTable.rebuildFromFlatArray(tree)
				// Prefer server-fresh member permissions from response; fall back to locally cached
				var serverPerms = membersPage.apiClient ? membersPage.apiClient.memberOrganizationPermissions : []
				var cur = (serverPerms && serverPerms.length > 0)
					? serverPerms
					: (orgPermsPopup.currentMember.organizationPermissions || [])
				orgPermsTable.applySelection(cur)
				orgPermsPopup.__checkedCount = orgPermsTable.getCheckedIds().length
				permsLoading.stop()
			}
		}
	}

	// --- Combined model: invitations + active members ---
	property var __combinedModel: []

	function __rebuildCombinedModel() {
		var result = []
		var filterText = membersPage.filterText || ""
		var lowerFilter = filterText.toLowerCase()

		let matches = function(name) {
			if (!lowerFilter)
				return true
			return (name || "").toLowerCase().indexOf(lowerFilter) >= 0
		}

		var invitations = membersPage.stateManager ? membersPage.stateManager.pendingInvitations : []
		var members = membersPage.stateManager ? membersPage.stateManager.pendingMembers : []

		for (var j = 0; j < invitations.length; j++) {
			var invName = invitations[j].userName || invitations[j].name || invitations[j].id || ""
			if (!matches(invName))
				continue
			result.push({
				id: "inv_" + invitations[j].id,
				title: invName,
				description: qsTr("Invited"),
				totalCount: invitations.length + members.length,
				kind: "invitation",
				sourceData: invitations[j]
			})
		}

		for (var i = 0; i < members.length; i++) {
			var memName = members[i].name || members[i].id || ""
			if (!matches(memName))
				continue
			result.push({
				id: members[i].id,
				title: memName,
				totalCount: invitations.length + members.length,
				kind: "member",
				sourceData: members[i]
			})
		}

		membersPage.__combinedModel = result
	}

	onFilterTextChanged: __rebuildCombinedModel()

	Connections {
		target: membersPage.stateManager
		function onPendingMembersChanged() { membersPage.__rebuildCombinedModel() }
		function onPendingInvitationsChanged() { membersPage.__rebuildCombinedModel() }
	}

	Component.onCompleted: {
		__rebuildCombinedModel()
		if (membersPage.apiClient && membersPage.model) {
			var tid = membersPage.model.m_id
			membersPage.apiClient.fetchOrganizationPermissions(tid)
		}
	}

	Connections {
		target: membersPage.apiClient ? membersPage.apiClient.userDocumentManager : null
		function onDocumentSaved(documentId) {
			if (membersPage.stateManager)
				membersPage.stateManager.loadMembersFromModel()
		}
	}

	Connections {
		target: membersPage.apiClient
		function onMemberRemoved(userId) {
			if (membersPage.stateManager)
				membersPage.stateManager.removeMemberById(userId)
		}
		function onInvitationRevoked(invitationId) {
			if (membersPage.stateManager)
				membersPage.stateManager.removePendingInvitation(invitationId)
		}
	}

	// --- Custom header buttons ---
	Component {
		id: memberHeaderButtonsComp

		Row {
			id: headerButtonsRow
			spacing: Style.marginL
			visible: membersPage.__canCreateMember
				|| membersPage.__canInviteMember
				|| membersPage.__canEditMember
				|| membersPage.__canExcludeMember

			Text {
				text: qsTr("Exclude")
				font.pixelSize: Style.fontSizeM
				font.bold: true
				visible: membersPage.__canExcludeMember
				color: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length > 0 ? Style.errorColor : Style.inactiveTextColor
				opacity: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length > 0 ? 1.0 : 0.5

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: membersPage.__canExcludeMember
						&& membersPage.selectionManager
						&& membersPage.selectionManager.selectedIds.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
					enabled: membersPage.__canExcludeMember
						&& membersPage.selectionManager
						&& membersPage.selectionManager.selectedIds.length > 0
					onClicked: {
						if (!membersPage.__canExcludeMember)
							return

						var count = membersPage.selectionManager.selectedIds.length
						ModalDialogManager.showConfirmationDialog(
									qsTr("Exclude Members"),
									qsTr("Are you sure you want to exclude %1 selected member(s) from this tenant?").arg(count),
									function(result) {
										if (result === Enums.yes) {
											var ids = membersPage.selectionManager.selectedIds.slice()
											for (var i = 0; i < ids.length; i++)
												membersPage.removeItem(ids[i])
											membersPage.selectionManager.clear()
										}
									}
									)
					}
				}
			}

			Text {
				text: qsTr("Edit")
				font.pixelSize: Style.fontSizeM
				font.bold: true
				visible: membersPage.__canEditMember
				color: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length === 1 && membersPage.selectionManager.selectedIds[0].indexOf("inv_") !== 0 ? Style.linkColor : Style.inactiveTextColor
				opacity: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length === 1 && membersPage.selectionManager.selectedIds[0].indexOf("inv_") !== 0 ? 1.0 : 0.5

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: membersPage.__canEditMember
						&& membersPage.selectionManager
						&& membersPage.selectionManager.selectedIds.length === 1
						&& membersPage.selectionManager.selectedIds[0].indexOf("inv_") !== 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
					enabled: membersPage.__canEditMember
						&& membersPage.selectionManager
						&& membersPage.selectionManager.selectedIds.length === 1
						&& membersPage.selectionManager.selectedIds[0].indexOf("inv_") !== 0
					onClicked: {
						if (!membersPage.__canEditMember)
							return

						var selId = membersPage.selectionManager.selectedIds[0]
						var items = membersPage.__combinedModel
						for (var i = 0; i < items.length; i++) {
							if (items[i].id === selId) {
								membersPage.openEdit(selId, items[i].title, items[i].description)
								break
							}
						}
					}
				}
			}

			Text {
				text: "+ " + qsTr("Create User")
				font.pixelSize: Style.fontSizeM
				font.bold: true
				visible: membersPage.__canCreateMember
				color: membersPage.__canCreateMember ? Style.linkColor : Style.inactiveTextColor

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: membersPage.__canCreateMember ? Qt.PointingHandCursor : Qt.ArrowCursor
					enabled: membersPage.__canCreateMember
					onClicked: {
						if (membersPage.__canCreateMember)
							membersPage.openCreate()
					}
				}
			}

			Text {
				id: inviteMemberBtn
				text: "+ " + qsTr("Create Invitation")
				font.pixelSize: Style.fontSizeM
				font.bold: true
				visible: membersPage.__canInviteMember
				color: membersPage.__canInviteMember ? Style.linkColor : Style.inactiveTextColor

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: membersPage.__canInviteMember ? Qt.PointingHandCursor : Qt.ArrowCursor
					enabled: membersPage.__canInviteMember
					onClicked: {
						if (!membersPage.__canInviteMember)
							return

						var ids = []
						var members = membersPage.stateManager ? membersPage.stateManager.pendingMembers : []
						for (var i = 0; i < members.length; i++)
							ids.push(members[i].id)
						// Also exclude users who already have a pending invitation
						var invitations = membersPage.stateManager ? membersPage.stateManager.pendingInvitations : []
						for (var j = 0; j < invitations.length; j++) {
							var invUserId = invitations[j].userId || ""
							if (invUserId && ids.indexOf(invUserId) < 0)
								ids.push(invUserId)
						}
						var point = inviteMemberBtn.mapToItem(null, 0, inviteMemberBtn.height)
						ModalDialogManager.openDialog(membersSelectPopupComp, {
														  "x": point.x,
														  "y": point.y,
														  "preselectedIds": ids
													  })
					}
				}
			}
		}
	}

	// --- Custom delegate ---
	Component {
		id: memberDelegateComp

		TenantMemberDelegate {
			width: parent.width
			kind: modelData.kind || "member"
			memberData: modelData.sourceData || modelData
			tenantData: membersPage.model
			stateManager: membersPage.stateManager
			actionHandler: membersPage
			isOwner: membersPage.stateManager ? membersPage.stateManager.isOwner : false
			selectionManager: membersPage.selectionManager
			showCheckBox: true

			onMemberActionsRequested: {
				memberActionMenu.targetUserId = userId
				memberActionMenu.targetUserName = userName
				memberActionMenu.showManagePermissions = false
				memberActionMenu.showExclude = false
				memberActionMenu.showTransfer = false
				memberActionMenu.showLeave = false
				for (var i = 0; i < menuItems.length; i++) {
					if (menuItems[i].action === "managePermissions") memberActionMenu.showManagePermissions = true
					else if (menuItems[i].action === "remove") memberActionMenu.showExclude = true
					else if (menuItems[i].action === "transfer") memberActionMenu.showTransfer = true
					else if (menuItems[i].action === "leave") memberActionMenu.showLeave = true
				}
				memberActionContextModel.fillModel()
				ModalDialogManager.openDialog(memberActionPopupComp, {
					"x": menuX,
					"y": menuY,
					"model": memberActionContextModel
				})
			}

			onMemberEditRequested: {
				if (membersPage.stateManager && membersPage.stateManager.canChangeOrganizationMember)
					membersPage.openEdit(userId, userName, "")
			}

			onInviteActionsRequested: {
				inviteActionMenu.menuItems = menuItems
				inviteActionMenu.targetInvitationId = invitationId
				inviteActionMenu.targetUserName = userName
				inviteActionContextModel.fillModel()
				ModalDialogManager.openDialog(inviteActionPopupComp, {
					"x": menuX,
					"y": menuY,
					"model": inviteActionContextModel
				})
			}
		}
	}

	ListModel {
		id: memberActionContextModel

		function fillModel(){
			memberActionContextModel.clear()

			if (memberActionMenu.showManagePermissions) {
				memberActionContextModel.append({"id": "ManagePermissions", "name": qsTr("Manage Organization Permissions..."), "icon": "", "isEnabled": true})
			}

			if (memberActionMenu.showExclude || memberActionMenu.showTransfer || memberActionMenu.showLeave) {
				if (memberActionContextModel.count > 0)
					memberActionContextModel.append({"id": "", "name": "", "icon": ""})

				if (memberActionMenu.showExclude)
					memberActionContextModel.append({"id": "Exclude", "name": qsTr("Exclude from Tenant"), "icon": "", "isEnabled": true})
				if (memberActionMenu.showTransfer)
					memberActionContextModel.append({"id": "Transfer", "name": qsTr("Transfer Ownership"), "icon": "", "isEnabled": true})
				if (memberActionMenu.showLeave)
					memberActionContextModel.append({"id": "Leave", "name": qsTr("Leave Workspace"), "icon": "", "isEnabled": true})
			}
		}
	}

	Component {
		id: memberActionPopupComp

		PopupMenuDialog {
			id: memberActionPopupDialog
			onFinished: {
				if (commandId === "ManagePermissions") {
					var uid = memberActionMenu.targetUserId
					var items = membersPage.__combinedModel
					for (var i = 0; i < items.length; i++) {
						if (items[i].id === uid) {
							membersPage.showMemberOrgPermissionsPopup(items[i].sourceData || items[i])
							break
						}
					}
				}
				else if (commandId === "Exclude") {
					if (!membersPage.stateManager || !membersPage.stateManager.canExcludeOrganizationMember)
						return

					ModalDialogManager.showConfirmationDialog(
						qsTr("Exclude Member"),
						qsTr("Are you sure you want to exclude \"%1\" from this tenant?").arg(memberActionMenu.targetUserName),
						function(result) {
							if (result === Enums.yes && membersPage.apiClient) {
								var tenantIdExclude = membersPage.model ? membersPage.model.m_id : ""
								membersPage.apiClient.removeMember(tenantIdExclude, memberActionMenu.targetUserId)
							}
						}
					)
				}
				else if (commandId === "Transfer") {
					if (membersPage.stateManager && membersPage.stateManager.isOwner)
						membersPage.confirmTransferOwnership()
				}
				else if (commandId === "Leave") {
					if (membersPage.apiClient) {
						var ltid = membersPage.model ? membersPage.model.m_id : ""
						membersPage.apiClient.removeMember(ltid, memberActionMenu.targetUserId)
					}
				}
			}
		}
	}

	ListModel {
		id: inviteActionContextModel

		function fillModel(){
			inviteActionContextModel.clear()
			for (var i = 0; i < inviteActionMenu.menuItems.length; i++) {
				var item = inviteActionMenu.menuItems[i]
				if (!item)
					continue
				inviteActionContextModel.append({"id": item.action || "", "name": item.text || "", "icon": "", "isEnabled": true})
			}
		}
	}

	Component {
		id: inviteActionPopupComp

		PopupMenuDialog {
			id: inviteActionPopupDialog
			onFinished: {
				if (commandId === "resend" && membersPage.apiClient && membersPage.stateManager && membersPage.stateManager.canInviteOrganizationMember)
					membersPage.apiClient.resendInvitation(inviteActionMenu.targetInvitationId)
				else if (commandId === "revoke" && membersPage.apiClient && membersPage.stateManager && membersPage.stateManager.canExcludeOrganizationMember)
					membersPage.apiClient.revokeInvitation(inviteActionMenu.targetInvitationId)
			}
		}
	}

	// --- Menus ---
	Menu {
		id: memberActionMenu
		property string targetUserId: ""
		property string targetUserName: ""
		property bool showManagePermissions: false
		property bool showExclude: false
		property bool showTransfer: false
		property bool showLeave: false

		closePolicy: Enums.popupCloseOnEscape | Enums.popupCloseOnPressOutside

		MenuItem {
			text: qsTr("Exclude from Tenant")
			visible: memberActionMenu.showExclude
			height: visible ? implicitHeight : 0
			onTriggered: {
				ModalDialogManager.showConfirmationDialog(
					qsTr("Exclude Member"),
					qsTr("Are you sure you want to exclude \"%1\" from this tenant?").arg(memberActionMenu.targetUserName),
					function(result) {
						if (result === Enums.yes && membersPage.apiClient) {
							var tenantId = membersPage.model ? membersPage.model.m_id : ""
							membersPage.apiClient.removeMember(tenantId, memberActionMenu.targetUserId)
						}
					}
				)
			}
		}

		MenuItem {
			text: qsTr("Transfer Ownership")
			visible: memberActionMenu.showTransfer
			height: visible ? implicitHeight : 0
			onTriggered: {
				membersPage.confirmTransferOwnership()
			}
		}

		MenuItem {
			text: qsTr("Leave Workspace")
			visible: memberActionMenu.showLeave
			height: visible ? implicitHeight : 0
			onTriggered: {
				if (membersPage.apiClient) {
					var ltid = membersPage.model ? membersPage.model.m_id : ""
					membersPage.apiClient.removeMember(ltid, memberActionMenu.targetUserId)
				}
			}
		}
	}

	Menu {
		id: inviteActionMenu
		property var menuItems: []
		property string targetInvitationId: ""
		property string targetUserName: ""

		Instantiator {
			model: inviteActionMenu.menuItems
			delegate: MenuItem {
				text: modelData.text || ""
				onTriggered: {
					var action = modelData.action
					if (action === "resend" && membersPage.apiClient)
						membersPage.apiClient.resendInvitation(inviteActionMenu.targetInvitationId)
					else if (action === "revoke" && membersPage.apiClient)
						membersPage.apiClient.revokeInvitation(inviteActionMenu.targetInvitationId)
				}
			}
			onObjectAdded: inviteActionMenu.insertItem(index, object)
			onObjectRemoved: inviteActionMenu.removeItem(object)
		}
	}

	// --- Invite-user popup ---
	Component {
		id: membersSelectPopupComp

		FilterableSelectPopup {
			id: invitePopup
			dataProvider: membersPage.apiClient ? membersPage.apiClient.invitableUsersListDataProvider : null

			itemWidth: 280
			showCheckBox: true
			showSelectedGroup: true
			filterPlaceholder: qsTr("Type or choose a user to invite")

			onSelectionChanged: {
				var arr = []
				for (var i = 0; i < selectedIds.length; i++) {
					var selId = selectedIds[i]
					var selName = dataProvider ? dataProvider.getSelectedItemText(selId) : ""
					if (!selName)
						selName = selId
					arr.push({id: selId, name: selName})
				}
				if (membersPage.stateManager)
					membersPage.stateManager.inviteSelectedUsers(arr)
			}

			Component.onDestruction: {
				if (membersPage.stateManager && membersPage.stateManager.__membersModifiedLocally) {
					membersPage.doUpdateModel()
					membersPage.stateManager.__membersModifiedLocally = false
				}
			}
		}
	}
}
