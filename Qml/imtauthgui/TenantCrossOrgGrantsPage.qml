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
 * TenantCrossOrgGrantsPage
 *
 * Cross-Org Grants tab of the TenantEditor.
 *
 * Displays grants via TenantSimpleCollectionPage. Create opens a separate
 * CrossOrgGrantView editor (ViewBase) with GqlBasedCommandsController.
 * Revocation is performed per-item in the list delegate.
 */
TenantSimpleCollectionPage {
id: grantsPage

entityName: qsTr("Grant")
entityNamePlural: qsTr("Cross-Org Grants")
descriptionText: qsTr("Delegate scoped access from this tenant to another tenant.")

listModel: apiClient ? apiClient.crossOrgGrantsModel : null

customEditorComponent: createGrantComp

headerButtonsComponent: headerBtnsComp

function updateGui() {
if (apiClient && tenantData && tenantData.m_id) {
apiClient.fetchCrossOrgGrants(tenantData.m_id)
}
}

Component.onCompleted: {
if (apiClient && tenantData && tenantData.m_id) {
apiClient.fetchCrossOrgGrants(tenantData.m_id)
}
}

onVisibleChanged: {
if (visible && apiClient && tenantData && tenantData.m_id) {
apiClient.fetchCrossOrgGrants(tenantData.m_id)
}
}

Connections {
target: grantsPage.apiClient

function onCrossOrgGrantCreated(grantId) {
PopupManager.addSuccessMessage(qsTr("Cross-org grant created successfully"), true)
grantsPage.popEditor()
if (grantsPage.apiClient && grantsPage.tenantData && grantsPage.tenantData.m_id) {
grantsPage.apiClient.fetchCrossOrgGrants(grantsPage.tenantData.m_id)
}
}

function onCrossOrgGrantRevoked(grantId) {
PopupManager.addSuccessMessage(qsTr("Cross-org grant revoked"), true)
if (grantsPage.apiClient && grantsPage.tenantData && grantsPage.tenantData.m_id) {
grantsPage.apiClient.fetchCrossOrgGrants(grantsPage.tenantData.m_id)
}
}
}

// --- Custom header: only show "+ Create Grant" ---
Component {
id: headerBtnsComp

Text {
text: qsTr("+ Create Grant")
font.pixelSize: Style.fontSizeM
font.bold: true
color: (grantsPage.stateManager && (grantsPage.stateManager.isCreator || grantsPage.stateManager.isOwner))
   ? Style.linkColor : Style.inactiveTextColor

MouseArea {
anchors.fill: parent
hoverEnabled: true
cursorShape: Qt.PointingHandCursor
enabled: grantsPage.stateManager && (grantsPage.stateManager.isCreator || grantsPage.stateManager.isOwner)
onClicked: { grantsPage.openCreate() }
}
}
}

// --- Custom list delegate ---
delegateComponent: Component {
Rectangle {
id: grantDelegate
width: parent ? parent.width : 0
height: grantDelegateContent.height + 2 * Style.marginM
color: Style.alternateBaseColor
radius: Style.radiusS
border.color: Style.borderColor
border.width: 1

readonly property var __grant: modelData
readonly property bool __canRevoke: grantsPage.stateManager
&& (grantsPage.stateManager.isCreator || grantsPage.stateManager.isOwner)
&& (__grant.isActive === undefined || __grant.isActive)

Row {
id: grantDelegateContent
anchors.left: parent.left
anchors.right: parent.right
anchors.top: parent.top
anchors.margins: Style.marginM
spacing: Style.marginM

Column {
width: parent.width - (revokeBtn.visible ? revokeBtn.width + Style.marginM : 0)
spacing: Style.marginXS

BaseText {
width: parent.width
elide: Text.ElideRight
text: qsTr("Target: %1").arg(__grant.targetTenantId || "")
font.pixelSize: Style.fontSizeM
color: Style.textColor
}

BaseText {
width: parent.width
elide: Text.ElideRight
text: qsTr("Level: %1   Scope: %2")
.arg(__grant.accessLevel || qsTr("None"))
.arg((__grant.resourceScope && __grant.resourceScope !== "") ? __grant.resourceScope : qsTr("All"))
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: parent.width
visible: __grant.relationshipId && __grant.relationshipId !== ""
elide: Text.ElideRight
text: qsTr("Relationship: %1").arg(__grant.relationshipId || "")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: parent.width
visible: __grant.description && __grant.description !== ""
elide: Text.ElideRight
text: __grant.description || ""
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: parent.width
visible: __grant.expiresAt && __grant.expiresAt !== ""
elide: Text.ElideRight
text: qsTr("Expires: %1").arg(__grant.expiresAt || "")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}
}

Button {
id: revokeBtn
anchors.verticalCenter: undefined
visible: __canRevoke
text: qsTr("Revoke")
onClicked: {
if (grantsPage.apiClient) {
grantsPage.apiClient.revokeCrossOrgGrant(__grant.grantId || "")
}
}
}
}
}
}

// --- Create grant editor (ViewBase with GqlBasedCommandsController) ---
Component {
id: createGrantComp

CrossOrgGrantView {
apiClient: grantsPage.apiClient
tenantData: grantsPage.tenantData
commandsControllerComp: Component {
GqlBasedCommandsController {
typeId: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantObjectTypeId : ""
}
}

onCommandActivated: function(commandId) {
if (commandId === "save" || commandId === "create") {
submitGrant()
}
}
}
}
}
