// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/**
 * PartnerTermsDialog
 *
 * The terms of a partnership: the roles of both organizations and the scope.
 * With the code field it invites a partner, the request carrying the terms;
 * without it it proposes new terms to an existing partner.
 *
 * The result is read in onFinished from contentItem: code, myRole, partnerRole, scope, message.
 */
Dialog {
	id: termsDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))

	// The organizations do not see each other: a partner is reached only by the code it gave
	property bool showCode: true
	property string initialMyRole: "Partner"
	property string initialPartnerRole: "Partner"
	property string initialScope: ""

	readonly property var roles: ["Partner", "Supplier", "Customer", "Affiliate", "Parent", "Child"]

	function roleName(role){
		if (role === "Supplier")
			return qsTr("Supplier")
		if (role === "Customer")
			return qsTr("Customer")
		if (role === "Affiliate")
			return qsTr("Affiliate")
		if (role === "Parent")
			return qsTr("Parent organization")
		if (role === "Child")
			return qsTr("Subsidiary")
		return qsTr("Partner")
	}

	Component.onCompleted: {
		addButton(Enums.ok, termsDialog.showCode ? qsTr("Send request") : qsTr("Propose terms"), !termsDialog.showCode, false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	contentComp: Component {
		Item {
			id: termsContent

			width: termsDialog.width
			height: termsColumn.height + 2 * Style.marginXL

			readonly property string code: codeInput.text.trim()
			readonly property string myRole: termsDialog.roles[myRoleCB.currentIndex >= 0 ? myRoleCB.currentIndex : 0]
			readonly property string partnerRole: termsDialog.roles[partnerRoleCB.currentIndex >= 0 ? partnerRoleCB.currentIndex : 0]
			readonly property string scope: scopeInput.text.trim()
			readonly property string message: messageInput.text.trim()

			Component.onCompleted: {
				for (var i = 0; i < termsDialog.roles.length; i++) {
					var index = roleModel.insertNewItem()
					roleModel.setData("id", termsDialog.roles[i], index)
					roleModel.setData("name", termsDialog.roleName(termsDialog.roles[i]), index)
				}
				myRoleCB.currentIndex = Math.max(termsDialog.roles.indexOf(termsDialog.initialMyRole), 0)
				partnerRoleCB.currentIndex = Math.max(termsDialog.roles.indexOf(termsDialog.initialPartnerRole), 0)
				scopeInput.text = termsDialog.initialScope
			}

			TreeItemModel {
				id: roleModel
			}

			Column {
				id: termsColumn

				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL

				spacing: Style.marginL

				BaseText {
					width: parent.width
					visible: termsDialog.showCode
					wrapMode: Text.WordWrap
					text: qsTr("Organizations do not see each other. You can connect only by the code the partner gave you.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				GroupElementView {
					width: parent.width

					TextInputElementView {
						id: codeInput
						visible: termsDialog.showCode
						name: qsTr("Partner's code")
						placeHolderText: qsTr("Enter code...")

						onTextChanged: {
							if (termsDialog.buttons && termsDialog.showCode){
								termsDialog.buttons.setButtonState(Enums.ok, codeInput.text.trim().length > 0)
							}
						}
					}

					ComboBoxElementView {
						id: myRoleCB
						name: qsTr("We are for them")
						model: roleModel
					}

					ComboBoxElementView {
						id: partnerRoleCB
						name: qsTr("They are for us")
						model: roleModel
					}

					TextInputElementView {
						id: scopeInput
						name: qsTr("Scope")
						placeHolderText: qsTr("Optional — empty applies to all resources")
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
					text: termsDialog.showCode
						? qsTr("The partner accepts the request once: the connection and the relationship come into force together. Access to your data is given separately, with grants.")
						: qsTr("The new terms come into force when the partner accepts them.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
			}
		}
	}
}
