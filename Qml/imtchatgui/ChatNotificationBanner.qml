// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: chatNotificationBannerRoot
	objectName: "ChatNotificationBanner"
	visible: false
	height: visible ? bannerContent.implicitHeight + Style.paddingS * 2 : 0
	color: Style.accentColor
	radius: Style.radiusS
	opacity: 0

	property string senderName: ""
	property string messagePreview: ""
	property string conversationId: ""

	signal bannerTapped(string conversationId)

	Behavior on opacity {
		NumberAnimation { duration: 200 }
	}

	Row {
		id: bannerContent
		anchors {
			left: parent.left
			right: parent.right
			verticalCenter: parent.verticalCenter
			leftMargin: Style.paddingM
			rightMargin: Style.paddingS
		}
		spacing: Style.paddingS

		// Avatar
		Rectangle {
			width: Style.avatarSizeXS
			height: Style.avatarSizeXS
			radius: width / 2
			color: "white"
			opacity: 0.3
			anchors.verticalCenter: parent.verticalCenter

			Text {
				anchors.centerIn: parent
				text: chatNotificationBannerRoot.senderName.charAt(0).toUpperCase()
				font.pixelSize: Style.fontSizeXS
				color: "white"
				font.bold: true
			}
		}

		Column {
			anchors.verticalCenter: parent.verticalCenter
			width: parent.width - Style.avatarSizeXS - Style.paddingS - dismissBtn.width - Style.paddingXS

			Text {
				text: chatNotificationBannerRoot.senderName
				font.pixelSize: Style.fontSizeXS
				font.bold: true
				color: "white"
			}

			Text {
				text: chatNotificationBannerRoot.messagePreview
				font.pixelSize: Style.fontSizeXS
				color: "white"
				opacity: 0.85
				elide: Text.ElideRight
				width: parent.width
			}
		}

		Text {
			id: dismissBtn
			text: "×"
			font.pixelSize: Style.fontSizeM
			color: "white"
			opacity: 0.75
			anchors.verticalCenter: parent.verticalCenter

			MouseArea {
				anchors.fill: parent
				onClicked: chatNotificationBannerRoot.dismiss()
			}
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			chatNotificationBannerRoot.bannerTapped(chatNotificationBannerRoot.conversationId);
			chatNotificationBannerRoot.dismiss();
		}
	}

	Timer {
		id: autoDismissTimer
		interval: 4000
		onTriggered: chatNotificationBannerRoot.dismiss()
	}

	function show(sender, preview, convId) {
		senderName = sender || "";
		messagePreview = preview || "";
		conversationId = convId || "";
		visible = true;
		opacity = 1.0;
		autoDismissTimer.restart();
	}

	function dismiss() {
		opacity = 0;
		dismissTimer.start();
	}

	Timer {
		id: dismissTimer
		interval: 200
		onTriggered: {
			chatNotificationBannerRoot.visible = false;
		}
	}
}
