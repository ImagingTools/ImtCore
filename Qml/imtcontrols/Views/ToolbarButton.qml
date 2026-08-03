import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

// Flat command for the toolbars of TreeExplorerView and CheckableListPanel.
// Secondary commands carry no frame at all and only tint on hover; the primary
// one is a filled accent pill. Its leading "+" is drawn rather than loaded from
// the icon set, because the themed icons are dark and would disappear on it.
Item {
	id: control

	property string text: ""
	property string iconSource: ""
	property bool primary: false
	property bool plusGlyph: false
	property bool active: true
	// Menu entries read as a list, so their content starts at the left edge
	// instead of sitting in the middle of a stretched row.
	property bool leftAligned: false
	property string tooltipText: ""

	property color contentColor: !control.active ? Style.inactiveTextColor
		: control.primary ? Style.baseColor : Style.textColor

	signal clicked()

	height: Style.controlHeightM
	width: Math.max(Style.controlHeightM, contentRow.width + 2 * Style.marginL)

	Rectangle {
		anchors.fill: parent
		radius: height / 2
		color: !control.active ? (control.primary ? Style.backgroundColor2 : "transparent")
			: control.primary ? (commandArea.containsMouse ? Style.textSelectedColor : Style.titleColor)
			: commandArea.containsMouse ? Style.hover : "transparent"
	}

	Row {
		id: contentRow
		anchors.left: control.leftAligned ? parent.left : undefined
		anchors.leftMargin: control.leftAligned ? Style.marginM : 0
		anchors.horizontalCenter: control.leftAligned ? undefined : parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		spacing: Style.spacingS

		Item {
			anchors.verticalCenter: parent.verticalCenter
			visible: control.plusGlyph
			width: visible ? Style.iconSizeXS : 0
			height: Style.iconSizeXS

			Rectangle {
				anchors.centerIn: parent
				width: parent.width
				height: 2
				radius: 1
				color: control.contentColor
			}
			Rectangle {
				anchors.centerIn: parent
				width: 2
				height: parent.height
				radius: 1
				color: control.contentColor
			}
		}

		Image {
			anchors.verticalCenter: parent.verticalCenter
			visible: control.iconSource !== ""
			width: visible ? Style.iconSizeS : 0
			height: Style.iconSizeS
			source: control.iconSource
			sourceSize.width: width
			sourceSize.height: height
		}

		BaseText {
			anchors.verticalCenter: parent.verticalCenter
			visible: control.text !== ""
			text: control.text
			font.family: control.primary ? Style.fontFamilyBold : Style.fontFamily
			color: control.contentColor
		}
	}

	onActiveChanged: {
		if (!control.active)
			tooltip.hide()
	}

	onVisibleChanged: {
		if (!control.visible)
			tooltip.hide()
	}

	MouseArea {
		id: commandArea
		anchors.fill: parent
		hoverEnabled: true
		cursorShape: control.active ? Qt.PointingHandCursor : Qt.ArrowCursor
		onClicked: {
			if (control.active)
				control.clicked()
		}
		onPositionChanged: {
			if (control.tooltipText !== "" && control.active)
				tooltip.show(mouseX, mouseY)
		}
		onExited: {
			tooltip.hide()
		}
		// The grab can be taken away without the pointer ever leaving.
		onCanceled: {
			tooltip.hide()
		}
	}

	CustomTooltip {
		id: tooltip
		text: control.tooltipText
		fitToTextWidth: true
	}
}
