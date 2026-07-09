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
		createTokenInput.m_productId = AuthorizationController.productId
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
				width: parent.width
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
									id: revokeButton
									anchors.centerIn: parent
									width: Style.buttonWidthM
									height: width
									iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Restore", Icon.State.On, Icon.Mode.Normal) :
												"../../../" + Style.getIconPath("Icons/Restore", Icon.State.Off, Icon.Mode.Disabled)
									tooltipText: enabled ? qsTr("Revoke Token") : qsTr("Revoked")
									onClicked: {
										let tokenId = tokenTable.elements.get(revokeColumnDelegateBase.rowIndex).item.m_id
										patTokenEditor.revokeToken(tokenId)
									}
								}
								onReused: {
									let revoked = tokenTable.elements.get(revokeColumnDelegateBase.rowIndex).item.m_revoked
									revokeButton.enabled = !revoked
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
					title: qsTr("New Personal Access Token")
					width: 780
					height: 720
					
					property string tokenName: ""
					property string tokenDescription: ""
					property string expiresAt: ""
					property var selectedScopes: []
					
					Component.onCompleted: {
						clearButtons()
						addButton(Enums.apply, qsTr("Generate Token"), false)
						addButton(Enums.cancel, qsTr("Cancel"), true)
					}
					
					onFinished: {
						if (buttonId === Enums.apply){
							patTokenEditor.addNewToken(tokenName.trim(), tokenDescription, selectedScopes, expiresAt)
						}
					}
					
					contentComp: Component {
						Item {
							id: contentItem
							width: addDialog.width
							height: 600

							function checkAddButtonEnabled(){
								var hasName = nameInputElementView.text !== ""
								var hasExpiration = expirationCb.currentIndex >= 0
								var checkedIds = scopesPermissionsView.getCheckedIds()
								var hasScopes = checkedIds.length > 0
								addDialog.selectedScopes = checkedIds
								addDialog.setButtonEnabled(Enums.apply, hasName && hasExpiration && hasScopes)

								if (!hasName)
									validationHint.text = qsTr("Enter a token name to continue")
								else if (!hasScopes)
									validationHint.text = qsTr("Select at least one permission")
								else
									validationHint.text = ""
							}

							property bool scopesLoading: true
							property bool scopesEmpty: false

							GqlBasedPermissionsProvider {
								id: dialogPermissionsProvider
								productId: AuthorizationController.productId
								onUserPermissionsReceived: {
									scopesPermissionsView.rebuildFromFlatArray(userPermissions)
									scopesPermissionsView.expandAll()
									contentItem.scopesEmpty = !userPermissions || userPermissions.length === 0
									contentItem.scopesLoading = false
								}
								onRequestFailed: {
									contentItem.scopesEmpty = true
									contentItem.scopesLoading = false
								}
							}

							Component.onCompleted: {
								contentItem.scopesLoading = true
								dialogPermissionsProvider.requestUserPermissions()
							}

							// --- Top section: token metadata (compact horizontal grid) ---
							Item {
								id: formSection
								anchors.left: parent.left
								anchors.right: parent.right
								anchors.top: parent.top
								anchors.leftMargin: Style.marginXL
								anchors.rightMargin: Style.marginXL
								anchors.topMargin: Style.marginL
								height: formRow.height + descriptionRow.height + Style.marginM

								// Row 1: Name + Expiration side by side
								Row {
									id: formRow
									anchors.left: parent.left
									anchors.right: parent.right
									anchors.top: parent.top
									spacing: Style.marginXL

									Column {
										width: (formRow.width - Style.marginXL) * 0.6
										spacing: Style.marginXS

										BaseText {
											text: qsTr("Token Name")
											font.pixelSize: Style.fontSizeM
											font.bold: true
										}
										CustomTextField {
											id: nameInputElementView
											width: parent.width
											height: Style.controlHeightM
											placeHolderText: qsTr("e.g. CI/CD Pipeline, API Client...")
											text: addDialog.tokenName
											onEditingFinished: {
												addDialog.tokenName = text
												contentItem.checkAddButtonEnabled()
											}
											RegularExpressionValidator {
												id: notEmptyRegexp
												regularExpression: /^(?!\s*$).+/
											}
										}
									}

									Column {
										width: (formRow.width - Style.marginXL) * 0.4
										spacing: Style.marginXS

										BaseText {
											text: qsTr("Expiration")
											font.pixelSize: Style.fontSizeM
											font.bold: true
										}
										ComboBox {
											id: expirationCb
											width: parent.width
											height: Style.controlHeightM
											model: expirationModel
											currentIndex: 1
											TreeItemModel {
												id: expirationModel
												Component.onCompleted: {
													var index = expirationModel.insertNewItem()
													expirationModel.setData("id", "7", index)
													expirationModel.setData("name", qsTr("7 Days"), index)

													index = expirationModel.insertNewItem()
													expirationModel.setData("id", "30", index)
													expirationModel.setData("name", qsTr("30 Days"), index)

													index = expirationModel.insertNewItem()
													expirationModel.setData("id", "60", index)
													expirationModel.setData("name", qsTr("60 Days"), index)

													index = expirationModel.insertNewItem()
													expirationModel.setData("id", "90", index)
													expirationModel.setData("name", qsTr("90 Days"), index)

													index = expirationModel.insertNewItem()
													expirationModel.setData("id", "unlimited", index)
													expirationModel.setData("name", qsTr("No Expiration"), index)
												}
											}

											function computeExpiresAtIso() {
												var id = expirationModel.getData("id", currentIndex)
												if (id === "unlimited" || id === "" || id === undefined || id === null)
													return ""
												var days = Number(id)
												if (days <= 0)
													return ""
												var d = new Date()
												d.setDate(d.getDate() + days)
												return d.toISOString()
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

								// Row 2: Description (full width)
								Column {
									id: descriptionRow
									anchors.left: parent.left
									anchors.right: parent.right
									anchors.top: formRow.bottom
									anchors.topMargin: Style.marginM
									spacing: Style.marginXS

									BaseText {
										text: qsTr("Description (optional)")
										font.pixelSize: Style.fontSizeM
										color: Style.inactiveTextColor
									}
									CustomTextField {
										width: parent.width
										height: Style.controlHeightM
										placeHolderText: qsTr("What will this token be used for?")
										text: addDialog.tokenDescription
										onEditingFinished: {
											addDialog.tokenDescription = text
										}
									}
								}
							}

							BaseText {
								id: validationHint
								anchors.left: parent.left
								anchors.right: parent.right
								anchors.top: formSection.bottom
								anchors.leftMargin: Style.marginXL
								anchors.rightMargin: Style.marginXL
								anchors.topMargin: Style.marginS
								font.pixelSize: Style.fontSizeS
								color: Style.errorTextColor
								text: qsTr("Enter a token name to continue")
								height: text !== "" ? implicitHeight : 0
								visible: text !== ""
							}

// --- Bottom section: Permissions tree (takes all remaining space) ---
									Item {
										id: scopesSection
										anchors.left: parent.left
										anchors.right: parent.right
										anchors.top: validationHint.visible ? validationHint.bottom : formSection.bottom
										anchors.bottom: parent.bottom
										anchors.leftMargin: Style.marginXL
										anchors.rightMargin: Style.marginXL
										anchors.topMargin: Style.marginL
								anchors.bottomMargin: Style.marginS

								Row {
									id: scopesHeader
									anchors.left: parent.left
									anchors.right: parent.right
									anchors.top: parent.top
									spacing: Style.marginS

									BaseText {
										text: qsTr("Select Permissions")
										font.pixelSize: Style.fontSizeL
										font.bold: true
										anchors.verticalCenter: parent.verticalCenter
									}
									BaseText {
										text: qsTr("— grant only the access this token needs")
										font.pixelSize: Style.fontSizeS
										color: Style.inactiveTextColor
										anchors.verticalCenter: parent.verticalCenter
										visible: !contentItem.scopesLoading && !contentItem.scopesEmpty
									}
								}

								Rectangle {
									id: scopesArea
									anchors.left: parent.left
									anchors.right: parent.right
									anchors.top: scopesHeader.bottom
									anchors.topMargin: Style.marginS
									anchors.bottom: parent.bottom
									color: Style.baseColor
									border.width: 1
									border.color: Style.borderColor
									radius: Style.radiusM
									clip: true

									PermissionsTableView {
										id: scopesPermissionsView
										anchors.fill: parent
										anchors.leftMargin: Style.marginM
										anchors.rightMargin: Style.marginM
										anchors.topMargin: Style.marginXS
										anchors.bottomMargin: Style.marginS
										controlPanelTopMargin: Style.marginS
										treeTopMargin: Style.marginXS
										treeBottomMargin: Style.marginS
										readOnly: false
										showControlPanel: true
										visible: !contentItem.scopesLoading && !contentItem.scopesEmpty
										onSelectionChanged: contentItem.checkAddButtonEnabled()
									}

									Loading {
										anchors.centerIn: parent
										width: 48
										height: 48
										indicatorSize: 40
										color: "transparent"
										background.color: "transparent"
										visible: contentItem.scopesLoading
									}

									BaseText {
										anchors.centerIn: parent
										width: parent.width - 2*Style.marginXL
										horizontalAlignment: Text.AlignHCenter
										wrapMode: Text.WordWrap
										text: qsTr("No permissions available to assign to this token.")
										font.pixelSize: Style.fontSizeM
										color: Style.inactiveTextColor
										visible: !contentItem.scopesLoading && contentItem.scopesEmpty
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
