// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/**
 * SimpleCollectionStateOverlay
 *
 * The "nothing useful to show" band of a collection view: initial-loading,
 * error and empty-list messages, in that order of precedence.
 *
 * Error deliberately outranks emptiness *and* is shown even when the list
 * still holds items, so a refresh that fails over an already-populated list
 * is still reported instead of silently leaving stale rows on screen. The
 * band collapses to zero height when there is nothing to say.
 */
Item {
	id: root

	// Whether the list it describes currently has no rows.
	property bool isEmpty: false
	property bool initialLoading: false
	property string errorMessage: ""
	property string emptyText: ""

	readonly property bool hasError: root.errorMessage !== ""
	readonly property string stateText: root.hasError
		? root.errorMessage
		: (root.initialLoading ? qsTr("Loading...") : root.emptyText)

	// Emitted when the user clicks the error message.
	signal retryRequested()

	visible: root.hasError || root.isEmpty
	height: visible ? Style.controlHeightL + Style.marginL : 0

	BaseText {
		anchors.centerIn: parent
		text: root.stateText
		font.pixelSize: Style.fontSizeM
		color: root.hasError ? Style.errorColor : Style.inactiveTextColor
	}

	MouseArea {
		anchors.fill: parent
		enabled: root.hasError && !root.initialLoading
		cursorShape: Qt.PointingHandCursor
		onClicked: root.retryRequested()
	}
}
