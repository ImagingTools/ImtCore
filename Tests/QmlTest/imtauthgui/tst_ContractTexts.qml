import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0

TestCase {
	id: testCase

	name: "ContractTexts"

	// A typed day goes to the API as the start of that day and comes back as typed
	function test_aTypedDateRoundTrips(){
		let iso = ContractTexts.isoOfDate("05.03.2027")
		verify(iso !== "", "A valid day gives a time")
		compare(ContractTexts.dateText(iso), "05.03.2027")
	}

	function test_anEmptyDateIsNone(){
		compare(ContractTexts.isoOfDate(""), "")
		compare(ContractTexts.isoOfDate("   "), "")
		compare(ContractTexts.dateText(""), "")
		verify(ContractTexts.isDateTextValid(""), "No date is a valid input of an optional date")
	}

	// A day that does not exist is not moved to another one
	function test_aWrongDateIsRejected(){
		compare(ContractTexts.isoOfDate("31.02.2027"), "")
		compare(ContractTexts.isoOfDate("2027-03-05"), "")
		compare(ContractTexts.isoOfDate("5.3"), "")
		verify(!ContractTexts.isDateTextValid("31.02.2027"))
	}

	function test_thePeriodNamesItsOpenEnds(){
		let start = ContractTexts.isoOfDate("01.01.2027")
		compare(ContractTexts.periodText(start, ""), "01.01.2027 – " + qsTr("open-ended"))
		compare(ContractTexts.periodText("", ""), "")
	}

	// The kinds of the attachments are the numbers of the API in the order of ContractAttachmentKind
	function test_theAttachmentKindsFollowTheApiOrder(){
		compare(ContractTexts.attachmentKindAt(0), "MainText")
		compare(ContractTexts.attachmentKindAt(2), "SignedCopy")
		compare(ContractTexts.attachmentKindAt(4), "Other")
		compare(ContractTexts.attachmentKindAt(9), "Other")
	}
}
