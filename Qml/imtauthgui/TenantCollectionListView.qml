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
	property string createButtonText: ""
	property bool canManage: false

	signal createRequested()
	signal editRequested(string itemId, string itemName, string itemDescription)
	signal deleteRequested(string itemId, string itemName)

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

		Row {
			width: parent.width
			spacing: Style.marginM

			Item {
				width: parent.width
					- (createBtn.visible ? createBtn.width : 0)
					- parent.spacing
				height: 1
			}

			Text {
				id: createBtn
				visible: root.canManage && root.createButtonText.length > 0
				anchors.verticalCenter: parent.verticalCenter
				text: "+ " + root.createButtonText
				font.pixelSize: Style.fontSizeM
				font.bold: true
				color: Style.linkColor

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: root.createRequested()
				}
			}
		}

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
			width: listViewArea.width
			height: Style.controlHeightL
			color: itemMouseArea.containsMouse ? Style.buttonHoverColor : "transparent"

			MouseArea {
				id: itemMouseArea
				anchors.fill: parent
				hoverEnabled: true
				cursorShape: Qt.PointingHandCursor
				acceptedButtons: Qt.LeftButton | Qt.RightButton
				onClicked: {
					if (mouse.button === Qt.RightButton && root.canManage)
						itemMenu.popup()
					else if (mouse.button === Qt.LeftButton && root.canManage)
						root.editRequested(modelData.id, modelData.title || modelData.id || "", modelData.description || "")
				}
			}

			Menu {
				id: itemMenu

				MenuItem {
					text: qsTr("Edit")
					enabled: root.canManage
					onTriggered: root.editRequested(modelData.id, modelData.title || modelData.id || "", modelData.description || "")
				}

				MenuItem {
					text: qsTr("Delete")
					enabled: root.canManage
					onTriggered: root.deleteRequested(modelData.id, modelData.title || modelData.id || "")
				}
			}

			Row {
				id: itemRow
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				anchors.margins: Style.marginM
				spacing: Style.marginM

				Column {
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginXS
					width: parent.width

					BaseText {
						text: modelData.title || modelData.id || ""
						font.pixelSize: Style.fontSizeM
						color: Style.textColor
					}

					BaseText {
						visible: (modelData.description || "") !== ""
						text: modelData.description || ""
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
						elide: Text.ElideRight
						width: parent.width
					}
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
