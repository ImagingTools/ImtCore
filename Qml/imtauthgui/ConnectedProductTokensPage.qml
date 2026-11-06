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
			tokensPage.apiClient.requestProductServers(tokensPage.productId)
		}
	}

	// The permissions a token for the server can hold, from the catalog of the authorization server
	function getServerPermissions(receiverId){
		let servers = tokensPage.apiClient ? tokensPage.apiClient.productServers : null
		if (!servers){
			return null
		}

		for (let i = 0; i < servers.getItemsCount(); ++i){
			let server = servers.get(i).item
			if (server && String(server.m_productId) === receiverId){
				return server.m_permissions
			}
		}

		return null
	}

	function openTokenDialog(tokenId, receiverId, scopes){
		ModalDialogManager.openDialog(tokenDialogComp, {"tokenId": tokenId, "receiverId": receiverId, "initialScopes": scopes})
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
					tokensPage.openTokenDialog("", "", [])
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
					addHeader("permissions", "")
					addHeader("revoke", "")
					tokenTable.headers = this
				}
			}

			onHeadersChanged: {
				setColumnContentById("permissions", permissionsColumnComp)
				setColumnContentById("revoke", revokeColumnComp)
			}

			Component {
				id: permissionsColumnComp

				TableCellDelegateBase {
					id: permissionsDelegate

					ToolButton {
						id: permissionsButton
						objectName: "ServerTokenPermissionsButton"
						anchors.centerIn: parent
						width: Style.buttonWidthM
						height: width
						iconSource: "qrc:/" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)
						tooltipText: qsTr("Change the permissions of the token")
						onClicked: {
							let token = tokenTable.elements.get(permissionsDelegate.rowIndex).item
							tokensPage.openTokenDialog(String(token.m_id), String(token.m_productId), token.m_scopes ? token.m_scopes : [])
						}
					}

					onReused: {
						permissionsButton.enabled = !tokenTable.elements.get(permissionsDelegate.rowIndex).item.m_revoked
					}
				}
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
		id: tokenDialogComp

		Dialog {
			id: tokenDialog
			title: tokenDialog.tokenId === "" ? qsTr("New server token") : qsTr("Permissions of the token")
			width: 560
			height: 640
			backgroundColor: Style.baseColor

			// Set by openDialog after the dialog is created
			property string tokenId: ""
			property string receiverId: ""
			property var initialScopes: []

			property string tokenName: ""
			property string tokenDescription: ""
			property var selectedScopes: []

			onReceiverIdChanged: {
				if (tokenDialog.receiverId === "" && receiversModel.count > 0){
					tokenDialog.receiverId = receiversModel.get(0).productId

					return
				}

				tokenDialog.updatePermissions()
			}

			onInitialScopesChanged: {
				tokenDialog.selectedScopes = tokenDialog.initialScopes ? tokenDialog.initialScopes.slice(0) : []
				tokenDialog.updatePermissions()
			}

			onTokenIdChanged: {
				tokenDialog.updateApplyButton()
			}

			ListModel {
				id: receiversModel
			}

			ListModel {
				id: permissionsModel
			}

			function updateReceivers(){
				receiversModel.clear()

				let servers = tokensPage.apiClient ? tokensPage.apiClient.productServers : null
				if (servers){
					for (let i = 0; i < servers.getItemsCount(); ++i){
						let server = servers.get(i).item
						if (server){
							receiversModel.append({"productId": String(server.m_productId)})
						}
					}
				}

				if (tokenDialog.receiverId === "" && receiversModel.count > 0){
					tokenDialog.receiverId = receiversModel.get(0).productId
				}
			}

			// The required permissions are always held
			function updatePermissions(){
				permissionsModel.clear()

				let permissions = tokensPage.getServerPermissions(tokenDialog.receiverId)
				if (!permissions){
					tokenDialog.updateApplyButton()

					return
				}

				let scopes = tokenDialog.selectedScopes.slice(0)
				for (let i = 0; i < permissions.getItemsCount(); ++i){
					let permission = permissions.get(i).item
					let permissionId = String(permission.m_id)
					let isRequired = permission.m_required === true
					permissionsModel.append({
						"permissionId": permissionId,
						"name": String(permission.m_name),
						"description": permission.m_description ? String(permission.m_description) : "",
						"required": isRequired
					})

					if (isRequired && scopes.indexOf(permissionId) < 0){
						scopes.push(permissionId)
					}
				}

				tokenDialog.selectedScopes = scopes
				tokenDialog.updateApplyButton()
			}

			function isSelected(permissionId){
				return tokenDialog.selectedScopes.indexOf(permissionId) >= 0
			}

			function togglePermission(permissionId){
				let scopes = tokenDialog.selectedScopes.slice(0)
				let index = scopes.indexOf(permissionId)
				if (index >= 0){
					scopes.splice(index, 1)
				}
				else{
					scopes.push(permissionId)
				}

				tokenDialog.selectedScopes = scopes
			}

			// "SignIn": the required permissions and the changes of users, "All": everything
			function selectPreset(presetId){
				let scopes = []
				for (let i = 0; i < permissionsModel.count; ++i){
					let permission = permissionsModel.get(i)
					if (presetId === "All" || permission.required || permission.permissionId === "/ProductServer/Watch/Users"){
						scopes.push(permission.permissionId)
					}
				}

				tokenDialog.selectedScopes = scopes
			}

			function updateApplyButton(){
				let isNameSet = tokenDialog.tokenId !== "" || tokenDialog.tokenName !== ""
				tokenDialog.setButtonEnabled(Enums.apply, isNameSet && tokenDialog.receiverId !== "" && permissionsModel.count > 0)
			}

			onFinished: {
				if (buttonId !== Enums.apply)
					return

				if (tokenDialog.tokenId === ""){
					tokensPage.apiClient.createToken(tokensPage.productId, tokenDialog.receiverId, tokenDialog.tokenName, tokenDialog.tokenDescription, tokenDialog.selectedScopes)
				}
				else{
					tokensPage.apiClient.updateTokenScopes(tokensPage.productId, tokenDialog.tokenId, tokenDialog.selectedScopes)
				}
			}

			Component.onCompleted: {
				clearButtons()
				addButton(Enums.apply, qsTr("Apply"), false)
				addButton(Enums.cancel, qsTr("Cancel"), true)

				tokenDialog.updateReceivers()
			}

			contentComp: Component {
				Item {
					width: tokenDialog.width
					height: tokenDialog.height - 3 * Style.controlHeightM - 2 * Style.marginXL

					Flickable {
						id: dialogFlickable
						anchors.fill: parent
						anchors.topMargin: Style.marginL
						contentWidth: width
						contentHeight: dialogContent.height + Style.marginL
						clip: true
						boundsBehavior: Flickable.StopAtBounds

						Column {
							id: dialogContent
							anchors.horizontalCenter: parent.horizontalCenter
							width: dialogFlickable.width - 2 * Style.marginXL
							spacing: Style.marginM

							BaseText {
								visible: tokenDialog.tokenId === ""
								text: qsTr("Name")
							}

							CustomTextField {
								objectName: "ServerTokenNameInput"
								visible: tokenDialog.tokenId === ""
								width: parent.width
								height: Style.controlHeightM
								onTextChanged: {
									tokenDialog.tokenName = text.trim()
									tokenDialog.updateApplyButton()
								}
							}

							BaseText {
								visible: tokenDialog.tokenId === ""
								text: qsTr("Description")
							}

							CustomTextField {
								objectName: "ServerTokenDescriptionInput"
								visible: tokenDialog.tokenId === ""
								width: parent.width
								height: Style.controlHeightM
								onTextChanged: {
									tokenDialog.tokenDescription = text
								}
							}

							BaseText {
								text: tokenDialog.tokenId === "" ? qsTr("Server accepting the token") : qsTr("Server accepting the token: %1").arg(tokenDialog.receiverId)
							}

							Row {
								visible: tokenDialog.tokenId === ""
								spacing: Style.marginL

								Repeater {
									model: receiversModel

									delegate: CheckBox {
										objectName: "ServerTokenReceiver" + model.productId
										text: model.productId
										checkState: tokenDialog.receiverId === model.productId ? Qt.Checked : Qt.Unchecked
										onClicked: {
											tokenDialog.selectedScopes = []
											tokenDialog.receiverId = model.productId
										}
									}
								}
							}

							Row {
								spacing: Style.marginM

								BaseText {
									anchors.verticalCenter: parent.verticalCenter
									text: qsTr("Permissions")
									font.bold: true
								}

								Button {
									objectName: "ServerTokenSignInPresetButton"
									text: qsTr("Sign-in only")
									onClicked: {
										tokenDialog.selectPreset("SignIn")
									}
								}

								Button {
									objectName: "ServerTokenAllPresetButton"
									text: qsTr("All")
									onClicked: {
										tokenDialog.selectPreset("All")
									}
								}
							}

							Repeater {
								model: permissionsModel

								delegate: Column {
									width: dialogContent.width
									spacing: Style.spacingXS

									CheckBox {
										objectName: "ServerTokenPermission" + model.permissionId
										text: model.required ? model.name + " " + qsTr("(required)") : model.name
										enabled: !model.required
										checkState: tokenDialog.isSelected(model.permissionId) ? Qt.Checked : Qt.Unchecked
										onClicked: {
											tokenDialog.togglePermission(model.permissionId)
										}
									}

									BaseText {
										visible: model.description !== ""
										width: parent.width
										leftPadding: Style.marginXL
										text: model.description
										color: Style.inactiveTextColor
										wrapMode: Text.WordWrap
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
