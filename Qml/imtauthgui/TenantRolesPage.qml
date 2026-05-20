// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import QtQuick.Controls
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantRolesPage
 *
 * Roles tab — list / create / edit / delete tenant roles via the abstract apiClient.
 * The orchestrator provides a factory for role data objects since this page can't
 * import the SDL module.
 */
Item {
	id: rolesPage

	property var tenantData: null
	property var stateManager: null
	property var apiClient: null

	// Factory injected by the orchestrator that returns a RoleData instance.
	property var roleDataFactory: null

	property string __editRoleId: ""
	property string __editRoleName: ""
	property string __editRoleDescription: ""

	readonly property bool __canManage: rolesPage.stateManager ? rolesPage.stateManager.canManageMembers : false

	Connections {
		target: rolesPage.apiClient
		function onRoleDataReceived(data) {
			if (rolesPage.stateManager)
				rolesPage.stateManager.receivedRoleData = data
		}
	}

	StackViewHeader {
		id: rolesStackViewHeader
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: rolesCreateBtn.visible ? rolesCreateBtn.left : parent.right
		height: Style.controlHeightL
		initialItemTitleVisible: true

		onCloseClicked: {
			rolesStackView.previous()
			rolesStackViewHeader.popHeader()
		}

		Component.onCompleted: {
			rolesStackViewHeader.addHeader("roles_list", qsTr("Roles"))
		}
	}

	Text {
		id: rolesCreateBtn
		visible: rolesPage.__canManage && rolesStackView.currentIndex === 0
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		anchors.verticalCenter: rolesStackViewHeader.verticalCenter
		text: "+ " + qsTr("Create Role")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: Style.linkColor

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: Qt.PointingHandCursor
			onClicked: {
				while (rolesStackView.count > 1)
					rolesStackView.removePage(rolesStackView.count - 1)
				rolesStackViewHeader.addHeader("create_role", qsTr("Create New Role"))
				rolesStackView.addPage(roleEditorView)
				rolesStackView.next()
			}
		}
	}

	BaseText {
		id: rolesDescription
		anchors.top: rolesStackViewHeader.bottom
		anchors.topMargin: Style.marginS
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		visible: rolesStackView.currentIndex === 0
		text: qsTr("Manage tenant roles and assign permissions to team members.")
		font.pixelSize: Style.fontSizeS
		color: Style.inactiveTextColor
	}

	StackView {
		id: rolesStackView
		anchors.top: rolesDescription.visible ? rolesDescription.bottom : rolesStackViewHeader.bottom
		anchors.topMargin: Style.marginS
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		Component.onCompleted: {
			rolesStackView.addPage(rolesListView)
		}
	}

	Component {
		id: rolesListView

		Item {
			TenantCollectionListView {
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				collectionId: "Roles"
				filterPlaceholder: qsTr("Filter roles...")
				emptyMessage: qsTr("No roles created yet.")
				canManage: rolesPage.__canManage

				onEditRequested: {
					rolesPage.__editRoleId = itemId
					rolesPage.__editRoleName = itemName
					rolesPage.__editRoleDescription = itemDescription
					while (rolesStackView.count > 1)
						rolesStackView.removePage(rolesStackView.count - 1)
					rolesStackViewHeader.addHeader("edit_role", itemName || qsTr("Edit Role"))
					rolesStackView.addPage(roleEditView)
					rolesStackView.next()
					if (rolesPage.apiClient)
						rolesPage.apiClient.getRoleData(itemId)
				}

				onDeleteRequested: {
					ModalDialogManager.showConfirmationDialog(
						qsTr("Delete Role"),
						qsTr("Are you sure you want to delete the role \"%1\"? This action cannot be undone.").arg(itemName),
						function(result) {
							if (result === true && rolesPage.apiClient)
								rolesPage.apiClient.removeRole(itemId)
						}
					)
				}
			}
		}
	}

	Component {
		id: roleEditorView

		Item {
			RoleView {
				id: createRoleView
				anchors.fill: parent
				commandsPanelVisible: false

				Component.onCompleted: {
					createRoleView.model = rolesPage.roleDataFactory ? rolesPage.roleDataFactory() : null
					createRoleView.updateGui()
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
						createRoleView.updateModel()
						var roleData = createRoleView.model
						if (rolesPage.apiClient)
							rolesPage.apiClient.insertRole(
								roleData ? roleData.m_name : "",
								roleData ? roleData.m_description : "")
						rolesStackViewHeader.popHeader()
						rolesStackView.previous()
					}
				}

				Button {
					text: qsTr("Cancel")
					onClicked: {
						rolesStackViewHeader.popHeader()
						rolesStackView.previous()
					}
				}
			}
		}
	}

	Component {
		id: roleEditView

		Item {
			RoleView {
				id: editRoleView
				anchors.fill: parent
				commandsPanelVisible: false

				Component.onCompleted: {
					var roleData = rolesPage.roleDataFactory ? rolesPage.roleDataFactory() : null
					if (roleData) {
						roleData.m_id = rolesPage.__editRoleId
						roleData.m_name = rolesPage.__editRoleName
						roleData.m_description = rolesPage.__editRoleDescription
					}
					editRoleView.model = roleData
					editRoleView.updateGui()
				}

				Connections {
					target: rolesPage.stateManager
					function onReceivedRoleDataChanged() {
						if (rolesPage.stateManager
								&& rolesPage.stateManager.receivedRoleData
								&& rolesPage.__editRoleId) {
							var roleData = rolesPage.roleDataFactory ? rolesPage.roleDataFactory() : null
							if (roleData) {
								roleData.m_id = rolesPage.__editRoleId
								roleData.m_name = rolesPage.stateManager.receivedRoleData.name || rolesPage.__editRoleName
								roleData.m_description = rolesPage.stateManager.receivedRoleData.description || rolesPage.__editRoleDescription
							}
							editRoleView.model = roleData
							editRoleView.updateGui()
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
						editRoleView.updateModel()
						var roleData = editRoleView.model
						if (rolesPage.apiClient)
							rolesPage.apiClient.setRoleData(
								rolesPage.__editRoleId,
								roleData ? roleData.m_name : "",
								roleData ? roleData.m_description : "")
						rolesStackViewHeader.popHeader()
						rolesStackView.previous()
					}
				}

				Button {
					text: qsTr("Cancel")
					onClicked: {
						rolesStackViewHeader.popHeader()
						rolesStackView.previous()
					}
				}
			}
		}
	}
}
