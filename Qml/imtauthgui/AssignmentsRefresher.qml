// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12

/**
 * AssignmentsRefresher
 *
 * Re-reads the inherited assignments of an open document after the editor changed its direct ones.
 * The read is sent only when no document update is in flight, so the server has applied the change.
 * Requests made meanwhile are coalesced into one, and an answer that an update overtook is dropped
 * and asked again.
 *
 * Usage: request() after the change, send the read on sendRequested(), then call finished() with the
 * answer and apply it only when it returns true, or failed() on an error.
 */
QtObject {
	id: root

	// DocumentRepresentationController whose document updates are tracked.
	property var controller: null

	readonly property bool busy: root.__requested || root.__inFlight

	property bool __requested: false
	property bool __inFlight: false
	property int __pendingUpdates: 0
	property int __version: 0
	property int __sentVersion: 0

	signal sendRequested()

	function request(){
		root.__requested = true
		root.__sendTimer.restart()
	}

	// Returns true when the answer is still current and may be applied.
	function finished(){
		root.__inFlight = false
		if (root.__sentVersion !== root.__version){
			root.__requested = true
		}
		root.__sendTimer.restart()

		return !root.__requested
	}

	function failed(){
		root.__inFlight = false
		root.__requested = false
	}

	function __trySend(){
		if (!root.__requested || root.__inFlight || root.__pendingUpdates > 0){
			return
		}

		root.__requested = false
		root.__inFlight = true
		root.__sentVersion = root.__version
		root.sendRequested()
	}

	property Timer __sendTimer: Timer {
		interval: 0
		repeat: false
		onTriggered: {
			root.__trySend()
		}
	}

	property Connections __controllerConnections: Connections {
		target: root.controller

		function onStartUpdateDocument(documentId){
			root.__pendingUpdates = root.__pendingUpdates + 1
			root.__version = root.__version + 1
		}

		function onDocumentUpdated(documentId){
			root.__pendingUpdates = Math.max(0, root.__pendingUpdates - 1)
			root.__sendTimer.restart()
		}

		function onUpdateDocumentFailed(documentId, message){
			root.__pendingUpdates = Math.max(0, root.__pendingUpdates - 1)
			root.__sendTimer.restart()
		}
	}
}
