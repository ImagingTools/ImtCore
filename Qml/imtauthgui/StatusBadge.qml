// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import imtcontrols 1.0

/**
 * StatusBadge
 *
 * Small rounded badge with a centered label, used for role / status labels
 * (Owner, Admin, Member, Active, Pending, Expired, Revoked, …).
 *
 * Defaults match the inactive-style badge previously inlined in TenantEditor.
 */
Rectangle {
	id: badge

	property alias text: label.text
	property color textColor: Style.textColor
	property color badgeColor: Style.baseColor
	property color badgeBorderColor: Style.borderColor
	property int badgeBorderWidth: 1
	property real horizontalPadding: Style.marginL
	property real verticalPadding: Style.marginS
	property int fontPixelSize: Style.fontSizeS
	property bool boldText: false

	width: label.implicitWidth + horizontalPadding
	height: label.implicitHeight + verticalPadding
	radius: Style.radiusS
	color: badge.badgeColor
	border.color: badge.badgeBorderColor
	border.width: badge.badgeBorderWidth

	BaseText {
		id: label
		anchors.centerIn: parent
		font.pixelSize: badge.fontPixelSize
		font.bold: badge.boldText
		color: badge.textColor
	}
}
