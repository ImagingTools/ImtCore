// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractPartyDialog
 *
 * A party of a contract: the organization, its role and the number of the signatures it gives.
 * The organization is chosen from the candidates (the address book and the parties of common contracts),
 * typed as a connection code (byCode), or fixed when only the role of a party changes (fixedTenantId).
 *
 * The result is read in onFinished from contentItem: tenantId, tenantName, code, roleId, roleName, requiredSignatures.
 */
Dialog {
	id: partyDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))

	// [{tenantId, tenantName, channel}]
	property var candidates: []
	// [{roleId, name}]
	property var roles: []
	property bool byCode: false
	property string fixedTenantId: ""
	property string fixedTenantName: ""
	property string initialRoleId: ""
	property int initialRequiredSignatures: 1

	readonly property bool __choosesTenant: !partyDialog.byCode && partyDialog.fixedTenantId === ""

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		addButton(Enums.ok, qsTr("OK"), partyDialog.fixedTenantId !== "", false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
		if (partyDialog.contentItem)
			partyDialog.contentItem.init()
	}

	function __updateOk(content) {
		if (!partyDialog.buttons)
			return
		// Every party of a contract has a role
		var ready = partyDialog.fixedTenantId !== ""
		if (partyDialog.byCode)
			ready = content.code !== ""
		else if (partyDialog.__choosesTenant)
			ready = content.tenantId !== ""
		partyDialog.buttons.setButtonState(Enums.ok, ready && content.roleId !== "")
	}

	contentComp: Component {
		Item {
			id: partyContent

			width: partyDialog.width
			height: partyColumn.height + 2 * Style.marginXL

			readonly property string tenantId: partyDialog.fixedTenantId !== "" ? partyDialog.fixedTenantId : tenantModel.idAt(tenantCB.currentIndex)
			readonly property string tenantName: partyDialog.fixedTenantId !== "" ? partyDialog.fixedTenantName : tenantModel.nameAt(tenantCB.currentIndex)
			readonly property string code: codeInput.text.trim()
			readonly property string roleId: roleModel.idAt(roleCB.currentIndex)
			readonly property string roleName: roleModel.nameAt(roleCB.currentIndex)
			readonly property int requiredSignatures: Math.max(1, signaturesInput.value)

			onTenantIdChanged: {
				partyDialog.__updateOk(partyContent)
			}

			onCodeChanged: {
				partyDialog.__updateOk(partyContent)
			}

			onRoleIdChanged: {
				partyDialog.__updateOk(partyContent)
			}

			function init() {
				tenantModel.fill(partyDialog.candidates, "tenantId", "tenantName", "")
				roleCB.currentIndex = roleModel.fill(partyDialog.roles, "roleId", "name", partyDialog.initialRoleId)
				signaturesInput.value = Math.max(1, partyDialog.initialRequiredSignatures)
				partyDialog.__updateOk(partyContent)
			}

			ChoiceModel {
				id: tenantModel
			}

			ChoiceModel {
				id: roleModel
			}

			Column {
				id: partyColumn

				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL

				spacing: Style.marginL

				BaseText {
					width: parent.width
					visible: partyDialog.__choosesTenant
					wrapMode: Text.WordWrap
					text: partyDialog.candidates.length > 0
						? qsTr("The organizations of the address book and the parties of your other contracts.")
						: qsTr("No organization to invite yet: add a partner to the address book or invite one by its code.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				RegularExpressionValidator {
					id: codeValidator
					regularExpression: /^(?!\s*$).+/
				}

				GroupElementView {
					width: parent.width

					TextElementView {
						visible: partyDialog.fixedTenantId !== ""
						name: qsTr("Organization")
						text: partyDialog.fixedTenantName
					}

					ComboBoxElementView {
						id: tenantCB
						isRequired: true
						isSelectionRequired: true
						errorText: qsTr("Choose the organization")
						visible: partyDialog.__choosesTenant
						name: qsTr("Organization")
						model: tenantModel
					}

					TextInputElementView {
						id: codeInput
						isRequired: true
						showErrorWhenInvalid: true
						deferErrorUntilEdited: true
						errorText: qsTr("Enter the code the organization gave you")
						textInputValidator: codeValidator
						visible: partyDialog.byCode
						name: qsTr("Connection code")
						placeHolderText: qsTr("The code the organization gave you")
					}

					ComboBoxElementView {
						id: roleCB
						isRequired: true
						isSelectionRequired: true
						errorText: qsTr("Choose the role of the organization in the contract")
						name: qsTr("Role")
						model: roleModel
					}

					IntegerInputElementView {
						id: signaturesInput
						placeHolderText: qsTr("1")
						name: qsTr("Signatures needed")
						bottomValue: 1
						topValue: 10
					}
				}
			}
		}
	}
}
