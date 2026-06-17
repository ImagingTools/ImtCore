// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import imtcontrols 1.0
import Acf 1.0

/**
 * TenantTableHeader
 *
 * Header row for the tenant table with:
 * - A CheckBox for select all / deselect all
 * - Display of selected count
 * - Slot for future filter controls
 *
 * Sits inside TenantTableContainer so the container's border provides
 * the outer rounded frame. Header draws its own background (flat)
 * and a bottom separator.
 */
Item {
	id: tableHeader

	property int selectedCount: 0
	property int totalCount: 0
	property int checkState: Qt.Unchecked
	readonly property int checkBoxSize: Style.itemSizeS + Style.marginXS

	signal selectAllToggled()

	height: Style.controlHeightL + Style.marginM
	width: parent.width

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
			checkState: tableHeader.checkState
			tristate: true
			height: tableHeader.checkBoxSize
			width: tableHeader.checkBoxSize
		}

		BaseText {
			anchors.verticalCenter: parent.verticalCenter
			text: tableHeader.selectedCount > 0
				? qsTr("%1 selected").arg(tableHeader.selectedCount)
				: qsTr("Select all")
			font.pixelSize: Style.fontSizeS
			color: Style.inactiveTextColor
		}
	}

	MouseArea {
		anchors.left: headerRow.left
		anchors.top: headerRow.top
		anchors.bottom: headerRow.bottom
		width: selectAllCheckBox.width + Style.marginM
		cursorShape: Qt.PointingHandCursor
		onClicked: tableHeader.selectAllToggled()
	}

	// Bottom separator
	Rectangle {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		height: 1
		color: Style.borderColor
	}
}
