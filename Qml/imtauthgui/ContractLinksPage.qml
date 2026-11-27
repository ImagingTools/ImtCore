// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractLinksPage
 *
 * The links between the parties of a contract: the parties of a link share documents
 * and give each other permissions. The links follow the parties and the scheme of the type.
 * A page of ContractView: the links are the list of the view, changed through the view.
 */
Item {
	id: linksPage

	anchors.fill: parent

	// ContractView
	property var editor: null

	readonly property bool __isEditable: linksPage.editor ? linksPage.editor.isEditable : false

	readonly property var linkRows: linksPage.__linkRows(linksPage.editor ? linksPage.editor.links : [])

	// Whether the two work with each other now, in the words of the user
	function __stateText(link) {
		if (!link.linkId)
			return qsTr("Not saved yet")
		if (link.isInForce)
			return qsTr("In force")
		var contract = linksPage.editor ? linksPage.editor.contract : null
		if (!contract || !contract.signedAt)
			return qsTr("Once all parties sign")
		return linksPage.editor.contractState === "Signed" ? qsTr("Once the contract comes into force") : qsTr("Ended")
	}

	function __linkRows(links) {
		var rows = []
		for (var i = 0; i < links.length; i++) {
			var link = links[i]
			rows.push({"id": String(i),
					   "index": i,
					   "partyA": linksPage.editor.partyName(link.tenantAId),
					   "roleA": link.roleAName || linksPage.editor.roleName(link.roleAId || ""),
					   "partyB": linksPage.editor.partyName(link.tenantBId),
					   "roleB": link.roleBName || linksPage.editor.roleName(link.roleBId || ""),
					   "state": linksPage.__stateText(link),
					   "isSharingAllowed": link.isSharingAllowed !== false})
		}
		return rows
	}

	BaseText {
		id: descriptionText

		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL

		wrapMode: Text.WordWrap
		text: (linksPage.linkRows.length === 0 ? qsTr("The links appear with the parties.") + " " : "")
			+ qsTr("A link is a pair of parties that work with each other under the contract: every two parties, or the central party with each other one when the type says so. While the contract is in force, Documents lets the two share documents, and permissions are given only within a link (page Permissions). Without a link two parties of the same contract have nothing to do with each other.")
		font.pixelSize: Style.fontSizeS
		color: Style.inactiveTextColor
	}

	Table {
		id: linksTable

		anchors.top: descriptionText.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL

		visible: linksPage.linkRows.length > 0
		selectable: false
		elements: linksPage.linkRows
	}

	TableHeaders {
		id: linksHeaders

		Component.onCompleted: {
			addHeader("partyA", qsTr("Party"), false, false)
			addHeader("roleA", qsTr("Its role"), false, false)
			addHeader("partyB", qsTr("Other party"), false, false)
			addHeader("roleB", qsTr("Its role"), false, false)
			addHeader("documents", qsTr("Documents"), false, false)
			addHeader("state", qsTr("State"), false, false)
			linksTable.setColumnContentById("documents", documentsCellComp)
			linksTable.headers = linksHeaders
		}
	}

	Component {
		id: documentsCellComp

		TableCellDelegateBase {
			id: documentsCell

			readonly property var rowData: documentsCell.getRowData()

			CheckBox {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				enabled: linksPage.__isEditable
				checkState: documentsCell.rowData && documentsCell.rowData.isSharingAllowed ? Qt.Checked : Qt.Unchecked
				onCheckStateChanged: {
					if (documentsCell.rowData && (checkState === Qt.Checked) !== documentsCell.rowData.isSharingAllowed)
						linksPage.editor.setLinkFlag(documentsCell.rowData.index, "isSharingAllowed", checkState === Qt.Checked)
				}
			}
		}
	}
}
