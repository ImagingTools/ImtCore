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
 * TenantRelationshipsPage
 *
 * Relationships tab of the TenantEditor.
 *
 * Displays tenant relationships via TenantSimpleCollectionPage. Create opens
 * a separate RelationshipView editor (ViewBase) with GqlBasedCommandsController.
 * Removal is done per-item in the list delegate.
 */
TenantSimpleCollectionPage {
id: relationshipsPage

entityName: qsTr("Relationship")
entityNamePlural: qsTr("Tenant Relationships")
descriptionText: qsTr("Define asymmetric relationships between this tenant and others.")

listModel: apiClient ? apiClient.tenantRelationshipsModel : null

customEditorComponent: createRelationshipComp

headerButtonsComponent: headerBtnsComp

function updateGui() {
if (relationshipsPage.apiClient && relationshipsPage.tenantData && relationshipsPage.tenantData.m_id) {
relationshipsPage.apiClient.fetchTenantRelationships(relationshipsPage.tenantData.m_id)
}
}

Component.onCompleted: {
if (relationshipsPage.apiClient && relationshipsPage.tenantData && relationshipsPage.tenantData.m_id) {
relationshipsPage.apiClient.fetchTenantRelationships(relationshipsPage.tenantData.m_id)
}
}

onVisibleChanged: {
if (relationshipsPage.visible && relationshipsPage.apiClient && relationshipsPage.tenantData && relationshipsPage.tenantData.m_id) {
relationshipsPage.apiClient.fetchTenantRelationships(relationshipsPage.tenantData.m_id)
}
}

Connections {
target: relationshipsPage.apiClient

function onTenantRelationshipAdded(relationshipId) {
PopupManager.addSuccessMessage(qsTr("Relationship created successfully"), true)
relationshipsPage.popEditor()
if (relationshipsPage.apiClient && relationshipsPage.tenantData && relationshipsPage.tenantData.m_id) {
relationshipsPage.apiClient.fetchTenantRelationships(relationshipsPage.tenantData.m_id)
}
}

function onTenantRelationshipRemoved(relationshipId) {
PopupManager.addSuccessMessage(qsTr("Relationship removed"), true)
if (relationshipsPage.apiClient && relationshipsPage.tenantData && relationshipsPage.tenantData.m_id) {
relationshipsPage.apiClient.fetchTenantRelationships(relationshipsPage.tenantData.m_id)
}
}
}

// --- Custom header ---
Component {
id: headerBtnsComp

Text {
text: qsTr("+ Create Relationship")
font.pixelSize: Style.fontSizeM
font.bold: true
color: (relationshipsPage.stateManager && (relationshipsPage.stateManager.isCreator || relationshipsPage.stateManager.isOwner))
? Style.linkColor : Style.inactiveTextColor

MouseArea {
anchors.fill: parent
hoverEnabled: true
cursorShape: Qt.PointingHandCursor
enabled: relationshipsPage.stateManager && (relationshipsPage.stateManager.isCreator || relationshipsPage.stateManager.isOwner)
onClicked: {
relationshipsPage.openCreate()
}
}
}
}

// --- Custom list delegate ---
delegateComponent: Component {
Rectangle {
id: relDelegate
width: relDelegate.parent ? relDelegate.parent.width : 0
height: relDelegateContent.height + 2 * Style.marginM
color: Style.alternateBaseColor
radius: Style.radiusS
border.color: Style.borderColor
border.width: 1

readonly property var __rel: modelData
readonly property bool __canManage: relationshipsPage.stateManager
&& (relationshipsPage.stateManager.isCreator || relationshipsPage.stateManager.isOwner)

Column {
id: relDelegateContent
anchors.left: relDelegate.left
anchors.right: relDelegate.right
anchors.top: relDelegate.top
anchors.margins: Style.marginM
spacing: Style.marginXS

BaseText {
width: relDelegateContent.width
elide: Text.ElideRight
text: qsTr("Target: %1").arg(relDelegate.__rel.targetTenantId || "")
font.pixelSize: Style.fontSizeM
color: Style.textColor
}

BaseText {
width: relDelegateContent.width
elide: Text.ElideRight
text: qsTr("Source role: %1   Target role: %2")
.arg(relDelegate.__rel.sourceRole || relDelegate.__rel.role || qsTr("Partner"))
.arg(relDelegate.__rel.targetRole || qsTr("Partner"))
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: relDelegateContent.width
visible: relDelegate.__rel.scope && relDelegate.__rel.scope !== ""
elide: Text.ElideRight
text: qsTr("Scope: %1").arg(relDelegate.__rel.scope || "")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: relDelegateContent.width
visible: relDelegate.__rel.description && relDelegate.__rel.description !== ""
elide: Text.ElideRight
text: relDelegate.__rel.description || ""
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

// --- Action button ---
Row {
spacing: Style.marginM
visible: relDelegate.__canManage

Button {
text: qsTr("Remove")
onClicked: {
if (relationshipsPage.apiClient) {
relationshipsPage.apiClient.removeTenantRelationship(
relationshipsPage.tenantData ? relationshipsPage.tenantData.m_id : "",
relDelegate.__rel.relationshipId || "")
}
}
}
}
}
}
}

// --- Create relationship editor (ViewBase with GqlBasedCommandsController) ---
Component {
id: createRelationshipComp

RelationshipView {
apiClient: relationshipsPage.apiClient
tenantData: relationshipsPage.tenantData
commandsControllerComp: Component {
GqlBasedCommandsController {
typeId: relationshipsPage.apiClient ? relationshipsPage.apiClient.relationshipObjectTypeId : ""
}
}

onCommandActivated: {
if (commandId === "save" || commandId === "create") {
submitRelationship()
}
}
}
}
}
