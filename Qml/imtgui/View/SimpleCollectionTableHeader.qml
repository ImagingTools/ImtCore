// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/**
 * SimpleCollectionTableHeader
 *
 * Select-all bar above a collection list. Presentation only: it renders the
 * tri-state checkbox and the selection summary, and reports clicks through
 * `selectAllToggled`. Deciding what a toggle means is the caller's job.
 */
Item {
	id: root

	property int selectedCount: 0
	property int checkState: Qt.Unchecked
	readonly property int checkBoxSize: Style.itemSizeS + Style.marginXS

	signal selectAllToggled()

	// Callers that hide the bar collapse it with `height: visible ? implicitHeight : 0`.
	implicitHeight: Style.controlHeightL + Style.marginM
	height: implicitHeight

	Rectangle {
		anchors.fill: parent
		color: Style.backgroundColor2
	}

	Row {
		id: headerRow
		anchors.left: parent.left
		anchors.leftMargin: Style.marginL
		anchors.verticalCenter: parent.verticalCenter
		spacing: Style.marginL

		CheckBox {
			id: selectAllCheckBox
			anchors.verticalCenter: parent.verticalCenter
			checkState: root.checkState
			tristate: true
			height: root.checkBoxSize
			width: root.checkBoxSize
		}

		BaseText {
			id: summaryText
			anchors.verticalCenter: parent.verticalCenter
			text: root.selectedCount > 0
				? qsTr("%1 selected").arg(root.selectedCount)
				: qsTr("Select all")
			font.pixelSize: Style.fontSizeS
			color: Style.inactiveTextColor
		}
	}

	// Covers the checkbox and its label - clicking either toggles, which is
	// what users expect from a select-all row.
	MouseArea {
		anchors.left: headerRow.left
		anchors.top: headerRow.top
		anchors.bottom: headerRow.bottom
		width: headerRow.width
		cursorShape: Qt.PointingHandCursor
		onClicked: root.selectAllToggled()
	}

	Rectangle {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		height: 1
		color: Style.borderColor
	}
}
