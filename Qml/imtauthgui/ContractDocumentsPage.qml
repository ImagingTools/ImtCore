// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractDocumentsPage
 *
 * The documents of a contract: the ones seen by all parties and the internal ones of the organization.
 * The shared files attached before the contract is concluded are part of its terms: the initiator attaches them,
 * and a new one asks the parties again. After the conclusion every party attaches files that are no terms.
 * A page of ContractView.
 */
Item {
	id: documentsPage

	anchors.fill: parent

	// ContractView
	property var editor: null

	readonly property bool __canAttach: documentsPage.editor ? documentsPage.editor.canEdit && documentsPage.editor.myParty !== null : false
	// An ended contract gets only internal documents
	readonly property bool __isEnded: documentsPage.editor !== null && documentsPage.editor.contract !== null
		&& (documentsPage.editor.contractState === "Terminated" || documentsPage.editor.contractState === "Expired"
			|| documentsPage.editor.contractState === "Superseded" || documentsPage.editor.contractState === "Cancelled")
	readonly property bool __isSigned: documentsPage.editor !== null && documentsPage.editor.contract !== null && !!documentsPage.editor.contract.signedAt
	readonly property bool __isInitiator: documentsPage.editor ? documentsPage.editor.isInitiator : false
	// The organization attaches a shared file now; the server checks the same
	readonly property bool __canShare: !documentsPage.__isEnded && (documentsPage.__isSigned || documentsPage.__isInitiator)
	readonly property bool __isInternalChosen: !documentsPage.__canShare || internalCheckBox.checkState === Qt.Checked

	readonly property var attachmentFilters: ["PDF (*.pdf)", "Word (*.docx)", "Word 97 (*.doc)", "Excel (*.xlsx)", "Image (*.png)", "Image (*.jpg)", "Text (*.txt)", "Archive (*.zip)"]
	readonly property var attachmentMimeTypes: ["application/pdf", "application/vnd.openxmlformats-officedocument.wordprocessingml.document", "application/msword",
		"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "image/png", "image/jpeg", "text/plain", "application/zip"]

	readonly property var documentRows: documentsPage.__documentRows(documentsPage.editor ? documentsPage.editor.contract : null,
																	 documentsPage.editor ? documentsPage.editor.privateInfo : null)

	property bool __isInternal: false
	property string __replacesId: ""

	// A shared file is a part of the terms when it was attached before the contract was concluded
	function __isTerms(attachment) {
		if (attachment.isInternal)
			return false
		var signedAt = documentsPage.editor && documentsPage.editor.contract ? documentsPage.editor.contract.signedAt : ""
		return !signedAt || (!!attachment.addedAt && new Date(attachment.addedAt) <= new Date(signedAt))
	}

	// A new version of an own current file; the signed files of the terms stay
	function __canReplace(attachment) {
		if (!documentsPage.__canAttach || attachment.isReplaced)
			return false
		if (attachment.isInternal)
			return true
		return documentsPage.__canShare && attachment.addedByTenantId === documentsPage.editor.tenantId
			&& documentsPage.__isTerms(attachment) === !documentsPage.__isSigned
	}

	function __chooseFile(isInternal, replacesId) {
		documentsPage.__isInternal = isInternal
		documentsPage.__replacesId = replacesId
		var dialog = importDialogComp.createObject(documentsPage)
		dialog.open()
	}

	function __documentRows(contract, privateInfo) {
		var rows = []
		var attachments = documentsPage.editor ? documentsPage.editor.attachments() : []
		for (var i = 0; i < attachments.length; i++) {
			var attachment = attachments[i]
			rows.push({"id": attachment.attachmentId,
					   "fileName": attachment.fileName || "",
					   "isInternal": attachment.isInternal === true,
					   "canReplace": documentsPage.__canReplace(attachment),
					   "isTerms": documentsPage.__isTerms(attachment) ? qsTr("Yes") : qsTr("No"),
					   "addedAt": ContractTexts.dateText(attachment.addedAt),
					   "addedBy": documentsPage.editor.partyName(attachment.addedByTenantId),
					   "visibility": attachment.isInternal ? qsTr("Internal") : qsTr("All parties"),
					   "state": attachment.isReplaced ? qsTr("Replaced") : qsTr("Current")})
		}
		return rows
	}

	Item {
		id: headerItem

		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL

		height: Math.max(descriptionText.height, attachControls.height)

		BaseText {
			id: descriptionText
			anchors.left: parent.left
			anchors.right: attachControls.left
			anchors.rightMargin: Style.marginL
			anchors.verticalCenter: parent.verticalCenter
			wrapMode: Text.WordWrap
			text: documentsPage.__isSigned
				? qsTr("The files of the terms were signed with the contract and stay. An internal document is seen by your organization only.")
				: documentsPage.__isInitiator
					? qsTr("The shared files are part of the terms: a new one asks the parties again. An internal document is seen by your organization only.")
					: qsTr("The initiator attaches the files of the terms. Your organization attaches internal documents, seen by it only.")
			font.pixelSize: Style.fontSizeS
			color: Style.inactiveTextColor
		}

		Row {
			id: attachControls
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			spacing: Style.marginS
			visible: documentsPage.__canAttach

			CheckBox {
				id: internalCheckBox
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Internal")
				tooltipText: documentsPage.__canShare ? qsTr("Seen by your organization only") : qsTr("Your organization attaches only internal documents now")
				enabled: documentsPage.__canShare
				checkState: documentsPage.__canShare ? Qt.Unchecked : Qt.Checked
			}

			Button {
				variant: "primary"
				text: qsTr("Attach file")
				enabled: documentsPage.editor ? !documentsPage.editor.isBusy : false
				onClicked: {
					documentsPage.__chooseFile(documentsPage.__isInternalChosen, "")
				}
			}
		}
	}

	Table {
		id: documentsTable

		anchors.top: headerItem.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL

		visible: documentsPage.documentRows.length > 0
		selectable: false
		elements: documentsPage.documentRows
	}

	TableHeaders {
		id: documentsHeaders

		Component.onCompleted: {
			addHeader("fileName", qsTr("File"), false, false)
			addHeader("isTerms", qsTr("Part of the terms"), false, false)
			addHeader("addedAt", qsTr("Added"), false, false)
			addHeader("addedBy", qsTr("By"), false, false)
			addHeader("visibility", qsTr("Seen by"), false, false)
			addHeader("state", qsTr("State"), false, false)
			addHeader("actions", "", false, false)
			documentsTable.setColumnContentById("actions", documentActionsComp)
			documentsTable.headers = documentsHeaders

			// The buttons need their own width only, the rest goes to the text columns
			documentsTable.setWidth()
			let actionsIndex = documentsTable.getHeaderIndex("actions")
			if (actionsIndex >= 0 && documentsTable.widthDecorator.getItemsCount() > actionsIndex){
				documentsTable.widthDecorator.setData("Width", Style.sizeHintXXS + Style.marginXL, actionsIndex)
				documentsTable.widthDecorator.setData("WidthPercent", -1, actionsIndex)
				documentsTable.setWidth()
			}
		}
	}

	Component {
		id: documentActionsComp

		TableCellDelegateBase {
			id: documentActions

			readonly property var rowData: documentActions.getRowData()

			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				spacing: Style.marginS

				Button {
					visible: documentActions.rowData ? documentActions.rowData.canReplace === true : false
					enabled: documentsPage.editor ? !documentsPage.editor.isBusy : false
					text: qsTr("New version")
					tooltipText: qsTr("The current version is kept as replaced")
					onClicked: {
						documentsPage.__chooseFile(documentActions.rowData.isInternal, documentActions.rowData.id)
					}
				}

				Button {
					text: qsTr("Download")
					onClicked: {
						documentsPage.editor.downloadDocument(documentActions.rowData.id)
					}
				}
			}
		}
	}

	Component {
		id: importDialogComp

		FileImportDialog {
			id: importDialog
			nameFilters: documentsPage.attachmentFilters
			mimeTypes: documentsPage.attachmentMimeTypes

			onImportDialogResult: {
				documentsPage.editor.attachDocument(fileNameWithExtension, mimeType, encodedData, documentsPage.__isInternal, documentsPage.__replacesId)
				importDialog.destroy()
			}

			onRejected: {
				importDialog.destroy()
			}
		}
	}
}
