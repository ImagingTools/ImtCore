// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: chatSearchBarRoot
	objectName: "ChatSearchBar"
	color: Style.backgroundColor
	radius: Style.radiusS
	height: Style.buttonHeightM

	property string placeholderText: qsTr("Search...")
	property string searchText: textInput.text

	signal searchTextChanged(string text)

	Row {
		anchors.fill: parent
		anchors.leftMargin: Style.paddingS
		anchors.rightMargin: Style.paddingS
		spacing: Style.paddingS

		// Search icon
		Text {
			text: "🔍"
			font.pixelSize: Style.fontSizeS
			anchors.verticalCenter: parent.verticalCenter
			color: Style.buttonInactiveTextColor
		}

		TextInput {
			id: textInput
			width: parent.width - Style.paddingS - Style.iconSizeXS - clearBtn.width - Style.paddingS * 2
			anchors.verticalCenter: parent.verticalCenter
			font.pixelSize: Style.fontSizeXS
			color: Style.imaginToolsAccentColor
			clip: true
			selectByMouse: true

			Text {
				anchors.fill: parent
				text: chatSearchBarRoot.placeholderText
				font.pixelSize: Style.fontSizeXS
				verticalAlignment: Text.AlignVCenter
				visible: textInput.text.length === 0
			}

			onTextChanged: {
				chatSearchBarRoot.searchTextChanged(text);
			}

			Keys.onEscapePressed: {
				textInput.text = "";
			}
		}

		// Clear button
		Text {
			id: clearBtn
			visible: textInput.text.length > 0
			text: "×"
			font.pixelSize: Style.fontSizeM
			color: Style.buttonInactiveTextColor
			anchors.verticalCenter: parent.verticalCenter

			MouseArea {
				anchors.fill: parent
				onClicked: {
					textInput.text = ""
				}
			}
		}
	}
}
