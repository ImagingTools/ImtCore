// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/*!
	\qmltype TagChip
	\inqmlmodule imtcolgui
	\brief A tag rendered as a colored pill, like a GitHub label.
*/
Rectangle {
	id: tagChipRoot
	objectName: "TagChip"

	property string tagName: ""

	//! Hex color without the leading '#'.
	property string tagColor: "ededed"
	property bool isSystem: false

	width: chipText.width + Style.paddingM * 2
	height: chipText.height + Style.paddingS
	radius: height / 2
	color: "#" + tagChipRoot.tagColor
	border.width: 1
	border.color: Style.borderColor

	function isDarkColor(hexColor){
		if (hexColor.length !== 6){
			return false
		}

		var red = parseInt(hexColor.substring(0, 2), 16)
		var green = parseInt(hexColor.substring(2, 4), 16)
		var blue = parseInt(hexColor.substring(4, 6), 16)

		return (red * 299 + green * 587 + blue * 114) / 1000 < 150
	}

	BaseText {
		id: chipText
		anchors.centerIn: parent
		text: tagChipRoot.isSystem ? tagChipRoot.tagName + " •" : tagChipRoot.tagName
		font.pixelSize: Style.fontSizeXS
		color: tagChipRoot.isDarkColor(tagChipRoot.tagColor) ? "white" : "black"
	}
}
