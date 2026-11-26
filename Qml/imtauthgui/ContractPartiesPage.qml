// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractPartiesPage
 *
 * The parties of a contract with their roles and states, and the signature each one gave.
 * A page of ContractView: the parties are the list of the view, changed through the view.
 */
Item {
	id: partiesPage

	anchors.fill: parent

	// ContractView
	property var editor: null

	readonly property bool __isEditable: partiesPage.editor ? partiesPage.editor.isEditable : false

	signal addPartyRequested()
	signal addPartyByCodeRequested()
	signal partyRoleRequested(string tenantId)

	readonly property var partyRows: partiesPage.__partyRows(partiesPage.editor ? partiesPage.editor.parties : [], partiesPage.editor ? partiesPage.editor.contract : null)

	// When the party signed; who signed is in the journal
	function __signedText(party) {
		var signatures = party.signatures || []
		return signatures.length > 0 ? ContractTexts.dateText(signatures[signatures.length - 1].signedAt) : ""
	}

	// The parties of the working copy; their states and signatures are the stored ones, so they follow the others while the user edits
	function __partyRows(parties, contract) {
		var rows = []
		for (var i = 0; i < parties.length; i++) {
			var party = parties[i]
			var stored = partiesPage.editor ? partiesPage.editor.contractParty(party.tenantId) : null
			var facts = stored !== null ? stored : party
			var isMe = partiesPage.editor && party.tenantId === partiesPage.editor.tenantId
			var notes = []
			if (facts.stateComment)
				notes.push(facts.stateComment)
			if (facts.withdrawAt)
				notes.push(qsTr("leaves on %1").arg(ContractTexts.dateText(facts.withdrawAt)))
			if (facts.allocationsSuspended)
				notes.push(qsTr("its permissions are stopped"))
			rows.push({"id": party.tenantId,
					   "organization": (party.tenantName || party.tenantId) + (isMe ? " " + qsTr("(you)") : ""),
					   "role": party.roleName || "",
					   "state": partiesPage.editor && partiesPage.editor.isSubmitted ? ContractTexts.partyStateText(facts.state) : qsTr("Not sent yet"),
					   "signed": partiesPage.__signedText(facts),
					   "channel": ContractTexts.channelText(party.channel),
					   "notes": notes.join(" · "),
					   "isMe": isMe})
		}
		return rows
	}

	Column {
		id: headerColumn

		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL

		spacing: Style.marginM

		Item {
			width: parent.width
			height: Math.max(descriptionText.height, partyButtons.height)

			BaseText {
				id: descriptionText
				anchors.left: parent.left
				anchors.right: partyButtons.left
				anchors.rightMargin: Style.marginL
				anchors.verticalCenter: parent.verticalCenter
				wrapMode: Text.WordWrap
				text: partiesPage.__isEditable && partiesPage.partyRows.length < 2
					  ? qsTr("Add the other organizations of the contract: a partner from the address book, or, once the draft is saved, any organization by its connection code.")
					  : qsTr("The organizations bound by the contract. Each one signs it for itself: one signature of a user with the right to sign is enough.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Row {
				id: partyButtons
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.marginS
				visible: partiesPage.__isEditable

				Button {
					variant: "primary"
					text: qsTr("Add party")
					onClicked: {
						partiesPage.addPartyRequested()
					}
				}

				Button {
					text: qsTr("Add by code")
					visible: partiesPage.editor ? !partiesPage.editor.isNew : false
					onClicked: {
						partiesPage.addPartyByCodeRequested()
					}
				}
			}
		}
	}

	Table {
		id: partiesTable

		anchors.top: headerColumn.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL

		selectable: false
		elements: partiesPage.partyRows
	}

	TableHeaders {
		id: partiesHeaders

		Component.onCompleted: {
			addHeader("organization", qsTr("Organization"), false, false)
			addHeader("role", qsTr("Role"), false, false)
			addHeader("state", qsTr("State"), false, false)
			addHeader("signed", qsTr("Signed"), false, false)
			addHeader("channel", qsTr("Found by"), false, false)
			addHeader("notes", qsTr("Notes"), false, false)
			addHeader("actions", "", false, false)
			partiesTable.setColumnContentById("actions", partyActionsComp)
			partiesTable.headers = partiesHeaders
		}
	}

	Component {
		id: partyActionsComp

		TableCellDelegateBase {
			id: partyActions

			readonly property var rowData: partyActions.getRowData()
			readonly property string tenantId: partyActions.rowData ? (partyActions.rowData.id || "") : ""
			readonly property bool isMe: partyActions.rowData ? partyActions.rowData.isMe === true : false

			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				spacing: Style.marginS

				Button {
					text: qsTr("Role")
					visible: partiesPage.__isEditable
					onClicked: {
						partiesPage.partyRoleRequested(partyActions.tenantId)
					}
				}

				Button {
					text: qsTr("Remove")
					visible: partiesPage.__isEditable && !partyActions.isMe
					onClicked: {
						partiesPage.editor.removeParty(partyActions.tenantId)
					}
				}
			}
		}
	}
}
