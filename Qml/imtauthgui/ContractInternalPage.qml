// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractInternalPage
 *
 * What the organization keeps about a contract for itself: its own number, notes and the reminders of the deadlines.
 * The other parties never see it. A page of ContractView: it shows the working copy of the view and writes it back.
 */
Item {
	id: internalPage

	anchors.fill: parent

	// ContractView
	property var editor: null

	readonly property bool __canEdit: internalPage.editor ? internalPage.editor.canEdit && internalPage.editor.myParty !== null : false

	function updateGui() {
		if (!internalPage.editor)
			return
		var data = internalPage.editor.internalData
		internalNumberInput.text = data.internalNumber || ""
		notesInput.text = data.notes || ""
		reminderDaysInput.value = data.reminderDays || 0
	}

	function updateModel() {
		if (!internalPage.editor)
			return
		var data = internalPage.editor.internalData
		data.internalNumber = internalNumberInput.text
		data.notes = notesInput.text
		data.reminderDays = reminderDaysInput.value
	}

	CustomScrollbar {
		z: flickable.z + 1
		anchors.right: parent.right
		anchors.top: flickable.top
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		targetItem: flickable
	}

	Flickable {
		id: flickable

		anchors.fill: parent
		anchors.margins: Style.marginXL
		contentHeight: internalColumn.height
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: internalColumn

			width: Math.min(flickable.width - Style.marginM, Style.contentWidthMax)
			spacing: Style.marginL

			BaseText {
				width: parent.width
				wrapMode: Text.WordWrap
				text: qsTr("Seen by your organization only.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			GroupElementView {
				width: parent.width

				TextInputElementView {
					id: internalNumberInput
					placeHolderText: qsTr("e.g. 2026-017")
					name: qsTr("Internal number")
					readOnly: !internalPage.__canEdit
				}

				TextInputElementView {
					id: notesInput
					placeHolderText: qsTr("Seen only by your organization")
					name: qsTr("Notes")
					readOnly: !internalPage.__canEdit
				}

				SpinBoxElementView {
					id: reminderDaysInput
					name: qsTr("Remind of the deadlines")
					description: qsTr("Days before a deadline; 0: the default of the system")
					readOnly: !internalPage.__canEdit
					from: 0
					units: qsTr("days")
					to: 36500
				}
			}

			Button {
				variant: "primary"
				visible: internalPage.__canEdit
				enabled: internalPage.editor ? !internalPage.editor.isBusy : false
				text: qsTr("Save internal data")
				onClicked: {
					internalPage.editor.saveInternalData()
				}
			}
		}
	}
}
