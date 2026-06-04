// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantMyConnectionsPage
 *
 * Displays all confirmed connections for the tenant.
 * Allows removing a connection (which cascades to archive related relationships).
 */
ViewBase {
	id: connectionsPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: connectionsPage.model
	property var stateManager: null
	property var apiClient: null

	function updateGui() {
		if (connectionsPage.apiClient && connectionsPage.tenantData
				&& connectionsPage.tenantData.m_id) {
			connectionsPage.apiClient.fetchConnections(connectionsPage.tenantData.m_id)
		}
	}

	Component.onCompleted: {
		updateGui()
	}

	Connections {
		target: connectionsPage.apiClient

		function onConnectionsReceived() {
			connectionsList.model = connectionsPage.apiClient.connectionsModel
		}

		function onConnectionRemoved(connectionId) {
			PopupManager.addSuccessMessage(qsTr("Connection removed"), true)
			updateGui()
		}
	}

	Column {
		anchors.fill: parent
		anchors.margins: Style.marginL
		spacing: Style.marginL

		Text {
			text: qsTr("My Connections")
			font.pixelSize: Style.fontSizeH2
			font.bold: true
			color: Style.textColor
		}

		Text {
			text: qsTr("Confirmed connections with other organizations. Removing a connection will archive all associated relationships.")
			font.pixelSize: Style.fontSizeDefault
			color: Style.textSecondaryColor
			wrapMode: Text.WordWrap
			width: parent.width
		}

		ListView {
			id: connectionsList
			width: parent.width
			height: parent.height - y
			clip: true
			spacing: Style.marginS

			delegate: Rectangle {
				width: connectionsList.width
				height: connectionRow.height + Style.marginM * 2
				color: Style.panelColor
				radius: Style.radiusS

				Row {
					id: connectionRow
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: parent.left
					anchors.right: removeBtn.left
					anchors.margins: Style.marginM
					spacing: Style.marginM

					Column {
						spacing: Style.marginXS

						Text {
							text: modelData.partnerName || modelData.partnerId || ""
							font.pixelSize: Style.fontSizeDefault
							font.bold: true
							color: Style.textColor
						}

						Text {
							text: qsTr("Connected since: %1").arg(
								modelData.createdAt ? new Date(modelData.createdAt).toLocaleDateString() : "—")
							font.pixelSize: Style.fontSizeSmall
							color: Style.textSecondaryColor
						}
					}
				}

				Button {
					id: removeBtn
					text: qsTr("Remove")
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					onClicked: {
						if (connectionsPage.apiClient && connectionsPage.tenantData) {
							connectionsPage.apiClient.removeConnection(
								modelData.id, connectionsPage.tenantData.m_id)
						}
					}
				}
			}

			Text {
				visible: connectionsList.count === 0
				text: qsTr("No connections yet. Use the Connection Code to connect with other organizations.")
				font.pixelSize: Style.fontSizeDefault
				color: Style.textSecondaryColor
				anchors.centerIn: parent
				wrapMode: Text.WordWrap
			}
		}
	}
}
