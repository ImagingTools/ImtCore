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
 * Displays tenant relationships via TenantSimpleCollectionPage with a
 * dedicated create form. Removal is done per-item in the list delegate.
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
if (apiClient && tenantData && tenantData.m_id)
apiClient.fetchTenantRelationships(tenantData.m_id)
}

Component.onCompleted: {
if (apiClient && tenantData && tenantData.m_id)
apiClient.fetchTenantRelationships(tenantData.m_id)
}

onVisibleChanged: {
if (visible && apiClient && tenantData && tenantData.m_id)
apiClient.fetchTenantRelationships(tenantData.m_id)
}

Connections {
target: relationshipsPage.apiClient

function onTenantRelationshipAdded(relationshipId) {
PopupManager.addSuccessMessage(qsTr("Relationship created successfully"), true)
relationshipsPage.popEditor()
if (relationshipsPage.apiClient && relationshipsPage.tenantData && relationshipsPage.tenantData.m_id)
relationshipsPage.apiClient.fetchTenantRelationships(relationshipsPage.tenantData.m_id)
}

function onTenantRelationshipRemoved(relationshipId) {
PopupManager.addSuccessMessage(qsTr("Relationship removed"), true)
if (relationshipsPage.apiClient && relationshipsPage.tenantData && relationshipsPage.tenantData.m_id)
relationshipsPage.apiClient.fetchTenantRelationships(relationshipsPage.tenantData.m_id)
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
onClicked: { relationshipsPage.openCreate() }
}
}
}

// --- Custom list delegate ---
delegateComponent: Component {
Rectangle {
id: relDelegate
width: parent ? parent.width : 0
height: relDelegateContent.height + 2 * Style.marginM
color: Style.alternateBaseColor
radius: Style.radiusS
border.color: Style.borderColor
border.width: 1

readonly property var __rel: modelData
readonly property bool __canManage: relationshipsPage.stateManager
&& (relationshipsPage.stateManager.isCreator || relationshipsPage.stateManager.isOwner)

Row {
id: relDelegateContent
anchors.left: parent.left
anchors.right: parent.right
anchors.top: parent.top
anchors.margins: Style.marginM
spacing: Style.marginM

Column {
width: parent.width - (removeRelBtn.visible ? removeRelBtn.width + Style.marginM : 0)
spacing: Style.marginXS

BaseText {
width: parent.width
elide: Text.ElideRight
text: qsTr("Target: %1").arg(__rel.targetTenantId || "")
font.pixelSize: Style.fontSizeM
color: Style.textColor
}

BaseText {
width: parent.width
elide: Text.ElideRight
text: qsTr("Source role: %1   Target role: %2")
.arg(__rel.sourceRole || __rel.role || qsTr("Partner"))
.arg(__rel.targetRole || qsTr("Partner"))
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: parent.width
visible: __rel.scope && __rel.scope !== ""
elide: Text.ElideRight
text: qsTr("Scope: %1").arg(__rel.scope || "")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: parent.width
visible: __rel.description && __rel.description !== ""
elide: Text.ElideRight
text: __rel.description || ""
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}
}

Button {
id: removeRelBtn
visible: __canManage
text: qsTr("Remove")
onClicked: {
if (relationshipsPage.apiClient)
relationshipsPage.apiClient.removeTenantRelationship(
relationshipsPage.tenantData ? relationshipsPage.tenantData.m_id : "",
__rel.relationshipId || "")
}
}
}
}
}

// --- Create relationship form ---
Component {
id: createRelationshipComp

Item {
id: createRelationshipForm

property string __selectedTargetTenantId: ""
property string __selectedTargetTenantName: ""

CustomScrollbar {
z: parent.z + 1
anchors.right: parent.right
anchors.top: createRelFlickable.top
anchors.bottom: createRelFlickable.bottom
secondSize: Style.marginM
targetItem: createRelFlickable
}

Flickable {
id: createRelFlickable
anchors.fill: parent
anchors.margins: Style.marginXL
contentHeight: createRelColumn.height + 2 * Style.marginXL
clip: true

Column {
id: createRelColumn
width: Style.sizeHintXXL
spacing: Style.marginL

GroupElementView {
width: parent.width

// --- Target Tenant selector ---
ElementView {
name: qsTr("Target Tenant")

controlComp: Component {
Row {
spacing: Style.marginM

BaseText {
anchors.verticalCenter: parent.verticalCenter
text: createRelationshipForm.__selectedTargetTenantName
  || createRelationshipForm.__selectedTargetTenantId
  || qsTr("Select tenant...")
color: createRelationshipForm.__selectedTargetTenantId
   ? Style.textColor : Style.inactiveTextColor
font.pixelSize: Style.fontSizeM
}

Button {
text: qsTr("Select")
onClicked: {
ModalDialogManager.openDialog(tenantSelectComp, {})
}
}
}
}
}

// --- Source Role ---
ComboBoxElementView {
id: sourceRoleCB
name: qsTr("Source Role")
model: roleModel
currentIndex: 2
}

// --- Target Role ---
ComboBoxElementView {
id: targetRoleCB
name: qsTr("Target Role")
model: roleModel
currentIndex: 2
}

// --- Scope ---
TextInputElementView {
id: relScopeInput
name: qsTr("Scope")
placeHolderText: qsTr("Optional — empty applies to all resources")
}

// --- Valid From ---
DateTimePickerElementView {
id: relValidFromPicker
name: qsTr("Valid From")
}

// --- Valid Until ---
DateTimePickerElementView {
id: relValidUntilPicker
name: qsTr("Valid Until")
}

// --- Description ---
TextInputElementView {
id: relDescriptionInput
name: qsTr("Description")
placeHolderText: qsTr("Optional description")
}

// --- Actions ---
Row {
spacing: Style.marginM

Button {
text: qsTr("Create Relationship")
onClicked: {
if (!createRelationshipForm.__selectedTargetTenantId) {
ModalDialogManager.showInfoDialog(qsTr("Target tenant is required."))
return
}
var roleTokens = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
var srcIdx = sourceRoleCB.currentIndex >= 0 ? sourceRoleCB.currentIndex : 2
var tgtIdx = targetRoleCB.currentIndex >= 0 ? targetRoleCB.currentIndex : 2
relationshipsPage.apiClient.addTenantRelationship(
relationshipsPage.tenantData ? relationshipsPage.tenantData.m_id : "",
createRelationshipForm.__selectedTargetTenantId,
roleTokens[srcIdx],
roleTokens[tgtIdx],
relScopeInput.text.trim(),
relValidFromPicker.getDateAsString(),
relValidUntilPicker.getDateAsString(),
relDescriptionInput.text.trim())
}
}

Button {
text: qsTr("Cancel")
onClicked: { relationshipsPage.popEditor() }
}
}
}
}
}

Component {
id: tenantSelectComp

FilterableSelectPopup {
dataProvider: FilterableSelectGqlDataProvider {
collectionId: "Tenants"
multiSelect: false
}
filterPlaceholder: qsTr("Select tenant...")
preselectedIds: createRelationshipForm.__selectedTargetTenantId
? [createRelationshipForm.__selectedTargetTenantId] : []

onItemSelected: function(itemId, index) {
createRelationshipForm.__selectedTargetTenantId = itemId
createRelationshipForm.__selectedTargetTenantName = dataProvider
? dataProvider.getSelectedItemText(itemId) : ""
}
}
}

TreeItemModel {
id: roleModel
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
