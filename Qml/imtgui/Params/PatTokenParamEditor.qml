import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtbaseImtBaseTypesSdl 1.0
import imtauthPersonalAccessTokensSdl 1.0
import imtbaseImtCollectionSdl 1.0


ParamEditorBase {
	id: patTokenEditor
	
	typeId: "PatTokenList"
	
	editorModelComp: Component {
		PatTokenListParam {}
	}

	property PersonalAccessTokenList personalAccessTokenList: editorModel

	function addNewToken(name, description, scopes, expiresAt){
		createTokenInput.m_userId = AuthorizationController.getUserId()
		createTokenInput.m_name = name
		createTokenInput.m_description = description
		createTokenInput.m_scopes = scopes
		createTokenInput.m_expiresAt = expiresAt

		createTokenRequest.send(createTokenInput)
	}

	function deleteToken(tokenId){
		inputId.m_id = tokenId
		deleteTokenRequest.send(inputId)
	}

	function revokeToken(tokenId){
		inputId.m_id = tokenId
		revokeTokenRequest.send(inputId)
	}

	function getTokenList(){
		userIdInput.m_userId = AuthorizationController.getUserId()
		getTokenListRequest.send(userIdInput)
	}

	CreateTokenInput {
		id: createTokenInput
	}

	InputId {
		id: inputId
	}

	UserIdInput {
		id: userIdInput
	}
	
	GqlSdlRequestSender {
		id: getTokenListRequest
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_getTokenList
		sdlObjectComp: Component {
			PersonalAccessTokenList {
				onFinished: {
					patTokenEditor.personalAccessTokenList = this
				}
			}
		}
	}
	
	GqlSdlRequestSender {
		id: createTokenRequest
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_createToken
		requestType: 1
		sdlObjectComp: Component {
			CreateTokenPayload {
				onFinished: {
					if (m_success){
						ModalDialogManager.openDialog(tokenCreatedDialogComp, {"token":m_token})
						patTokenEditor.getTokenList()
					}
					else{
						ModalDialogManager.showErrorDialog(m_message)
					}
				}
			}
		}
	}

	GqlSdlRequestSender {
		id: deleteTokenRequest
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_deleteToken
		requestType: 1
		sdlObjectComp: Component {
			DeleteTokenPayload {
				onFinished: {
					if (m_success){
						ModalDialogManager.showInfoDialog(m_message)
						patTokenEditor.getTokenList()
					}
					else{
						ModalDialogManager.showErrorDialog(m_message)
					}
				}
			}
		}
	}

	GqlSdlRequestSender {
		id: revokeTokenRequest
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_revokeToken
		requestType: 1
		sdlObjectComp: Component {
			RevokeTokenPayload {
				onFinished: {
					if (m_success){
						ModalDialogManager.showInfoDialog(m_message)
						patTokenEditor.getTokenList()
					}
					else{
						ModalDialogManager.showErrorDialog(m_message)
					}
				}
			}
		}
	}

	Component.onCompleted: {
		patTokenEditor.getTokenList()
	}

	Component {
		id: tokenCreatedDialogComp
		Dialog {
			id: tokenCreatedDialog
			title: qsTr("Token Created Successfully")
			canMove: false
			width: 500
			height: 200

			Component.onCompleted: {
				addButton(Enums.ok, qsTr("OK"), true)
			}

			property string token
			contentComp: Component {
				Item {
					width: tokenCreatedDialog.width
					height: tokenContent.height
					Column {
						id: tokenContent
						anchors.horizontalCenter: parent.horizontalCenter
						anchors.top: parent.top
						anchors.topMargin: Style.marginL
						width: parent.width - 2*Style.marginL
						spacing: Style.marginL

						BaseText {
							text: qsTr("Please copy and save the token:")
						}

						CustomTextField {
							id: tokenInput
							width: parent.width
							readOnly: true
							text: tokenCreatedDialog.token
							height: Style.controlHeightM
							focus: true

							property bool copied: false
							ToolButton {
								z: parent.z + 1
								anchors.verticalCenter: parent.verticalCenter
								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								width: Style.buttonWidthM
								height: width
								enabled: !tokenInput.copied
								iconSource: tokenInput.copied ? 
												"../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal) :
												"../../../" + Style.getIconPath("Icons/Copy", Icon.State.On, Icon.Mode.Normal)
								tooltipText: tokenInput.copied ? qsTr("The token is copied") : qsTr("Copy the token")
								onClicked: {
									//TODO: Not working -> check later
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

	sourceComp: Component {
		Item {
			id: tokenEditorView
			width: patTokenEditor.width
			height: headerElementView.height

			ButtonElementView {
				id: headerElementView
				width: contentColumn.width
				name: patTokenEditor.name
				description: patTokenEditor.description
				text: qsTr("New Token")
				onClicked: {
					ModalDialogManager.openDialog(addTokenDialogComp, {})
				}
				
				bottomComp: Component {
					Table {
						id: tokenTable
						width: headerElementView.contentWidth
						height: contentHeight + headerHeight
						property PersonalAccessTokenList personalAccessTokenList: patTokenEditor.personalAccessTokenList
					
						onPersonalAccessTokenListChanged: {
							if (personalAccessTokenList){
								tokenTable.elements = personalAccessTokenList.m_tokens
							}
							else{
								tokenTable.elements = 0
							}
						}

						TableHeaders {
							Component.onCompleted: {
								addHeader("name", "Name")
								addHeader("description", "Description")
								// addHeader("createdAt", "Created At")
								addHeader("expiresAt", "Expires At")
								addHeader("revoked", "Revoked")
								addHeader("remove", "")
								tokenTable.headers = this
							}
						}
						
						onHeadersChanged: {
							// setColumnContentById("createdAt", timeColumnComp)
							setColumnContentById("expiresAt", timeColumnComp)
							setColumnContentById("remove", removeColumnComp)
							setColumnContentById("revoked", revokeColumnComp)
						}
						
						Component {
							id: timeColumnComp
							TableCellDelegateBase {
								Text {
									id: timeTextFromISO
									anchors.verticalCenter: parent.verticalCenter
									anchors.left: parent.left
									anchors.leftMargin: Style.marginM
									anchors.right: parent.right
									font.pixelSize: Style.fontSizeM
									font.family: Style.fontFamily
									color: Style.textColor;
									elide: Text.ElideRight
								}
					
								onReused: {
									if (rowIndex >= 0){
										let value = getValue()
										if (value && value !== ""){
											timeTextFromISO.text = value.slice(0, 10)
										}
										else{
											timeTextFromISO.text = qsTr("No Expiration")
										}
									}
								}
							}
						}
						
						Component {
							id: removeColumnComp // REMOVE
							TableCellDelegateBase {
								id: removeColumnDelegateBase
								ToolButton {
									anchors.centerIn: parent
									width: Style.buttonWidthM
									height: width
									iconSource: "../../../" + Style.getIconPath("Icons/Delete", Icon.State.On, Icon.Mode.Normal)
									tooltipText: qsTr("Delete Token")
									onClicked: {
										let tokenId = tokenTable.elements.get(removeColumnDelegateBase.rowIndex).item.m_id
										let tokenName = tokenTable.elements.get(removeColumnDelegateBase.rowIndex).item.m_name
										
										ModalDialogManager.openDialog(confirmDeleteDialogComp, {"tokenId": tokenId, "tokenName": tokenName})
									}
								}
							}
						}

						Component {
							id: revokeColumnComp // REVOKE
							TableCellDelegateBase {
								id: revokeColumnDelegateBase
								ToolButton {
									anchors.centerIn: parent
									width: Style.buttonWidthM
									height: width
									iconSource: "../../../" + Style.getIconPath("Icons/Restore", Icon.State.On, Icon.Mode.Normal)
									tooltipText: qsTr("Revoke Token")
									onClicked: {
										let tokenId = tokenTable.elements.get(revokeColumnDelegateBase.rowIndex).item.m_id
										patTokenEditor.revokeToken(tokenId)
									}
								}
							}
						}

						Component {
							id: confirmDeleteDialogComp
							MessageDialog {
								title: qsTr("Are you sure you want to delete this token?");
								message: qsTr("Any applications or scripts using this token will no longer be able to access the API. You cannot undo this action.")

								property string tokenId
								property string tokenName
	
								onFinished: {
									if (buttonId == Enums.yes){
										patTokenEditor.deleteToken(tokenId)
									}
								}
							}
						}
					}
				}
			}

			// Add/Edit Token Dialog
			Component {
				id: addTokenDialogComp
				
				Dialog {
					id: addDialog
					title: qsTr("Add PAT Token")
					width: Style.sizeHintXL
					
					property string tokenName: ""
					property string tokenDescription: ""
					property string expiresAt: "" // "" = no expiration
					
					Component.onCompleted: {
						clearButtons()
						addButton(Enums.apply, qsTr("Add"), false)
						addButton(Enums.cancel, qsTr("Cancel"), true)
					}
					
					onFinished: {
						if (buttonId === Enums.apply){
							patTokenEditor.addNewToken(tokenName.trim(), tokenDescription, [], expiresAt)
						}
					}
					
					contentComp: Component {
						Item {
							id: contentItem
							width: addDialog.width
							height: groupElementView.height

							function checkAddButtonEnabled(){
								let enabled = nameInputElementView.text !== "" && expirationCb.currentIndex >= 0
								addDialog.setButtonEnabled(Enums.apply, enabled)
							}
						
							GroupElementView {
								id: groupElementView
								anchors.horizontalCenter: parent.horizontalCenter
								width: parent.width - 2*Style.marginL

								TextInputElementView {
									id: nameInputElementView
									width: parent.width
									name: qsTr("Token Name")
									description: qsTr("A descriptive name for this token")
									showErrorWhenInvalid: true
									textInputValidator: notEmptyRegexp
									errorText: qsTr("Please enter the token name")
									text: addDialog.tokenName
									controlWidth: 300
									onEditingFinished: {
										addDialog.tokenName = text
										contentItem.checkAddButtonEnabled()
									}
									
									RegularExpressionValidator {
										id: notEmptyRegexp;
										regularExpression: /^(?!\s*$).+/;
									}
								}

								TextInputElementView {
									width: parent.width
									name: qsTr("Description")
									description: qsTr("Optional description for this token")
									text: addDialog.tokenDescription
									controlWidth: 300
									onEditingFinished: {
										addDialog.tokenDescription = text
										contentItem.checkAddButtonEnabled()
									}
								}

								ComboBoxElementView {
									id: expirationCb
									width: parent.width
									name: qsTr("Expiration")
									controlWidth: 300
									description: qsTr("The token will expire on the selected date")
									model: expirationModel
									currentIndex: 1
									TreeItemModel {
										id: expirationModel
										Component.onCompleted: {
											let index = expirationModel.insertNewItem()
											expirationModel.setData("id", "7", index)
											expirationModel.setData("name", "7 Days", index)

											index = expirationModel.insertNewItem()
											expirationModel.setData("id", "30", index)
											expirationModel.setData("name", "30 Days", index)
											
											index = expirationModel.insertNewItem()
											expirationModel.setData("id", "60", index)
											expirationModel.setData("name", "60 Days", index)
											
											index = expirationModel.insertNewItem()
											expirationModel.setData("id", "90", index)
											expirationModel.setData("name", "90 Days", index)

											index = expirationModel.insertNewItem()
											expirationModel.setData("id", "unlimited", index)
											expirationModel.setData("name", "No Expiration", index)
										}
									}

									function computeExpiresAtIso() {
										var id = expirationModel.getData("id", currentIndex)
									
										if (id === "unlimited")
											return ""
									
										if (id === "" || id === undefined || id === null)
											return ""
									
										let days = Number(id)
										if (days <= 0)
											return ""
									
										let d = new Date()
										d.setDate(d.getDate() + days)
										return d.toISOString()//.slice(0, 10)
									}
								
									onCurrentIndexChanged: {
										addDialog.expiresAt = computeExpiresAtIso()
										contentItem.checkAddButtonEnabled()
									}
								
									Component.onCompleted: {
										addDialog.expiresAt = computeExpiresAtIso()
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
