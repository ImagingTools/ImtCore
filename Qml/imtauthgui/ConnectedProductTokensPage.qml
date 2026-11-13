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
			tokensPage.apiClient.productId = tokensPage.productId
			tokensPage.apiClient.requestTokens()
			tokensPage.apiClient.requestProductServer()
		}
	}

	// The tokens are issued by this server for itself
	function openTokenDialog(tokenId, scopes){
		ModalDialogManager.openDialog(tokenDialogComp, {"tokenId": tokenId, "initialScopes": scopes})
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
					tokensPage.openTokenDialog("", [])
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
					addHeader("createdAt", qsTr("Created"))
					addHeader("lastUsedAt", qsTr("Last used"))
					addHeader("revoked", qsTr("Status"))
					addHeader("actions", "")
					tokenTable.headers = this
				}
			}

			onHeadersChanged: {
				setColumnContentById("createdAt", dateColumnComp)
				setColumnContentById("lastUsedAt", dateColumnComp)
				setColumnContentById("revoked", statusColumnComp)
				setColumnContentById("actions", actionsColumnComp)

				// The buttons need their own width only, the rest goes to the text columns
				tokenTable.setWidth()
				let actionsIndex = tokenTable.getHeaderIndex("actions")
				if (actionsIndex >= 0 && tokenTable.widthDecorator.getItemsCount() > actionsIndex){
					tokenTable.widthDecorator.setData("Width", 2 * Style.buttonWidthM + 3 * Style.marginM, actionsIndex)
					tokenTable.widthDecorator.setData("WidthPercent", -1, actionsIndex)
					tokenTable.setWidth()
				}
			}

			Component {
				id: dateColumnComp

				TableCellDateDelegate {}
			}

			Component {
				id: statusColumnComp

				TableCellDelegateBase {
					id: statusDelegate

					TableCellText {
						id: statusText
						anchors.left: parent.left
						anchors.leftMargin: statusDelegate.rowDelegate ? statusDelegate.rowDelegate.textLeftMargin : 0
						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter
						rowDelegate: statusDelegate.rowDelegate
						columnIndex: statusDelegate.columnIndex
					}

					onReused: {
						statusText.text = tokenTable.elements.get(statusDelegate.rowIndex).item.m_revoked ? qsTr("Revoked") : qsTr("Active")
					}
				}
			}

			// An active token is edited or revoked, a revoked one can only be deleted
			Component {
				id: actionsColumnComp

				TableCellDelegateBase {
					id: actionsDelegate

					property bool isRevoked: false

					onReused: {
						actionsDelegate.isRevoked = tokenTable.elements.get(actionsDelegate.rowIndex).item.m_revoked ? true : false
					}

					Row {
						anchors.centerIn: parent
						spacing: Style.marginM

						ToolButton {
							objectName: "ServerTokenPermissionsButton"
							visible: !actionsDelegate.isRevoked
							width: Style.buttonWidthM
							height: width
							iconSource: "qrc:/" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)
							tooltipText: qsTr("Change the permissions of the token")
							onClicked: {
								let token = tokenTable.elements.get(actionsDelegate.rowIndex).item
								tokensPage.openTokenDialog(String(token.m_id), token.m_scopes ? token.m_scopes : [])
							}
						}

						ToolButton {
							objectName: "RevokeServerTokenButton"
							visible: !actionsDelegate.isRevoked
							width: Style.buttonWidthM
							height: width
							iconSource: "qrc:/" + Style.getIconPath("Icons/Restore", Icon.State.On, Icon.Mode.Normal)
							tooltipText: qsTr("Revoke the token")
							onClicked: {
								let tokenId = tokenTable.elements.get(actionsDelegate.rowIndex).item.m_id
								ModalDialogManager.openDialog(confirmRevokeDialogComp, {"tokenId": tokenId})
							}
						}

						ToolButton {
							objectName: "DeleteServerTokenButton"
							visible: actionsDelegate.isRevoked
							width: Style.buttonWidthM
							height: width
							iconSource: "qrc:/" + Style.getIconPath("Icons/Delete", Icon.State.On, Icon.Mode.Normal)
							tooltipText: qsTr("Delete the token")
							onClicked: {
								let tokenId = tokenTable.elements.get(actionsDelegate.rowIndex).item.m_id
								ModalDialogManager.openDialog(confirmDeleteDialogComp, {"tokenId": tokenId})
							}
						}
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
					tokensPage.apiClient.revokeToken(tokenId)
			}
		}
	}

	Component {
		id: confirmDeleteDialogComp

		MessageDialog {
			title: qsTr("Delete the token?")
			message: qsTr("The revoked token is removed from the list. You cannot undo this action.")

			property string tokenId

			onFinished: {
				if (buttonId === Enums.yes)
					tokensPage.apiClient.deleteToken(tokenId)
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
							id: tokenInput
							objectName: "ServerTokenValueField"
							width: parent.width
							height: Style.controlHeightM
							readOnly: true
							text: tokenCreatedDialog.token
							focus: true

							property bool copied: false

							ToolButton {
								objectName: "CopyServerTokenButton"
								z: parent.z + 1
								anchors.verticalCenter: parent.verticalCenter
								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								width: Style.buttonWidthM
								height: width
								enabled: !tokenInput.copied
								iconSource: tokenInput.copied ?
												"qrc:/" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal) :
												"qrc:/" + Style.getIconPath("Icons/Copy", Icon.State.On, Icon.Mode.Normal)
								tooltipText: tokenInput.copied ? qsTr("The token is copied") : qsTr("Copy the token")
								onClicked: {
									if (!tokenInput.copied){
										tokenInput.selectAll()
										tokenInput.copy()
										tokenInput.copied = true
									}
								}
							}
						}
					}
				}
			}
		}
	}

	Component {
		id: tokenDialogComp

		Dialog {
			id: tokenDialog
			title: tokenDialog.tokenId === "" ? qsTr("New server token") : qsTr("Permissions of the token")
			width: Math.max(Style.sizeHintXXL, Math.min(ModalDialogManager.activeView.width - 100, 900))
			height: Math.max(Style.sizeHintL, Math.min(ModalDialogManager.activeView.height - 100, 900))
			backgroundColor: Style.baseColor

			// Set by openDialog after the dialog is created
			property string tokenId: ""
			property var initialScopes: []

			property string tokenName: ""
			property string tokenDescription: ""

			// Set by the content when it is created
			property var permissionsView: null

			onInitialScopesChanged: {
				tokenDialog.updatePermissions()
			}

			onTokenIdChanged: {
				tokenDialog.updateApplyButton()
			}

			Connections {
				target: tokensPage.apiClient

				function onProductServerChanged(){
					tokenDialog.updatePermissions()
				}
			}

			function updatePermissions(){
				if (!tokenDialog.permissionsView){
					return
				}

				let server = tokensPage.apiClient ? tokensPage.apiClient.productServer : null
				tokenDialog.permissionsView.setPermissions(
							server ? server.groups : [],
							tokenDialog.initialScopes ? tokenDialog.initialScopes : [])

				tokenDialog.updateApplyButton()
			}

			function updateApplyButton(){
				let isNameSet = tokenDialog.tokenId !== "" || tokenDialog.tokenName !== ""
				let hasPermissions = tokenDialog.permissionsView !== null && tokenDialog.permissionsView.hasPermissions
				tokenDialog.setButtonEnabled(Enums.apply, isNameSet && hasPermissions)
			}

			onFinished: {
				if (buttonId !== Enums.apply || !tokenDialog.permissionsView)
					return

				let scopes = tokenDialog.permissionsView.getCheckedIds()
				if (tokenDialog.tokenId === ""){
					tokensPage.apiClient.createToken(tokenDialog.tokenName, tokenDialog.tokenDescription, scopes)
				}
				else{
					tokensPage.apiClient.updateTokenScopes(tokenDialog.tokenId, scopes)
				}
			}

			Component.onCompleted: {
				clearButtons()
				addButton(Enums.apply, qsTr("Apply"), false)
				addButton(Enums.cancel, qsTr("Cancel"), true)
			}

			contentComp: Component {
				Item {
					id: dialogContent
					width: tokenDialog.width
					height: tokenDialog.height - 3 * Style.controlHeightM - 2 * Style.marginXL

					Component.onCompleted: {
						tokenDialog.permissionsView = permissionsView
						tokenDialog.updatePermissions()
					}

					Column {
						id: headerColumn
						anchors.top: parent.top
						anchors.topMargin: Style.marginL
						anchors.horizontalCenter: parent.horizontalCenter
						width: parent.width - 2 * Style.marginXL
						spacing: Style.marginM

						// One line for the name and the description, as in the dialog of personal tokens: the dialog is about the permissions
						Row {
							id: metadataRow
							visible: tokenDialog.tokenId === ""
							width: parent.width
							spacing: Style.marginXL

							Column {
								width: Math.round((metadataRow.width - metadataRow.spacing) * 0.4)
								spacing: Style.marginXS

								Text {
									text: qsTr("Name")
									font.pixelSize: Style.fontSizeS
									font.family: Style.fontFamily
									color: Style.inactiveTextColor
								}

								CustomTextField {
									objectName: "ServerTokenNameInput"
									width: parent.width
									height: Style.controlHeightM
									placeHolderText: qsTr("e.g. ProLife production")
									onTextChanged: {
										tokenDialog.tokenName = text.trim()
										tokenDialog.updateApplyButton()
									}
								}
							}

							Column {
								width: metadataRow.width - metadataRow.spacing - Math.round((metadataRow.width - metadataRow.spacing) * 0.4)
								spacing: Style.marginXS

								Text {
									text: qsTr("Description")
									font.pixelSize: Style.fontSizeS
									font.family: Style.fontFamily
									color: Style.inactiveTextColor
								}

								CustomTextField {
									objectName: "ServerTokenDescriptionInput"
									width: parent.width
									height: Style.controlHeightM
									placeHolderText: qsTr("Optional: where the server runs")
									onTextChanged: {
										tokenDialog.tokenDescription = text
									}
								}
							}
						}

						BaseText {
							text: qsTr("Permissions")
							font.bold: true
						}
					}

					ServerTokenPermissionsView {
						id: permissionsView
						tableObjectName: "ServerTokenPermissionsView"
						anchors.top: headerColumn.bottom
						anchors.topMargin: Style.marginS
						anchors.bottom: parent.bottom
						anchors.left: headerColumn.left
						anchors.right: headerColumn.right
					}
				}
			}
		}
	}
}
