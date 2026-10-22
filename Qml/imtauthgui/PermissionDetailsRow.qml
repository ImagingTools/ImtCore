// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/**
 * PermissionDetailsRow
 *
 * A labelled line of PermissionDetailsPanel. A text that does not fit is shown in full in a
 * tooltip and can be expanded in place, one part per line, scrolling when it is very long.
 */
Item {
	id: detailsRow

	property string label: ""
	property string text: ""
	// Splits the expanded text into lines; empty keeps it whole.
	property string separator: ""
	property real labelWidth: labelText.implicitWidth
	property real maximumExpandedHeight: Style.sizeHintXXS
	property bool expanded: false
	readonly property real labelImplicitWidth: labelText.implicitWidth

	readonly property bool __overflow: measureText.implicitWidth > valueText.width + 1
	readonly property string __fullText: detailsRow.separator !== "" ? detailsRow.text.split(detailsRow.separator).join("\n") : detailsRow.text

	height: detailsRow.expanded ? Math.max(labelText.height, expandedArea.height) : labelText.height

	onTextChanged: detailsRow.expanded = false

	BaseText {
		id: labelText
		anchors.left: parent.left
		anchors.top: parent.top
		width: detailsRow.labelWidth
		text: detailsRow.label
		font.pixelSize: Style.fontSizeS
		color: Style.inactiveTextColor
	}

	// Measured apart: the web runtime caps implicitWidth of an eliding Text at its width.
	BaseText {
		id: measureText
		visible: false
		text: detailsRow.text
		font.pixelSize: Style.fontSizeS
	}

	BaseText {
		id: valueText
		objectName: "DetailsValue"
		anchors.left: labelText.right
		anchors.leftMargin: Style.marginM
		anchors.right: toggleText.visible ? toggleText.left : parent.right
		anchors.rightMargin: toggleText.visible ? Style.marginM : 0
		anchors.top: parent.top
		visible: !detailsRow.expanded
		text: detailsRow.text !== "" ? detailsRow.text : "—"
		font.pixelSize: Style.fontSizeS
		color: Style.textColor
		elide: Text.ElideRight

		MouseArea {
			id: valueMouseArea
			anchors.fill: parent
			hoverEnabled: true
			acceptedButtons: Qt.NoButton
		}

		TooltipArea {
			text: detailsRow.__overflow ? detailsRow.__fullText : ""
			mouseArea: valueMouseArea
		}
	}

	Flickable {
		id: expandedArea
		anchors.left: labelText.right
		anchors.leftMargin: Style.marginM
		anchors.right: toggleText.left
		anchors.rightMargin: Style.marginM
		anchors.top: parent.top
		visible: detailsRow.expanded
		height: Math.min(expandedText.implicitHeight, detailsRow.maximumExpandedHeight)
		contentHeight: expandedText.implicitHeight
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		BaseText {
			id: expandedText
			objectName: "DetailsExpandedValue"
			width: expandedArea.width
			text: detailsRow.__fullText
			font.pixelSize: Style.fontSizeS
			color: Style.textColor
			// A path has no spaces to break at.
			wrapMode: Text.WrapAnywhere
		}
	}

	BaseText {
		id: toggleText
		objectName: "DetailsToggle"
		anchors.right: parent.right
		anchors.top: parent.top
		visible: detailsRow.__overflow || detailsRow.expanded
		text: detailsRow.expanded ? qsTr("Hide") : qsTr("Show all")
		font.pixelSize: Style.fontSizeS
		color: Style.linkColor

		MouseArea {
			objectName: "MouseArea"
			anchors.fill: parent
			cursorShape: Qt.PointingHandCursor
			onClicked: detailsRow.expanded = !detailsRow.expanded
		}
	}
}
