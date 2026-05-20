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
 * Members tab of the TenantEditor — displays tenant users via TenantCollectionListView
 * with StackView navigation. Opens UserView for create/edit.
 * Also provides Create Invitation to invite external users not yet in the tenant.
 *
 * Users created within the tenant automatically become members with "Member" role.
 *
 * Inherits ViewBase so model updates are protected by doUpdateGui / doUpdateModel.
 */
ViewBase {
	id: membersPage

	commandsPanelVisible: false
	contentColor: Style.baseColor
	readonly property var tenantData: membersPage.model
	property var stateManager: null
	property var apiClient: null

	// Factory injected by the orchestrator that returns a UserData instance.
	property var userDataFactory: null

	function updateGui() {
		// UI is bound directly to stateManager / TenantCollectionListView.
	}

	function updateModel() {
		// Members are synced from the state manager in the orchestrator.
	}

	property string __editUserId: ""
	property string __editUserName: ""
	property string __editUserDescription: ""

	readonly property bool __canManage: membersPage.stateManager ? membersPage.stateManager.canManageMembers : false

	Connections {
		target: membersPage.apiClient
		function onUserDataReceived(data) {
			if (membersPage.stateManager)
				membersPage.stateManager.receivedUserData = data
		}
		function onUserCreated() {
			// User created within our tenant -> auto-becomes member with role Member
			if (membersPage.stateManager && membersPage.tenantData) {
				membersPage.stateManager.loadMembersFromModel()
			}
		}
	}

	StackViewHeader {
		id: membersStackViewHeader
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: membersHeaderButtons.left
		height: Style.controlHeightL
		initialItemTitleVisible: true

		onCloseClicked: {
			membersStackView.previous()
			membersStackViewHeader.popHeader()
		}

		Component.onCompleted: {
			membersStackViewHeader.addHeader("members_list", qsTr("Members"))
		}
	}

	Row {
		id: membersHeaderButtons
		visible: membersPage.__canManage && membersStackView.currentIndex === 0
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		anchors.verticalCenter: membersStackViewHeader.verticalCenter
		spacing: Style.marginL

		Text {
			id: createUserBtn
			text: "+ " + qsTr("Create User")
			font.pixelSize: Style.fontSizeM
			font.bold: true
			color: Style.linkColor

			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				cursorShape: Qt.PointingHandCursor
				onClicked: {
					while (membersStackView.count > 1)
						membersStackView.removePage(membersStackView.count - 1)
					membersStackViewHeader.addHeader("create_user", qsTr("Create New User"))
					membersStackView.addPage(userEditorView)
					membersStackView.next()
				}
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

	BaseText {
		id: membersDescription
		anchors.top: membersStackViewHeader.bottom
		anchors.topMargin: Style.marginS
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		visible: membersStackView.currentIndex === 0
		text: qsTr("Manage tenant members. Users created here automatically become members.")
		font.pixelSize: Style.fontSizeS
		color: Style.inactiveTextColor
	}

	StackView {
		id: membersStackView
		anchors.top: membersDescription.visible ? membersDescription.bottom : membersStackViewHeader.bottom
		anchors.topMargin: Style.marginS
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		Component.onCompleted: {
			membersStackView.addPage(membersListView)
		}
	}

	Component {
		id: membersListView

		Item {
			TenantCollectionListView {
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				collectionId: "Users"
				filterPlaceholder: qsTr("Filter members...")
				emptyMessage: qsTr("No members found.")
				canManage: membersPage.__canManage

				onEditRequested: {
					membersPage.__editUserId = itemId
					membersPage.__editUserName = itemName
					membersPage.__editUserDescription = itemDescription
					while (membersStackView.count > 1)
						membersStackView.removePage(membersStackView.count - 1)
					membersStackViewHeader.addHeader("edit_user", itemName || qsTr("Edit User"))
					membersStackView.addPage(userEditView)
					membersStackView.next()
					if (membersPage.apiClient)
						membersPage.apiClient.getUserData(itemId)
				}

				onDeleteRequested: {
					ModalDialogManager.showConfirmationDialog(
						qsTr("Remove Member"),
						qsTr("Are you sure you want to remove \"%1\"? They will lose access to this workspace.").arg(itemName),
						function(result) {
							if (result === true && membersPage.apiClient)
								membersPage.apiClient.removeUser(itemId)
						}
					)
				}
			}
		}
	}

	Component {
		id: userEditorView

		Item {
			UserView {
				id: createUserView
				anchors.fill: parent
				commandsPanelVisible: false

				Component.onCompleted: {
					createUserView.model = membersPage.userDataFactory ? membersPage.userDataFactory() : null
					createUserView.updateGui()
				}
			}

			Row {
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				spacing: Style.marginM

				Button {
					text: qsTr("Create")
					onClicked: {
						createUserView.updateModel()
						var userData = createUserView.model
						if (membersPage.apiClient)
							membersPage.apiClient.insertUser(
								userData ? userData.m_name : "",
								userData ? userData.m_description : "")
						membersStackViewHeader.popHeader()
						membersStackView.previous()
					}
				}

				Button {
					text: qsTr("Cancel")
					onClicked: {
						membersStackViewHeader.popHeader()
						membersStackView.previous()
					}
				}
			}
		}
	}

	Component {
		id: userEditView

		Item {
			UserView {
				id: editUserView
				anchors.fill: parent
				commandsPanelVisible: false

				Component.onCompleted: {
					var userData = membersPage.userDataFactory ? membersPage.userDataFactory() : null
					if (userData) {
						userData.m_id = membersPage.__editUserId
						userData.m_name = membersPage.__editUserName
						userData.m_description = membersPage.__editUserDescription
					}
					editUserView.model = userData
					editUserView.updateGui()
				}

				Connections {
					target: membersPage.stateManager
					function onReceivedUserDataChanged() {
						if (membersPage.stateManager
								&& membersPage.stateManager.receivedUserData
								&& membersPage.__editUserId) {
							var userData = membersPage.userDataFactory ? membersPage.userDataFactory() : null
							if (userData) {
								userData.m_id = membersPage.__editUserId
								userData.m_name = membersPage.stateManager.receivedUserData.name || membersPage.__editUserName
								userData.m_description = membersPage.stateManager.receivedUserData.description || membersPage.__editUserDescription
							}
							editUserView.model = userData
							editUserView.updateGui()
						}
					}
				}
			}

			Row {
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				spacing: Style.marginM

				Button {
					text: qsTr("Save")
					onClicked: {
						editUserView.updateModel()
						var userData = editUserView.model
						if (membersPage.apiClient)
							membersPage.apiClient.setUserData(
								membersPage.__editUserId,
								userData ? userData.m_name : "",
								userData ? userData.m_description : "")
						membersStackViewHeader.popHeader()
						membersStackView.previous()
					}
				}

				Button {
					text: qsTr("Cancel")
					onClicked: {
						membersStackViewHeader.popHeader()
						membersStackView.previous()
					}
				}
			}
		}
	}

	// ===== Invite-user popup (invites external users not in this tenant) =====
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
