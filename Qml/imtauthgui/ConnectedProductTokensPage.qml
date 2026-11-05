// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * The tokens of the servers of a product: a token is shown once, when it is issued.
 */
Item {
	id: tokensPage

	anchors.fill: parent

	property string productId: ""
	property var apiClient: null
	readonly property int tokenCount: tokensPage.apiClient && tokensPage.apiClient.tokens ? tokensPage.apiClient.tokens.count : 0

	Component.onCompleted: {
		if (tokensPage.apiClient){
			tokensPage.apiClient.requestTokens(tokensPage.productId)
		}
	}

	Connections {
		target: tokensPage.apiClient

		function onTokenCreated(token){
			ModalDialogManager.openDialog(tokenCreatedDialogComp, {"token": token})
		}

		function onOperationFailed(message){
			PopupManager.addErrorMessage(message, true)
		}
	}

	Column {
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Style.marginXL
		spacing: Style.marginXL

		Row {
			width: parent.width
			spacing: Style.marginM

			BaseText {
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width - newTokenButton.width - parent.spacing
				text: qsTr("A token identifies a server of this product at another server. Set it in the environment variable of the service.")
				wrapMode: Text.WordWrap
			}

			Button {
				id: newTokenButton
				objectName: "NewServerTokenButton"
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("New Token")
				onClicked: {
					ModalDialogManager.openDialog(addTokenDialogComp, {})
				}
			}
		}

		Table {
			id: tokenTable
			width: parent.width
			height: contentHeight + headerHeight
			visible: tokensPage.tokenCount > 0
			elements: tokensPage.apiClient && tokensPage.apiClient.tokens ? tokensPage.apiClient.tokens : 0

			TableHeaders {
				Component.onCompleted: {
					addHeader("name", qsTr("Name"))
					addHeader("productId", qsTr("Server"))
					addHeader("createdAt", qsTr("Created"))
					addHeader("lastUsedAt", qsTr("Last used"))
					addHeader("revoked", qsTr("Revoked"))
					addHeader("revoke", "")
					tokenTable.headers = this
				}
			}

			onHeadersChanged: {
				setColumnContentById("revoke", revokeColumnComp)
			}

			Component {
				id: revokeColumnComp

				TableCellDelegateBase {
					id: revokeDelegate

					ToolButton {
						id: revokeButton
						objectName: "RevokeServerTokenButton"
						anchors.centerIn: parent
						width: Style.buttonWidthM
						height: width
						iconSource: "qrc:/" + Style.getIconPath("Icons/Delete", Icon.State.On, Icon.Mode.Normal)
						tooltipText: qsTr("Revoke the token")
						onClicked: {
							let tokenId = tokenTable.elements.get(revokeDelegate.rowIndex).item.m_id
							ModalDialogManager.openDialog(confirmRevokeDialogComp, {"tokenId": tokenId})
						}
					}

					onReused: {
						revokeButton.enabled = !tokenTable.elements.get(revokeDelegate.rowIndex).item.m_revoked
					}
				}
			}
		}

		BaseText {
			width: parent.width
			visible: tokensPage.tokenCount === 0
			text: qsTr("This product has no tokens yet.")
			color: Style.inactiveTextColor
			wrapMode: Text.WordWrap
		}
	}

	Component {
		id: confirmRevokeDialogComp

		MessageDialog {
			title: qsTr("Revoke the token?")
			message: qsTr("A server using this token stops working with the receiving server.")

			property string tokenId

			onFinished: {
				if (buttonId === Enums.yes)
					tokensPage.apiClient.revokeToken(tokensPage.productId, tokenId)
			}
		}
	}

	Component {
		id: tokenCreatedDialogComp

		Dialog {
			id: tokenCreatedDialog
			title: qsTr("Token issued")
			canMove: false
			width: 500
			height: 200
			backgroundColor: Style.baseColor

			property string token

			Component.onCompleted: {
				addButton(Enums.ok, qsTr("OK"), true)
			}

			contentComp: Component {
				Item {
					width: tokenCreatedDialog.width
					height: tokenContent.height

					Column {
						id: tokenContent
						anchors.horizontalCenter: parent.horizontalCenter
						anchors.top: parent.top
						anchors.topMargin: Style.marginL
						width: parent.width - 2 * Style.marginL
						spacing: Style.marginL

						BaseText {
							text: qsTr("Copy the token now, it is not shown again:")
						}

						CustomTextField {
							objectName: "ServerTokenValueField"
							width: parent.width
							height: Style.controlHeightM
							readOnly: true
							text: tokenCreatedDialog.token
						}
					}
				}
			}
		}
	}

	Component {
		id: addTokenDialogComp

		Dialog {
			id: addDialog
			title: qsTr("New server token")
			width: 520
			height: 420
			backgroundColor: Style.baseColor

			property string receiverId: "Puma"
			property string tokenName: ""
			property string tokenDescription: ""
			property bool canWatch: true
			property bool canSend: true
			property bool canPublish: true

			function updateApplyButton(){
				addDialog.setButtonEnabled(Enums.apply, addDialog.tokenName !== "" && addDialog.receiverId !== "")
			}

			onFinished: {
				if (buttonId !== Enums.apply)
					return

				let scopes = []
				if (addDialog.canWatch)
					scopes.push("/ProductServer/Collections/Watch")
				if (addDialog.canSend)
					scopes.push("/ProductServer/Notifications/Send")
				if (addDialog.canPublish)
					scopes.push("/ProductServer/Permissions/Publish")

				tokensPage.apiClient.createToken(tokensPage.productId, addDialog.receiverId, addDialog.tokenName, addDialog.tokenDescription, scopes)
			}

			Component.onCompleted: {
				clearButtons()
				addButton(Enums.apply, qsTr("Issue"), false)
				addButton(Enums.cancel, qsTr("Cancel"), true)
			}

			contentComp: Component {
				Item {
					width: addDialog.width
					height: dialogContent.height

					Column {
						id: dialogContent
						anchors.horizontalCenter: parent.horizontalCenter
						anchors.top: parent.top
						anchors.topMargin: Style.marginL
						width: parent.width - 2 * Style.marginXL
						spacing: Style.marginM

						BaseText {
							text: qsTr("Name")
						}

						CustomTextField {
							objectName: "ServerTokenNameInput"
							width: parent.width
							height: Style.controlHeightM
							onTextChanged: {
								addDialog.tokenName = text.trim()
								addDialog.updateApplyButton()
							}
						}

						BaseText {
							text: qsTr("Server accepting the token")
						}

						CustomTextField {
							objectName: "ServerTokenReceiverInput"
							width: parent.width
							height: Style.controlHeightM
							text: addDialog.receiverId
							onTextChanged: {
								addDialog.receiverId = text.trim()
								addDialog.updateApplyButton()
							}
						}

						BaseText {
							text: qsTr("Description")
						}

						CustomTextField {
							objectName: "ServerTokenDescriptionInput"
							width: parent.width
							height: Style.controlHeightM
							onTextChanged: {
								addDialog.tokenDescription = text
							}
						}

						CheckBox {
							text: qsTr("Watch collections")
							checkState: addDialog.canWatch ? Qt.Checked : Qt.Unchecked
							onClicked: {
								addDialog.canWatch = !addDialog.canWatch
							}
						}

						CheckBox {
							text: qsTr("Send notifications")
							checkState: addDialog.canSend ? Qt.Checked : Qt.Unchecked
							onClicked: {
								addDialog.canSend = !addDialog.canSend
							}
						}

						CheckBox {
							text: qsTr("Publish permissions")
							checkState: addDialog.canPublish ? Qt.Checked : Qt.Unchecked
							onClicked: {
								addDialog.canPublish = !addDialog.canPublish
							}
						}
					}
				}
			}
		}
	}
}
