// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import QtQuick.Controls
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantGroupsPage
 *
 * Groups tab — list / create / edit / delete tenant groups via the abstract apiClient.
 *
 * Inherits ViewBase so its `model` (tenantData) is wired in from the orchestrator
 * and updates can flow through the protected doUpdateGui / doUpdateModel wrappers.
 */
ViewBase {
	id: groupsPage

	commandsPanelVisible: false
	contentColor: Style.baseColor
	readonly property var tenantData: groupsPage.model
	property var stateManager: null
	property var apiClient: null

	property var groupDataFactory: null

	function updateGui() {
		// Groups list/edit UI is bound to apiClient/stateManager directly.
	}

	function updateModel() {
		// Groups mutations are pushed via apiClient; nothing to write back here.
	}

	property string __editGroupId: ""
	property string __editGroupName: ""
	property string __editGroupDescription: ""

	readonly property bool __canManage: groupsPage.stateManager ? groupsPage.stateManager.canManageMembers : false

	Connections {
		target: groupsPage.apiClient
		function onGroupDataReceived(data) {
			if (groupsPage.stateManager)
				groupsPage.stateManager.receivedGroupData = data
		}
	}

	StackViewHeader {
		id: groupsStackViewHeader
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: groupsCreateBtn.visible ? groupsCreateBtn.left : parent.right
		height: Style.controlHeightL
		initialItemTitleVisible: true

		onCloseClicked: {
			groupsStackView.previous()
			groupsStackViewHeader.popHeader()
		}

		Component.onCompleted: {
			groupsStackViewHeader.addHeader("groups_list", qsTr("Groups"))
		}
	}

	Text {
		id: groupsCreateBtn
		visible: groupsPage.__canManage && groupsStackView.currentIndex === 0
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		anchors.verticalCenter: groupsStackViewHeader.verticalCenter
		text: "+ " + qsTr("Create Group")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: Style.linkColor

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: Qt.PointingHandCursor
			onClicked: {
				while (groupsStackView.count > 1)
					groupsStackView.removePage(groupsStackView.count - 1)
				groupsStackViewHeader.addHeader("create_group", qsTr("Create New Group"))
				groupsStackView.addPage(groupEditorView)
				groupsStackView.next()
			}
		}
	}

	BaseText {
		id: groupsDescription
		anchors.top: groupsStackViewHeader.bottom
		anchors.topMargin: Style.marginS
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		visible: groupsStackView.currentIndex === 0
		text: qsTr("Organize members into groups for easier permission management.")
		font.pixelSize: Style.fontSizeS
		color: Style.inactiveTextColor
	}

	StackView {
		id: groupsStackView
		anchors.top: groupsDescription.visible ? groupsDescription.bottom : groupsStackViewHeader.bottom
		anchors.topMargin: Style.marginS
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		Component.onCompleted: {
			groupsStackView.addPage(groupsListView)
		}
	}

	Component {
		id: groupsListView

		Item {
			TenantCollectionListView {
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				collectionId: "Groups"
				filterPlaceholder: qsTr("Filter groups...")
				emptyMessage: qsTr("No groups created yet.")
				canManage: groupsPage.__canManage

				onEditRequested: {
					groupsPage.__editGroupId = itemId
					groupsPage.__editGroupName = itemName
					groupsPage.__editGroupDescription = itemDescription
					while (groupsStackView.count > 1)
						groupsStackView.removePage(groupsStackView.count - 1)
					groupsStackViewHeader.addHeader("edit_group", itemName || qsTr("Edit Group"))
					groupsStackView.addPage(groupEditView)
					groupsStackView.next()
					if (groupsPage.apiClient)
						groupsPage.apiClient.getGroupData(itemId)
				}

				onDeleteRequested: {
					ModalDialogManager.showConfirmationDialog(
						qsTr("Delete Group"),
						qsTr("Are you sure you want to delete the group \"%1\"? This action cannot be undone.").arg(itemName),
						function(result) {
							if (result === true && groupsPage.apiClient)
								groupsPage.apiClient.removeGroup(itemId)
						}
					)
				}
			}
		}
	}

	Component {
		id: groupEditorView

		Item {
			UserGroupView {
				id: createGroupView
				anchors.fill: parent
				commandsPanelVisible: false

				Component.onCompleted: {
					createGroupView.model = groupsPage.groupDataFactory ? groupsPage.groupDataFactory() : null
					createGroupView.updateGui()
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
						createGroupView.updateModel()
						var groupData = createGroupView.model
						if (groupsPage.apiClient)
							groupsPage.apiClient.insertGroup(
								groupData ? groupData.m_name : "",
								groupData ? groupData.m_description : "")
						groupsStackViewHeader.popHeader()
						groupsStackView.previous()
					}
				}

				Button {
					text: qsTr("Cancel")
					onClicked: {
						groupsStackViewHeader.popHeader()
						groupsStackView.previous()
					}
				}
			}
		}
	}

	Component {
		id: groupEditView

		Item {
			UserGroupView {
				id: editGroupView
				anchors.fill: parent
				commandsPanelVisible: false

				Component.onCompleted: {
					var groupData = groupsPage.groupDataFactory ? groupsPage.groupDataFactory() : null
					if (groupData) {
						groupData.m_id = groupsPage.__editGroupId
						groupData.m_name = groupsPage.__editGroupName
						groupData.m_description = groupsPage.__editGroupDescription
					}
					editGroupView.model = groupData
					editGroupView.updateGui()
				}

				Connections {
					target: groupsPage.stateManager
					function onReceivedGroupDataChanged() {
						if (groupsPage.stateManager
								&& groupsPage.stateManager.receivedGroupData
								&& groupsPage.__editGroupId) {
							var groupData = groupsPage.groupDataFactory ? groupsPage.groupDataFactory() : null
							if (groupData) {
								groupData.m_id = groupsPage.__editGroupId
								groupData.m_name = groupsPage.stateManager.receivedGroupData.name || groupsPage.__editGroupName
								groupData.m_description = groupsPage.stateManager.receivedGroupData.description || groupsPage.__editGroupDescription
							}
							editGroupView.model = groupData
							editGroupView.updateGui()
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
						editGroupView.updateModel()
						var groupData = editGroupView.model
						if (groupsPage.apiClient)
							groupsPage.apiClient.setGroupData(
								groupsPage.__editGroupId,
								groupData ? groupData.m_name : "",
								groupData ? groupData.m_description : "")
						groupsStackViewHeader.popHeader()
						groupsStackView.previous()
					}
				}

				Button {
					text: qsTr("Cancel")
					onClicked: {
						groupsStackViewHeader.popHeader()
						groupsStackView.previous()
					}
				}
			}
		}
	}
}
