// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import QtQuick.Controls
import imtgui 1.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

Item {
	id: root

	property string collectionId: ""
	property string filterPlaceholder: qsTr("Filter...")
	property string emptyMessage: qsTr("No items found.")
	property bool canManage: false

	// Selection mode: "single" or "multi"
	property string selectionMode: "single"

	// Configurable context menu commands for the 3-dot button.
	// Each element: { text: "Label", commandId: "someId", enabled: true }
	// If empty, a default Edit/Delete menu is shown (when canManage is true).
	property var menuCommands: []

	// Expose selection manager for external access
	readonly property alias selectionManager: selectionManager

	signal editRequested(string itemId, string itemName, string itemDescription)
	signal deleteRequested(string itemId, string itemName)
	signal menuCommandTriggered(string commandId, string itemId, string itemName)
	signal selectionChanged(var selectedIds)

	IdSelectionManager {
		id: selectionManager
		multiSelect: root.selectionMode === "multi"
		onSelectionChanged: root.selectionChanged(selectedIds)
	}

	FilterableSelectGqlDataProvider {
		id: dataProvider
		collectionId: root.collectionId
		pageSize: 50
	}

	Component.onCompleted: {
		dataProvider.fetch("")
	}

	function refresh() {
		dataProvider.fetch(filterInput.text)
	}

	// Header (fixed)
	Column {
		id: listHeader
		anchors.top: parent.top
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		spacing: Style.marginM

		SearchTextInput {
			id: filterInput
			width: parent.width
			placeHolderText: root.filterPlaceholder
			onTextChanged: dataProvider.fetch(text)
		}

		Rectangle {
			width: parent.width
			height: 1
			color: Style.borderColor
		}
	}

	// Scrollable list
	CustomScrollbar {
		id: listScrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: listViewArea.top
		anchors.bottom: listViewArea.bottom
		secondSize: Style.marginM
		targetItem: listViewArea
	}

	ListView {
		id: listViewArea
		anchors.top: listHeader.bottom
		anchors.topMargin: Style.marginM
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: listScrollbar.left
		anchors.rightMargin: Style.marginXL
		model: dataProvider.items
		clip: true
		boundsBehavior: Flickable.StopAtBounds

		delegate: Rectangle {
			id: delegateRoot
			width: listViewArea.width
			height: Style.controlHeightL + Style.marginL

			property string itemId: modelData.id || ""
			property string itemTitle: modelData.title || modelData.id || ""
			property string itemDescription: modelData.description || ""
			property bool isSelected: selectionManager.isSelected(itemId)

			color: isSelected ? Style.selectedColor
				 : itemMouseArea.containsMouse ? Style.buttonHoverColor
				 : "transparent"

			MouseArea {
				id: itemMouseArea
				anchors.fill: parent
				hoverEnabled: true
				cursorShape: Qt.PointingHandCursor
				acceptedButtons: Qt.LeftButton | Qt.RightButton
				onClicked: {
					if (mouse.button === Qt.RightButton) {
						if (root.canManage)
							itemMenu.popup()
					} else {
						// Selection logic
						if (root.selectionMode === "multi" && (mouse.modifiers & Qt.ControlModifier)) {
							selectionManager.toggleSelect(delegateRoot.itemId)
						} else {
							selectionManager.singleSelect(delegateRoot.itemId)
						}
					}
				}
				onDoubleClicked: {
					if (root.canManage)
						root.editRequested(delegateRoot.itemId, delegateRoot.itemTitle, delegateRoot.itemDescription)
				}
			}

			Menu {
				id: itemMenu

				Instantiator {
					model: root.menuCommands.length > 0 ? root.menuCommands : null
					delegate: MenuItem {
						text: modelData.text || ""
						enabled: modelData.enabled !== undefined ? modelData.enabled : true
						onTriggered: root.menuCommandTriggered(modelData.commandId || "", delegateRoot.itemId, delegateRoot.itemTitle)
					}
					onObjectAdded: itemMenu.insertItem(index, object)
					onObjectRemoved: itemMenu.removeItem(object)
				}

				// Default menu items (only when no custom commands provided)
				MenuItem {
					visible: root.menuCommands.length === 0
					text: qsTr("Edit")
					enabled: root.canManage
					height: visible ? implicitHeight : 0
					onTriggered: root.editRequested(delegateRoot.itemId, delegateRoot.itemTitle, delegateRoot.itemDescription)
				}

				MenuItem {
					visible: root.menuCommands.length === 0
					text: qsTr("Delete")
					enabled: root.canManage
					height: visible ? implicitHeight : 0
					onTriggered: root.deleteRequested(delegateRoot.itemId, delegateRoot.itemTitle)
				}
			}

			Row {
				id: itemRow
				anchors.left: parent.left
				anchors.right: moreButton.left
				anchors.verticalCenter: parent.verticalCenter
				anchors.leftMargin: Style.marginM
				anchors.rightMargin: Style.marginM
				spacing: Style.marginM

				Column {
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginXS
					width: parent.width

					BaseText {
						text: delegateRoot.itemTitle
						font.pixelSize: Style.fontSizeM
						font.bold: true
						color: Style.textColor
					}

					BaseText {
						visible: delegateRoot.itemDescription !== ""
						text: delegateRoot.itemDescription
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
						elide: Text.ElideRight
						width: parent.width
					}
				}
			}

			// 3-dot menu button
			Rectangle {
				id: moreButton
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				anchors.verticalCenter: parent.verticalCenter
				width: Style.controlHeightM
				height: Style.controlHeightM
				radius: Style.controlHeightM / 2
				color: moreButtonMA.containsMouse ? Style.buttonHoverColor : "transparent"
				visible: root.canManage && (itemMouseArea.containsMouse || delegateRoot.isSelected || moreButtonMA.containsMouse)

				BaseText {
					anchors.centerIn: parent
					text: "\u2026"
					font.pixelSize: Style.fontSizeL
					color: Style.textColor
				}

				MouseArea {
					id: moreButtonMA
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: itemMenu.popup()
				}
			}

			Rectangle {
				anchors.bottom: parent.bottom
				width: parent.width
				height: 1
				color: Style.borderColor
				opacity: 0.5
			}
		}

		// Empty state
		BaseText {
			visible: listViewArea.count === 0 && !dataProvider.isInitialLoading
			anchors.centerIn: parent
			text: root.emptyMessage
			font.pixelSize: Style.fontSizeM
			color: Style.inactiveTextColor
		}

		// Loading state
		BaseText {
			visible: dataProvider.isInitialLoading
			anchors.centerIn: parent
			text: qsTr("Loading...")
			font.pixelSize: Style.fontSizeM
			color: Style.inactiveTextColor
		}
	}
}
