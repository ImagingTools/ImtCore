// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import imtcontrols 1.0

Column {
	id: root

	property alias titleText: titleInput.text
	property alias descriptionText: descriptionInput.text
	property bool titleReadOnly: false
	property bool descriptionReadOnly: false
	property string titleLabelText: qsTr("Title")
	property string descriptionLabelText: qsTr("Description")
	property string titlePlaceholderText: qsTr("Enter ticket title...")
	property string descriptionPlaceholderText: qsTr("Describe the issue...")
	property int minDescriptionHeight: 96
	property int maxDescriptionHeight: 220
	readonly property int _descriptionInnerHeight: Math.min(root.maxDescriptionHeight, Math.max(root.minDescriptionHeight, descriptionInput.contentHeight))

	width: parent ? parent.width : implicitWidth
	spacing: Style.spacingM

	function clearFields() {
		titleInput.text = ""
		descriptionInput.text = ""
	}

	function focusTitle() {
		titleInput.forceActiveFocus()
	}

	Column {
		width: parent.width
		spacing: 4

		Text {
			text: root.titleLabelText
			font.pixelSize: Style.fontSizeM
			font.bold: true
			color: Style.textColor
		}

		CustomTextField {
			id: titleInput
			width: parent.width
			height: Style.controlHeightM
			placeHolderText: root.titlePlaceholderText
			readOnly: root.titleReadOnly
		}
	}

	Column {
		width: parent.width
		spacing: 4

		Text {
			text: root.descriptionLabelText
			font.pixelSize: Style.fontSizeM
			font.bold: true
			color: Style.textColor
		}

		Rectangle {
			width: parent.width
			height: root._descriptionInnerHeight + Style.paddingM * 2
			radius: Style.radiusM
			border.color: descriptionInput.activeFocus ? Style.accentColor : Style.borderColor
			border.width: descriptionInput.activeFocus ? 2 : 1
			color: Style.backgroundColor

			Flickable {
				id: descriptionFlick
				anchors.fill: parent
				anchors.margins: Style.paddingM
				anchors.rightMargin: Style.paddingM + Style.marginM
				contentWidth: width
				contentHeight: descriptionInput.height
				clip: true
				boundsBehavior: Flickable.StopAtBounds

				TextEdit {
					id: descriptionInput
					width: descriptionFlick.width
					height: Math.max(root._descriptionInnerHeight - Style.paddingM * 2, contentHeight)
					font.pixelSize: Style.fontSizeM
					color: Style.textColor
					wrapMode: TextEdit.Wrap
					textFormat: TextEdit.PlainText
					readOnly: root.descriptionReadOnly

					onCursorRectangleChanged: {
						var cy = cursorRectangle.y
						var ch = cursorRectangle.height
						if (cy < descriptionFlick.contentY) {
							descriptionFlick.contentY = cy
						} else if (cy + ch > descriptionFlick.contentY + descriptionFlick.height) {
							descriptionFlick.contentY = cy + ch - descriptionFlick.height
						}
					}

					Text {
						anchors.fill: parent
						text: root.descriptionPlaceholderText
						color: Style.inactiveTextColor
						font.pixelSize: Style.fontSizeM
						visible: descriptionInput.text.length === 0
					}
				}
			}

			CustomScrollbar {
				id: descriptionScrollV
				z: parent.z + 1
				anchors.right: parent.right
				anchors.rightMargin: 2
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				anchors.topMargin: 2
				anchors.bottomMargin: 2
				secondSize: Style.marginM
				targetItem: descriptionFlick
				visible: descriptionFlick.contentHeight > descriptionFlick.height
			}
		}
	}
}
