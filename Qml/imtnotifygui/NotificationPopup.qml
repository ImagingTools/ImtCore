// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtnotifygui 1.0

// Global popup card raised when a new notification arrives. Instantiated by
// PopupManager.addCustomMessage; the property bag is filled from the live
// notification payload.
Rectangle {
	id: popupRoot

	// Set by PopupManager infrastructure.
	property string messageId: ""
	property var popupContainer: null

	// Set from the notification payload.
	property string notificationId: ""
	property string title: ""
	property string preview: ""
	property string iconName: ""
	property string severity: "Info"
	property string sourceType: ""
	property string sourceId: ""
	property string targetRoute: ""

	property color accentColor: severityColor(popupRoot.severity)

	width: parent ? parent.width : Style.sizeHintXS
	height: contentColumn.height + 2 * Style.marginL
	radius: Style.radiusM
	color: Style.backgroundColor2
	border.color: popupRoot.accentColor
	border.width: Style.buttonBorderWidth

	function severityColor(severity){
		if (severity === "Critical"){
			return Style.negativeAccentColor
		}
		if (severity === "Warning"){
			return Style.secondColor
		}
		if (severity === "Success"){
			return Style.positiveAccentColor
		}
		return Style.highlightColor
	}

	Rectangle {
		id: accentBar

		anchors.left: parent.left
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.margins: Style.buttonBorderWidth
		width: Style.marginXS
		radius: Style.radiusM
		color: popupRoot.accentColor
	}

	MouseArea {
		id: popupMouseArea

		anchors.fill: parent
		cursorShape: Qt.PointingHandCursor
		hoverEnabled: true
		onClicked: {
			ActivityController.openNotification(popupRoot.notificationId, popupRoot.targetRoute, popupRoot.sourceType, popupRoot.sourceId, popupRoot.title)
			if (popupRoot.popupContainer){
				popupRoot.popupContainer.removeMessageById(popupRoot.messageId)
			}
		}
	}

	Column {
		id: contentColumn

		anchors.left: accentBar.right
		anchors.right: closeButton.left
		anchors.top: parent.top
		anchors.margins: Style.marginL
		spacing: Style.marginM

		Row {
			id: titleRow

			width: parent.width
			spacing: Style.marginS

			Image {
				id: popupIcon

				anchors.verticalCenter: parent.verticalCenter
				width: Style.iconSizeS
				height: width
				visible: popupRoot.iconName !== ""
				source: popupRoot.iconName !== "" ? "../../../" + Style.getIconPath(popupRoot.iconName, Icon.State.On, Icon.Mode.Normal) : ""
				sourceSize.width: width
				sourceSize.height: height
			}

			BaseText {
				id: titleText

				width: parent.width - Style.iconSizeS - Style.marginS
				text: popupRoot.title
				font.bold: true
				elide: Text.ElideRight
				maximumLineCount: 1
			}
		}

		BaseText {
			id: previewText

			width: parent.width
			text: popupRoot.preview
			color: Style.subtitleColor
			elide: Text.ElideRight
			maximumLineCount: 2
			wrapMode: Text.WordWrap
			visible: popupRoot.preview !== ""
		}

		BaseText {
			id: hintText

			text: qsTr("Click to open →")
			color: Style.inactiveTextColor
			visible: popupRoot.targetRoute !== ""
		}
	}

	ToolButton {
		id: closeButton

		width: Style.iconSizeS + Style.marginM
		height: width
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.margins: Style.marginS
		iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
		decorator: Component {
			ToolButtonDecorator {
				color: "transparent"
				icon.width: Style.iconSizeXS
			}
		}
		onClicked: {
			if (popupRoot.popupContainer){
				popupRoot.popupContainer.removeMessageById(popupRoot.messageId)
			}
		}
	}
}
