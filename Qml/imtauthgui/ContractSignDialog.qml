// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractSignDialog
 *
 * The signature of the user for the organization: the position, and the way it is given -
 * in the system, by the attached scan of the signed copy, or by an external electronic signature.
 * The signature binds the revision of the terms the user sees.
 *
 * The result is read in onFinished from contentItem: position, method, externalSignatureId, attachmentId.
 */
Dialog {
	id: signDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))

	// The attached signed copies: [{attachmentId, fileName}]
	property var signedCopies: []
	property int revision: 0

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		addButton(Enums.ok, qsTr("Sign"), true, false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
		if (signDialog.contentItem)
			signDialog.contentItem.init()
	}

	function __updateOk(content) {
		if (!signDialog.buttons)
			return
		var ready = true
		if (content.method === "Scan")
			ready = content.attachmentId !== ""
		else if (content.method === "External")
			ready = content.externalSignatureId !== ""
		signDialog.buttons.setButtonState(Enums.ok, ready)
	}

	contentComp: Component {
		Item {
			id: signContent

			width: signDialog.width
			height: signColumn.height + 2 * Style.marginXL

			readonly property string position: positionInput.text.trim()
			readonly property string method: methodModel.idAt(methodCB.currentIndex) !== "" ? methodModel.idAt(methodCB.currentIndex) : "InSystem"
			readonly property string externalSignatureId: externalInput.text.trim()
			readonly property string attachmentId: copyModel.idAt(copyCB.currentIndex)

			onMethodChanged: {
				signDialog.__updateOk(signContent)
			}

			onExternalSignatureIdChanged: {
				signDialog.__updateOk(signContent)
			}

			onAttachmentIdChanged: {
				signDialog.__updateOk(signContent)
			}

			function init() {
				methodCB.currentIndex = methodModel.fill(ContractTexts.signatureMethodItems(), "id", "name", "InSystem")
				var copies = signDialog.signedCopies
				copyCB.currentIndex = copyModel.fill(copies, "attachmentId", "fileName", copies.length > 0 ? copies[0].attachmentId : "")
			}

			ChoiceModel {
				id: methodModel
			}

			ChoiceModel {
				id: copyModel
			}

			Column {
				id: signColumn

				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL

				spacing: Style.marginL

				BaseText {
					width: parent.width
					wrapMode: Text.WordWrap
					text: qsTr("You sign revision %1 of the terms for your organization. A change of the terms asks for the signatures again.").arg(signDialog.revision)
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				GroupElementView {
					width: parent.width

					TextInputElementView {
						id: positionInput
						name: qsTr("Position")
						placeHolderText: qsTr("e.g. Director")
					}

					ComboBoxElementView {
						id: methodCB
						isRequired: true
						isSelectionRequired: true
						errorText: qsTr("Choose how the contract is signed")
						name: qsTr("Signature")
						model: methodModel
					}

					ComboBoxElementView {
						id: copyCB
						visible: signContent.method === "Scan"
						name: qsTr("Signed copy")
						model: copyModel
					}

					TextInputElementView {
						id: externalInput
						visible: signContent.method === "External"
						name: qsTr("Id of the signature")
						placeHolderText: qsTr("The id given by the signature service")
					}
				}

				BaseText {
					width: parent.width
					visible: signContent.method === "Scan" && signDialog.signedCopies.length === 0
					wrapMode: Text.WordWrap
					text: qsTr("Attach the scan of the signed copy to the contract first.")
					font.pixelSize: Style.fontSizeS
					color: Style.errorColor
				}
			}
		}
	}
}
