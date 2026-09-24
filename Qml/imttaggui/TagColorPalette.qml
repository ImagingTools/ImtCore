// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/*!
	\qmltype TagColorPalette
	\inqmlmodule imttaggui
	\brief Picks a tag color from a fixed palette or as a 6-digit hex value.
*/
Column {
	id: tagColorPaletteRoot
	objectName: "TagColorPalette"

	//! Hex color without the leading '#'.
	property string color: "ededed"

	property var paletteColors: ["b60205", "d93f0b", "fbca04", "0e8a16", "006b75", "1d76db", "0052cc", "5319e7", "e99695", "f9d0c4", "fef2c0", "c2e0c6", "bfdadc", "c5def5", "bfd4f2", "d4c5f9", "ededed", "d73a4a", "0075ca", "a2eeef"]

	readonly property bool isValid: tagColorPaletteRoot.isValidColor(tagColorPaletteRoot.color)

	signal colorEdited(string color)

	spacing: Style.spacingM

	function isValidColor(value){
		if (value.length !== 6){
			return false
		}

		for (var i = 0; i < value.length; i++){
			if ("0123456789abcdef".indexOf(value.charAt(i)) < 0){
				return false
			}
		}

		return true
	}

	function setColor(value){
		var normalized = value.trim().toLowerCase()
		if (normalized.charAt(0) === "#"){
			normalized = normalized.substring(1)
		}

		tagColorPaletteRoot.color = normalized
		tagColorPaletteRoot.colorEdited(normalized)
	}

	function setRandomColor(){
		var index = Math.floor(Math.random() * tagColorPaletteRoot.paletteColors.length)
		tagColorPaletteRoot.setColor(tagColorPaletteRoot.paletteColors[index])
	}

	Flow {
		width: parent.width
		spacing: Style.spacingS

		Repeater {
			model: tagColorPaletteRoot.paletteColors

			delegate: Rectangle {
				width: Style.iconSizeS * 1.5
				height: width
				radius: Style.radiusS
				color: "#" + String(modelData)
				border.width: String(modelData) === tagColorPaletteRoot.color ? 2 : 1
				border.color: String(modelData) === tagColorPaletteRoot.color ? Style.textColor : Style.borderColor

				MouseArea {
					anchors.fill: parent
					cursorShape: Qt.PointingHandCursor
					onClicked: {
						tagColorPaletteRoot.setColor(String(modelData))
					}
				}
			}
		}
	}

	Row {
		spacing: Style.spacingS

		Rectangle {
			anchors.verticalCenter: parent.verticalCenter
			width: Style.iconSizeS * 1.5
			height: width
			radius: Style.radiusS
			color: tagColorPaletteRoot.isValid ? "#" + tagColorPaletteRoot.color : "transparent"
			border.width: 1
			border.color: tagColorPaletteRoot.isValid ? Style.borderColor : Style.errorTextColor
		}

		TextField {
			id: colorField
			anchors.verticalCenter: parent.verticalCenter
			width: Style.sizeHintXXS
			placeHolderText: qsTr("Hex color")
			text: tagColorPaletteRoot.color
			maximumLength: 7

			onEditingFinished: {
				tagColorPaletteRoot.setColor(colorField.text)
			}
		}

		Button {
			anchors.verticalCenter: parent.verticalCenter
			text: qsTr("Random")
			onClicked: {
				tagColorPaletteRoot.setRandomColor()
			}
		}
	}
}
