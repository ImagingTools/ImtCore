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

	// --- JQML-compatible child relocation -------------------------------- //
	// Native Qt honours `default property alias` and parents declared children
	// into the alias target. JQML parents them onto the component root instead.
	// We therefore relocate non-own children into contentItem manually, exactly
	// as SimpleCollectionItemDelegateBase does for its content column.
	function __isOwnItem(child) {
		return child === frame
	}

	property bool __isRelocatingExternalChildren: false

	function __relocateExternalChildren() {
		if (root.__isRelocatingExternalChildren)
			return

		root.__isRelocatingExternalChildren = true
		var kids = []
		for (var i = 0; i < root.children.length; i++)
			kids.push(root.children[i])

		for (i = 0; i < kids.length; i++) {
			var child = kids[i]
			if (!child || root.__isOwnItem(child))
				continue
			child.parent = contentItem
		}
		root.__isRelocatingExternalChildren = false
	}

	onChildrenChanged: root.__relocateExternalChildren()
	Component.onCompleted: {
		root.__relocateExternalChildren()
	}

	Rectangle {
		id: frame
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
