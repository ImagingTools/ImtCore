// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/**
 * ProfileNoticeBanner
 *
 * Small in-dialog notification banner (success / error) for the Profile pages.
 * Each page owns its own instance and shows feedback for its own actions only —
 * notifications about what happened inside this dialog stay inside this dialog
 * rather than going through the app-wide PopupManager / ModalDialogManager.
 */
Rectangle {
	id: banner

	property string message: ""
	property bool isError: false

	width: parent ? parent.width : 0
	height: message !== "" ? contentRow.implicitHeight + 2 * Style.marginM : 0
	visible: message !== ""
	clip: true
	radius: Style.radiusL
	color: Style.baseColor
	border.width: 1
	border.color: banner.isError ? Style.negativeAccentColor : Style.positiveAccentColor

	Behavior on height {
		NumberAnimation { duration: 120 }
	}

	function show(text, error) {
		banner.isError = !!error
		banner.message = text || ""
		dismissTimer.restart()
	}

	function hide() {
		banner.message = ""
		dismissTimer.stop()
	}

	Timer {
		id: dismissTimer
		interval: 5000
		onTriggered: banner.hide()
	}

	Row {
		id: contentRow
		anchors.left: parent.left
		anchors.right: closeButton.left
		anchors.verticalCenter: parent.verticalCenter
		anchors.leftMargin: Style.marginM
		anchors.rightMargin: Style.marginS
		spacing: Style.marginS

		Rectangle {
			width: 8
			height: 8
			radius: 4
			anchors.verticalCenter: parent.verticalCenter
			color: banner.isError ? Style.negativeAccentColor : Style.positiveAccentColor
		}

		Text {
			width: contentRow.width - 8 - contentRow.spacing
			text: banner.message
			color: Style.textColor
			font.family: Style.fontFamily
			font.pixelSize: Style.fontSizeM
			wrapMode: Text.WordWrap
		}
	}

	ToolButton {
		id: closeButton
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXS
		anchors.verticalCenter: parent.verticalCenter
		width: Style.buttonWidthM
		height: width
		iconSource: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
		tooltipText: qsTr("Dismiss")
		onClicked: {
			banner.hide()
		}
	}
}
