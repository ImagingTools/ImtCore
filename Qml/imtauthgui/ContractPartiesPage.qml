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
 * The parties of a contract with their roles and states, and the signatures they gave.
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

	readonly property var partyRows: partiesPage.__partyRows(partiesPage.editor ? partiesPage.editor.parties : [])
	readonly property var signatureRows: partiesPage.__signatureRows(partiesPage.editor ? partiesPage.editor.parties : [])

	function __partyRows(parties) {
		var rows = []
		for (var i = 0; i < parties.length; i++) {
			var party = parties[i]
			var isMe = partiesPage.editor && party.tenantId === partiesPage.editor.tenantId
			var notes = []
			if (party.stateComment)
				notes.push(party.stateComment)
			if (party.withdrawAt)
				notes.push(qsTr("leaves on %1").arg(ContractTexts.dateText(party.withdrawAt)))
			if (party.allocationsSuspended)
				notes.push(qsTr("its permissions are stopped"))
			rows.push({"id": party.tenantId,
					   "organization": (party.tenantName || party.tenantId) + (isMe ? " " + qsTr("(you)") : ""),
					   "role": party.roleName || "",
					   "state": partiesPage.editor && partiesPage.editor.isSubmitted ? ContractTexts.partyStateText(party.state) : qsTr("Not sent yet"),
					   "signatures": qsTr("%1 of %2").arg(party.signatures ? party.signatures.length : 0).arg(party.requiredSignatures || 1),
					   "channel": ContractTexts.channelText(party.channel),
					   "notes": notes.join(" · "),
					   "isMe": isMe})
		}
		return rows
	}

	function __signatureRows(parties) {
		var rows = []
		for (var i = 0; i < parties.length; i++) {
			var signatures = parties[i].signatures || []
			for (var j = 0; j < signatures.length; j++) {
				var signature = signatures[j]
				rows.push({"id": parties[i].tenantId + "/" + j,
						   "organization": parties[i].tenantName || parties[i].tenantId,
						   "signedBy": signature.userName || signature.userId || "",
						   "position": signature.position || "",
						   "signedAt": ContractTexts.dateTimeText(signature.signedAt),
						   "method": ContractTexts.signatureMethodText(signature.method),
						   "revision": String(signature.revision || "")})
			}
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
					  : qsTr("The organizations bound by the contract. Each one signs it for itself.")
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
		height: Math.min(partiesTable.contentHeight + partiesTable.headerHeight, (partiesPage.height - headerColumn.height) / 2)

		selectable: false
		elements: partiesPage.partyRows
	}

	BaseText {
		id: signaturesTitle

		anchors.top: partiesTable.bottom
		anchors.topMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL

		text: qsTr("Signatures")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: Style.textColor
	}

	BaseText {
		anchors.top: signaturesTitle.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		visible: partiesPage.signatureRows.length === 0
		text: qsTr("No signatures yet.")
		font.pixelSize: Style.fontSizeS
		color: Style.inactiveTextColor
	}

	Table {
		id: signaturesTable

		anchors.top: signaturesTitle.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL

		visible: partiesPage.signatureRows.length > 0
		selectable: false
		elements: partiesPage.signatureRows
	}

	TableHeaders {
		id: partiesHeaders

		Component.onCompleted: {
			addHeader("organization", qsTr("Organization"), false, false)
			addHeader("role", qsTr("Role"), false, false)
			addHeader("state", qsTr("State"), false, false)
			addHeader("signatures", qsTr("Signatures"), false, false)
			addHeader("channel", qsTr("Found by"), false, false)
			addHeader("notes", qsTr("Notes"), false, false)
			addHeader("actions", "", false, false)
			partiesTable.setColumnContentById("actions", partyActionsComp)
			partiesTable.headers = partiesHeaders
		}
	}

	TableHeaders {
		id: signaturesHeaders

		Component.onCompleted: {
			addHeader("organization", qsTr("Organization"), false, false)
			addHeader("signedBy", qsTr("Signed by"), false, false)
			addHeader("position", qsTr("Position"), false, false)
			addHeader("signedAt", qsTr("Signed"), false, false)
			addHeader("method", qsTr("Signature"), false, false)
			addHeader("revision", qsTr("Revision"), false, false)
			signaturesTable.headers = signaturesHeaders
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
				visible: partiesPage.__isEditable

				Button {
					text: qsTr("Role")
					onClicked: {
						partiesPage.partyRoleRequested(partyActions.tenantId)
					}
				}

				Button {
					text: qsTr("Remove")
					visible: !partyActions.isMe
					onClicked: {
						partiesPage.editor.removeParty(partyActions.tenantId)
					}
				}
			}
		}
	}
}
