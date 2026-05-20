// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import QtQuick.Controls
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0

/**
 * TenantMembersPage
 *
 * Members tab of the TenantEditor — lists active members and pending invitations,
 * and exposes invite / leave / actions menus.
 *
 * Talks to the server exclusively through `apiClient` (abstract contract).
 * Holds no SDL imports. Inherits ViewBase so model updates are protected by
 * doUpdateGui / doUpdateModel.
 */
ViewBase {
	id: membersPage

	commandsPanelVisible: false
	contentColor: Style.baseColor
	readonly property var tenantData: membersPage.model
	property var stateManager: null
	property var apiClient: null

	function updateGui() {
		// UI is bound directly to stateManager.pendingMembers / pendingInvitations.
	}

	function updateModel() {
		// Members are synced from the state manager in the orchestrator.
	}

	// --- Confirmation state ---
	property string __confirmRemoveUserId: ""
	property string __confirmRemoveUserName: ""
	property string __confirmTransferUserId: ""
	property string __confirmTransferUserName: ""
	property string __confirmRevokeInvitationId: ""
	property string __confirmRevokeUserName: ""

	// --- Pending menu target ---
	property string __pendingMenuUserId: ""
	property string __pendingMenuUserName: ""
	property bool __pendingMenuIsOwner: false
	property bool __pendingMenuIsCurrentUser: false

	function __removeMemberById(userId) {
		if (!membersPage.stateManager) return
		membersPage.stateManager.removeMemberById(userId)
		membersPage.doUpdateModel()
	}

	function __transferOwnershipTo(newOwnerId) {
		if (!membersPage.apiClient || !membersPage.tenantData) return
		membersPage.apiClient.transferOwnership(membersPage.tenantData.m_id || "", newOwnerId)
	}

	function __showActionsMenu(menuItems, userId, userName, isOwnerTarget, isCurrentUserTarget) {
		if (menuItems.length === 0)
			return
		if (menuItems.length === 1) {
			membersPage.__executeAction(menuItems[0].action, userId, userName)
			return
		}
		membersPage.__pendingMenuUserId = userId
		membersPage.__pendingMenuUserName = userName
		membersPage.__pendingMenuIsOwner = isOwnerTarget
		membersPage.__pendingMenuIsCurrentUser = isCurrentUserTarget
		memberActionsMenu.popup()
	}

	function __showInviteActionsMenu(menuItems, invitationId, userName) {
		if (menuItems.length === 1) {
			membersPage.__executeInviteAction(menuItems[0].action, invitationId, userName)
			return
		}
		membersPage.__pendingMenuUserId = invitationId
		membersPage.__pendingMenuUserName = userName
		inviteActionsMenu.popup()
	}

	function __executeAction(action, userId, userName) {
		if (action === "remove") {
			membersPage.__confirmRemoveUserId = userId
			membersPage.__confirmRemoveUserName = userName
			ModalDialogManager.showConfirmationDialog(
				qsTr("Remove Member"),
				qsTr("Are you sure you want to remove %1? They will lose access to this workspace.").arg(userName),
				function(result) {
					if (result === true)
						membersPage.__removeMemberById(membersPage.__confirmRemoveUserId)
				}
			)
		} else if (action === "changeRole") {
			membersPage.__pendingMenuUserId = userId
			membersPage.__pendingMenuUserName = userName
			roleAssignMenu.popup()
		} else if (action === "transfer") {
			membersPage.__confirmTransferUserId = userId
			membersPage.__confirmTransferUserName = userName
			ModalDialogManager.showConfirmationDialog(
				qsTr("Transfer Ownership"),
				qsTr("Are you sure you want to transfer ownership to %1? You will become a regular member and %1 will become the new owner.").arg(userName),
				function(result) {
					if (result === true)
						membersPage.__transferOwnershipTo(membersPage.__confirmTransferUserId)
				}
			)
		} else if (action === "leave") {
			ModalDialogManager.showConfirmationDialog(
				qsTr("Leave Workspace"),
				qsTr("Are you sure you want to leave this workspace? You will lose access to all workspace resources."),
				function(result) {
					if (result === true && membersPage.tenantData)
						membersPage.__removeMemberById(membersPage.tenantData.m_currentUserId)
				}
			)
		}
	}

	function __executeInviteAction(action, invitationId, userName) {
		if (!membersPage.apiClient)
			return
		if (action === "resend") {
			membersPage.apiClient.resendInvitation(invitationId)
		} else if (action === "revoke") {
			membersPage.__confirmRevokeInvitationId = invitationId
			membersPage.__confirmRevokeUserName = userName
			ModalDialogManager.showConfirmationDialog(
				qsTr("Revoke Invitation"),
				qsTr("Are you sure you want to revoke the invitation for %1? They will no longer be able to join this workspace.").arg(userName),
				function(result) {
					if (result === true) {
						membersPage.apiClient.revokeInvitation(membersPage.__confirmRevokeInvitationId)
						if (membersPage.stateManager)
							membersPage.stateManager.removePendingInvitation(membersPage.__confirmRevokeInvitationId)
					}
				}
			)
		}
	}

	CustomScrollbar {
		id: membersScrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: membersFlickable.top
		anchors.bottom: membersFlickable.bottom
		secondSize: Style.marginM
		targetItem: membersFlickable
	}

	Flickable {
		id: membersFlickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: membersScrollbar.left
		anchors.rightMargin: Style.marginXL
		contentWidth: membersColumn.width
		contentHeight: membersColumn.height + 2 * Style.marginXL

		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: membersColumn
			width: Style.sizeHintXXL
			spacing: Style.marginXL

			// ===== Header =====
			Row {
				id: membersHeader
				width: parent.width
				spacing: Style.marginM

				Column {
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginXS

					BaseText {
						text: qsTr("Members")
						font.pixelSize: Style.fontSizeXL
						font.bold: true
						color: Style.textColor
					}

					BaseText {
						text: {
							var members = membersPage.stateManager ? membersPage.stateManager.pendingMembers : []
							var invs = membersPage.stateManager ? membersPage.stateManager.pendingInvitations : []
							return invs.length > 0
								? qsTr("%1 member(s) · %2 pending invite(s)").arg(members.length).arg(invs.length)
								: qsTr("%1 member(s)").arg(members.length)
						}
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}
				}

				Item {
					width: parent.width
						- parent.children[0].width
						- (inviteMemberBtn.visible ? inviteMemberBtn.width : 0)
						- (leaveWorkspaceBtn.visible ? leaveWorkspaceBtn.width : 0)
						- parent.spacing * 2
					height: 1
				}

				Text {
					id: inviteMemberBtn
					visible: membersPage.stateManager ? membersPage.stateManager.canManageMembers : false
					anchors.verticalCenter: parent.verticalCenter
					text: "+ " + qsTr("Create invitation")
					font.pixelSize: Style.fontSizeM
					font.bold: true
					color: Style.linkColor

					MouseArea {
						anchors.fill: parent
						hoverEnabled: true
						cursorShape: Qt.PointingHandCursor
						onClicked: {
							var ids = []
							var members = membersPage.stateManager ? membersPage.stateManager.pendingMembers : []
							for (var i = 0; i < members.length; i++)
								ids.push(members[i].id)
							var point = inviteMemberBtn.mapToItem(null, 0, inviteMemberBtn.height)
							ModalDialogManager.openDialog(membersSelectPopupComp, {
								"x": point.x,
								"y": point.y,
								"preselectedIds": ids
							})
						}
					}
				}

				Button {
					id: leaveWorkspaceBtn
					visible: membersPage.stateManager
						&& !membersPage.stateManager.isOwner
						&& !membersPage.stateManager.isCreator
						&& membersPage.tenantData
						&& membersPage.tenantData.m_currentUserId
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Leave Workspace")
					tooltipText: qsTr("Leave this workspace")
					onClicked: {
						ModalDialogManager.showConfirmationDialog(
							qsTr("Leave Workspace"),
							qsTr("Are you sure you want to leave this workspace? You will lose access to all workspace resources."),
							function(result) {
								if (result === true)
									membersPage.__removeMemberById(membersPage.tenantData.m_currentUserId)
							}
						)
					}
				}
			}

			// ===== Active Members =====
			Column {
				id: activeMembersSection
				width: parent.width
				spacing: Style.marginS

				BaseText {
					text: qsTr("Active Members")
					font.pixelSize: Style.fontSizeL
					font.bold: true
					color: Style.textColor
				}

				BaseText {
					visible: !membersPage.stateManager || membersPage.stateManager.pendingMembers.length === 0
					text: qsTr("No members found")
					font.pixelSize: Style.fontSizeM
					color: Style.inactiveTextColor
					topPadding: Style.marginM
					bottomPadding: Style.marginM
				}

				Repeater {
					model: membersPage.stateManager ? membersPage.stateManager.pendingMembers : []

					delegate: TenantMemberDelegate {
						width: activeMembersSection.width
						kind: "member"
						memberData: modelData
						tenantData: membersPage.tenantData
						stateManager: membersPage.stateManager
						canManageMembers: membersPage.stateManager ? membersPage.stateManager.canManageMembers : false
						isOwner: membersPage.stateManager ? membersPage.stateManager.isOwner : false

						onMemberActionsRequested: membersPage.__showActionsMenu(menuItems, userId, userName, isOwnerTarget, isCurrentUserTarget)
					}
				}
			}

			// ===== Separator =====
			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
				visible: membersPage.stateManager
					&& (membersPage.stateManager.canManageMembers
						|| (membersPage.stateManager.pendingInvitations
							&& membersPage.stateManager.pendingInvitations.length > 0))
			}

			// ===== Pending Invitations =====
			Column {
				id: pendingInvitationsSection
				width: parent.width
				spacing: Style.marginS
				visible: membersPage.stateManager
					&& (membersPage.stateManager.canManageMembers
						|| (membersPage.stateManager.pendingInvitations
							&& membersPage.stateManager.pendingInvitations.length > 0))

				BaseText {
					text: qsTr("Pending Invitations")
					font.pixelSize: Style.fontSizeL
					font.bold: true
					color: Style.textColor
				}

				BaseText {
					visible: !membersPage.stateManager
						|| !membersPage.stateManager.pendingInvitations
						|| membersPage.stateManager.pendingInvitations.length === 0
					text: qsTr("No pending invitations")
					font.pixelSize: Style.fontSizeM
					color: Style.inactiveTextColor
					topPadding: Style.marginM
					bottomPadding: Style.marginM
				}

				Repeater {
					model: membersPage.stateManager ? membersPage.stateManager.pendingInvitations : []

					delegate: TenantMemberDelegate {
						width: pendingInvitationsSection.width
						kind: "invitation"
						memberData: modelData
						tenantData: membersPage.tenantData
						stateManager: membersPage.stateManager
						canManageMembers: membersPage.stateManager ? membersPage.stateManager.canManageMembers : false
						isOwner: membersPage.stateManager ? membersPage.stateManager.isOwner : false

						onInviteActionsRequested: membersPage.__showInviteActionsMenu(menuItems, invitationId, userName)
					}
				}
			}
		}
	}

	// ===== Menus =====
	Menu {
		id: memberActionsMenu
		font.pixelSize: Style.fontSizeM
		font.family: Style.fontFamily

		MenuItem {
			text: qsTr("Change Environment Role")
			visible: membersPage.stateManager
				&& membersPage.stateManager.canManageMembers
				&& !membersPage.__pendingMenuIsOwner
			height: visible ? implicitHeight : 0
			onTriggered: roleAssignMenu.popup()
		}
		MenuItem {
			text: qsTr("Remove Member")
			visible: membersPage.stateManager
				&& membersPage.stateManager.canManageMembers
				&& !membersPage.__pendingMenuIsOwner
			height: visible ? implicitHeight : 0
			onTriggered: membersPage.__executeAction("remove", membersPage.__pendingMenuUserId, membersPage.__pendingMenuUserName)
		}
		MenuItem {
			text: qsTr("Transfer Ownership")
			visible: membersPage.stateManager && membersPage.stateManager.isOwner
			height: visible ? implicitHeight : 0
			onTriggered: membersPage.__executeAction("transfer", membersPage.__pendingMenuUserId, membersPage.__pendingMenuUserName)
		}
		MenuItem {
			text: qsTr("Leave Workspace")
			visible: membersPage.stateManager
				&& !membersPage.stateManager.canManageMembers
				&& membersPage.__pendingMenuIsCurrentUser
			height: visible ? implicitHeight : 0
			onTriggered: membersPage.__executeAction("leave", membersPage.__pendingMenuUserId, membersPage.__pendingMenuUserName)
		}
	}

	Menu {
		id: roleAssignMenu
		title: qsTr("Assign Environment Role")
		font.pixelSize: Style.fontSizeM
		font.family: Style.fontFamily

		MenuItem {
			text: qsTr("Admin")
			onTriggered: {
				if (membersPage.apiClient && membersPage.tenantData)
					membersPage.apiClient.setMemberRole(
						membersPage.tenantData.m_id || "",
						membersPage.__pendingMenuUserId,
						"Admin")
			}
		}
		MenuItem {
			text: qsTr("Member")
			onTriggered: {
				if (membersPage.apiClient && membersPage.tenantData)
					membersPage.apiClient.setMemberRole(
						membersPage.tenantData.m_id || "",
						membersPage.__pendingMenuUserId,
						"Member")
			}
		}

		MenuSeparator {}

		MenuItem {
			text: qsTr("Assign Global Role...")
			onTriggered: {
				// TODO: open global role picker for membersPage.__pendingMenuUserId
			}
		}
	}

	Menu {
		id: inviteActionsMenu
		font.pixelSize: Style.fontSizeM
		font.family: Style.fontFamily

		MenuItem {
			text: qsTr("Resend Invitation")
			onTriggered: membersPage.__executeInviteAction("resend", membersPage.__pendingMenuUserId, membersPage.__pendingMenuUserName)
		}
		MenuItem {
			text: qsTr("Revoke Invitation")
			onTriggered: membersPage.__executeInviteAction("revoke", membersPage.__pendingMenuUserId, membersPage.__pendingMenuUserName)
		}
	}

	// ===== Invite-user popup =====
	Component {
		id: membersSelectPopupComp

		FilterableSelectPopup {
			dataProvider: FilterableSelectGqlDataProvider {
				collectionId: "Users"
				multiSelect: true
			}

			itemWidth: 280
			showCheckBox: true
			showSelectedGroup: true
			filterPlaceholder: qsTr("Type or choose a user")

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
