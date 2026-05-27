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
		var filterText = ""  // filter is applied via refresh
		var creatorId = (membersPage.model && membersPage.model.m_creatorId) ? membersPage.model.m_creatorId : ""

		var invitations = membersPage.stateManager ? membersPage.stateManager.pendingInvitations : []
		for (var j = 0; j < invitations.length; j++) {
			result.push({
				id: "inv_" + invitations[j].id,
				title: invitations[j].name || invitations[j].id || "",
				description: qsTr("Invited"),
				kind: "invitation",
				sourceData: invitations[j]
			})
		}

		var members = membersPage.stateManager ? membersPage.stateManager.pendingMembers : []
		for (var i = 0; i < members.length; i++) {
			// Creator is already filtered on the server side; this is a
			// client-side safety check in case old data is cached.
			if (creatorId && members[i].id === creatorId && members[i].role === "Creator")
				continue
			result.push({
				id: members[i].id,
				title: members[i].name || members[i].id || "",
				description: members[i].role || "Member",
				kind: "member",
				sourceData: members[i]
			})
		}

		membersPage.__combinedModel = result
	}

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
			spacing: Style.marginL

			property var pg: membersPage

			Text {
				text: qsTr("Exclude")
				font.pixelSize: Style.fontSizeM
				font.bold: true
				color: pg.selectionManager && pg.selectionManager.selectedIds.length > 0 ? Style.errorColor : Style.inactiveTextColor
				opacity: pg.selectionManager && pg.selectionManager.selectedIds.length > 0 ? 1.0 : 0.5

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: pg.selectionManager && pg.selectionManager.selectedIds.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
					enabled: pg.selectionManager && pg.selectionManager.selectedIds.length > 0
					onClicked: {
						var count = pg.selectionManager.selectedIds.length
						ModalDialogManager.showConfirmationDialog(
									qsTr("Exclude Members"),
									qsTr("Are you sure you want to exclude %1 selected member(s) from this tenant?").arg(count),
									function(result) {
										if (result === Enums.yes) {
											var ids = pg.selectionManager.selectedIds.slice()
											for (var i = 0; i < ids.length; i++)
												pg.removeItem(ids[i])
											pg.selectionManager.clear()
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
				color: pg.selectionManager && pg.selectionManager.selectedIds.length === 1 && pg.selectionManager.selectedIds[0].indexOf("inv_") !== 0 ? Style.linkColor : Style.inactiveTextColor
				opacity: pg.selectionManager && pg.selectionManager.selectedIds.length === 1 && pg.selectionManager.selectedIds[0].indexOf("inv_") !== 0 ? 1.0 : 0.5

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: pg.selectionManager && pg.selectionManager.selectedIds.length === 1 && pg.selectionManager.selectedIds[0].indexOf("inv_") !== 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
					enabled: pg.selectionManager && pg.selectionManager.selectedIds.length === 1 && pg.selectionManager.selectedIds[0].indexOf("inv_") !== 0
					onClicked: {
						var selId = pg.selectionManager.selectedIds[0]
						var items = pg.__combinedModel
						for (var i = 0; i < items.length; i++) {
							if (items[i].id === selId) {
								pg.openEdit(selId, items[i].title, items[i].description)
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
					onClicked: pg.openCreate()
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
			width: ListView.view ? ListView.view.width : 0
			kind: modelData.kind || "member"
			memberData: modelData.sourceData || modelData
			tenantData: membersPage.model
			stateManager: membersPage.stateManager
			canManageMembers: membersPage.stateManager ? membersPage.stateManager.canManageMembers : false
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
				memberActionMenu.popup()
			}

			onMemberEditRequested: {
				if (membersPage.stateManager && membersPage.stateManager.canManageMembers)
					membersPage.openEdit(userId, userName, "")
			}

			onInviteActionsRequested: {
				inviteActionMenu.menuItems = menuItems
				inviteActionMenu.targetInvitationId = invitationId
				inviteActionMenu.targetUserName = userName
				inviteActionMenu.popup()
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

		Menu {
			id: changeRoleSubmenu
			title: qsTr("Change Environment Role")
			visible: memberActionMenu.showChangeRole
			height: visible ? implicitHeight : 0

			MenuItem {
				text: qsTr("Member")
				checkable: true
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
				checkable: true
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
			dataProvider: invitableUsersLoader.item

			Loader {
				id: invitableUsersLoader
				sourceComponent: membersPage.apiClient ? membersPage.apiClient.invitableUsersListDataProviderComp : null
			}

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
