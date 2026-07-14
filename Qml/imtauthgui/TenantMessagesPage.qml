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
if (messagesPage.apiClient && messagesPage.tenantData && messagesPage.tenantData.m_id) {
messagesPage.apiClient.fetchCrossTenantMessages(messagesPage.tenantData.m_id)
}
}

Component.onCompleted: {
if (messagesPage.apiClient && messagesPage.tenantData && messagesPage.tenantData.m_id) {
messagesPage.apiClient.fetchCrossTenantMessages(messagesPage.tenantData.m_id)
}
}

onVisibleChanged: {
if (messagesPage.visible && messagesPage.apiClient && messagesPage.tenantData && messagesPage.tenantData.m_id) {
messagesPage.apiClient.fetchCrossTenantMessages(messagesPage.tenantData.m_id)
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
width: msgDelegate.parent ? msgDelegate.parent.width : 0
height: msgDelegateContent.height + 2 * Style.marginM
color: Style.alternateBaseColor
radius: Style.radiusS
border.color: Style.borderColor
border.width: 1

readonly property var __msg: modelData
readonly property bool __isOutgoing: messagesPage.tenantData
&& msgDelegate.__msg.sourceTenantId === messagesPage.tenantData.m_id

Column {
id: msgDelegateContent
anchors.left: msgDelegate.left
anchors.right: msgDelegate.right
anchors.top: msgDelegate.top
anchors.margins: Style.marginM
spacing: Style.marginXS

Row {
spacing: Style.marginS

BaseText {
anchors.verticalCenter: parent.verticalCenter
text: msgDelegate.__isOutgoing ? qsTr("▶ Outgoing") : qsTr("◀ Incoming")
font.pixelSize: Style.fontSizeS
font.bold: true
color: msgDelegate.__isOutgoing ? Style.linkColor : Style.textColor
}

BaseText {
anchors.verticalCenter: parent.verticalCenter
visible: (msgDelegate.__msg.messageType && msgDelegate.__msg.messageType !== "")
text: qsTr("· %1").arg(msgDelegate.__msg.messageType || "")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
anchors.verticalCenter: parent.verticalCenter
visible: (msgDelegate.__msg.status && msgDelegate.__msg.status !== "")
text: qsTr("· %1").arg(msgDelegate.__msg.status || "")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}
}

BaseText {
width: msgDelegateContent.width
elide: Text.ElideRight
text: msgDelegate.__isOutgoing
? qsTr("To: %1").arg(msgDelegate.__msg.targetTenantId || "")
: qsTr("From: %1").arg(msgDelegate.__msg.sourceTenantId || "")
font.pixelSize: Style.fontSizeM
color: Style.textColor
}

BaseText {
width: msgDelegateContent.width
visible: msgDelegate.__msg.payload && msgDelegate.__msg.payload !== ""
wrapMode: Text.WordWrap
text: msgDelegate.__msg.payload || ""
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: msgDelegateContent.width
visible: (msgDelegate.__msg.createdAt && msgDelegate.__msg.createdAt !== "")
|| (msgDelegate.__msg.processedAt && msgDelegate.__msg.processedAt !== "")
elide: Text.ElideRight
text: qsTr("Sent: %1  Processed: %2")
.arg((msgDelegate.__msg.createdAt && msgDelegate.__msg.createdAt !== "") ? msgDelegate.__msg.createdAt : qsTr("—"))
.arg((msgDelegate.__msg.processedAt && msgDelegate.__msg.processedAt !== "") ? msgDelegate.__msg.processedAt : qsTr("—"))
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}
}
}
}
}
