// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import imtcontrols 1.0

/**
 * TenantTableContainer
 *
 * Reusable wrapper that provides:
 * - Fixed max width (1000px by default), centered on page
 * - Light gray border with rounding (unified border around header + content)
 * - Shrinks with page when viewport is smaller than maxWidth
 *
 * Content is placed inside the `contentItem` default property.
 */
Item {
	id: tableContainer

	property int maxTableWidth: 1000
	default property alias contentItem: innerContent.data

	anchors.horizontalCenter: parent.horizontalCenter
	width: Math.min(parent.width - Style.marginXL * 2, tableContainer.maxTableWidth)

	Rectangle {
		id: borderFrame
		anchors.fill: parent
		radius: Style.radiusM
		color: "transparent"
		border.color: Style.borderColor
		border.width: 1
		clip: true

		Item {
			id: innerContent
			anchors.fill: parent
			anchors.margins: 1
		}
	}
}
