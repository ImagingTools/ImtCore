// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ProfileTokensPage
 *
 * Access Tokens tab of the ProfileView — lists the user's Personal Access Tokens
 * and hosts the "New Token" dialog (with the permission-scopes tree).
 *
 * Transport-free: token list / create / delete / revoke and the scopes provider
 * all come from the injected `apiClient` (ProfileApiClient contract). Previously
 * this UI lived in imtgui/Params/PatTokenParamEditor.qml inside application Settings.
 */
ViewBase {
	id: tokensPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	property var apiClient: null
	// PersonalAccessTokenList model (opaque), used as the table's elements source.
	property var tokenList: null

	Component.onCompleted: {
		if (tokensPage.apiClient)
			tokensPage.apiClient.getTokenList()
	}

	// See ProfileView's onApiClientChanged for why this is needed: apiClient is
	// injected through several Loader layers and may not be settled yet when
	// Component.onCompleted above runs.
	onApiClientChanged: {
		if (tokensPage.apiClient)
			tokensPage.apiClient.getTokenList()
	}

	Connections {
		target: tokensPage.apiClient ? tokensPage.apiClient : null
		function onTokenListReceived(tokens) {
			tokensPage.tokenList = tokens
		}
		function onTokenCreated(token) {
			ModalDialogManager.openDialog(tokenCreatedDialogComp, {"token": token})
		}
		function onTokenDeleted(message) {
			banner.show(message || qsTr("Token deleted"), false)
		}
		function onTokenRevoked(message) {
			banner.show(message || qsTr("Token revoked"), false)
		}
		function onTokenOperationFailed(message) {
			banner.show(message || qsTr("Unable to complete the request."), true)
		}
	}

	readonly property int __tokenCount: tokensPage.tokenList && tokensPage.tokenList.m_tokens ? tokensPage.tokenList.m_tokens.count : 0

	ProfileNoticeBanner {
		id: banner
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Style.marginXL
	}

	CustomScrollbar {
		id: scrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: flickable.top
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		targetItem: flickable
	}

	Flickable {
		id: flickable
		anchors.top: banner.visible ? banner.bottom : parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: Style.marginXL
		anchors.topMargin: banner.visible ? Style.marginM : Style.marginXL
		contentWidth: width
		contentHeight: bodyColumn.height + Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: parent.width
			spacing: Style.marginXL

			// --- Header ---
			Column {
				width: parent.width
				spacing: Style.marginXS

				Text {
					text: qsTr("Access tokens")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Row {
					width: parent.width
					spacing: Style.marginM

					Text {
						anchors.verticalCenter: parent.verticalCenter
						width: parent.width - newTokenButton.width - parent.spacing
						text: qsTr("Tokens let scripts and integrations authenticate as you.")
						font.pixelSize: Style.fontSizeM
						font.family: Style.fontFamily
						color: Style.textColor
						wrapMode: Text.WordWrap
					}

					Button {
						id: newTokenButton
						anchors.verticalCenter: parent.verticalCenter
						text: qsTr("New Token")
						decorator: Component {
							ButtonDecorator {
								widthDefault: Style.buttonWidthXXL
							}
						}
						onClicked: {
							ModalDialogManager.openDialog(addTokenDialogComp, {})
						}
					}
				}
			}

			// --- Token list ---
			Rectangle {
				id: tokensCard
				width: parent.width
				height: tokenTable.height + 2 * Style.marginL
				visible: tokensPage.__tokenCount > 0
				border.width: 1
				border.color: Style.borderColor
				radius: Style.radiusL
				color: Style.baseColor

				Table {
					id: tokenTable
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.top: parent.top
					anchors.margins: Style.marginL
					height: contentHeight + headerHeight
					property var tokenList: tokensPage.tokenList

					onTokenListChanged: {
						if (tokenList)
							tokenTable.elements = tokenList.m_tokens
						else
							tokenTable.elements = 0
					}

					TableHeaders {
						Component.onCompleted: {
							addHeader("name", "Name")
							addHeader("description", "Description")
							addHeader("expiresAt", "Expires At")
							addHeader("revoked", "Revoked")
							addHeader("remove", "")
							tokenTable.headers = this
						}
					}

					onHeadersChanged: {
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
								color: Style.textColor
								elide: Text.ElideRight
							}

							onReused: {
								if (rowIndex >= 0) {
									let value = getValue()
									if (value && value !== "")
										timeTextFromISO.text = value.slice(0, 10)
									else
										timeTextFromISO.text = qsTr("No Expiration")
								}
							}
						}
					}

					Component {
						id: removeColumnComp // REMOVE
						TableCellDelegateBase {
							id: removeColumnDelegateBase
							ToolButton {
								// Test instrumentation: icon-only ToolButton (no text) falls back to
								// the generic, non-unique "Button" objectName otherwise. Inert.
								objectName: "DeleteTokenButton"
								anchors.centerIn: parent
								width: Style.buttonWidthM
								height: width
								iconSource: "qrc:/" + Style.getIconPath("Icons/Delete", Icon.State.On, Icon.Mode.Normal)
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
								objectName: "RevokeTokenButton"
								anchors.centerIn: parent
								width: Style.buttonWidthM
								height: width
								iconSource: enabled ? "qrc:/" + Style.getIconPath("Icons/Restore", Icon.State.On, Icon.Mode.Normal) :
										"qrc:/" + Style.getIconPath("Icons/Restore", Icon.State.Off, Icon.Mode.Disabled)
								tooltipText: enabled ? qsTr("Revoke Token") : qsTr("Revoked")
								onClicked: {
									let tokenId = tokenTable.elements.get(revokeColumnDelegateBase.rowIndex).item.m_id
									if (tokensPage.apiClient)
										tokensPage.apiClient.revokeToken(tokenId)
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
							title: qsTr("Are you sure you want to delete this token?")
							message: qsTr("Any applications or scripts using this token will no longer be able to access the API. You cannot undo this action.")

							property string tokenId
							property string tokenName

							onFinished: {
								if (buttonId == Enums.yes && tokensPage.apiClient)
									tokensPage.apiClient.deleteToken(tokenId)
							}
						}
					}
				}
			}

			// --- Empty state ---
			Text {
				width: parent.width
				visible: !tokensCard.visible
				text: qsTr("You don't have any access tokens yet. Create one to let a script or integration authenticate as you.")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				wrapMode: Text.WordWrap
			}
		}
	}

	// Token created — show the freshly generated secret once.
	Component {
		id: tokenCreatedDialogComp
		Dialog {
			id: tokenCreatedDialog
			title: qsTr("Token Created Successfully")
			canMove: false
			width: 500
			height: 200
			backgroundColor: Style.baseColor

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
						width: parent.width - 2 * Style.marginL
						spacing: Style.marginL

						BaseText {
							text: qsTr("Please copy and save the token:")
						}

						CustomTextField {
							id: tokenInput
							objectName: "TokenValueField"
							width: parent.width
							readOnly: true
							text: tokenCreatedDialog.token
							height: Style.controlHeightM
							focus: true

							property bool copied: false
							ToolButton {
								objectName: "CopyTokenButton"
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
									if (!tokenInput.copied) {
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

	// New Personal Access Token — metadata + permission scopes.
	Component {
		id: addTokenDialogComp

		Dialog {
			id: addDialog
			title: qsTr("New Personal Access Token")
			width: 780
			height: 720
			backgroundColor: Style.baseColor

			property string tokenName: ""
			property string tokenDescription: ""
			property int expirationDays: 30
			property var selectedScopes: []

			function computeExpiresAtIso() {
				if (addDialog.expirationDays <= 0)
					return ""
				var d = new Date()
				d.setDate(d.getDate() + addDialog.expirationDays)
				return d.toISOString()
			}

			Component.onCompleted: {
				clearButtons()
				addButton(Enums.apply, qsTr("Generate Token"), false)
				addButton(Enums.cancel, qsTr("Cancel"), true)
			}

			onFinished: {
				if (buttonId === Enums.apply && tokensPage.apiClient)
					tokensPage.apiClient.createToken(tokenName.trim(), tokenDescription, selectedScopes, computeExpiresAtIso())
			}

			contentComp: Component {
				Item {
					id: contentItem
					width: addDialog.width
					height: 600

					function checkAddButtonEnabled() {
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

					Connections {
						target: tokensPage.apiClient ? tokensPage.apiClient.permissionsProvider : null
						function onUserPermissionsReceived() {
							var userPermissions = tokensPage.apiClient.permissionsProvider.userPermissions
							scopesPermissionsView.rebuildFromFlatArray(userPermissions)
							contentItem.scopesEmpty = !userPermissions || userPermissions.length === 0
							contentItem.scopesLoading = false
						}
						function onRequestFailed(message, tenantId) {
							contentItem.scopesEmpty = true
							contentItem.scopesLoading = false
						}
					}

					Component.onCompleted: {
						contentItem.scopesLoading = true
						if (tokensPage.apiClient && tokensPage.apiClient.permissionsProvider)
							tokensPage.apiClient.permissionsProvider.requestUserPermissions()
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

								Text {
									text: qsTr("Token Name")
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}
								CustomTextField {
									id: nameInputElementView
									objectName: "TokenNameInput"
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

								Text {
									text: qsTr("Expiration")
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}
								ComboBox {
									id: expirationCb
									width: parent.width
									height: Style.controlHeightM
									model: expirationModel
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

									function updateExpirationDays() {
										var id = expirationModel.getData("id", currentIndex)
										if (id === "unlimited" || id === "" || id === undefined || id === null)
											addDialog.expirationDays = 0
										else
											addDialog.expirationDays = Number(id)
									}

									onCurrentIndexChanged: {
										updateExpirationDays()
										contentItem.checkAddButtonEnabled()
									}

									Component.onCompleted: {
										currentIndex = 4
										updateExpirationDays()
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

							Text {
								text: qsTr("Description (optional)")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							CustomTextField {
								objectName: "TokenDescriptionInput"
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
								width: parent.width - 2 * Style.marginXL
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
