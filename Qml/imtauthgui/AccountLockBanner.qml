// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

// Lock of an account after invalid sign-in attempts, with the unlock action of the superuser.
Rectangle {
	id: banner

	property bool locked: false
	// Seconds until the lock ends; negative while it lasts until the account is unlocked.
	property int remainingSecs: -1
	property bool canUnlock: false
	property bool unlocking: false

	signal unlockRequested()

	height: visible ? Math.max(textColumn.height, unlockButton.height) + 2 * Style.marginM : 0
	visible: banner.locked
	radius: Style.radiusL
	color: Style.attentionSubtleColor
	border.width: 1
	border.color: Style.attentionColor

	Image {
		id: lockIcon

		anchors.left: parent.left
		anchors.leftMargin: Style.marginM
		anchors.verticalCenter: parent.verticalCenter
		width: Style.iconSizeM
		height: width
		sourceSize.width: width
		sourceSize.height: height
		source: "qrc:/" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal)
	}

	Column {
		id: textColumn

		anchors.left: lockIcon.right
		anchors.leftMargin: Style.marginM
		anchors.right: unlockButton.visible ? unlockButton.left : parent.right
		anchors.rightMargin: Style.marginM
		anchors.verticalCenter: parent.verticalCenter
		spacing: Style.marginXS

		Text {
			objectName: "AccountLockTitle"
			width: parent.width
			text: qsTr("Account locked")
			color: Style.textColor
			font.family: Style.fontFamily
			font.pixelSize: Style.fontSizeM
			font.bold: true
			wrapMode: Text.WordWrap
		}

		Text {
			objectName: "AccountLockDescription"
			width: parent.width
			text: banner.remainingSecs < 0
					? qsTr("Too many invalid sign-in attempts. Sign-in stays blocked until the account is unlocked.")
					: qsTr("Too many invalid sign-in attempts. Sign-in is blocked for about %1 min.").arg(Math.max(1, Math.ceil(banner.remainingSecs / 60)))
			color: Style.textColor
			font.family: Style.fontFamily
			font.pixelSize: Style.fontSizeM
			wrapMode: Text.WordWrap
		}
	}

	Button {
		id: unlockButton

		objectName: "UnlockAccountButton"
		anchors.right: parent.right
		anchors.rightMargin: Style.marginM
		anchors.verticalCenter: parent.verticalCenter
		width: Style.buttonWidthXXL
		height: Style.controlHeightM
		visible: banner.canUnlock
		enabled: !banner.unlocking
		variant: "primary"
		text: qsTr("Unlock")
		onClicked: {
			banner.unlockRequested()
		}
	}
}
