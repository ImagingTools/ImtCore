// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantMessagesPage
 *
 * Cross-tenant messages tab of the TenantEditor (RFC phase 2, §10/§13).
 *
 * Read-only inbox/outbox view of the cross-tenant messages exchanged through
 * the broker for the current tenant. Each message records the domain type, the
 * lifecycle status and the source/target tenants. The data is fetched through
 * the abstract TenantManagementApiClient contract.
 */
ViewBase {
	id: messagesPage

	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: messagesPage.model
	property var stateManager: null
	property var apiClient: null

	function updateGui() {
		messagesPage.__refresh()
	}

	function __refresh() {
		if (messagesPage.apiClient && messagesPage.tenantData && messagesPage.tenantData.m_id)
			messagesPage.apiClient.fetchCrossTenantMessages(messagesPage.tenantData.m_id, "")
	}

	onVisibleChanged: {
		if (messagesPage.visible)
			messagesPage.__refresh()
	}

	Component.onCompleted: messagesPage.__refresh()

	CustomScrollbar {
		id: messagesScrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: messagesFlickable.top
		anchors.bottom: messagesFlickable.bottom
		secondSize: Style.marginM
		targetItem: messagesFlickable
	}

	Flickable {
		id: messagesFlickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		contentHeight: messagesColumn.height + 2 * Style.marginXL
		clip: true

		Column {
			id: messagesColumn
			width: Style.sizeHintXXL
			spacing: Style.marginXL

			Column {
				width: parent.width
				spacing: Style.marginXS

				BaseText {
					text: qsTr("Cross-Tenant Messages")
					font.pixelSize: Style.fontSizeXL
					font.bold: true
					color: Style.textColor
				}

				BaseText {
					width: parent.width
					wrapMode: Text.WordWrap
					text: qsTr("Messages exchanged with partner tenants through the cross-tenant message broker. The broker only ever propagates a projected payload — never direct access to another tenant's data.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			Button {
				text: qsTr("Refresh")
				onClicked: messagesPage.__refresh()
			}

			BaseText {
				width: parent.width
				visible: !messagesList.count
				text: qsTr("No cross-tenant messages for this tenant.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Column {
				id: messagesList
				width: parent.width
				spacing: Style.marginM

				property int count: messagesPage.apiClient && messagesPage.apiClient.crossTenantMessagesModel
					? messagesPage.apiClient.crossTenantMessagesModel.count
					: 0

				Repeater {
					model: messagesPage.apiClient ? messagesPage.apiClient.crossTenantMessagesModel : null

					delegate: Rectangle {
						width: messagesList.width
						height: messageColumn.height + 2 * Style.marginM
						color: Style.alternateBaseColor
						radius: Style.radiusS
						border.color: Style.borderColor
						border.width: 1

						Column {
							id: messageColumn
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							anchors.leftMargin: Style.marginM
							anchors.rightMargin: Style.marginM
							spacing: Style.marginXS

							BaseText {
								width: parent.width
								elide: Text.ElideRight
								text: qsTr("Type: %1   Status: %2")
									.arg(model.messageType || qsTr("Custom"))
									.arg(model.status || qsTr("Created"))
								font.pixelSize: Style.fontSizeM
								color: Style.textColor
							}

							BaseText {
								width: parent.width
								elide: Text.ElideRight
								text: qsTr("From: %1   To: %2")
									.arg(model.sourceTenantId || "")
									.arg(model.targetTenantId || "")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							BaseText {
								width: parent.width
								visible: model.errorMessage && model.errorMessage !== ""
								wrapMode: Text.WordWrap
								text: qsTr("Error: %1").arg(model.errorMessage || "")
								font.pixelSize: Style.fontSizeS
								color: Style.errorColor
							}

							BaseText {
								width: parent.width
								visible: model.createdAt && model.createdAt !== ""
								elide: Text.ElideRight
								text: qsTr("Created: %1").arg(model.createdAt || "")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
						}
					}
				}
			}
		}
	}
}
