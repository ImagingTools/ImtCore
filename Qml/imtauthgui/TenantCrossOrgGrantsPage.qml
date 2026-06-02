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
 * Displays the list of grants via TenantSimpleCollectionPage and opens a
 * dedicated create form (customEditorComponent) when the user clicks "Create".
 * Grants are immutable once created — revocation is performed per-item in the
 * list delegate.
 */
TenantSimpleCollectionPage {
id: grantsPage

entityName: qsTr("Grant")
entityNamePlural: qsTr("Cross-Org Grants")
descriptionText: qsTr("Delegate scoped access from this tenant to another tenant.")

listModel: apiClient ? apiClient.crossOrgGrantsModel : null

customEditorComponent: createGrantComp

// Revocation goes through the item delegate, not through the list Remove button.
headerButtonsComponent: headerBtnsComp

function updateGui() {
if (apiClient && tenantData && tenantData.m_id)
apiClient.fetchCrossOrgGrants(tenantData.m_id)
}

Component.onCompleted: {
if (apiClient && tenantData && tenantData.m_id)
apiClient.fetchCrossOrgGrants(tenantData.m_id)
}

onVisibleChanged: {
if (visible && apiClient && tenantData && tenantData.m_id)
apiClient.fetchCrossOrgGrants(tenantData.m_id)
}

Connections {
target: grantsPage.apiClient

function onCrossOrgGrantCreated(grantId) {
PopupManager.addSuccessMessage(qsTr("Cross-org grant created successfully"), true)
grantsPage.popEditor()
if (grantsPage.apiClient && grantsPage.tenantData && grantsPage.tenantData.m_id)
grantsPage.apiClient.fetchCrossOrgGrants(grantsPage.tenantData.m_id)
}

function onCrossOrgGrantRevoked(grantId) {
PopupManager.addSuccessMessage(qsTr("Cross-org grant revoked"), true)
if (grantsPage.apiClient && grantsPage.tenantData && grantsPage.tenantData.m_id)
grantsPage.apiClient.fetchCrossOrgGrants(grantsPage.tenantData.m_id)
}
}

// --- Custom header: only show "+ Create Grant" (no Edit / Remove) ---
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
if (grantsPage.apiClient)
grantsPage.apiClient.revokeCrossOrgGrant(__grant.grantId || "")
}
}
}
}
}

// --- Create grant form ---
Component {
id: createGrantComp

Item {
id: createGrantForm

property string __selectedTargetTenantId: ""
property string __selectedTargetTenantName: ""
property string __selectedRelationshipId: ""
property string __selectedRelationshipName: ""

CustomScrollbar {
z: parent.z + 1
anchors.right: parent.right
anchors.top: createGrantFlickable.top
anchors.bottom: createGrantFlickable.bottom
secondSize: Style.marginM
targetItem: createGrantFlickable
}

Flickable {
id: createGrantFlickable
anchors.fill: parent
anchors.margins: Style.marginXL
contentHeight: createGrantColumn.height + 2 * Style.marginXL
clip: true

Column {
id: createGrantColumn
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
text: createGrantForm.__selectedTargetTenantName
  || createGrantForm.__selectedTargetTenantId
  || qsTr("Select tenant...")
color: createGrantForm.__selectedTargetTenantId
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

// --- Relationship selector ---
ElementView {
name: qsTr("Relationship")

controlComp: Component {
Row {
spacing: Style.marginM

BaseText {
anchors.verticalCenter: parent.verticalCenter
text: createGrantForm.__selectedRelationshipName
  || createGrantForm.__selectedRelationshipId
  || qsTr("Select relationship...")
color: createGrantForm.__selectedRelationshipId
   ? Style.textColor : Style.inactiveTextColor
font.pixelSize: Style.fontSizeM
}

Button {
text: qsTr("Select")
onClicked: {
ModalDialogManager.openDialog(relationshipSelectComp, {})
}
}
}
}
}

// --- Access Level ---
ComboBoxElementView {
id: accessLevelCB
name: qsTr("Access Level")
model: accessLevelModel
currentIndex: 1
}

// --- Resource Scope ---
TextInputElementView {
id: resourceScopeInput
name: qsTr("Resource Scope")
placeHolderText: qsTr("Optional — empty grants all resources")
}

// --- Description ---
TextInputElementView {
id: grantDescriptionInput
name: qsTr("Description")
placeHolderText: qsTr("Optional description")
}

// --- Expires At ---
DateTimePickerElementView {
id: expiresAtPicker
name: qsTr("Expires At")
}

// --- Actions ---
Row {
spacing: Style.marginM

Button {
text: qsTr("Create Grant")
onClicked: {
if (!createGrantForm.__selectedTargetTenantId || !createGrantForm.__selectedRelationshipId) {
ModalDialogManager.showInfoDialog(qsTr("Target tenant and relationship are required."))
return
}
var levelIndex = accessLevelCB.currentIndex >= 0 ? accessLevelCB.currentIndex : 1
var levelTokens = ["None", "Read", "Write", "Admin"]
var accessLevel = levelTokens[levelIndex]
grantsPage.apiClient.createCrossOrgGrant(
grantsPage.tenantData ? grantsPage.tenantData.m_id : "",
createGrantForm.__selectedTargetTenantId,
createGrantForm.__selectedRelationshipId,
accessLevel,
resourceScopeInput.text.trim(),
"",
grantDescriptionInput.text.trim(),
expiresAtPicker.getDateAsString())
}
}

Button {
text: qsTr("Cancel")
onClicked: { grantsPage.popEditor() }
}
}
}
}
}

// --- Tenant selector popup ---
Component {
id: tenantSelectComp

FilterableSelectPopup {
dataProvider: FilterableSelectGqlDataProvider {
collectionId: "Tenants"
multiSelect: false
}
filterPlaceholder: qsTr("Select tenant...")
preselectedIds: createGrantForm.__selectedTargetTenantId
? [createGrantForm.__selectedTargetTenantId] : []

onItemSelected: function(itemId, index) {
createGrantForm.__selectedTargetTenantId = itemId
createGrantForm.__selectedTargetTenantName = dataProvider
? dataProvider.getSelectedItemText(itemId) : ""
}
}
}

// --- Relationship selector popup ---
Component {
id: relationshipSelectComp

FilterableSelectPopup {
dataProvider: FilterableSelectGqlDataProvider {
collectionId: "TenantRelationships"
multiSelect: false
}
filterPlaceholder: qsTr("Select relationship...")
preselectedIds: createGrantForm.__selectedRelationshipId
? [createGrantForm.__selectedRelationshipId] : []

onItemSelected: function(itemId, index) {
createGrantForm.__selectedRelationshipId = itemId
createGrantForm.__selectedRelationshipName = dataProvider
? dataProvider.getSelectedItemText(itemId) : ""
}
}
}

// --- Access level model ---
TreeItemModel {
id: accessLevelModel
Component.onCompleted: {
var levels = ["None", "Read", "Write", "Admin"]
for (var i = 0; i < levels.length; i++) {
var idx = insertNewItem()
setData("id", levels[i], idx)
setData("name", levels[i], idx)
}
}
}
}
}
}
