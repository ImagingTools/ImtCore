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

	//! Tab order around the palette: swatches (arrow keys pick), hex field, "Random".
	property Item previousItem: null
	property Item nextItem: null
	readonly property Item firstItem: swatchFlow
	readonly property Item lastItem: randomButton

	signal colorEdited(string color)

	//! Enter in the hex field.
	signal accepted()

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

	function moveSelection(step){
		var index = tagColorPaletteRoot.paletteColors.indexOf(tagColorPaletteRoot.color) + step
		if (index < 0){
			index = tagColorPaletteRoot.paletteColors.length - 1
		}
		else if (index >= tagColorPaletteRoot.paletteColors.length){
			index = 0
		}

		tagColorPaletteRoot.setColor(tagColorPaletteRoot.paletteColors[index])
	}

	function setRandomColor(){
		var index = Math.floor(Math.random() * tagColorPaletteRoot.paletteColors.length)
		tagColorPaletteRoot.setColor(tagColorPaletteRoot.paletteColors[index])
	}

	Flow {
		id: swatchFlow
		objectName: "TagColorSwatches"
		width: parent.width
		spacing: Style.spacingS

		KeyNavigation.tab: colorField
		KeyNavigation.backtab: tagColorPaletteRoot.previousItem

		Keys.onLeftPressed: {
			tagColorPaletteRoot.moveSelection(-1)
		}

		Keys.onRightPressed: {
			tagColorPaletteRoot.moveSelection(1)
		}

		Repeater {
			model: tagColorPaletteRoot.paletteColors

			delegate: Rectangle {
				width: Style.iconSizeS * 1.5
				height: width
				radius: Style.radiusS
				color: "#" + String(modelData)
				border.width: String(modelData) === tagColorPaletteRoot.color ? 2 : 1
				border.color: String(modelData) !== tagColorPaletteRoot.color ? Style.borderColor : (swatchFlow.activeFocus ? Style.textSelectedColor : Style.textColor)

				MouseArea {
					anchors.fill: parent
					cursorShape: Qt.PointingHandCursor
					onClicked: {
						tagColorPaletteRoot.setColor(String(modelData))
						swatchFlow.forceActiveFocus()
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
			objectName: "TagColorHexField"
			anchors.verticalCenter: parent.verticalCenter
			width: Style.sizeHintXXS
			placeHolderText: qsTr("Hex color")
			text: tagColorPaletteRoot.color
			maximumLength: 7

			KeyNavigation.tab: randomButton
			KeyNavigation.backtab: swatchFlow

			onEditingFinished: {
				tagColorPaletteRoot.setColor(colorField.text)
			}

			onAccepted: {
				tagColorPaletteRoot.setColor(colorField.text)
				tagColorPaletteRoot.accepted()
			}
		}

		Button {
			id: randomButton
			objectName: "TagColorRandomButton"
			anchors.verticalCenter: parent.verticalCenter
			text: qsTr("Random")

			KeyNavigation.tab: tagColorPaletteRoot.nextItem
			KeyNavigation.backtab: colorField

			onClicked: {
				tagColorPaletteRoot.setRandomColor()
			}
		}
	}
}
