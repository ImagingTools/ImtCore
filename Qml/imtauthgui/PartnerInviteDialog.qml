// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/**
 * PartnerInviteDialog
 *
 * A request to add an organization to the address book: by the code the partner gave,
 * or to an organization found in the directory (targetTenantName is set, no code is asked).
 * The roles and the terms of the cooperation are agreed later, in a contract.
 *
 * The result is read in onFinished from contentItem: code, message.
 */
Dialog {
	id: inviteDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))

	// Empty: the partner is reached by its code
	property string targetTenantName: ""

	readonly property bool showCode: inviteDialog.targetTenantName === ""

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		addButton(Enums.ok, qsTr("Send request"), !inviteDialog.showCode, false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	contentComp: Component {
		Item {
			id: inviteContent

			width: inviteDialog.width
			height: inviteColumn.height + 2 * Style.marginXL

			readonly property string code: codeInput.text.trim()
			readonly property string message: messageInput.text.trim()

			Column {
				id: inviteColumn

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
					text: inviteDialog.showCode
						? qsTr("Organizations do not see each other. You can connect by the code the partner gave you, or to an organization listed in the directory.")
						: qsTr("The request goes to \"%1\" found in the directory.").arg(inviteDialog.targetTenantName)
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				RegularExpressionValidator {
					id: codeValidator
					regularExpression: /^(?!\s*$).+/
				}

				GroupElementView {
					width: parent.width

					TextInputElementView {
						id: codeInput
						isRequired: true
						showErrorWhenInvalid: true
						deferErrorUntilEdited: true
						errorText: qsTr("Enter the code the partner gave you")
						textInputValidator: codeValidator
						visible: inviteDialog.showCode
						name: qsTr("Partner's code")
						placeHolderText: qsTr("Enter code...")

						onTextChanged: {
							if (inviteDialog.buttons && inviteDialog.showCode){
								inviteDialog.buttons.setButtonState(Enums.ok, codeInput.text.trim().length > 0)
							}
						}
					}

					TextInputElementView {
						id: messageInput
						name: qsTr("Message to the partner")
						placeHolderText: qsTr("Optional")
					}
				}

				BaseText {
					width: parent.width
					wrapMode: Text.WordWrap
					text: qsTr("Once the partner accepts, the organizations are in each other's address book. The roles, the terms and the access to the data are agreed in a contract.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
			}
		}
	}
}
