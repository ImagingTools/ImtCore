// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import QtQuick.Controls
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantMembersPage
 *
 * Members tab — list / create / edit / exclude tenant members.
 * Extends TenantSimpleCollectionPage with custom header buttons,
 * a custom delegate (TenantMemberDelegate), and member-specific
 * commands (invite, exclude, role management).
 */
TenantSimpleCollectionPage {
	id: membersPage

	entityName: qsTr("User")
	entityNamePlural: qsTr("Members")
	descriptionText: qsTr("Manage tenant members. Users created here automatically become members.")

	documentManager: apiClient ? apiClient.userDocumentManager : null
	objectTypeId: apiClient ? apiClient.userObjectTypeId : ""

	// Members use stateManager-driven model, not a data provider
	listModel: __combinedModel

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
		for (var j = 0; j < invitations.length; j++) {
			var invName = invitations[j].userName || invitations[j].name || invitations[j].id || ""
			if (!matches(invName))
				continue
			result.push({
				id: "inv_" + invitations[j].id,
				title: invName,
				description: qsTr("Invited"),
				kind: "invitation",
				sourceData: invitations[j]
			})
		}

		var members = membersPage.stateManager ? membersPage.stateManager.pendingMembers : []
		for (var i = 0; i < members.length; i++) {
			var memName = members[i].name || members[i].id || ""
			if (!matches(memName))
				continue
			result.push({
				id: members[i].id,
				title: memName,
				description: members[i].role || "Member",
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
		function onMemberRoleChanged(userId, role) {
			if (membersPage.stateManager)
				membersPage.stateManager.setUserRole(userId, role)
		}
	}

	// --- Custom header buttons ---
	Component {
		id: memberHeaderButtonsComp

		Row {
			id: headerButtonsRow
			spacing: Style.marginL
			visible: membersPage.stateManager ? membersPage.stateManager.canManageMembers : false

			Text {
				text: qsTr("Exclude")
				font.pixelSize: Style.fontSizeM
				font.bold: true
				color: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length > 0 ? Style.errorColor : Style.inactiveTextColor
				opacity: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length > 0 ? 1.0 : 0.5

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
					enabled: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length > 0
					onClicked: {
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
				color: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length === 1 && membersPage.selectionManager.selectedIds[0].indexOf("inv_") !== 0 ? Style.linkColor : Style.inactiveTextColor
				opacity: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length === 1 && membersPage.selectionManager.selectedIds[0].indexOf("inv_") !== 0 ? 1.0 : 0.5

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length === 1 && membersPage.selectionManager.selectedIds[0].indexOf("inv_") !== 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
					enabled: membersPage.selectionManager && membersPage.selectionManager.selectedIds.length === 1 && membersPage.selectionManager.selectedIds[0].indexOf("inv_") !== 0
					onClicked: {
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
				color: Style.linkColor

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: membersPage.openCreate()
				}
			}

			Text {
				id: inviteMemberBtn
				text: "+ " + qsTr("Create Invitation")
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
			collectionPage: membersPage
			isOwner: membersPage.stateManager ? membersPage.stateManager.isOwner : false
			selectionManager: membersPage.selectionManager
			showCheckBox: true

			onMemberActionsRequested: {
				memberActionMenu.targetUserId = userId
				memberActionMenu.targetUserName = userName
				memberActionMenu.targetCurrentRole = membersPage.stateManager
					? membersPage.stateManager.getUserRole(userId) : "Member"
				memberActionMenu.showChangeRole = false
				memberActionMenu.showExclude = false
				memberActionMenu.showTransfer = false
				memberActionMenu.showLeave = false
				for (var i = 0; i < menuItems.length; i++) {
					if (menuItems[i].action === "changeRole") memberActionMenu.showChangeRole = true
					else if (menuItems[i].action === "remove") memberActionMenu.showExclude = true
					else if (menuItems[i].action === "transfer") memberActionMenu.showTransfer = true
					else if (menuItems[i].action === "leave") memberActionMenu.showLeave = true
				}
				var localPos = membersPage.mapFromItem(null, menuX, menuY)
				memberActionMenu.popup(localPos.x, localPos.y)
			}

			onMemberEditRequested: {
				if (membersPage.stateManager && membersPage.stateManager.canManageMembers)
					membersPage.openEdit(userId, userName, "")
			}

			onInviteActionsRequested: {
				inviteActionMenu.menuItems = menuItems
				inviteActionMenu.targetInvitationId = invitationId
				inviteActionMenu.targetUserName = userName
				var localPos = membersPage.mapFromItem(null, menuX, menuY)
				inviteActionMenu.popup(localPos.x, localPos.y)
			}
		}
	}

	// --- Menus ---
	Menu {
		id: memberActionMenu
		property string targetUserId: ""
		property string targetUserName: ""
		property string targetCurrentRole: ""
		property bool showChangeRole: false
		property bool showExclude: false
		property bool showTransfer: false
		property bool showLeave: false

		closePolicy: Enums.popupCloseOnEscape | Enums.popupCloseOnPressOutside

		Menu {
			id: changeRoleSubmenu
			title: qsTr("Change Role")
			visible: memberActionMenu.showChangeRole
			height: visible ? implicitHeight : 0

			MenuItem {
				text: qsTr("Member")
				checked: memberActionMenu.targetCurrentRole === "Member"
				onTriggered: {
					if (membersPage.apiClient) {
						var tenantId = membersPage.model ? membersPage.model.m_id : ""
						membersPage.apiClient.setMemberRole(tenantId, memberActionMenu.targetUserId, "Member")
					}
				}
			}
			MenuItem {
				text: qsTr("Admin")
				checked: memberActionMenu.targetCurrentRole === "Admin"
				onTriggered: {
					if (membersPage.apiClient) {
						var tenantId = membersPage.model ? membersPage.model.m_id : ""
						membersPage.apiClient.setMemberRole(tenantId, memberActionMenu.targetUserId, "Admin")
					}
				}
			}
		}

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
				if (membersPage.apiClient) {
					var tid = membersPage.model ? membersPage.model.m_id : ""
					membersPage.apiClient.transferOwnership(tid, memberActionMenu.targetUserId)
				}
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
