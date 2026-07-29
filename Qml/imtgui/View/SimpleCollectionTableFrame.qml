// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0

/**
 * SimpleCollectionTableFrame
 *
 * Rounded, clipped border around a collection table. Presentation only: it
 * draws the frame and hosts content, it does not decide where it sits or how
 * wide it is - the caller anchors and sizes it (see SimpleCollectionTable).
 *
 * Declared children land in the content area, inset by `borderWidth` so they
 * never overdraw the rounded border.
 */
Item {
	id: root

	// Border thickness, and therefore the inset of the content area. Callers
	// sizing themselves to their content must add `borderWidth * 2`.
	readonly property int borderWidth: 1

	default property alias content: contentItem.data

	Rectangle {
		anchors.fill: parent
		radius: Style.radiusM
		color: "transparent"
		border.color: Style.borderColor
		border.width: root.borderWidth
		clip: true

		Item {
			id: contentItem
			anchors.fill: parent
			anchors.margins: root.borderWidth
		}
	}
}
