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

	property string connectionCode: ""
	property bool allowConnectionsByCode: true
	property bool loading: false

	function updateGui() {
		if (connectionCodePage.apiClient && connectionCodePage.tenantData
				&& connectionCodePage.tenantData.m_id) {
			connectionCodePage.loading = true
			connectionCodePage.apiClient.getConnectionCode(connectionCodePage.tenantData.m_id)
		}
	}

	Component.onCompleted: {
		updateGui()
	}

	Connections {
		target: connectionCodePage.apiClient

		function onConnectionCodeReceived(code, allowByCode) {
			connectionCodePage.connectionCode = code
			connectionCodePage.allowConnectionsByCode = allowByCode
			connectionCodePage.loading = false
		}

		function onConnectionCodeRegenerated(newCode) {
			connectionCodePage.connectionCode = newCode
			PopupManager.addSuccessMessage(qsTr("Connection code regenerated"), true)
		}

		function onAllowConnectionsByCodeChanged(allow) {
			connectionCodePage.allowConnectionsByCode = allow
			PopupManager.addSuccessMessage(
				allow ? qsTr("Connections by code enabled") : qsTr("Connections by code disabled"), true)
		}
	}

	Column {
		anchors.top: parent.top
		anchors.topMargin: Style.marginL
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, 1000)
		spacing: Style.marginL

		Text {
			text: qsTr("Connection Code")
			font.pixelSize: Style.fontSizeXL
			font.bold: true
			color: Style.textColor
		}

		Text {
			text: qsTr("Share this code with other organizations so they can send you a connection request.")
			font.pixelSize: Style.fontSizeM
			color: Style.inactiveTextColor
			wrapMode: Text.WordWrap
			width: parent.width
		}

		Rectangle {
			width: parent.width
			height: codeRow.height + Style.marginL * 2
			color: Style.backgroundColor2
			radius: Style.radiusL

			Row {
				id: codeRow
				anchors.centerIn: parent
				spacing: Style.marginM

				Text {
					id: codeText
					text: connectionCodePage.connectionCode ? connectionCodePage.connectionCode : qsTr("Loading...")
					font.pixelSize: Style.fontSizeXXL
					color: Style.textColor
					anchors.verticalCenter: parent.verticalCenter
				}

				Button {
					text: qsTr("Copy")
					anchors.verticalCenter: parent.verticalCenter
					onClicked: {
						if (connectionCodePage.connectionCode) {
							// ClipboardHelper.setText(connectionCodePage.connectionCode)
							// PopupManager.addSuccessMessage(qsTr("Code copied to clipboard"), true)
						}
					}
				}

				Button {
					text: qsTr("Regenerate")
					anchors.verticalCenter: parent.verticalCenter
					onClicked: {
						if (connectionCodePage.apiClient && connectionCodePage.tenantData) {
							connectionCodePage.apiClient.regenerateConnectionCode(
								connectionCodePage.tenantData.m_id)
						}
					}
				}
			}
		}

		Item {
			width: parent.width
			height: Style.marginM
		}

		Row {
			spacing: Style.marginM

			SwitchCustom {
				id: allowSwitch
				checked: connectionCodePage.allowConnectionsByCode
				onCheckedChanged: {
					if (connectionCodePage.apiClient && connectionCodePage.tenantData) {
						connectionCodePage.apiClient.setAllowConnectionsByCode(
							connectionCodePage.tenantData.m_id, allowSwitch.checked)
					}
				}
			}

			Text {
				text: qsTr("Allow other organizations to connect by code")
				font.pixelSize: Style.fontSizeM
				color: Style.textColor
				anchors.verticalCenter: parent.verticalCenter
			}
		}

		Text {
			visible: !connectionCodePage.allowConnectionsByCode
			text: qsTr("Connection requests by code are currently disabled. Other organizations will not be able to connect to you using your code.")
			font.pixelSize: Style.fontSizeXS
			color: Style.inactiveTextColor
			wrapMode: Text.WordWrap
			width: parent.width
		}
	}
}
