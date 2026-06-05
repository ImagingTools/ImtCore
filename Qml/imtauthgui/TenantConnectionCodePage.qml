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
		width: Math.min(parent.width - Style.marginXL * 2, 600)
		spacing: Style.marginXL

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
			height: Style.controlHeightL * 2
			color: Style.backgroundColor2
			radius: Style.radiusL
			border.width: 1
			border.color: Style.borderColor

			Row {
				anchors.centerIn: parent
				spacing: Style.marginL

				Text {
					text: connectionCodePage.connectionCode ? connectionCodePage.connectionCode : qsTr("Loading...")
					font.pixelSize: Style.fontSizeXXL
					font.bold: true
					color: Style.titleColor
					anchors.verticalCenter: parent.verticalCenter
				}

				ToolButton {
					id: copyButton
					anchors.verticalCenter: parent.verticalCenter
					width: Style.buttonWidthL
					height: Style.buttonWidthL
					iconSource: copyButton.__copied
						? "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal)
						: "../../../" + Style.getIconPath("Icons/Copy", Icon.State.On, Icon.Mode.Normal)
					tooltipText: copyButton.__copied ? qsTr("Copied!") : qsTr("Copy code")
					enabled: connectionCodePage.connectionCode !== ""

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
		}

		Button {
			text: qsTr("Regenerate Code")
			onClicked: {
				if (connectionCodePage.apiClient && connectionCodePage.tenantData) {
					connectionCodePage.apiClient.regenerateConnectionCode(
						connectionCodePage.tenantData.m_id)
				}
			}
		}

		Rectangle {
			width: parent.width
			height: 1
			color: Style.borderColor
		}

		Row {
			spacing: Style.marginM

			SwitchCustom {
				id: allowSwitch
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

			Text {
				text: qsTr("Allow other organizations to connect by code")
				font.pixelSize: Style.fontSizeM
				color: Style.textColor
				anchors.verticalCenter: parent.verticalCenter
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
