// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: entitySearchPopupRoot
	objectName: "EntitySearchPopup"
	visible: false
	border.color: Style.borderColor2
	border.width: 1
	radius: Style.radiusS
	clip: true

	property string searchQuery: ""
	property string triggerChar: "@"
	property int triggerPos: 0
	property var searchResults: []

	signal entitySelected(string entityType, string entityId, string displayName)

	function open() {
		visible = true;
		performSearch();
	}

	function close() {
		visible = false;
		searchResults = [];
	}

	onSearchQueryChanged: {
		if (visible) {
			performSearch();
		}
	}

	function performSearch() {
		// In real usage this queries the domain model / GQL layer.
		// Here we provide a filtered mock list for demonstration.
		if (searchQuery.length < 1) {
			searchResults = [];
			return;
		}
		// Placeholder: emit empty; real implementation hooks into collection APIs.
		searchResults = [];
	}

	ListView {
		id: resultsListView
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.margins: Style.paddingS
		height: contentHeight
		model: entitySearchPopupRoot.searchResults
		spacing: 2

		delegate: Rectangle {
			width: resultsListView.width
			height: Style.listItemHeightS
			color: "transparent"
			radius: Style.radiusXS

			Row {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				anchors.leftMargin: Style.paddingS
				spacing: Style.paddingS

				Text {
					text: modelData.icon || "📎"
					font.pixelSize: Style.fontSizeS
					anchors.verticalCenter: parent.verticalCenter
				}

				Column {
					anchors.verticalCenter: parent.verticalCenter

					Text {
						text: modelData.displayName || modelData.entityId
						font.pixelSize: Style.fontSizeS
						color: Style.imaginToolsAccentColor
					}

					Text {
						text: modelData.entityType || ""
						font.pixelSize: Style.fontSizeXS
						color: Style.buttonInactiveTextColor
					}
				}
			}

			MouseArea {
				anchors.fill: parent
				onClicked: {
					entitySearchPopupRoot.entitySelected(
						modelData.entityType,
						modelData.entityId,
						modelData.displayName
					);
				}
			}

		}

		// "No results" placeholder
		Text {
			visible: entitySearchPopupRoot.searchResults.length === 0
				&& entitySearchPopupRoot.searchQuery.length > 0
			anchors.centerIn: parent
			text: qsTr("No results for \"%1\"").arg(entitySearchPopupRoot.searchQuery)
			font.pixelSize: Style.fontSizeXS
			color: Style.buttonInactiveTextColor
		}
	}
}
