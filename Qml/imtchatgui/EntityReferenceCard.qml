// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: entityRefCardRoot
	objectName: "EntityReferenceCard"

	property string entityType: ""
	property string entityId: ""
	property string displayName: ""
	property string previewData: ""
	property bool compact: false

	signal removeRequested()

	radius: Style.radiusS
	border.color: Style.imaginToolsAccentColor
	border.width: 1

	// Compact chip mode (used inside MessageInput)
	Row {
		id: compactRow
		visible: entityRefCardRoot.compact
		anchors.centerIn: parent
		spacing: Style.paddingS

		Text {
			text: entityRefCardRoot.entityTypeIcon(entityRefCardRoot.entityType)
			font.pixelSize: Style.fontSizeXS
			color: Style.imaginToolsAccentColor
			anchors.verticalCenter: parent.verticalCenter
		}

		Text {
			text: entityRefCardRoot.displayName || entityRefCardRoot.entityId
			font.pixelSize: Style.fontSizeXS
			color: Style.imaginToolsAccentColor
			anchors.verticalCenter: parent.verticalCenter
			elide: Text.ElideRight
			maximumLineCount: 1
		}

		Text {
			text: "×"
			font.pixelSize: Style.fontSizeS
			color: Style.buttonInactiveTextColor
			anchors.verticalCenter: parent.verticalCenter

			MouseArea {
				anchors.fill: parent
				onClicked: {
					entityRefCardRoot.removeRequested()
				}
			}
		}
	}

	// Full card mode (used in message body)
	Column {
		visible: !entityRefCardRoot.compact
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.margins: Style.paddingS
		spacing: Style.paddingS

		Row {
			spacing: Style.paddingS

			Text {
				text: entityRefCardRoot.entityTypeIcon(entityRefCardRoot.entityType)
				font.pixelSize: Style.fontSizeM
				color: Style.imaginToolsAccentColor
			}

			Text {
				text: entityRefCardRoot.entityType
				font.pixelSize: Style.fontSizeXS
				color: Style.buttonInactiveTextColor
				anchors.verticalCenter: parent.verticalCenter
			}
		}

		Text {
			text: entityRefCardRoot.displayName || entityRefCardRoot.entityId
			font.pixelSize: Style.fontSizeS
			font.bold: true
			color: Style.imaginToolsAccentColor
			elide: Text.ElideRight
			width: parent.width
		}

		Text {
			visible: entityRefCardRoot.previewData.length > 0
			text: entityRefCardRoot.previewData
			font.pixelSize: Style.fontSizeXS
			color: Style.buttonInactiveTextColor
			wrapMode: Text.Wrap
			width: parent.width
			maximumLineCount: 2
			elide: Text.ElideRight
		}
	}

	function entityTypeIcon(type) {
		switch (type.toLowerCase()) {
			case "user":       return "👤";
			case "license":    return "🔑";
			case "product":    return "📦";
			case "ticket":     return "🎫";
			case "role":       return "🎭";
			default:           return "📎";
		}
	}
}
