// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import imtcontrols 1.0

/**
 * TenantTableHeader
 *
 * Header row for the tenant table with:
 * - A CheckBox for select all / deselect all
 * - Display of selected count
 * - Slot for future filter controls
 */
Item {
	id: tableHeader

	property int selectedCount: 0
	property int totalCount: 0
	property int checkState: Qt.Unchecked

	signal selectAllToggled()

	height: Style.controlHeightL
	width: parent.width

	Rectangle {
		anchors.fill: parent
		color: Style.backgroundColor2
		radius: Style.radiusM

		// Only round top corners
		Rectangle {
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			height: parent.radius
			color: parent.color
		}
	}

	Row {
		anchors.left: parent.left
		anchors.leftMargin: Style.marginM
		anchors.verticalCenter: parent.verticalCenter
		spacing: Style.marginM

		CheckBox {
			id: selectAllCheckBox
			anchors.verticalCenter: parent.verticalCenter
			checkState: tableHeader.checkState
			tristate: true
			height: Style.itemSizeS
			width: Style.itemSizeS

			onCheckStateChanged: {
				if (checkState !== tableHeader.checkState)
					tableHeader.selectAllToggled()
			}
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

	// Bottom separator
	Rectangle {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		height: 1
		color: Style.borderColor
	}
}
