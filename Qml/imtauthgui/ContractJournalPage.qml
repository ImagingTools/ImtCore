// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractJournalPage
 *
 * The journal of a contract as the organization sees it: what every party did, and the internal entries of its own.
 * A page of ContractView.
 */
Item {
	id: journalPage

	anchors.fill: parent

	// ContractView
	property var editor: null

	readonly property var eventRows: journalPage.__eventRows(journalPage.editor ? journalPage.editor.events : [])

	function __eventRows(events) {
		var rows = []
		// The newest first
		for (var i = events.length - 1; i >= 0; i--) {
			var event = events[i]
			rows.push({"id": event.id || String(i),
					   "time": ContractTexts.dateTimeText(event.time),
					   "event": ContractTexts.eventText(event.eventType) + (event.isPrivate ? " · " + qsTr("internal") : ""),
					   "organization": event.tenantName || "",
					   "user": event.userName || "",
					   "comment": event.comment || ""})
		}
		return rows
	}

	BaseText {
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		visible: journalPage.eventRows.length === 0
		text: qsTr("No entries yet.")
		font.pixelSize: Style.fontSizeS
		color: Style.inactiveTextColor
	}

	Table {
		id: eventsTable

		anchors.fill: parent
		anchors.margins: Style.marginXL

		visible: journalPage.eventRows.length > 0
		selectable: false
		elements: journalPage.eventRows
	}

	TableHeaders {
		id: eventsHeaders

		Component.onCompleted: {
			addHeader("time", qsTr("Time"), false, false)
			addHeader("event", qsTr("Event"), false, false)
			addHeader("organization", qsTr("Organization"), false, false)
			addHeader("user", qsTr("User"), false, false)
			addHeader("comment", qsTr("Comment"), false, false)
			eventsTable.headers = eventsHeaders
		}
	}
}
