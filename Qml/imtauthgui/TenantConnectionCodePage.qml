// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantConnectionCodePage
 *
 * Displays the tenant's public Connection Code and the setting to
 * allow/disallow other organizations to connect by code.
 */
ViewBase {
	id: connectionCodePage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: connectionCodePage.model
	property var stateManager: null
	property var apiClient: null
	readonly property bool __canViewConnectionCode: connectionCodePage.stateManager ? connectionCodePage.stateManager.canViewOrganizationConnectionCode : false
	readonly property bool __canManageConnections: connectionCodePage.stateManager ? connectionCodePage.stateManager.canManageOrganizationConnections : false

	property string connectionCode: ""
	property bool allowConnectionsByCode: true
	property bool loading: false
	property bool __ready: false

	function updateGui() {
		if (connectionCodePage.apiClient && connectionCodePage.tenantData
				&& connectionCodePage.tenantData.m_id) {
			connectionCodePage.loading = true
			connectionCodePage.apiClient.getConnectionCode(connectionCodePage.tenantData.m_id)
		}
	}

	Component.onCompleted: {
		connectionCodePage.updateGui()
	}

	Connections {
		target: connectionCodePage.apiClient

		function onConnectionCodeReceived(code, allowByCode) {
			connectionCodePage.connectionCode = code
			connectionCodePage.allowConnectionsByCode = allowByCode
			connectionCodePage.loading = false
			connectionCodePage.__ready = true
		}

		function onConnectionCodeRegenerated(newCode) {
			connectionCodePage.connectionCode = newCode
		}

		function onAllowConnectionsByCodeChanged(allow) {
			connectionCodePage.allowConnectionsByCode = allow
		}
	}

	TextEdit {
		id: clipboardHelper
		visible: false
		text: connectionCodePage.connectionCode
	}

	Column {
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, Style.contentWidthMax)
		spacing: Style.marginM

		GroupHeaderView {
			width: parent.width
			title: qsTr("Connection Code")
		}

		Text {
			text: qsTr("Share this code with other organizations so they can send you a connection request.")
			font.pixelSize: Style.fontSizeS
			color: Style.inactiveTextColor
			wrapMode: Text.WordWrap
			width: parent.width
		}

		Rectangle {
			width: parent.width
			height: Style.controlHeightL + 2 * Style.marginL
			color: Style.baseColor
			radius: Style.radiusM
			border.width: 1
			border.color: Style.borderColor

			Row {
				anchors.left: parent.left
				anchors.leftMargin: Style.marginL
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.marginL

				TextInput {
					anchors.verticalCenter: parent.verticalCenter
					width: contentWidth
					height: contentHeight
					text: connectionCodePage.connectionCode ? connectionCodePage.connectionCode : qsTr("Loading...")
					font.pixelSize: Style.fontSizeXL
					font.family: Style.fontFamilyBold
					font.bold: true
					color: Style.textColor
					readOnly: true
					selectByMouse: true
					selectionColor: Style.selectedColor
				}

				ToolButton {
					id: copyButton
					anchors.verticalCenter: parent.verticalCenter
					width: Style.buttonWidthL
					height: Style.buttonWidthL
					iconSource: copyButton.__copied
						? "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal)
						: "../../../" + Style.getIconPath("Icons/Copy", Icon.State.On, Icon.Mode.Normal)
					enabled: !copyButton.__copied && connectionCodePage.connectionCode !== "" && connectionCodePage.__canViewConnectionCode

					property bool __copied: false

					onClicked: {
						clipboardHelper.selectAll()
						clipboardHelper.copy()
						copyButton.__copied = true
						copiedPopup.open()
						copyResetTimer.restart()
					}

					Timer {
						id: copyResetTimer
						interval: 2000
						onTriggered: {
							copyButton.__copied = false
							copiedPopup.close()
						}
					}

					Popup {
						id: copiedPopup
						x: -copiedPopup.width / 2 + copyButton.width / 2
						y: copyButton.height + Style.marginS
						width: copiedPopupLabel.contentWidth + Style.marginL * 2
						height: copiedPopupLabel.contentHeight + Style.marginM * 2
						closePolicy: Enums.popupCloseOnEscape | Enums.popupCloseOnPressOutside

						background: Rectangle {
							color: Style.baseColor
							border.color: Style.borderColor
							border.width: 1
							radius: Style.radiusM
						}

						Text {
							id: copiedPopupLabel
							anchors.centerIn: parent
							text: qsTr("Copied!")
							font.pixelSize: Style.fontSizeS
							color: Style.textColor
						}
					}
				}
			}

			Button {
				anchors.right: parent.right
				anchors.rightMargin: Style.marginL
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Regenerate Code")
				enabled: connectionCodePage.__canManageConnections
				onClicked: {
					ModalDialogManager.showConfirmationDialog(
						qsTr("Regenerate Connection Code"),
						qsTr("Are you sure you want to regenerate the connection code? The old code will stop working."),
						function(result) {
							if (result === Enums.yes && connectionCodePage.apiClient && connectionCodePage.tenantData) {
								connectionCodePage.apiClient.regenerateConnectionCode(
									connectionCodePage.tenantData.m_id)
							}
						}
					)
				}
			}
		}

		Item {
			width: parent.width
			height: Style.marginM
		}

		GroupHeaderView {
			width: parent.width
			title: qsTr("Access")
		}

		Rectangle {
			width: parent.width
			height: allowColumn.height + 2 * Style.marginL
			color: Style.baseColor
			radius: Style.radiusM
			border.width: 1
			border.color: Style.borderColor

			Column {
				id: allowColumn
				anchors.left: parent.left
				anchors.leftMargin: Style.marginL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginL
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.marginXS

				Item {
					width: parent.width
					height: Style.controlHeightM

					Text {
						anchors.left: parent.left
						anchors.right: allowSwitch.left
						anchors.rightMargin: Style.marginL
						anchors.verticalCenter: parent.verticalCenter
						text: qsTr("Allow other organizations to connect by code")
						font.family: Style.fontFamilyBold
						font.pixelSize: Style.fontSizeM
						font.bold: true
						color: Style.textColor
						elide: Text.ElideRight
					}

					SwitchCustom {
						id: allowSwitch
						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter
						enabled: connectionCodePage.__canManageConnections
						checked: connectionCodePage.allowConnectionsByCode
						onCheckedChanged: {
							if (!connectionCodePage.__ready) {
								return
							}
							if (connectionCodePage.apiClient && connectionCodePage.tenantData) {
								connectionCodePage.apiClient.setAllowConnectionsByCode(
									connectionCodePage.tenantData.m_id, allowSwitch.checked)
							}
						}
					}
				}

				Text {
					visible: !connectionCodePage.allowConnectionsByCode
					text: qsTr("When disabled, other organizations cannot send you connection requests using your code.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
					wrapMode: Text.WordWrap
					width: parent.width
				}
			}
		}
	}
}
