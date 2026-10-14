import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

// Full values of the row selected in a table whose cells cut them short:
// a title, a key line (path or identifier) that can be copied, and the
// description. Keeps one height whatever is selected, so the table above it
// does not jump while the selection moves.
Rectangle {
	id: details

	property string title: ""
	property string keyText: ""
	property string description: ""
	property string placeholderText: qsTr("Select a row to see it in full here")
	property bool copyVisible: false
	property string copyIconSource: ""
	property string copyTooltipText: qsTr("Copy")

	signal copyRequested()

	height: contentColumn.height + 2 * Style.marginM
	radius: Style.radiusM
	color: Style.alternateBaseColor
	border.color: Style.borderColor
	border.width: 1

	Column {
		id: contentColumn
		anchors.top: parent.top
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.leftMargin: Style.marginL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginL
		spacing: Style.spacingXS

		Item {
			width: contentColumn.width
			height: Style.buttonWidthM

			BaseText {
				objectName: "DetailsTitle"
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				text: details.title !== "" ? details.title : details.placeholderText
				font.family: details.title !== "" ? Style.fontFamilyBold : Style.fontFamily
				color: details.title !== "" ? Style.textColor : Style.inactiveTextColor
				elide: Text.ElideRight
			}
		}

		Item {
			width: contentColumn.width
			height: Style.buttonWidthM

			BaseText {
				objectName: "DetailsKey"
				anchors.left: parent.left
				anchors.right: copyButton.visible ? copyButton.left : parent.right
				anchors.rightMargin: copyButton.visible ? Style.marginS : 0
				anchors.verticalCenter: parent.verticalCenter
				text: details.keyText
				font.pixelSize: Style.fontSizeS
				color: Style.subtitleColor
				// The tail of a path tells it apart from its siblings.
				elide: Text.ElideLeft
			}

			ToolButton {
				id: copyButton
				objectName: "DetailsCopyButton"
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				width: Style.buttonWidthM
				height: width
				visible: details.copyVisible && details.keyText !== ""
				iconSource: details.copyIconSource
				tooltipText: details.copyTooltipText
				onClicked: details.copyRequested()
			}
		}

		BaseText {
			objectName: "DetailsDescription"
			width: contentColumn.width
			height: 2 * Math.ceil(Style.fontSizeS * 1.5)
			text: details.description
			font.pixelSize: Style.fontSizeS
			color: Style.subtitleColor
			wrapMode: Text.WordWrap
			maximumLineCount: 2
			elide: Text.ElideRight
		}
	}
}
