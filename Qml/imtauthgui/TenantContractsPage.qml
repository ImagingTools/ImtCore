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
 * TenantContractsPage
 *
 * Cooperation Contracts tab of the TenantEditor.
 *
 * Displays contracts via TenantSimpleCollectionPage and opens a dedicated
 * create form. Status changes (Activate / Terminate) are per-item in the
 * list delegate.
 */
TenantSimpleCollectionPage {
id: contractsPage

entityName: qsTr("Contract")
entityNamePlural: qsTr("Cooperation Contracts")
descriptionText: qsTr("Manage bilateral cooperation agreements with partner tenants.")

listModel: apiClient ? apiClient.contractsModel : null

customEditorComponent: createContractComp

headerButtonsComponent: headerBtnsComp

function updateGui() {
if (apiClient && tenantData && tenantData.m_id)
apiClient.fetchContracts(tenantData.m_id)
}

Component.onCompleted: {
if (apiClient && tenantData && tenantData.m_id)
apiClient.fetchContracts(tenantData.m_id)
}

onVisibleChanged: {
if (visible && apiClient && tenantData && tenantData.m_id)
apiClient.fetchContracts(tenantData.m_id)
}

Connections {
target: contractsPage.apiClient

function onContractCreated(contractId) {
PopupManager.addSuccessMessage(qsTr("Contract created successfully"), true)
contractsPage.popEditor()
if (contractsPage.apiClient && contractsPage.tenantData && contractsPage.tenantData.m_id)
contractsPage.apiClient.fetchContracts(contractsPage.tenantData.m_id)
}

function onContractStatusUpdated(contractId) {
PopupManager.addSuccessMessage(qsTr("Contract status updated"), true)
if (contractsPage.apiClient && contractsPage.tenantData && contractsPage.tenantData.m_id)
contractsPage.apiClient.fetchContracts(contractsPage.tenantData.m_id)
}

function onContractTerminated(contractId) {
PopupManager.addSuccessMessage(qsTr("Contract terminated"), true)
if (contractsPage.apiClient && contractsPage.tenantData && contractsPage.tenantData.m_id)
contractsPage.apiClient.fetchContracts(contractsPage.tenantData.m_id)
}
}

// --- Custom header ---
Component {
id: headerBtnsComp

Text {
text: qsTr("+ Create Contract")
font.pixelSize: Style.fontSizeM
font.bold: true
color: (contractsPage.stateManager && (contractsPage.stateManager.isCreator || contractsPage.stateManager.isOwner))
   ? Style.linkColor : Style.inactiveTextColor

MouseArea {
anchors.fill: parent
hoverEnabled: true
cursorShape: Qt.PointingHandCursor
enabled: contractsPage.stateManager && (contractsPage.stateManager.isCreator || contractsPage.stateManager.isOwner)
onClicked: { contractsPage.openCreate() }
}
}
}

// --- Custom list delegate ---
delegateComponent: Component {
Rectangle {
id: contractDelegate
width: parent ? parent.width : 0
height: contractDelegateContent.height + 2 * Style.marginM
color: Style.alternateBaseColor
radius: Style.radiusS
border.color: Style.borderColor
border.width: 1

readonly property var __contract: modelData
readonly property bool __canManage: contractsPage.stateManager
&& (contractsPage.stateManager.isCreator || contractsPage.stateManager.isOwner)

Row {
id: contractDelegateContent
anchors.left: parent.left
anchors.right: parent.right
anchors.top: parent.top
anchors.margins: Style.marginM
spacing: Style.marginM

Column {
width: parent.width - (contractActions.visible ? contractActions.width + Style.marginM : 0)
spacing: Style.marginXS

BaseText {
width: parent.width
elide: Text.ElideRight
text: qsTr("Target: %1").arg(__contract.targetTenantId || "")
font.pixelSize: Style.fontSizeM
color: Style.textColor
}

BaseText {
width: parent.width
elide: Text.ElideRight
text: qsTr("Status: %1   Scope: %2")
.arg(__contract.status || qsTr("Draft"))
.arg((__contract.scope && __contract.scope !== "") ? __contract.scope : qsTr("All"))
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: parent.width
visible: (__contract.validFrom && __contract.validFrom !== "")
     || (__contract.validUntil && __contract.validUntil !== "")
elide: Text.ElideRight
text: qsTr("Valid: %1 – %2")
.arg((__contract.validFrom && __contract.validFrom !== "") ? __contract.validFrom : qsTr("now"))
.arg((__contract.validUntil && __contract.validUntil !== "") ? __contract.validUntil : qsTr("open"))
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

BaseText {
width: parent.width
visible: __contract.description && __contract.description !== ""
elide: Text.ElideRight
text: __contract.description || ""
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}
}

Column {
id: contractActions
visible: __canManage
spacing: Style.marginXS

Button {
visible: __contract.status === "Draft"
text: qsTr("Activate")
onClicked: {
if (contractsPage.apiClient)
contractsPage.apiClient.updateContractStatus(__contract.contractId || "", "Active")
}
}

Button {
visible: __contract.status !== "Terminated"
text: qsTr("Terminate")
onClicked: {
if (contractsPage.apiClient)
contractsPage.apiClient.terminateContract(__contract.contractId || "")
}
}
}
}
}
}

// --- Create contract form ---
Component {
id: createContractComp

Item {
id: createContractForm

property string __selectedRelationshipId: ""
property string __selectedRelationshipName: ""
property string __selectedTargetTenantId: ""
property string __selectedTargetTenantName: ""

CustomScrollbar {
z: parent.z + 1
anchors.right: parent.right
anchors.top: createContractFlickable.top
anchors.bottom: createContractFlickable.bottom
secondSize: Style.marginM
targetItem: createContractFlickable
}

Flickable {
id: createContractFlickable
anchors.fill: parent
anchors.margins: Style.marginXL
contentHeight: createContractColumn.height + 2 * Style.marginXL
clip: true

Column {
id: createContractColumn
width: Style.sizeHintXXL
spacing: Style.marginL

GroupElementView {
width: parent.width

// --- Relationship selector ---
ElementView {
name: qsTr("Relationship")

controlComp: Component {
Row {
spacing: Style.marginM

BaseText {
anchors.verticalCenter: parent.verticalCenter
text: createContractForm.__selectedRelationshipName
  || createContractForm.__selectedRelationshipId
  || qsTr("Select relationship...")
color: createContractForm.__selectedRelationshipId
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

// --- Target Tenant selector ---
ElementView {
name: qsTr("Counterparty Tenant")

controlComp: Component {
Row {
spacing: Style.marginM

BaseText {
anchors.verticalCenter: parent.verticalCenter
text: createContractForm.__selectedTargetTenantName
  || createContractForm.__selectedTargetTenantId
  || qsTr("Select tenant...")
color: createContractForm.__selectedTargetTenantId
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

// --- Scope ---
TextInputElementView {
id: contractScopeInput
name: qsTr("Scope")
placeHolderText: qsTr("Optional — agreed scope of cooperation")
}

// --- Valid From ---
DateTimePickerElementView {
id: validFromPicker
name: qsTr("Valid From")
}

// --- Valid Until ---
DateTimePickerElementView {
id: validUntilPicker
name: qsTr("Valid Until")
}

// --- Description ---
TextInputElementView {
id: contractDescriptionInput
name: qsTr("Description")
placeHolderText: qsTr("Optional description")
}

// --- Terms ---
TextInputElementView {
id: contractTermsInput
name: qsTr("Terms")
placeHolderText: qsTr("Optional free-form terms metadata")
}

// --- Actions ---
Row {
spacing: Style.marginM

Button {
text: qsTr("Create Contract")
onClicked: {
if (!createContractForm.__selectedRelationshipId || !createContractForm.__selectedTargetTenantId) {
ModalDialogManager.showInfoDialog(qsTr("Relationship and counterparty tenant are required."))
return
}
contractsPage.apiClient.createContract(
createContractForm.__selectedRelationshipId,
contractsPage.tenantData ? contractsPage.tenantData.m_id : "",
createContractForm.__selectedTargetTenantId,
contractScopeInput.text.trim(),
validFromPicker.getDateAsString(),
validUntilPicker.getDateAsString(),
contractDescriptionInput.text.trim(),
contractTermsInput.text.trim())
}
}

Button {
text: qsTr("Cancel")
onClicked: { contractsPage.popEditor() }
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
preselectedIds: createContractForm.__selectedTargetTenantId
? [createContractForm.__selectedTargetTenantId] : []

onItemSelected: function(itemId, index) {
createContractForm.__selectedTargetTenantId = itemId
createContractForm.__selectedTargetTenantName = dataProvider
? dataProvider.getSelectedItemText(itemId) : ""
}
}
}

Component {
id: relationshipSelectComp

FilterableSelectPopup {
dataProvider: FilterableSelectGqlDataProvider {
collectionId: "TenantRelationships"
multiSelect: false
}
filterPlaceholder: qsTr("Select relationship...")
preselectedIds: createContractForm.__selectedRelationshipId
? [createContractForm.__selectedRelationshipId] : []

onItemSelected: function(itemId, index) {
createContractForm.__selectedRelationshipId = itemId
createContractForm.__selectedRelationshipName = dataProvider
? dataProvider.getSelectedItemText(itemId) : ""
}
}
}
}
}
}
