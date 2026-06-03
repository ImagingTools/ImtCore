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
 * Standalone GUI (not TenantSimpleCollectionPage) with:
 *   - Form to send connection request by email/identifier
 *   - Form to generate a one-time connect code
 *   - Form to redeem a received connect code
 *   - Live list of incoming/outgoing requests with Accept / Reject / Revoke
 *   - Real-time updates via cross-tenant message subscription
 */
ViewBase {
id: connectionsPage

anchors.fill: parent
commandsPanelVisible: false
contentColor: Style.baseColor

readonly property var tenantData: connectionsPage.model
property var stateManager: null
property var apiClient: null

readonly property bool __canManage: connectionsPage.stateManager
&& (connectionsPage.stateManager.isCreator || connectionsPage.stateManager.isOwner)

function updateGui() {
if (connectionsPage.apiClient && connectionsPage.tenantData && connectionsPage.tenantData.m_id) {
connectionsPage.apiClient.fetchConnectionRequests(connectionsPage.tenantData.m_id)
}
}

function updateModel() {
}

Component.onCompleted: {
if (connectionsPage.apiClient && connectionsPage.tenantData && connectionsPage.tenantData.m_id) {
connectionsPage.apiClient.fetchConnectionRequests(connectionsPage.tenantData.m_id)
}
}

onVisibleChanged: {
if (connectionsPage.visible && connectionsPage.apiClient && connectionsPage.tenantData && connectionsPage.tenantData.m_id) {
connectionsPage.apiClient.fetchConnectionRequests(connectionsPage.tenantData.m_id)
}
}

// --- Real-time subscription-based updates ---
Connections {
target: connectionsPage.apiClient

function onConnectionRequestCreated(requestId) {
PopupManager.addSuccessMessage(qsTr("Connection request sent"), true)
connectionsPage.__refreshList()
}

function onConnectCodeCreated(requestId, connectCode) {
generatedCodeText.text = connectCode
generatedCodeText.visible = true
PopupManager.addSuccessMessage(qsTr("Connect code generated: %1").arg(connectCode), true)
connectionsPage.__refreshList()
}

function onConnectionRequestAccepted(requestId) {
PopupManager.addSuccessMessage(qsTr("Connection request accepted"), true)
connectionsPage.__refreshList()
}

function onConnectionRequestRejected(requestId) {
PopupManager.addSuccessMessage(qsTr("Connection request rejected"), true)
connectionsPage.__refreshList()
}

function onConnectionRequestRevoked(requestId) {
PopupManager.addSuccessMessage(qsTr("Connection request revoked"), true)
connectionsPage.__refreshList()
}

// --- Subscription-driven real-time updates ---
function onSubscriptionCrossTenantMessageReceived(notification) {
if (connectionsPage.visible) {
connectionsPage.__refreshList()
}
}

function onSubscriptionCrossTenantMessageStatusChanged(notification) {
if (connectionsPage.visible) {
connectionsPage.__refreshList()
}
}
}

function __refreshList() {
if (connectionsPage.apiClient && connectionsPage.tenantData && connectionsPage.tenantData.m_id) {
connectionsPage.apiClient.fetchConnectionRequests(connectionsPage.tenantData.m_id)
}
}

CustomScrollbar {
id: scrollbar
z: connectionsPage.z + 1
anchors.right: parent.right
anchors.top: mainFlickable.top
anchors.bottom: mainFlickable.bottom
secondSize: Style.marginM
targetItem: mainFlickable
}

Flickable {
id: mainFlickable
anchors.top: parent.top
anchors.topMargin: Style.marginXL
anchors.bottom: parent.bottom
anchors.bottomMargin: Style.marginXL
anchors.horizontalCenter: parent.horizontalCenter
width: Math.min(parent.width - Style.marginXL * 2, Style.sizeHintXXL)
contentWidth: mainColumn.width
contentHeight: mainColumn.height + 2 * Style.marginXL
boundsBehavior: Flickable.StopAtBounds
clip: true

Column {
id: mainColumn
width: mainFlickable.width
spacing: Style.marginXL

// =============================================================
// SECTION: Send Connection Request by Email / Identifier
// =============================================================
GroupHeaderView {
width: mainColumn.width
title: qsTr("Send Connection Request")
}

GroupElementView {
id: sendRequestGroup
width: mainColumn.width

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
placeHolderText: qsTr("Optional message to include")
}

DateTimePickerElementView {
id: connExpiresAtPicker
name: qsTr("Expires At")
}
}

Row {
spacing: Style.marginM

Button {
enabled: connectionsPage.__canManage
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
enabled: connectionsPage.__canManage
text: qsTr("Generate Connect Code")
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

// --- Generated code display ---
BaseText {
id: generatedCodeText
visible: false
width: mainColumn.width
font.pixelSize: Style.fontSizeL
font.bold: true
color: Style.linkColor
wrapMode: Text.WrapAnywhere
}

// =============================================================
// SECTION: Redeem a Connect Code
// =============================================================
GroupHeaderView {
width: mainColumn.width
title: qsTr("Redeem a Connect Code")
}

GroupElementView {
id: redeemGroup
width: mainColumn.width

TextInputElementView {
id: redeemCodeInput
name: qsTr("Connect Code")
placeHolderText: qsTr("Paste a connect code to establish connection")
}
}

Button {
enabled: connectionsPage.__canManage
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

// =============================================================
// SECTION: Connection Requests List (real-time)
// =============================================================
GroupHeaderView {
width: mainColumn.width
title: qsTr("Connection Requests")
}

BaseText {
width: mainColumn.width
visible: !requestsRepeater.model || requestsRepeater.model.count === 0
text: qsTr("No connection requests yet.")
font.pixelSize: Style.fontSizeM
color: Style.inactiveTextColor
}

Column {
width: mainColumn.width
spacing: Style.marginM

Repeater {
id: requestsRepeater
model: connectionsPage.apiClient ? connectionsPage.apiClient.connectionRequestsModel : null

delegate: Rectangle {
id: requestDelegate
width: mainColumn.width
height: reqContent.height + 2 * Style.marginM
color: Style.alternateBaseColor
radius: Style.radiusS
border.color: Style.borderColor
border.width: 1

readonly property var __req: model
readonly property bool __isOutgoing: connectionsPage.tenantData
&& requestDelegate.__req.sourceTenantId === connectionsPage.tenantData.m_id
readonly property bool __isPending: (requestDelegate.__req.status || "") === "Pending"

Column {
id: reqContent
anchors.left: requestDelegate.left
anchors.right: requestDelegate.right
anchors.top: requestDelegate.top
anchors.margins: Style.marginM
spacing: Style.marginXS

BaseText {
width: reqContent.width
elide: Text.ElideRight
text: requestDelegate.__isOutgoing
? qsTr("▶ To: %1").arg((requestDelegate.__req.targetIdentifier && requestDelegate.__req.targetIdentifier !== "")
? requestDelegate.__req.targetIdentifier
: (requestDelegate.__req.targetTenantId || qsTr("(connect code)")))
: qsTr("◀ From: %1").arg(requestDelegate.__req.sourceTenantId || "")
font.pixelSize: Style.fontSizeM
font.bold: true
color: Style.textColor
}

BaseText {
width: reqContent.width
elide: Text.ElideRight
text: qsTr("Status: %1   Roles: %2 / %3")
.arg(requestDelegate.__req.status || qsTr("Pending"))
.arg(requestDelegate.__req.proposedSourceRole || qsTr("Partner"))
.arg(requestDelegate.__req.proposedTargetRole || qsTr("Partner"))
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: reqContent.width
visible: requestDelegate.__req.connectCode && requestDelegate.__req.connectCode !== ""
elide: Text.ElideRight
text: qsTr("Connect code: %1").arg(requestDelegate.__req.connectCode || "")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: reqContent.width
visible: requestDelegate.__req.message && requestDelegate.__req.message !== ""
wrapMode: Text.WordWrap
text: requestDelegate.__req.message || ""
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: reqContent.width
visible: requestDelegate.__req.createdAt && requestDelegate.__req.createdAt !== ""
elide: Text.ElideRight
text: qsTr("Created: %1").arg(requestDelegate.__req.createdAt || "")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

// --- Action buttons ---
Row {
spacing: Style.marginM
visible: connectionsPage.__canManage && requestDelegate.__isPending

Button {
visible: !requestDelegate.__isOutgoing
text: qsTr("Accept")
onClicked: {
if (connectionsPage.apiClient) {
connectionsPage.apiClient.acceptConnectionRequest(
requestDelegate.__req.requestId || "",
connectionsPage.tenantData ? connectionsPage.tenantData.m_id : "")
}
}
}

Button {
visible: !requestDelegate.__isOutgoing
text: qsTr("Reject")
onClicked: {
if (connectionsPage.apiClient) {
connectionsPage.apiClient.rejectConnectionRequest(requestDelegate.__req.requestId || "")
}
}
}

Button {
visible: requestDelegate.__isOutgoing
text: qsTr("Revoke")
onClicked: {
if (connectionsPage.apiClient) {
connectionsPage.apiClient.revokeConnectionRequest(requestDelegate.__req.requestId || "")
}
}
}
}
}
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
var idx = connRoleModel.insertNewItem()
connRoleModel.setData("id", roles[i], idx)
connRoleModel.setData("name", roles[i], idx)
}
}
}
}
