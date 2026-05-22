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
 * TenantGroupsPage
 *
 * Groups tab — list / create / edit / delete tenant groups.
 * Table is fixed at 800px width, centered, with gray rounded border and checkbox selection.
 */
ViewBase {
	id: groupsPage
	
	commandsPanelVisible: false
	contentColor: Style.baseColor
	readonly property var tenantData: groupsPage.model
	property var stateManager: null
	property var apiClient: null
	property var groupDataFactory: null
	
	function updateGui() {}
	function updateModel() {}
	
	property string __editGroupId: ""
	property string __editGroupName: ""
	property string __editGroupDescription: ""
	property bool __isCreating: false
	
	readonly property bool __canManage: groupsPage.stateManager ? groupsPage.stateManager.canManageMembers : false
	
	property var __selectionManager: null
	property var __dataProvider: null
	
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
		anchors.leftMargin: Math.max((parent.width - Math.min(parent.width - Style.marginXL * 2, 1000)) / 2, Style.marginXL)
		width: Math.min(parent.width - Style.marginXL * 2, 1000)
		height: Style.controlHeightL
		initialItemTitleVisible: true
		
		onCloseClicked: {
			groupsStackView.previous()
			groupsStackViewHeader.popHeader()
		}
		
		onHeaderItemClicked: {
			while (groupsStackView.currentIndex > index) {
				groupsStackView.previous()
				groupsStackViewHeader.popHeader()
			}
		}
		
		Component.onCompleted: {
			groupsStackViewHeader.addHeader("groups_list", qsTr("Groups"))
		}
	}
	
	Text {
		id: groupsCreateBtn
		visible: groupsPage.__canManage && groupsStackView.currentIndex === 0
		anchors.right: groupsStackViewHeader.right
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
				groupsPage.__isCreating = true
				groupsStackViewHeader.addHeader("create_group", qsTr("Create New Group"))
				groupsStackView.addPage(groupEditorView)
				groupsStackView.next()
			}
		}
	}
	
	Text {
		id: groupsEditBtn
		visible: groupsPage.__canManage && groupsStackView.currentIndex === 0
		anchors.right: groupsCreateBtn.left
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: groupsStackViewHeader.verticalCenter
		text: qsTr("Edit")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: groupsPage.__selectionManager && groupsPage.__selectionManager.selectedIds.length === 1 ? Style.linkColor : Style.inactiveTextColor
		opacity: groupsPage.__selectionManager && groupsPage.__selectionManager.selectedIds.length === 1 ? 1.0 : 0.5
		
		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: groupsPage.__selectionManager && groupsPage.__selectionManager.selectedIds.length === 1 ? Qt.PointingHandCursor : Qt.ArrowCursor
			enabled: groupsPage.__selectionManager && groupsPage.__selectionManager.selectedIds.length === 1
			onClicked: {
				var selId = groupsPage.__selectionManager.selectedIds[0]
				var items = groupsPage.__dataProvider ? groupsPage.__dataProvider.items : []
				for (var i = 0; i < items.length; i++) {
					if (items[i] && items[i].id === selId) {
						groupsPage.__openEditGroup(selId, items[i].title || items[i].id || "", items[i].description || "")
						break
					}
				}
			}
		}
	}
	
	Text {
		id: groupsRemoveBtn
		visible: groupsPage.__canManage && groupsStackView.currentIndex === 0
		anchors.right: groupsEditBtn.left
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: groupsStackViewHeader.verticalCenter
		text: qsTr("Remove")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: groupsPage.__selectionManager && groupsPage.__selectionManager.selectedIds.length > 0 ? Style.errorColor : Style.inactiveTextColor
		opacity: groupsPage.__selectionManager && groupsPage.__selectionManager.selectedIds.length > 0 ? 1.0 : 0.5
		
		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: groupsPage.__selectionManager && groupsPage.__selectionManager.selectedIds.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
			enabled: groupsPage.__selectionManager && groupsPage.__selectionManager.selectedIds.length > 0
			onClicked: {
				var count = groupsPage.__selectionManager.selectedIds.length
				ModalDialogManager.showConfirmationDialog(
							qsTr("Delete Groups"),
							qsTr("Are you sure you want to delete %1 selected group(s)? This action cannot be undone.").arg(count),
							function(result) {
								if (result === Enums.yes && groupsPage.apiClient) {
									var ids = groupsPage.__selectionManager.selectedIds.slice()
									for (var i = 0; i < ids.length; i++)
										groupsPage.apiClient.removeGroup(ids[i])
									groupsPage.__selectionManager.clear()
								}
							}
							)
			}
		}
	}
	
	BaseText {
		id: groupsDescription
		anchors.top: groupsStackViewHeader.bottom
		anchors.topMargin: Style.marginS
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, 1000)
		visible: groupsStackView.currentIndex === 0
		text: qsTr("Organize members into groups for easier permission management.")
		font.pixelSize: Style.fontSizeM
		color: Style.inactiveTextColor
	}
	
	Text {
		id: groupsSaveBtn
		visible: groupsPage.__canManage && groupsStackView.currentIndex > 0
		anchors.right: groupsStackViewHeader.right
		anchors.verticalCenter: groupsStackViewHeader.verticalCenter
		text: qsTr("Save")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: Style.linkColor
		
		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: Qt.PointingHandCursor
			onClicked: groupsPage.__saveCurrentEditor()
		}
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
			SearchTextInput {
				id: groupsFilterInput
				anchors.top: parent.top
				anchors.topMargin: Style.marginM
				anchors.horizontalCenter: parent.horizontalCenter
				width: Math.min(parent.width - Style.marginXL * 2, 1000)
				placeHolderText: qsTr("Filter groups...")
				onTextChanged: {
					groupsSelectionManager.clear()
					groupsDataProvider.fetch(text)
				}
			}
			
			TenantTableContainer {
				anchors.top: groupsFilterInput.bottom
				anchors.topMargin: Style.marginM
				height: Math.min(groupsTableHeader.height + groupsListView2.contentHeight + 2,
								 parent.height - groupsFilterInput.height - groupsFilterInput.anchors.topMargin - Style.marginM - Style.marginL)
				
				IdSelectionManager {
					id: groupsSelectionManager
					multiSelect: true
					Component.onCompleted: {
						groupsPage.__selectionManager = groupsSelectionManager
					}
					Component.onDestruction: {
						if (groupsPage){
							groupsPage.__selectionManager = null
						}
					}
				}
				
				FilterableSelectGqlDataProvider {
					id: groupsDataProvider
					collectionId: "Groups"
					pageSize: 50
					Component.onCompleted: groupsPage.__dataProvider = groupsDataProvider
					Component.onDestruction: groupsPage.__dataProvider = null
				}
				
				Component.onCompleted: {
					groupsDataProvider.fetch("")
				}
				
				TenantTableHeader {
					id: groupsTableHeader
					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right
					selectedCount: groupsSelectionManager.selectedIds.length
					totalCount: groupsListView2.count
					checkState: groupsSelectionManager.selectedIds.length === 0
								? Qt.Unchecked
								: (groupsSelectionManager.selectedIds.length === groupsListView2.count
								   ? Qt.Checked : Qt.PartiallyChecked)
					
					onSelectAllToggled: {
						if (checkState === Qt.Checked) {
							groupsSelectionManager.clear()
						} else {
							var allIds = []
							var items = groupsDataProvider.items
							for (var i = 0; i < items.length; i++) {
								if (items[i] && items[i].id)
									allIds.push(items[i].id)
							}
							groupsSelectionManager.selectMultiple(allIds)
						}
					}
				}
				
				Item {
					id: groupsEmptyState
					visible: groupsDataProvider.items.length === 0
					anchors.top: groupsTableHeader.bottom
					anchors.left: parent.left
					anchors.right: parent.right
					height: visible ? Style.controlHeightL + Style.marginL : 0
					
					BaseText {
						anchors.centerIn: parent
						text: qsTr("No groups found.")
						font.pixelSize: Style.fontSizeM
						color: Style.inactiveTextColor
					}
				}
				
				ListView {
					id: groupsListView2
					anchors.top: groupsEmptyState.visible ? groupsEmptyState.bottom : groupsTableHeader.bottom
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.bottom: parent.bottom
					clip: true
					boundsBehavior: Flickable.StopAtBounds
					model: groupsDataProvider.items
					
					delegate: Rectangle {
						id: groupDelegateRoot
						width: groupsListView2.width
						height: Style.controlHeightL + Style.marginL
						
						property string itemId: modelData.id || ""
						property string itemTitle: modelData.title || modelData.id || ""
						property string itemDescription: modelData.description || ""
						property bool isSelected: groupsSelectionManager.isSelected(itemId)
						
						color: isSelected ? Style.selectedColor
										  : groupMouseArea.containsMouse ? Style.buttonHoverColor
																		 : "transparent"
						
						MouseArea {
							id: groupMouseArea
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onDoubleClicked: {
								if (groupsPage.__canManage)
									groupsPage.__openEditGroup(groupDelegateRoot.itemId, groupDelegateRoot.itemTitle, groupDelegateRoot.itemDescription)
							}
						}
						
						Row {
							anchors.left: parent.left
							anchors.right: groupMoreButton.left
							anchors.verticalCenter: parent.verticalCenter
							anchors.leftMargin: Style.marginM
							anchors.rightMargin: Style.marginM
							spacing: Style.marginM
							
							CheckBox {
								anchors.verticalCenter: parent.verticalCenter
								height: Style.itemSizeS
								width: Style.itemSizeS
								checkState: groupDelegateRoot.isSelected ? Qt.Checked : Qt.Unchecked
								onCheckStateChanged: {
									var shouldBeSelected = (checkState === Qt.Checked)
									var currentlySelected = groupsSelectionManager.isSelected(groupDelegateRoot.itemId)
									if (shouldBeSelected !== currentlySelected)
										groupsSelectionManager.toggleSelect(groupDelegateRoot.itemId)
								}
							}
							
							Column {
								anchors.verticalCenter: parent.verticalCenter
								spacing: Style.marginXS
								width: parent.width - Style.itemSizeS - parent.spacing
								
								BaseText {
									text: groupDelegateRoot.itemTitle
									font.pixelSize: Style.fontSizeL
									font.bold: true
									color: Style.textColor
								}
								
								BaseText {
									visible: groupDelegateRoot.itemDescription !== ""
									text: groupDelegateRoot.itemDescription
									font.pixelSize: Style.fontSizeM
									color: Style.inactiveTextColor
									elide: Text.ElideRight
									width: parent.width
								}
							}
						}
						
						Rectangle {
							id: groupMoreButton
							anchors.right: parent.right
							anchors.rightMargin: Style.marginM
							anchors.verticalCenter: parent.verticalCenter
							width: Style.controlHeightM
							height: Style.controlHeightM
							radius: Style.controlHeightM / 2
							color: groupMoreButtonMA.containsMouse ? Style.buttonHoverColor : "transparent"
							visible: groupsPage.__canManage && (groupMouseArea.containsMouse || groupDelegateRoot.isSelected || groupMoreButtonMA.containsMouse)
							
							Text {
								anchors.centerIn: parent
								text: "\u2026"
								font.pixelSize: Style.fontSizeL
								color: Style.textColor
								horizontalAlignment: Text.AlignHCenter
								verticalAlignment: Text.AlignVCenter
							}
							
							MouseArea {
								id: groupMoreButtonMA
								anchors.fill: parent
								hoverEnabled: true
								cursorShape: Qt.PointingHandCursor
								onClicked: groupItemMenu.popup()
							}
						}
						
						Menu {
							id: groupItemMenu
							MenuItem {
								text: qsTr("Edit")
								enabled: groupsPage.__canManage
								onTriggered: groupsPage.__openEditGroup(groupDelegateRoot.itemId, groupDelegateRoot.itemTitle, groupDelegateRoot.itemDescription)
							}
							MenuItem {
								text: qsTr("Delete")
								enabled: groupsPage.__canManage
								onTriggered: {
									ModalDialogManager.showConfirmationDialog(
												qsTr("Delete Group"),
												qsTr("Are you sure you want to delete the group \"%1\"? This action cannot be undone.").arg(groupDelegateRoot.itemTitle),
												function(result) {
													if (result === Enums.yes && groupsPage.apiClient)
														groupsPage.apiClient.removeGroup(groupDelegateRoot.itemId)
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
					anchors.top: groupsTableHeader.bottom
					anchors.bottom: parent.bottom
					targetItem: groupsListView2
					secondSize: 8
				}
			}
		}
	}
	
	function __saveCurrentEditor() {
		var page = groupsStackView.currentPage()
		if (!page) return
		var editorView = page.children[0]
		if (!editorView || !editorView.updateModel) return
		editorView.updateModel()
		var groupData = editorView.model
		if (groupsPage.__isCreating) {
			if (groupsPage.apiClient)
				groupsPage.apiClient.insertGroup(
							groupData ? groupData.m_name : "",
							groupData ? groupData.m_description : "")
		} else {
			if (groupsPage.apiClient)
				groupsPage.apiClient.setGroupData(
							groupsPage.__editGroupId,
							groupData ? groupData.m_name : "",
							groupData ? groupData.m_description : "")
		}
		groupsStackViewHeader.popHeader()
		groupsStackView.previous()
		while (groupsStackView.count > 1)
			groupsStackView.removePage(groupsStackView.count - 1)
	}
	
	function __openEditGroup(itemId, itemName, itemDescription) {
		groupsPage.__editGroupId = itemId
		groupsPage.__editGroupName = itemName
		groupsPage.__editGroupDescription = itemDescription
		groupsPage.__isCreating = false
		while (groupsStackView.count > 1)
			groupsStackView.removePage(groupsStackView.count - 1)
		groupsStackViewHeader.addHeader("edit_group", itemName || qsTr("Edit Group"))
		groupsStackView.addPage(groupEditView)
		groupsStackView.next()
		if (groupsPage.apiClient)
			groupsPage.apiClient.getGroupData(itemId)
	}
	
	Component {
		id: groupEditorView
		
		Item {
			UserGroupView {
				id: createGroupView
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.leftMargin: Math.max((parent.width - Math.min(parent.width - Style.marginXL * 2, 1000)) / 2, Style.marginXL)
				width: Math.min(parent.width - Style.marginXL * 2, 1000)
				commandsPanelVisible: false
				
				Component.onCompleted: {
					createGroupView.model = groupsPage.groupDataFactory ? groupsPage.groupDataFactory() : null
					createGroupView.updateGui()
				}
			}
		}
	}
	
	Component {
		id: groupEditView
		
		Item {
			UserGroupView {
				id: editGroupView
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.leftMargin: Math.max((parent.width - Math.min(parent.width - Style.marginXL * 2, 1000)) / 2, Style.marginXL)
				width: Math.min(parent.width - Style.marginXL * 2, 1000)
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
		}
	}
}
