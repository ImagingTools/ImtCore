// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractPeriodPage
 *
 * The period of a contract: when it comes into force, how long it runs, its renewal, the notice period
 * and the deadline of the signatures. The dates are picked in the calendar; an empty date is no date.
 * A page of ContractView: it shows the working copy of the view and writes it back.
 */
Item {
	id: periodPage

	anchors.fill: parent

	// ContractView
	property var editor: null

	readonly property bool __isEditable: periodPage.editor ? periodPage.editor.isEditable : false

	function updateGui() {
		if (!periodPage.editor)
			return
		var draft = periodPage.editor.draft
		ContractTexts.setPickerIso(effectiveFromInput, draft.effectiveFrom)
		termDaysInput.value = draft.termDays || 0
		ContractTexts.setPickerIso(validUntilInput, draft.validUntil)
		autoRenewSwitch.checked = draft.autoRenew === true
		renewalPeriodInput.value = draft.renewalPeriodDays || 0
		renewalNoticeInput.value = draft.renewalNoticeDays || 0
		terminationNoticeInput.value = draft.terminationNoticeDays || 0
		ContractTexts.setPickerIso(signDeadlineInput, draft.signDeadline)
		sequentialSwitch.checked = draft.signOrder === "Sequential"
	}

	function updateModel() {
		if (!periodPage.editor)
			return
		var draft = periodPage.editor.draft
		draft.effectiveFrom = ContractTexts.pickerIso(effectiveFromInput)
		draft.termDays = termDaysInput.value
		draft.validUntil = ContractTexts.pickerIso(validUntilInput)
		draft.autoRenew = autoRenewSwitch.checked
		draft.renewalPeriodDays = renewalPeriodInput.value
		draft.renewalNoticeDays = renewalNoticeInput.value
		draft.terminationNoticeDays = terminationNoticeInput.value
		draft.signDeadline = ContractTexts.pickerIso(signDeadlineInput)
		draft.signOrder = sequentialSwitch.checked ? "Sequential" : "Parallel"
	}

	CustomScrollbar {
		id: scrollbar
		z: parent.z + 1
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
		contentHeight: periodColumn.height
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: periodColumn

			width: Math.min(flickable.width - Style.marginM, Style.contentWidthMax)
			spacing: Style.marginL

			GroupElementView {
				width: parent.width

				DateTimePickerElementView {
					id: effectiveFromInput
					name: qsTr("In force from")
					description: qsTr("Empty from the conclusion")
					canShowCurrentDate: false
					readOnly: !periodPage.__isEditable
				}

				IntegerInputElementView {
					id: termDaysInput
					placeHolderText: qsTr("0 for no term")
					name: qsTr("Term, days")
					description: qsTr("0 for no term")
					readOnly: !periodPage.__isEditable
					bottomValue: 0
				}

				DateTimePickerElementView {
					id: validUntilInput
					name: qsTr("Valid until")
					description: qsTr("Empty for no end")
					canShowCurrentDate: false
					readOnly: !periodPage.__isEditable
				}

				SwitchElementView {
					id: autoRenewSwitch
					name: qsTr("Renewed automatically")
					readOnly: !periodPage.__isEditable
				}

				IntegerInputElementView {
					id: renewalPeriodInput
					placeHolderText: qsTr("e.g. 365")
					visible: autoRenewSwitch.checked
					name: qsTr("Renewal period, days")
					readOnly: !periodPage.__isEditable
					bottomValue: 0
				}

				IntegerInputElementView {
					id: renewalNoticeInput
					placeHolderText: qsTr("e.g. 30")
					visible: autoRenewSwitch.checked
					name: qsTr("Stop the renewal at least, days before")
					readOnly: !periodPage.__isEditable
					bottomValue: 0
				}

				IntegerInputElementView {
					id: terminationNoticeInput
					placeHolderText: qsTr("e.g. 30")
					name: qsTr("Notice period, days")
					readOnly: !periodPage.__isEditable
					bottomValue: 0
				}
			}

			GroupElementView {
				width: parent.width

				DateTimePickerElementView {
					id: signDeadlineInput
					name: qsTr("Sign until")
					description: qsTr("Empty for no deadline")
					canShowCurrentDate: false
					readOnly: !periodPage.__isEditable
				}

				SwitchElementView {
					id: sequentialSwitch
					name: qsTr("The parties sign in their order")
					readOnly: !periodPage.__isEditable
				}
			}
		}
	}
}
