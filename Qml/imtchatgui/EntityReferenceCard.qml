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

	width: compact ? compactRow.implicitWidth + Style.paddingS * 2 : Style.cardWidth
	height: compact ? Style.chipHeight : Style.cardHeight
	radius: Style.radiusS
	color: Style.accentColorLight
	border.color: Style.accentColor
	border.width: 1

	// Compact chip mode (used inside MessageInput)
	Row {
		id: compactRow
		visible: compact
		anchors.centerIn: parent
		spacing: Style.paddingXS

		Text {
			text: entityTypeIcon(entityRefCardRoot.entityType)
			font.pixelSize: Style.fontSizeXS
			color: Style.accentColor
			anchors.verticalCenter: parent.verticalCenter
		}

		Text {
			text: entityRefCardRoot.displayName || entityRefCardRoot.entityId
			font.pixelSize: Style.fontSizeXS
			color: Style.textPrimaryColor
			anchors.verticalCenter: parent.verticalCenter
			elide: Text.ElideRight
			maximumLineCount: 1
		}

		Text {
			text: "×"
			font.pixelSize: Style.fontSizeS
			color: Style.textSecondaryColor
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
		visible: !compact
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.margins: Style.paddingS
		spacing: Style.paddingXS

		Row {
			spacing: Style.paddingXS

			Text {
				text: entityTypeIcon(entityRefCardRoot.entityType)
				font.pixelSize: Style.fontSizeM
				color: Style.accentColor
			}

			Text {
				text: entityRefCardRoot.entityType
				font.pixelSize: Style.fontSizeXS
				color: Style.textSecondaryColor
				font.capitalization: Font.AllUppercase
				anchors.verticalCenter: parent.verticalCenter
			}
		}

		Text {
			text: entityRefCardRoot.displayName || entityRefCardRoot.entityId
			font.pixelSize: Style.fontSizeS
			font.bold: true
			color: Style.textPrimaryColor
			elide: Text.ElideRight
			width: parent.width
		}

		Text {
			visible: entityRefCardRoot.previewData.length > 0
			text: entityRefCardRoot.previewData
			font.pixelSize: Style.fontSizeXS
			color: Style.textSecondaryColor
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
