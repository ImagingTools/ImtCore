// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import QtQuick.Controls
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantRolesPage
 *
 * Roles tab — list / create / edit / delete tenant roles via the abstract apiClient.
 * Table is fixed at 800px width, centered, with gray rounded border and checkbox selection.
 */
ViewBase {
	id: rolesPage
	
	commandsPanelVisible: false
	contentColor: Style.baseColor
	readonly property var tenantData: rolesPage.model
	property var stateManager: null
	property var apiClient: null
	
	function updateGui() {}
	function updateModel() {}

	Component.onCompleted: {
		if (rolesPage.apiClient)
			rolesPage.apiClient.fetchPermissions()
	}
	
	property string __editRoleId: ""
	property bool __isCreating: false
	// Shell view of the currently visible single-document editor page (if any).
	property var __activeShellView: null
	
	readonly property bool __canManage: rolesPage.stateManager ? rolesPage.stateManager.canManageMembers : false
	readonly property string __productId: rolesPage.apiClient ? rolesPage.apiClient.tenantId : ""
	
	property var __selectionManager: null
	property var __dataProvider: null
	
	Connections {
		target: rolesPage.apiClient ? rolesPage.apiClient.roleDocumentManager : null
		function onDocumentSaved(documentId) {
			if (rolesPage.__dataProvider)
				rolesPage.__dataProvider.fetch("")
		}
	}
	
	Connections {
		target: rolesPage.apiClient
		function onRoleRemoved(roleId) {
			if (rolesPage.__dataProvider)
				rolesPage.__dataProvider.fetch("")
		}
	}
	
	StackViewHeader {
		id: rolesStackViewHeader
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.leftMargin: Math.max((parent.width - Math.min(parent.width - Style.marginXL * 2, 1000)) / 2, Style.marginXL)
		width: Math.min(parent.width - Style.marginXL * 2, 1000)
		height: Style.controlHeightL
		initialItemTitleVisible: true
		
		onCloseClicked: {
			if (rolesPage.__activeShellView
					&& rolesPage.__activeShellView.state === "content") {
				// Defer the pop to the shell's onClosed handler so dirty-check
				// can run and (optionally) save before closing.
				rolesPage.__activeShellView.closeDocument()
			}
			else {
				rolesStackView.previous()
				rolesStackViewHeader.popHeader()
			}
		}
		
		onHeaderItemClicked: {
			// Navigate back to the clicked breadcrumb level
			while (rolesStackView.currentIndex > index) {
				rolesStackView.previous()
				rolesStackViewHeader.popHeader()
			}
		}
		
		Component.onCompleted: {
			rolesStackViewHeader.addHeader("roles_list", qsTr("Roles"))
		}
	}
	
	Text {
		id: rolesCreateBtn
		visible: rolesPage.__canManage && rolesStackView.currentIndex === 0
		anchors.right: rolesStackViewHeader.right
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
				rolesPage.__isCreating = true
				rolesStackViewHeader.addHeader("create_role", qsTr("Create New Role"))
				rolesStackView.addPage(roleEditorView)
				rolesStackView.next()
			}
		}
	}
	
	Text {
		id: rolesEditBtn
		visible: rolesPage.__canManage && rolesStackView.currentIndex === 0
		anchors.right: rolesCreateBtn.left
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: rolesStackViewHeader.verticalCenter
		text: qsTr("Edit")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: rolesPage.__selectionManager && rolesPage.__selectionManager.selectedIds.length === 1 ? Style.linkColor : Style.inactiveTextColor
		opacity: rolesPage.__selectionManager && rolesPage.__selectionManager.selectedIds.length === 1 ? 1.0 : 0.5
		
		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: rolesPage.__selectionManager && rolesPage.__selectionManager.selectedIds.length === 1 ? Qt.PointingHandCursor : Qt.ArrowCursor
			enabled: rolesPage.__selectionManager && rolesPage.__selectionManager.selectedIds.length === 1
			onClicked: {
				var selId = rolesPage.__selectionManager.selectedIds[0]
				var items = rolesPage.__dataProvider ? rolesPage.__dataProvider.items : []
				for (var i = 0; i < items.length; i++) {
					if (items[i] && items[i].id === selId) {
						rolesPage.__openEditRole(selId, items[i].title || items[i].id || "", items[i].description || "")
						break
					}
				}
			}
		}
	}
	
	Text {
		id: rolesRemoveBtn
		visible: rolesPage.__canManage && rolesStackView.currentIndex === 0
		anchors.right: rolesEditBtn.left
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: rolesStackViewHeader.verticalCenter
		text: qsTr("Remove")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: rolesPage.__selectionManager && rolesPage.__selectionManager.selectedIds.length > 0 ? Style.errorColor : Style.inactiveTextColor
		opacity: rolesPage.__selectionManager && rolesPage.__selectionManager.selectedIds.length > 0 ? 1.0 : 0.5
		
		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: rolesPage.__selectionManager && rolesPage.__selectionManager.selectedIds.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
			enabled: rolesPage.__selectionManager && rolesPage.__selectionManager.selectedIds.length > 0
			onClicked: {
				var count = rolesPage.__selectionManager.selectedIds.length
				ModalDialogManager.showConfirmationDialog(
							qsTr("Delete Roles"),
							qsTr("Are you sure you want to delete %1 selected role(s)? This action cannot be undone.").arg(count),
							function(result) {
								if (result === Enums.yes && rolesPage.apiClient) {
									var ids = rolesPage.__selectionManager.selectedIds.slice()
									for (var i = 0; i < ids.length; i++)
										rolesPage.apiClient.removeRole(ids[i])
									rolesPage.__selectionManager.clear()
								}
							}
							)
			}
		}
	}
	
	BaseText {
		id: rolesDescription
		anchors.top: rolesStackViewHeader.bottom
		anchors.topMargin: Style.marginS
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, 1000)
		visible: rolesStackView.currentIndex === 0
		text: qsTr("Manage tenant roles and assign permissions to team members.")
		font.pixelSize: Style.fontSizeM
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
			property var rolesDataProvider: rolesDataProviderLoader.item

			Loader {
				id: rolesDataProviderLoader
				sourceComponent: rolesPage.apiClient ? rolesPage.apiClient.roleListDataProviderComp : null
				onLoaded: {
					rolesPage.__dataProvider = item
					item.fetch("")
				}
				Component.onDestruction: {
					if (rolesPage){
						rolesPage.__dataProvider = null
					}
				}
			}

			SearchTextInput {
				id: rolesFilterInput
				anchors.top: parent.top
				anchors.topMargin: Style.marginM
				anchors.horizontalCenter: parent.horizontalCenter
				width: Math.min(parent.width - Style.marginXL * 2, 1000)
				placeHolderText: qsTr("Filter roles...")
				onTextChanged: {
					rolesSelectionManager.clear()
					if (rolesDataProviderLoader.item)
						rolesDataProviderLoader.item.fetch(text)
				}
			}
			
			TenantTableContainer {
				anchors.top: rolesFilterInput.bottom
				anchors.topMargin: Style.marginM
				height: Math.min(rolesTableHeader.height + rolesEmptyState.height + rolesListView2.contentHeight + 2,
								 parent.height - rolesFilterInput.height - rolesFilterInput.anchors.topMargin - Style.marginM - Style.marginL)
				
				IdSelectionManager {
					id: rolesSelectionManager
					multiSelect: true
					Component.onCompleted: rolesPage.__selectionManager = rolesSelectionManager
					Component.onDestruction: rolesPage.__selectionManager = null
				}
				
				TenantTableHeader {
					id: rolesTableHeader
					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right
					selectedCount: rolesSelectionManager.selectedIds.length
					totalCount: rolesListView2.count
					checkState: rolesSelectionManager.selectedIds.length === 0
								? Qt.Unchecked
								: (rolesSelectionManager.selectedIds.length === rolesListView2.count
								   ? Qt.Checked : Qt.PartiallyChecked)
					
					onSelectAllToggled: {
						if (checkState === Qt.Checked) {
							rolesSelectionManager.clear()
						} else {
							var allIds = []
							var items = rolesDataProvider ? rolesDataProvider.items : []
							for (var i = 0; i < items.length; i++) {
								if (items[i] && items[i].id)
									allIds.push(items[i].id)
							}
							rolesSelectionManager.selectMultiple(allIds)
						}
					}
				}
				
				Item {
					id: rolesEmptyState
					visible: !rolesDataProvider || rolesDataProvider.items.length === 0
					anchors.top: rolesTableHeader.bottom
					anchors.left: parent.left
					anchors.right: parent.right
					height: visible ? Style.controlHeightL + Style.marginL : 0
					
					BaseText {
						anchors.centerIn: parent
						text: qsTr("No roles found.")
						font.pixelSize: Style.fontSizeM
						color: Style.inactiveTextColor
					}
				}
				
				ListView {
					id: rolesListView2
					anchors.top: rolesEmptyState.visible ? rolesEmptyState.bottom : rolesTableHeader.bottom
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.bottom: parent.bottom
					clip: true
					boundsBehavior: Flickable.StopAtBounds
					model: rolesDataProvider ? rolesDataProvider.items : []
					
					delegate: Rectangle {
						id: roleDelegateRoot
						width: rolesListView2.width
						height: Style.controlHeightL + Style.marginL
						
						property string itemId: modelData.id || ""
						property string itemTitle: modelData.title || modelData.id || ""
						property string itemDescription: modelData.description || ""
						property bool isSelected: rolesSelectionManager.isSelected(itemId)
						
						color: isSelected ? Style.selectedColor
										  : roleMouseArea.containsMouse ? Style.buttonHoverColor
																		: "transparent"
						
						MouseArea {
							id: roleMouseArea
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onDoubleClicked: {
								if (rolesPage.__canManage)
									rolesPage.__openEditRole(roleDelegateRoot.itemId, roleDelegateRoot.itemTitle, roleDelegateRoot.itemDescription)
							}
						}
						
						Row {
							anchors.left: parent.left
							anchors.right: roleMoreButton.left
							anchors.verticalCenter: parent.verticalCenter
							anchors.leftMargin: Style.marginM
							anchors.rightMargin: Style.marginM
							spacing: Style.marginM
							
							CheckBox {
								anchors.verticalCenter: parent.verticalCenter
								height: Style.itemSizeS
								width: Style.itemSizeS
								checkState: roleDelegateRoot.isSelected ? Qt.Checked : Qt.Unchecked
								onCheckStateChanged: {
									var shouldBeSelected = (checkState === Qt.Checked)
									var currentlySelected = rolesSelectionManager.isSelected(roleDelegateRoot.itemId)
									if (shouldBeSelected !== currentlySelected)
										rolesSelectionManager.toggleSelect(roleDelegateRoot.itemId)
								}
							}
							
							Column {
								anchors.verticalCenter: parent.verticalCenter
								spacing: Style.marginXS
								width: parent.width - Style.itemSizeS - parent.spacing
								
								BaseText {
									text: roleDelegateRoot.itemTitle
									font.pixelSize: Style.fontSizeL
									font.bold: true
									color: Style.textColor
								}
								
								BaseText {
									visible: roleDelegateRoot.itemDescription !== ""
									text: roleDelegateRoot.itemDescription
									font.pixelSize: Style.fontSizeM
									color: Style.inactiveTextColor
									elide: Text.ElideRight
									width: parent.width
								}
							}
						}
						
						Rectangle {
							id: roleMoreButton
							anchors.right: parent.right
							anchors.rightMargin: Style.marginM
							anchors.verticalCenter: parent.verticalCenter
							width: Style.controlHeightM
							height: Style.controlHeightM
							radius: Style.controlHeightM / 2
							color: roleMoreButtonMA.containsMouse ? Style.buttonHoverColor : "transparent"
							visible: rolesPage.__canManage && (roleMouseArea.containsMouse || roleDelegateRoot.isSelected || roleMoreButtonMA.containsMouse)
							
							Text {
								anchors.centerIn: parent
								text: "\u2026"
								font.pixelSize: Style.fontSizeL
								color: Style.textColor
								horizontalAlignment: Text.AlignHCenter
								verticalAlignment: Text.AlignVCenter
							}
							
							MouseArea {
								id: roleMoreButtonMA
								anchors.fill: parent
								hoverEnabled: true
								cursorShape: Qt.PointingHandCursor
								onClicked: roleItemMenu.popup()
							}
						}
						
						Menu {
							id: roleItemMenu
							MenuItem {
								text: qsTr("Edit")
								enabled: rolesPage.__canManage
								onTriggered: rolesPage.__openEditRole(roleDelegateRoot.itemId, roleDelegateRoot.itemTitle, roleDelegateRoot.itemDescription)
							}
							MenuItem {
								text: qsTr("Delete")
								enabled: rolesPage.__canManage
								onTriggered: {
									ModalDialogManager.showConfirmationDialog(
												qsTr("Delete Role"),
												qsTr("Are you sure you want to delete the role \"%1\"? This action cannot be undone.").arg(roleDelegateRoot.itemTitle),
												function(result) {
													if (result === Enums.yes && rolesPage.apiClient)
														rolesPage.apiClient.removeRole(roleDelegateRoot.itemId)
												}
												)
								}
							}
						}
						
						Rectangle {
							anchors.bottom: parent.bottom
							anchors.left: parent.left
							anchors.right: parent.right
							height: 1
							color: Style.borderColor
							opacity: 0.5
						}
					}
					
				}
				
				CustomScrollbar {
					anchors.right: parent.right
					anchors.top: rolesTableHeader.bottom
					anchors.bottom: parent.bottom
					targetItem: rolesListView2
					secondSize: 8
				}
			}
		}
	}
	
	function __openEditRole(itemId, itemName, itemDescription) {
		rolesPage.__editRoleId = itemId
		rolesPage.__isCreating = false
		while (rolesStackView.count > 1)
			rolesStackView.removePage(rolesStackView.count - 1)
		rolesStackViewHeader.addHeader("edit_role", itemName || qsTr("Edit Role"))
		rolesStackView.addPage(roleEditView)
		rolesStackView.next()
	}
	
	Component {
		id: roleEditorView
		
		Item {
			SingleDocumentWorkspaceShellView {
				id: createRoleShell
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.leftMargin: Math.max((parent.width - Math.min(parent.width - Style.marginXL * 2, 1000)) / 2, Style.marginXL)
				width: Math.min(parent.width - Style.marginXL * 2, 1000)
				
				documentManager: rolesPage.apiClient ? rolesPage.apiClient.roleDocumentManager : null
				objectTypeId: rolesPage.apiClient ? rolesPage.apiClient.roleObjectTypeId : ""
				objectId: ""
				createNew: true
				headerVisible: false
				
				Component.onCompleted: {
					rolesPage.__activeShellView = createRoleShell
				}
				Component.onDestruction: {
					if (rolesPage && rolesPage.__activeShellView === createRoleShell)
						rolesPage.__activeShellView = null
				}
				
				onClosed: {
					rolesStackViewHeader.popHeader()
					rolesStackView.previous()
					while (rolesStackView.count > 1)
						rolesStackView.removePage(rolesStackView.count - 1)
				}
			}
		}
	}
	
	Component {
		id: roleEditView
		
		Item {
			SingleDocumentWorkspaceShellView {
				id: editRoleShell
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.leftMargin: Math.max((parent.width - Math.min(parent.width - Style.marginXL * 2, 1000)) / 2, Style.marginXL)
				width: Math.min(parent.width - Style.marginXL * 2, 1000)
				
				documentManager: rolesPage.apiClient ? rolesPage.apiClient.roleDocumentManager : null
				objectTypeId: rolesPage.apiClient ? rolesPage.apiClient.roleObjectTypeId : ""
				objectId: rolesPage.__editRoleId
				createNew: false
				headerVisible: false
				
				Component.onCompleted: {
					rolesPage.__activeShellView = editRoleShell
				}
				Component.onDestruction: {
					if (rolesPage && rolesPage.__activeShellView === editRoleShell)
						rolesPage.__activeShellView = null
				}
				
				onClosed: {
					rolesStackViewHeader.popHeader()
					rolesStackView.previous()
					while (rolesStackView.count > 1)
						rolesStackView.removePage(rolesStackView.count - 1)
				}
			}
		}
	}
}
