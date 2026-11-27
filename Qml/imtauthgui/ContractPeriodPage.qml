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
 * When a contract starts and ends, its renewal and the notice of a party that leaves it, said in one sentence
 * above the fields. The start and the end are chosen as a way first; the field of the chosen way follows.
 * A page of ContractView: it shows the working copy of the view and writes it back.
 */
Item {
	id: periodPage

	anchors.fill: parent

	// ContractView
	property var editor: null

	readonly property bool __isEditable: periodPage.editor ? periodPage.editor.isEditable : false
	// The fields are filled from the working copy: their changes are not the user's then
	property bool __filling: false
	// Changes with every edited date, so the sentence follows the dates too
	property int __datesRevision: 0

	readonly property string startMode: startModel.idAt(startCB.currentIndex)
	readonly property string endMode: endModel.idAt(endCB.currentIndex)
	readonly property bool hasEnd: periodPage.endMode === "days" || periodPage.endMode === "date"

	Component.onCompleted: {
		periodPage.__filling = true
		startCB.currentIndex = startModel.fill([{"id": "signing", "name": qsTr("On the day the last party signs")},
												{"id": "date", "name": qsTr("On a date")}], "id", "name", "signing")
		endCB.currentIndex = endModel.fill([{"id": "none", "name": qsTr("Never: no end")},
											{"id": "days", "name": qsTr("After a number of days")},
											{"id": "date", "name": qsTr("On a date")}], "id", "name", "days")
		periodPage.__filling = false
	}

	function updateGui() {
		if (!periodPage.editor)
			return
		periodPage.__filling = true
		var draft = periodPage.editor.draft
		startCB.currentIndex = startModel.indexOfId(draft.effectiveFrom ? "date" : "signing")
		ContractTexts.setPickerIso(effectiveFromInput, draft.effectiveFrom)
		endCB.currentIndex = endModel.indexOfId(draft.validUntil ? "date" : (draft.termDays > 0 ? "days" : "none"))
		termDaysInput.value = draft.termDays > 0 ? draft.termDays : 365
		ContractTexts.setPickerIso(validUntilInput, draft.validUntil)
		autoRenewSwitch.checked = draft.autoRenew === true
		renewalPeriodInput.value = draft.renewalPeriodDays > 0 ? draft.renewalPeriodDays : 365
		renewalNoticeInput.value = draft.renewalNoticeDays || 0
		terminationNoticeInput.value = draft.terminationNoticeDays || 0
		periodPage.__filling = false
		periodPage.__datesRevision++
	}

	// A field the user changed: the view compares its copy
	function __changed() {
		if (!periodPage.__filling && periodPage.editor)
			periodPage.editor.markChanged()
	}

	function updateModel() {
		if (!periodPage.editor)
			return
		var draft = periodPage.editor.draft
		draft.effectiveFrom = periodPage.startMode === "date" ? ContractTexts.pickerIso(effectiveFromInput) : ""
		draft.termDays = periodPage.endMode === "days" ? termDaysInput.value : 0
		draft.validUntil = periodPage.endMode === "date" ? ContractTexts.pickerIso(validUntilInput) : ""
		// A contract without an end has nothing to renew
		draft.autoRenew = periodPage.hasEnd && autoRenewSwitch.checked
		draft.renewalPeriodDays = draft.autoRenew ? renewalPeriodInput.value : 0
		draft.renewalNoticeDays = draft.autoRenew ? renewalNoticeInput.value : 0
		draft.terminationNoticeDays = terminationNoticeInput.value
	}

	// A chosen date way starts with a date: today for the start, a year later for the end
	function __offerDate(picker, daysFromToday) {
		if (periodPage.__filling || picker.getDateAsString() !== "")
			return
		var date = new Date()
		date.setDate(date.getDate() + daysFromToday)
		ContractTexts.setPickerIso(picker, date.toISOString())
		periodPage.__datesRevision++
	}

	// The period in one sentence, as the fields say it now
	function periodSentence() {
		var startMode = startModel.idAt(startCB.currentIndex)
		var endMode = endModel.idAt(endCB.currentIndex)
		var text = startMode === "date" && effectiveFromInput.getDateAsString() !== ""
			? qsTr("The contract comes into force on %1").arg(effectiveFromInput.getDateAsString())
			: qsTr("The contract comes into force on the day the last party signs")
		if (endMode === "days")
			text += " " + qsTr("and runs for %1 days.").arg(termDaysInput.value)
		else if (endMode === "date" && validUntilInput.getDateAsString() !== "")
			text += " " + qsTr("and runs until %1.").arg(validUntilInput.getDateAsString())
		else
			text += " " + qsTr("and runs with no end.")
		if ((endMode === "days" || endMode === "date") && autoRenewSwitch.checked)
			text += " " + qsTr("Then it is renewed for %1 days each time, unless a party stops the renewal %2 days before the end.")
				.arg(renewalPeriodInput.value).arg(renewalNoticeInput.value)
		text += " " + (terminationNoticeInput.value > 0
			? qsTr("A party may leave it with a notice of %1 days.").arg(terminationNoticeInput.value)
			: qsTr("A party may leave it at once."))
		return text
	}

	ChoiceModel {
		id: startModel
	}

	ChoiceModel {
		id: endModel
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

			// What the fields below make of the contract
			Rectangle {
				width: parent.width
				height: sentenceText.height + 2 * Style.marginM
				radius: Style.radiusM
				color: Style.alternateBaseColor
				border.width: 1
				border.color: Style.borderColor

				BaseText {
					id: sentenceText
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					anchors.margins: Style.marginM
					wrapMode: Text.WordWrap
					text: periodPage.__datesRevision >= 0 ? periodPage.periodSentence() : ""
					color: Style.textColor
				}
			}

			GroupElementView {
				width: parent.width

				ComboBoxElementView {
					id: startCB
					name: qsTr("Comes into force")
					model: startModel
					changeable: periodPage.__isEditable

					// Read from the combo box: the bindings of the page may not be updated yet
					onCurrentIndexChanged: {
						if (startModel.idAt(startCB.currentIndex) === "date")
							periodPage.__offerDate(effectiveFromInput, 0)
						periodPage.__datesRevision++
						periodPage.__changed()
					}
				}

				DateTimePickerElementView {
					id: effectiveFromInput
					visible: periodPage.startMode === "date"
					name: qsTr("Date")
					canShowCurrentDate: false
					readOnly: !periodPage.__isEditable

					onEditingFinished: {
						periodPage.__datesRevision++
						periodPage.__changed()
					}
				}

				ComboBoxElementView {
					id: endCB
					name: qsTr("Ends")
					model: endModel
					changeable: periodPage.__isEditable

					onCurrentIndexChanged: {
						if (endModel.idAt(endCB.currentIndex) === "date")
							periodPage.__offerDate(validUntilInput, 365)
						periodPage.__datesRevision++
						periodPage.__changed()
					}
				}

				SpinBoxElementView {
					id: termDaysInput
					onValueChanged: {
						periodPage.__changed()
					}
					visible: periodPage.endMode === "days"
					name: qsTr("Runs for")
					description: qsTr("Counted from the day it comes into force")
					units: qsTr("days")
					from: 1
					to: 36500
					readOnly: !periodPage.__isEditable
				}

				DateTimePickerElementView {
					id: validUntilInput
					visible: periodPage.endMode === "date"
					name: qsTr("Last day")
					canShowCurrentDate: false
					readOnly: !periodPage.__isEditable

					onEditingFinished: {
						periodPage.__datesRevision++
						periodPage.__changed()
					}
				}

				SwitchElementView {
					id: autoRenewSwitch
					onCheckedChanged: {
						periodPage.__changed()
					}
					visible: periodPage.hasEnd
					name: qsTr("Renewed automatically")
					readOnly: !periodPage.__isEditable
				}

				SpinBoxElementView {
					id: renewalPeriodInput
					onValueChanged: {
						periodPage.__changed()
					}
					visible: periodPage.hasEnd && autoRenewSwitch.checked
					name: qsTr("Renewed for")
					units: qsTr("days")
					from: 1
					to: 36500
					readOnly: !periodPage.__isEditable
				}

				SpinBoxElementView {
					id: renewalNoticeInput
					onValueChanged: {
						periodPage.__changed()
					}
					visible: periodPage.hasEnd && autoRenewSwitch.checked
					name: qsTr("A party stops the renewal at least")
					description: qsTr("Days before the end")
					units: qsTr("days")
					to: 3650
					readOnly: !periodPage.__isEditable
				}
			}

			GroupElementView {
				width: parent.width

				SpinBoxElementView {
					id: terminationNoticeInput
					onValueChanged: {
						periodPage.__changed()
					}
					name: qsTr("Notice to leave")
					description: qsTr("A party leaves this long after its notice; 0: at once")
					units: qsTr("days")
					to: 3650
					readOnly: !periodPage.__isEditable
				}
			}
		}
	}
}
