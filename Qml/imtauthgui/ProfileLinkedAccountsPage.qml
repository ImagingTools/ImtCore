// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ProfileLinkedAccountsPage
 *
 * Linked accounts tab of the ProfileView: the external providers offered by the server
 * (Google, Apple, Facebook), whether an account of each is linked to the current user,
 * and the commands to link (a sign-in with the provider) or unlink it.
 *
 * Transport-free: reads and unlinks through `apiClient`, links through AuthorizationController.
 */
ViewBase {
	id: linkedAccountsPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	property var apiClient: null

	property var linkedAccounts: []
	property bool hasPassword: true

	// One row per offered provider: { id, name, isLinked, email }
	property var rows: []

	function updateRows() {
		let providers = AuthorizationController.externalLoginProviders
		let result = []
		for (let i = 0; i < providers.length; i++) {
			let providerId = String(providers[i].id)
			let email = ""
			let isLinked = false
			for (let j = 0; j < linkedAccountsPage.linkedAccounts.length; j++) {
				if (String(linkedAccountsPage.linkedAccounts[j].providerId) === providerId) {
					isLinked = true
					email = String(linkedAccountsPage.linkedAccounts[j].email)
				}
			}

			result.push({
				"id": providerId,
				"name": String(providers[i].name),
				"type": String(providers[i].type),
				"isLinked": isLinked,
				"email": email
			})
		}

		linkedAccountsPage.rows = result
	}

	function reload() {
		if (linkedAccountsPage.apiClient)
			linkedAccountsPage.apiClient.getLinkedAccounts()
	}

	Component.onCompleted: {
		linkedAccountsPage.updateRows()
		linkedAccountsPage.reload()
		AuthorizationController.prepareExternalLogins(true)
	}

	Connections {
		target: linkedAccountsPage.apiClient ? linkedAccountsPage.apiClient : null

		function onLinkedAccountsReceived(accounts, hasPassword) {
			linkedAccountsPage.linkedAccounts = accounts
			linkedAccountsPage.hasPassword = hasPassword
			linkedAccountsPage.updateRows()
		}

		function onAccountUnlinked(providerId) {
			PopupManager.addSuccessMessage(qsTr("The account has been unlinked"), true)
		}

		function onLinkedAccountOperationFailed(message) {
			PopupManager.addErrorMessage(message !== "" ? message : qsTr("The operation could not be completed"), true)
		}
	}

	Connections {
		target: AuthorizationController

		function onExternalLoginProvidersLoaded() {
			linkedAccountsPage.updateRows()
			AuthorizationController.prepareExternalLogins(true)
		}

		function onExternalAccountLinked(providerId) {
			linkedAccountsPage.reload()
		}

		function onExternalLoginFailed(message) {
			PopupManager.addErrorMessage(message, true)
		}
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
		anchors.fill: parent
		anchors.margins: Style.marginXL
		contentWidth: width
		contentHeight: bodyColumn.height + Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: parent.width
			spacing: Style.marginXL

			Text {
				width: parent.width
				text: qsTr("Link an account of another service to sign in with it instead of your password.")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				wrapMode: Text.WordWrap
			}

			Column {
				width: parent.width
				spacing: Style.marginXS

				Text {
					text: qsTr("Accounts")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Rectangle {
					width: parent.width
					height: providersColumn.height + 2 * Style.marginL
					border.width: 1
					border.color: Style.borderColor
					radius: Style.radiusL
					color: Style.baseColor

					Column {
						id: providersColumn
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.margins: Style.marginL
						spacing: Style.marginL

						Repeater {
							model: linkedAccountsPage.rows

							delegate: Item {
								id: providerRow
								objectName: "LinkedAccountRow_" + String(modelData.id)

								width: providersColumn.width
								height: Math.max(providerTextColumn.height, providerButton.height)

								Image {
									id: providerIcon
									anchors.left: parent.left
									anchors.verticalCenter: parent.verticalCenter
									width: Style.iconSizeL
									height: width
									sourceSize.width: width
									sourceSize.height: height
									source: "../../../" + Style.getIconPath(modelData.type === "Google" ? "Icons/Google" : modelData.type === "Apple" ? "Icons/Apple" : modelData.type === "Facebook" ? "Icons/Facebook" : "Icons/Key", Icon.State.On, Icon.Mode.Normal)
								}

								Column {
									id: providerTextColumn
									anchors.left: providerIcon.right
									anchors.leftMargin: Style.marginM
									anchors.right: providerButton.left
									anchors.rightMargin: Style.marginM
									anchors.verticalCenter: parent.verticalCenter
									spacing: Style.marginXS

									Text {
										width: parent.width
										text: String(modelData.name)
										color: Style.textColor
										font.family: Style.fontFamilyBold
										font.pixelSize: Style.fontSizeM
										elide: Text.ElideRight
									}

									Text {
										width: parent.width
										text: modelData.isLinked ? (modelData.email !== "" ? qsTr("Linked: %1").arg(modelData.email) : qsTr("Linked")) : qsTr("Not linked")
										color: Style.inactiveTextColor
										font.family: Style.fontFamily
										font.pixelSize: Style.fontSizeS
										elide: Text.ElideRight
									}
								}

								Button {
									id: providerButton
									objectName: modelData.isLinked ? "UnlinkAccountButton" : "LinkAccountButton"

									anchors.right: parent.right
									anchors.verticalCenter: parent.verticalCenter

									width: Style.buttonWidthXL
									height: Style.controlHeightM

									enabled: !AuthorizationController.externalLoginInProgress
									text: modelData.isLinked ? qsTr("Unlink") : qsTr("Link")

									onClicked: {
										if (modelData.isLinked) {
											linkedAccountsPage.apiClient.unlinkAccount(String(modelData.id))
										}
										else {
											AuthorizationController.linkExternalAccount(String(modelData.id))
										}
									}
								}
							}
						}

						Text {
							width: parent.width
							visible: linkedAccountsPage.rows.length === 0
							text: qsTr("No sign-in services are configured on the server.")
							color: Style.inactiveTextColor
							font.family: Style.fontFamily
							font.pixelSize: Style.fontSizeM
							wrapMode: Text.WordWrap
						}
					}
				}
			}

			Text {
				objectName: "ExternalLinkWaitingText"
				width: parent.width
				visible: AuthorizationController.externalLoginInProgress
				text: qsTr("Complete the sign-in in the browser window that has opened.")
				color: Style.textColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				wrapMode: Text.WordWrap
			}

			Text {
				width: parent.width
				visible: !linkedAccountsPage.hasPassword
				text: qsTr("Your account has no password, so the last linked account cannot be unlinked.")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeS
				wrapMode: Text.WordWrap
			}
		}
	}
}
