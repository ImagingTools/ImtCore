// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
pragma Singleton
import QtQuick 2.12

/**
 * The texts of the contracts shown to the user: the states, the events, the choices,
 * and the dates of the API (ISO, UTC) in the form the user types them (dd.MM.yyyy).
 */
QtObject {
	id: texts

	function contractStateText(state) {
		if (state === "Draft")
			return qsTr("Draft")
		if (state === "Negotiation")
			return qsTr("Negotiation")
		if (state === "Signing")
			return qsTr("Signing")
		if (state === "Rejected")
			return qsTr("Rejected")
		if (state === "Cancelled")
			return qsTr("Cancelled")
		if (state === "Signed")
			return qsTr("Signed")
		if (state === "Active")
			return qsTr("Active")
		if (state === "Expired")
			return qsTr("Expired")
		if (state === "Terminated")
			return qsTr("Terminated")
		if (state === "Superseded")
			return qsTr("Superseded")
		return state || ""
	}

	function partyStateText(state) {
		if (state === "Invited")
			return qsTr("Invited")
		if (state === "Reviewing")
			return qsTr("Reviewing")
		if (state === "ChangesRequested")
			return qsTr("Changes requested")
		if (state === "Approved")
			return qsTr("Approved")
		if (state === "Signed")
			return qsTr("Signed")
		if (state === "Declined")
			return qsTr("Declined")
		if (state === "NoticeGiven")
			return qsTr("Notice given")
		if (state === "Withdrawn")
			return qsTr("Withdrawn")
		return state || ""
	}

	function channelText(channel) {
		if (channel === "Initiator")
			return qsTr("Initiator")
		if (channel === "AddressBook")
			return qsTr("Address book")
		if (channel === "ConnectionCode")
			return qsTr("Connection code")
		if (channel === "Directory")
			return qsTr("Directory")
		if (channel === "CoParty")
			return qsTr("Party of a common contract")
		return channel || ""
	}

	readonly property var referenceTypes: ["Supersedes", "Framework", "Related"]

	function referenceTypeText(referenceType) {
		if (referenceType === "Supersedes")
			return qsTr("Replaces")
		if (referenceType === "Framework")
			return qsTr("Under the framework contract")
		return qsTr("Related to")
	}

	function eventText(eventType) {
		if (eventType === "Created")
			return qsTr("Created")
		if (eventType === "Updated")
			return qsTr("Terms changed")
		if (eventType === "PartyAdded")
			return qsTr("Party added")
		if (eventType === "Submitted")
			return qsTr("Sent to the parties")
		if (eventType === "Cancelled")
			return qsTr("Offer withdrawn")
		if (eventType === "ChangesRequested")
			return qsTr("Changes requested")
		if (eventType === "Approved")
			return qsTr("Approved")
		if (eventType === "Signed")
			return qsTr("Signed")
		if (eventType === "Declined")
			return qsTr("Declined")
		if (eventType === "Concluded")
			return qsTr("Concluded")
		if (eventType === "AttachmentAdded")
			return qsTr("Document attached")
		if (eventType === "AttachmentReplaced")
			return qsTr("Document replaced")
		if (eventType === "NoticeGiven")
			return qsTr("Notice given")
		if (eventType === "NoticeRevoked")
			return qsTr("Notice revoked")
		if (eventType === "Withdrawn")
			return qsTr("Party left")
		if (eventType === "NonRenewalNotice")
			return qsTr("Renewal stopped")
		if (eventType === "AllocationsSuspended")
			return qsTr("Permissions stopped")
		if (eventType === "AllocationsResumed")
			return qsTr("Permissions resumed")
		if (eventType === "Superseded")
			return qsTr("Replaced by a new contract")
		if (eventType === "PrivateInfoChanged")
			return qsTr("Internal data changed")
		if (eventType === "AllocationGroupsChanged")
			return qsTr("Permissions given to groups")
		if (eventType === "ReminderSent")
			return qsTr("Reminder sent")
		return eventType || ""
	}

	function __pad(value) {
		return value < 10 ? "0" + value : String(value)
	}

	// "dd.MM.yyyy" of a time of the API, empty for none
	function dateText(isoTime) {
		if (!isoTime || isoTime === "")
			return ""
		var date = new Date(isoTime)
		if (isNaN(date.getTime()))
			return ""
		return texts.__pad(date.getDate()) + "." + texts.__pad(date.getMonth() + 1) + "." + date.getFullYear()
	}

	function dateTimeText(isoTime) {
		if (!isoTime || isoTime === "")
			return ""
		var date = new Date(isoTime)
		if (isNaN(date.getTime()))
			return ""
		return texts.dateText(isoTime) + " " + texts.__pad(date.getHours()) + ":" + texts.__pad(date.getMinutes())
	}

	// The time of the API of a typed "dd.MM.yyyy": the start of the day, local time; empty for none or a wrong date
	function isoOfDate(text) {
		var parts = text ? text.trim().split(".") : []
		if (parts.length !== 3)
			return ""
		var date = new Date(Number(parts[2]), Number(parts[1]) - 1, Number(parts[0]))
		if (isNaN(date.getTime()) || date.getDate() !== Number(parts[0]))
			return ""
		return date.toISOString()
	}

	function isDateTextValid(text) {
		return !text || text.trim() === "" || texts.isoOfDate(text) !== ""
	}

	// --- The date fields (DateTimePickerElementView) ---

	function setPickerIso(picker, isoTime) {
		picker.setDateAsString(texts.dateText(isoTime))
	}

	// The time of the API of the date of the field, empty for none
	function pickerIso(picker) {
		return texts.isoOfDate(picker.getDateAsString())
	}

	// --- The choices of the enumerations: [{id, name}] ---

	function referenceTypeItems() {
		var result = []
		for (var i = 0; i < texts.referenceTypes.length; i++)
			result.push({"id": texts.referenceTypes[i], "name": texts.referenceTypeText(texts.referenceTypes[i])})
		return result
	}

	readonly property var linkSchemes: ["Mesh", "Star", "Manual"]

	function linkSchemeText(scheme) {
		if (scheme === "Star")
			return qsTr("One party linked with every other one")
		if (scheme === "Manual")
			return qsTr("Chosen by the initiator")
		return qsTr("Every party linked with every other one")
	}

	function linkSchemeItems() {
		var result = []
		for (var i = 0; i < texts.linkSchemes.length; i++)
			result.push({"id": texts.linkSchemes[i], "name": texts.linkSchemeText(texts.linkSchemes[i])})
		return result
	}

	function yesNoText(value) {
		return value ? qsTr("Yes") : qsTr("No")
	}

	function periodText(startIso, endIso) {
		var start = texts.dateText(startIso)
		var end = texts.dateText(endIso)
		if (start === "" && end === "")
			return ""
		return (start !== "" ? start : qsTr("not started")) + " – " + (end !== "" ? end : qsTr("open-ended"))
	}
}
