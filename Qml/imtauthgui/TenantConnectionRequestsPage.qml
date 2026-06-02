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
 * TenantConnectionRequestsPage
 *
 * Connections tab of the TenantEditor.
 *
 * Displays connection requests via TenantSimpleCollectionPage. The create
 * editor supports sending requests by identifier, generating connect codes,
 * and redeeming received codes. Per-item actions (Accept / Reject / Revoke)
 * appear in the list delegate.
 */
TenantSimpleCollectionPage {
id: connectionsPage

entityName: qsTr("Connection")
entityNamePlural: qsTr("Connections")
descriptionText: qsTr("Discover and connect with other tenants via requests or one-time connect codes.")

listModel: apiClient ? apiClient.connectionRequestsModel : null

customEditorComponent: createConnectionComp

headerButtonsComponent: headerBtnsComp

function updateGui() {
if (apiClient && tenantData && tenantData.m_id)
apiClient.fetchConnectionRequests(tenantData.m_id)
}

Component.onCompleted: {
if (apiClient && tenantData && tenantData.m_id)
apiClient.fetchConnectionRequests(tenantData.m_id)
}

onVisibleChanged: {
if (visible && apiClient && tenantData && tenantData.m_id)
apiClient.fetchConnectionRequests(tenantData.m_id)
}

Connections {
target: connectionsPage.apiClient

function onConnectionRequestCreated(requestId) {
PopupManager.addSuccessMessage(qsTr("Connection request sent"), true)
connectionsPage.popEditor()
if (connectionsPage.apiClient && connectionsPage.tenantData && connectionsPage.tenantData.m_id)
connectionsPage.apiClient.fetchConnectionRequests(connectionsPage.tenantData.m_id)
}

function onConnectCodeCreated(requestId, connectCode) {
ModalDialogManager.showInfoDialog(qsTr("Connect code created: %1").arg(connectCode))
connectionsPage.popEditor()
if (connectionsPage.apiClient && connectionsPage.tenantData && connectionsPage.tenantData.m_id)
connectionsPage.apiClient.fetchConnectionRequests(connectionsPage.tenantData.m_id)
}

function onConnectionRequestAccepted(requestId) {
PopupManager.addSuccessMessage(qsTr("Connection request accepted"), true)
if (connectionsPage.apiClient && connectionsPage.tenantData && connectionsPage.tenantData.m_id)
connectionsPage.apiClient.fetchConnectionRequests(connectionsPage.tenantData.m_id)
}

function onConnectionRequestRejected(requestId) {
PopupManager.addSuccessMessage(qsTr("Connection request rejected"), true)
if (connectionsPage.apiClient && connectionsPage.tenantData && connectionsPage.tenantData.m_id)
connectionsPage.apiClient.fetchConnectionRequests(connectionsPage.tenantData.m_id)
}

function onConnectionRequestRevoked(requestId) {
PopupManager.addSuccessMessage(qsTr("Connection request revoked"), true)
if (connectionsPage.apiClient && connectionsPage.tenantData && connectionsPage.tenantData.m_id)
connectionsPage.apiClient.fetchConnectionRequests(connectionsPage.tenantData.m_id)
}
}

// --- Custom header ---
Component {
id: headerBtnsComp

Text {
text: qsTr("+ New Connection")
font.pixelSize: Style.fontSizeM
font.bold: true
color: (connectionsPage.stateManager && (connectionsPage.stateManager.isCreator || connectionsPage.stateManager.isOwner))
   ? Style.linkColor : Style.inactiveTextColor

MouseArea {
anchors.fill: parent
hoverEnabled: true
cursorShape: Qt.PointingHandCursor
enabled: connectionsPage.stateManager && (connectionsPage.stateManager.isCreator || connectionsPage.stateManager.isOwner)
onClicked: { connectionsPage.openCreate() }
}
}
}

// --- Custom list delegate ---
delegateComponent: Component {
Rectangle {
id: connDelegate
width: parent ? parent.width : 0
height: connDelegateContent.height + 2 * Style.marginM
color: Style.alternateBaseColor
radius: Style.radiusS
border.color: Style.borderColor
border.width: 1

readonly property var __req: modelData
readonly property bool __isOutgoing: connectionsPage.tenantData
&& __req.sourceTenantId === connectionsPage.tenantData.m_id
readonly property bool __isPending: (__req.status || "") === "Pending"
readonly property bool __canManage: connectionsPage.stateManager
&& (connectionsPage.stateManager.isCreator || connectionsPage.stateManager.isOwner)

Column {
id: connDelegateContent
anchors.left: parent.left
anchors.right: parent.right
anchors.top: parent.top
anchors.margins: Style.marginM
spacing: Style.marginXS

BaseText {
width: parent.width
elide: Text.ElideRight
text: __isOutgoing
? qsTr("To: %1").arg((__req.targetIdentifier && __req.targetIdentifier !== "")
? __req.targetIdentifier
: (__req.targetTenantId || qsTr("(connect code)")))
: qsTr("From: %1").arg(__req.sourceTenantId || "")
font.pixelSize: Style.fontSizeM
color: Style.textColor
}

BaseText {
width: parent.width
elide: Text.ElideRight
text: qsTr("Status: %1   Roles: %2 / %3")
.arg(__req.status || qsTr("Pending"))
.arg(__req.proposedSourceRole || qsTr("Partner"))
.arg(__req.proposedTargetRole || qsTr("Partner"))
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: parent.width
visible: __req.connectCode && __req.connectCode !== ""
elide: Text.ElideRight
text: qsTr("Connect code: %1").arg(__req.connectCode || "")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: parent.width
visible: __req.message && __req.message !== ""
wrapMode: Text.WordWrap
text: __req.message || ""
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

Row {
spacing: Style.marginM
visible: __canManage && __isPending

Button {
visible: !__isOutgoing
text: qsTr("Accept")
onClicked: {
if (connectionsPage.apiClient)
connectionsPage.apiClient.acceptConnectionRequest(
__req.requestId || "",
connectionsPage.tenantData ? connectionsPage.tenantData.m_id : "")
}
}

Button {
visible: !__isOutgoing
text: qsTr("Reject")
onClicked: {
if (connectionsPage.apiClient)
connectionsPage.apiClient.rejectConnectionRequest(__req.requestId || "")
}
}

Button {
visible: __isOutgoing
text: qsTr("Revoke")
onClicked: {
if (connectionsPage.apiClient)
connectionsPage.apiClient.revokeConnectionRequest(__req.requestId || "")
}
}
}
}
}
}

// --- Create connection / redeem code form ---
Component {
id: createConnectionComp

Item {
id: createConnForm

CustomScrollbar {
z: parent.z + 1
anchors.right: parent.right
anchors.top: createConnFlickable.top
anchors.bottom: createConnFlickable.bottom
secondSize: Style.marginM
targetItem: createConnFlickable
}

Flickable {
id: createConnFlickable
anchors.fill: parent
anchors.margins: Style.marginXL
contentHeight: createConnColumn.height + 2 * Style.marginXL
clip: true

Column {
id: createConnColumn
width: Style.sizeHintXXL
spacing: Style.marginL

// --- Send request / create code ---
GroupElementView {
width: parent.width

GroupHeaderView {
width: parent.width
title: qsTr("Send Request or Create Connect Code")
}

TextInputElementView {
id: targetIdentifierInput
name: qsTr("Target Identifier")
placeHolderText: qsTr("E-mail or slug of the tenant to invite")
}

ComboBoxElementView {
id: connSourceRoleCB
name: qsTr("Proposed Source Role")
model: connRoleModel
currentIndex: 2
}

ComboBoxElementView {
id: connTargetRoleCB
name: qsTr("Proposed Target Role")
model: connRoleModel
currentIndex: 2
}

TextInputElementView {
id: connMessageInput
name: qsTr("Message")
placeHolderText: qsTr("Optional message")
}

DateTimePickerElementView {
id: connExpiresAtPicker
name: qsTr("Expires At")
}

Row {
spacing: Style.marginM

Button {
text: qsTr("Send Request")
onClicked: {
var target = targetIdentifierInput.text.trim()
if (target === "") {
ModalDialogManager.showInfoDialog(qsTr("Target identifier is required."))
return
}
var roleTokens = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
var srcIdx = connSourceRoleCB.currentIndex >= 0 ? connSourceRoleCB.currentIndex : 2
var tgtIdx = connTargetRoleCB.currentIndex >= 0 ? connTargetRoleCB.currentIndex : 2
connectionsPage.apiClient.createConnectionRequest(
connectionsPage.tenantData ? connectionsPage.tenantData.m_id : "",
target,
roleTokens[srcIdx],
roleTokens[tgtIdx],
connMessageInput.text.trim(),
connExpiresAtPicker.getDateAsString())
}
}

Button {
text: qsTr("Create Connect Code")
onClicked: {
var roleTokens = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
var srcIdx = connSourceRoleCB.currentIndex >= 0 ? connSourceRoleCB.currentIndex : 2
var tgtIdx = connTargetRoleCB.currentIndex >= 0 ? connTargetRoleCB.currentIndex : 2
connectionsPage.apiClient.createConnectCode(
connectionsPage.tenantData ? connectionsPage.tenantData.m_id : "",
roleTokens[srcIdx],
roleTokens[tgtIdx],
connMessageInput.text.trim(),
connExpiresAtPicker.getDateAsString())
}
}
}
}

// --- Redeem code ---
GroupElementView {
width: parent.width

GroupHeaderView {
width: parent.width
title: qsTr("Redeem a Connect Code")
}

TextInputElementView {
id: redeemCodeInput
name: qsTr("Connect Code")
placeHolderText: qsTr("Paste a connect code to connect")
}

Row {
spacing: Style.marginM

Button {
text: qsTr("Redeem Code")
onClicked: {
var code = redeemCodeInput.text.trim()
if (code === "") {
ModalDialogManager.showInfoDialog(qsTr("Connect code is required."))
return
}
connectionsPage.apiClient.acceptConnectCode(
code,
connectionsPage.tenantData ? connectionsPage.tenantData.m_id : "")
}
}

Button {
text: qsTr("Cancel")
onClicked: { connectionsPage.popEditor() }
}
}
}
}
}

TreeItemModel {
id: connRoleModel
Component.onCompleted: {
var roles = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
for (var i = 0; i < roles.length; i++) {
var idx = insertNewItem()
setData("id", roles[i], idx)
setData("name", roles[i], idx)
}
}
}
}
}
}
