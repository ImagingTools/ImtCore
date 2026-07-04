// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtnotifygui 1.0

// A single card row in the Events list.
Rectangle {
	id: row

	// Bound from the ListView model roles (notification item fields).
	property string notificationId: model.id ? model.id : ""
	property string title: model.title ? model.title : ""
	property string preview: model.preview ? model.preview : ""
	property string iconName: model.iconName ? model.iconName : ""
	property string severity: model.severity ? model.severity : "Info"
	property bool isRead: model.isRead ? true : false
	property string sourceType: model.sourceType ? model.sourceType : ""
	property string sourceId: model.sourceId ? model.sourceId : ""
	property string targetRoute: model.targetRoute ? model.targetRoute : ""
	property string createdAt: model.createdAt ? model.createdAt : ""

	property color accentColor: severityColor(row.severity)

	width: ListView.view ? ListView.view.width : implicitWidth
	height: contentColumn.height + 2 * Style.marginL
	color: rowMouseArea.containsMouse ? Style.backgroundColor2 : Style.backgroundColor

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

	function formatTime(iso){
		if (!iso || iso === ""){
			return ""
		}
		var parsed = new Date(iso)
		if (isNaN(parsed.getTime())){
			return iso
		}
		var now = new Date()
		if (parsed.toDateString() === now.toDateString()){
			return Qt.formatTime(parsed, "hh:mm")
		}
		return Qt.formatDateTime(parsed, "dd.MM hh:mm")
	}

	Rectangle {
		id: unreadMarker

		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		anchors.leftMargin: Style.marginS
		width: Style.marginS
		height: width
		radius: width / 2
		color: row.accentColor
		visible: !row.isRead
	}

	Image {
		id: icon

		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.top: parent.top
		anchors.topMargin: Style.marginL
		width: Style.iconSizeM
		height: width
		visible: row.iconName !== ""
		source: row.iconName !== "" ? "../../../" + Style.getIconPath(row.iconName, Icon.State.On, Icon.Mode.Normal) : ""
		sourceSize.width: width
		sourceSize.height: height
	}

	Column {
		id: contentColumn

		anchors.left: icon.right
		anchors.leftMargin: Style.marginM
		anchors.right: rightColumn.left
		anchors.rightMargin: Style.marginM
		anchors.verticalCenter: parent.verticalCenter
		spacing: Style.marginXS

		BaseText {
			id: titleText

			width: parent.width
			text: row.title
			font.bold: !row.isRead
			elide: Text.ElideRight
			maximumLineCount: 1
		}

		BaseText {
			id: previewText

			width: parent.width
			text: row.preview
			color: Style.subtitleColor
			elide: Text.ElideRight
			maximumLineCount: 2
			wrapMode: Text.WordWrap
			visible: row.preview !== ""
		}
	}

	Column {
		id: rightColumn

		anchors.right: parent.right
		anchors.rightMargin: Style.marginL
		anchors.top: parent.top
		anchors.topMargin: Style.marginL
		spacing: Style.marginXS
		width: Math.max(timeText.width, deleteButton.width)

		BaseText {
			id: timeText

			anchors.right: parent.right
			text: row.formatTime(row.createdAt)
			color: Style.inactiveTextColor
			font.pixelSize: Style.fontSizeXS
		}

		ToolButton {
			id: deleteButton

			anchors.right: parent.right
			width: Style.iconSizeS + Style.marginS
			height: width
			visible: rowMouseArea.containsMouse
			iconSource: Style.getIconPath("Icons/Delete", Icon.State.On, Icon.Mode.Normal)
			decorator: Component {
				ToolButtonDecorator {
					color: "transparent"
					icon.width: Style.iconSizeXS
				}
			}
			onClicked: ActivityController.remove(row.notificationId)
		}
	}

	Rectangle {
		id: separator

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		height: Style.buttonBorderWidth
		color: Style.borderColor
	}

	MouseArea {
		id: rowMouseArea

		anchors.fill: parent
		anchors.rightMargin: rightColumn.width + 2 * Style.marginL
		hoverEnabled: true
		cursorShape: row.targetRoute !== "" ? Qt.PointingHandCursor : Qt.ArrowCursor
		onClicked: ActivityController.openNotification(row.notificationId, row.targetRoute, row.sourceType, row.sourceId, row.title)
	}
}
