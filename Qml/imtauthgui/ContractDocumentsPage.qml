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
 * The main text and the appendices are part of the terms: attaching another one asks the parties again.
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
	readonly property bool __isInternalChosen: documentsPage.__isEnded || internalCheckBox.checkState === Qt.Checked
	// The kinds the organization attaches now; the server checks the same
	readonly property var kinds: documentsPage.__allowedKinds(documentsPage.__isInternalChosen, documentsPage.__isSigned, documentsPage.__isInitiator)

	readonly property var attachmentFilters: ["PDF (*.pdf)", "Word (*.docx)", "Word 97 (*.doc)", "Excel (*.xlsx)", "Image (*.png)", "Image (*.jpg)", "Text (*.txt)", "Archive (*.zip)"]
	readonly property var attachmentMimeTypes: ["application/pdf", "application/vnd.openxmlformats-officedocument.wordprocessingml.document", "application/msword",
		"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "image/png", "image/jpeg", "text/plain", "application/zip"]

	readonly property var documentRows: documentsPage.__documentRows(documentsPage.editor ? documentsPage.editor.contract : null,
																	 documentsPage.editor ? documentsPage.editor.privateInfo : null)

	property string __kind: "Other"
	property bool __isInternal: false
	property string __replacesId: ""

	Component.onCompleted: {
		documentsPage.__fillKinds()
	}

	onKindsChanged: {
		documentsPage.__fillKinds()
	}

	function __fillKinds() {
		var selectedId = kindCB.currentIndex >= 0 ? kindModel.idAt(kindCB.currentIndex) : ""
		var items = []
		for (var i = 0; i < documentsPage.kinds.length; i++)
			items.push({"id": documentsPage.kinds[i], "name": ContractTexts.attachmentKindText(documentsPage.kinds[i])})
		var index = kindModel.fill(items, "id", "name", selectedId)
		kindCB.currentIndex = index >= 0 ? index : 0
	}

	function __allowedKinds(isInternal, isSigned, isInitiator) {
		if (isInternal)
			return ContractTexts.attachmentKinds
		if (isSigned)
			return ["SignedCopy", "Other"]
		// A signed copy before the conclusion is the scan a party signs by
		if (isInitiator)
			return ["MainText", "Appendix", "SignedCopy", "AmendmentDraft", "Other"]
		return ["SignedCopy", "AmendmentDraft", "Other"]
	}

	// A new version of a document: of the same kind and visibility, by an organization allowed to attach this kind
	function __canReplace(attachment) {
		if (!documentsPage.__canAttach || attachment.isReplaced)
			return false
		if (attachment.isInternal)
			return true
		if (documentsPage.__isEnded || attachment.addedByTenantId !== documentsPage.editor.tenantId)
			return false
		return documentsPage.__allowedKinds(false, documentsPage.__isSigned, documentsPage.__isInitiator).indexOf(attachment.kind) >= 0
	}

	// The current shared main text: a new one replaces it
	function __currentMainTextId() {
		var attachments = documentsPage.editor ? documentsPage.editor.attachments() : []
		for (var i = 0; i < attachments.length; i++) {
			if (attachments[i].kind === "MainText" && !attachments[i].isInternal && !attachments[i].isReplaced)
				return attachments[i].attachmentId
		}
		return ""
	}

	function __chooseFile(kind, isInternal, replacesId) {
		documentsPage.__kind = kind
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
					   "kindId": attachment.kind,
					   "isInternal": attachment.isInternal === true,
					   "canReplace": documentsPage.__canReplace(attachment),
					   "kind": ContractTexts.attachmentKindText(attachment.kind),
					   "addedAt": ContractTexts.dateText(attachment.addedAt),
					   "addedBy": documentsPage.editor.partyName(attachment.addedByTenantId),
					   "visibility": attachment.isInternal ? qsTr("Internal") : qsTr("All parties"),
					   "state": attachment.isReplaced ? qsTr("Replaced") : qsTr("Current")})
		}
		return rows
	}

	ChoiceModel {
		id: kindModel
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
			text: documentsPage.documentRows.length === 0
				? qsTr("No documents yet.")
				: qsTr("An internal document is seen by your organization only.")
			font.pixelSize: Style.fontSizeS
			color: Style.inactiveTextColor
		}

		Row {
			id: attachControls
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			spacing: Style.marginS
			visible: documentsPage.__canAttach

			ComboBox {
				id: kindCB
				width: Style.sizeHintXS
				model: kindModel
			}

			CheckBox {
				id: internalCheckBox
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Internal")
				tooltipText: documentsPage.__isEnded ? qsTr("The contract has ended: only internal documents are attached") : qsTr("Seen by your organization only")
				enabled: !documentsPage.__isEnded
				checkState: documentsPage.__isEnded ? Qt.Checked : Qt.Unchecked
			}

			Button {
				variant: "primary"
				text: qsTr("Attach file")
				enabled: documentsPage.editor ? !documentsPage.editor.isBusy : false
				onClicked: {
					var kind = kindModel.idAt(kindCB.currentIndex) !== "" ? kindModel.idAt(kindCB.currentIndex) : "Other"
					var isInternal = documentsPage.__isInternalChosen
					// A contract has one main text: a new one is its new version
					documentsPage.__chooseFile(kind, isInternal, kind === "MainText" && !isInternal ? documentsPage.__currentMainTextId() : "")
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
			addHeader("kind", qsTr("Kind"), false, false)
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
						documentsPage.__chooseFile(documentActions.rowData.kindId, documentActions.rowData.isInternal, documentActions.rowData.id)
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
				documentsPage.editor.attachDocument(documentsPage.__kind, fileNameWithExtension, mimeType, encodedData, documentsPage.__isInternal, documentsPage.__replacesId)
				importDialog.destroy()
			}

			onRejected: {
				importDialog.destroy()
			}
		}
	}
}
