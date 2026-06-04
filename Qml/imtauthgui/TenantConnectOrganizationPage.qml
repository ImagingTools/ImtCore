// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantConnectOrganizationPage
 *
 * Allows the user to enter another organization's Connection Code to
 * send a connection request.
 */
ViewBase {
	id: connectPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: connectPage.model
	property var stateManager: null
	property var apiClient: null

	property bool sending: false
	property string lastError: ""

	Connections {
		target: connectPage.apiClient

		function onConnectionRequestCreated(requestId) {
			connectPage.sending = false
			connectPage.lastError = ""
			codeInput.text = ""
			messageInput.text = ""
			PopupManager.addSuccessMessage(qsTr("Connection request sent successfully"), true)
		}

		function onConnectionRequestError(errorMessage) {
			connectPage.sending = false
			connectPage.lastError = errorMessage
		}
	}

	Column {
		anchors.top: parent.top
		anchors.topMargin: Style.marginL
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, 1000)
		spacing: Style.marginL

		Text {
			text: qsTr("Connect Organization")
			font.pixelSize: Style.fontSizeXL
			font.bold: true
			color: Style.textColor
		}

		Text {
			text: qsTr("Enter the Connection Code of the organization you want to connect with.")
			font.pixelSize: Style.fontSizeM
			color: Style.inactiveTextColor
			wrapMode: Text.WordWrap
			width: parent.width
		}

		Column {
			width: parent.width
			spacing: Style.marginS

			Text {
				text: qsTr("Connection Code")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			TextInput {
				id: codeInput
				width: parent.width
				font.pixelSize: Style.fontSizeM
				font.family: "monospace"
				color: Style.textColor
				clip: true
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}
		}

		Column {
			width: parent.width
			spacing: Style.marginS

			Text {
				text: qsTr("Message (optional)")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			TextInput {
				id: messageInput
				width: parent.width
				font.pixelSize: Style.fontSizeM
				color: Style.textColor
				clip: true
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}
		}

		Text {
			visible: connectPage.lastError !== ""
			text: connectPage.lastError
			font.pixelSize: Style.fontSizeS
			color: Style.errorColor
			wrapMode: Text.WordWrap
			width: parent.width
		}

		Button {
			text: connectPage.sending ? qsTr("Sending...") : qsTr("Send Connection Request")
			enabled: codeInput.text.trim().length > 0 && !connectPage.sending
			onClicked: {
				if (connectPage.apiClient && connectPage.tenantData) {
					connectPage.sending = true
					connectPage.lastError = ""
					connectPage.apiClient.createConnectionRequest(
						connectPage.tenantData.m_id,
						codeInput.text.trim(),
						messageInput.text.trim())
				}
			}
		}
	}
}
