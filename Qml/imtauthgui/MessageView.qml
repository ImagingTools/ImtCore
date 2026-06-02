// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0

/**
 * MessageView
 *
 * ViewBase-inherited read-only viewer for cross-tenant messages.
 * Follows the RoleView / UserGroupView pattern with GqlBasedCommandsController.
 * Since messages are read-only, no submit action is needed.
 */
ViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property var apiClient: null
	property var tenantData: null
	property var messageData: null

	function updateGui() {
		if (!container.messageData) {
			return
		}
		directionText.text = __isOutgoing ? qsTr("▶ Outgoing") : qsTr("◀ Incoming")
		peerText.text = __isOutgoing
			? qsTr("To: %1").arg(container.messageData.targetTenantId || "")
			: qsTr("From: %1").arg(container.messageData.sourceTenantId || "")
		typeText.text = container.messageData.messageType || ""
		statusText.text = container.messageData.status || ""
		payloadText.text = container.messageData.payload || ""
		sentAtText.text = container.messageData.createdAt || qsTr("—")
		processedAtText.text = container.messageData.processedAt || qsTr("—")
	}

	function updateModel() {
	}

	readonly property bool __isOutgoing: container.tenantData
		&& container.messageData
		&& container.messageData.sourceTenantId === container.tenantData.m_id

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
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, Style.sizeHintXXL)
		contentWidth: bodyColumn.width
		contentHeight: bodyColumn.height + 2 * Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: Style.sizeHintXXL
			spacing: Style.marginXL

			GroupHeaderView {
				width: parent.width
				title: qsTr("Cross-Tenant Message")
			}

			GroupElementView {
				id: generalGroup
				width: parent.width

				ElementView {
					name: qsTr("Direction")
					controlComp: Component {
						BaseText {
							id: directionText
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
						}
					}
				}

				ElementView {
					name: qsTr("Peer")
					controlComp: Component {
						BaseText {
							id: peerText
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
						}
					}
				}

				ElementView {
					name: qsTr("Message Type")
					controlComp: Component {
						BaseText {
							id: typeText
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
						}
					}
				}

				ElementView {
					name: qsTr("Status")
					controlComp: Component {
						BaseText {
							id: statusText
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
						}
					}
				}

				ElementView {
					name: qsTr("Payload")
					controlComp: Component {
						BaseText {
							id: payloadText
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
							wrapMode: Text.WordWrap
						}
					}
				}

				ElementView {
					name: qsTr("Sent At")
					controlComp: Component {
						BaseText {
							id: sentAtText
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
						}
					}
				}

				ElementView {
					name: qsTr("Processed At")
					controlComp: Component {
						BaseText {
							id: processedAtText
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
						}
					}
				}
			}
		}
	}
}
