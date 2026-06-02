// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtauthgui 1.0

/**
 * TenantMessagesPage
 *
 * Cross-Tenant Messages tab of the TenantEditor.
 *
 * Read-only list of protocol messages. Clicking a message opens a
 * MessageView (ViewBase) with GqlBasedCommandsController for detail view.
 */
TenantSimpleCollectionPage {
id: messagesPage

entityName: qsTr("Message")
entityNamePlural: qsTr("Cross-Tenant Messages")
descriptionText: qsTr("Incoming and outgoing cross-tenant protocol messages.")

listModel: apiClient ? apiClient.crossTenantMessagesModel : null

headerButtonsComponent: emptyHeaderComp

delegateComponent: messageDelegateComp

function updateGui() {
if (apiClient && tenantData && tenantData.m_id) {
apiClient.fetchCrossTenantMessages(tenantData.m_id)
}
}

Component.onCompleted: {
if (apiClient && tenantData && tenantData.m_id) {
apiClient.fetchCrossTenantMessages(tenantData.m_id)
}
}

onVisibleChanged: {
if (visible && apiClient && tenantData && tenantData.m_id) {
apiClient.fetchCrossTenantMessages(tenantData.m_id)
}
}

Connections {
target: messagesPage.apiClient

function onSubscriptionCrossTenantMessageReceived(notification) {
if (messagesPage.visible && messagesPage.apiClient && messagesPage.tenantData && messagesPage.tenantData.m_id) {
messagesPage.apiClient.fetchCrossTenantMessages(messagesPage.tenantData.m_id)
}
}

function onSubscriptionCrossTenantMessageStatusChanged(notification) {
if (messagesPage.visible && messagesPage.apiClient && messagesPage.tenantData && messagesPage.tenantData.m_id) {
messagesPage.apiClient.fetchCrossTenantMessages(messagesPage.tenantData.m_id)
}
}
}

// --- No header buttons (read-only page) ---
Component {
id: emptyHeaderComp
Item {}
}

// --- Message delegate ---
Component {
id: messageDelegateComp

Rectangle {
id: msgDelegate
width: parent ? parent.width : 0
height: msgDelegateContent.height + 2 * Style.marginM
color: Style.alternateBaseColor
radius: Style.radiusS
border.color: Style.borderColor
border.width: 1

readonly property var __msg: modelData
readonly property bool __isOutgoing: messagesPage.tenantData
&& __msg.sourceTenantId === messagesPage.tenantData.m_id

Column {
id: msgDelegateContent
anchors.left: parent.left
anchors.right: parent.right
anchors.top: parent.top
anchors.margins: Style.marginM
spacing: Style.marginXS

Row {
spacing: Style.marginS

BaseText {
anchors.verticalCenter: parent.verticalCenter
text: __isOutgoing ? qsTr("▶ Outgoing") : qsTr("◀ Incoming")
font.pixelSize: Style.fontSizeS
font.bold: true
color: __isOutgoing ? Style.linkColor : Style.textColor
}

BaseText {
anchors.verticalCenter: parent.verticalCenter
visible: (__msg.messageType && __msg.messageType !== "")
text: qsTr("· %1").arg(__msg.messageType || "")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
anchors.verticalCenter: parent.verticalCenter
visible: (__msg.status && __msg.status !== "")
text: qsTr("· %1").arg(__msg.status || "")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}
}

BaseText {
width: parent.width
elide: Text.ElideRight
text: __isOutgoing
? qsTr("To: %1").arg(__msg.targetTenantId || "")
: qsTr("From: %1").arg(__msg.sourceTenantId || "")
font.pixelSize: Style.fontSizeM
color: Style.textColor
}

BaseText {
width: parent.width
visible: __msg.payload && __msg.payload !== ""
wrapMode: Text.WordWrap
text: __msg.payload || ""
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: parent.width
visible: (__msg.createdAt && __msg.createdAt !== "")
     || (__msg.processedAt && __msg.processedAt !== "")
elide: Text.ElideRight
text: qsTr("Sent: %1  Processed: %2")
.arg((__msg.createdAt && __msg.createdAt !== "") ? __msg.createdAt : qsTr("—"))
.arg((__msg.processedAt && __msg.processedAt !== "") ? __msg.processedAt : qsTr("—"))
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}
}
}
}
}
