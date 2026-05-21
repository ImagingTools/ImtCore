// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import QtQuick.Controls
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0

/**
 * TenantMembersPage
 *
 * Members tab — displays active members and invited/pending users in a centered
 * 800px table with gray rounded border, checkbox selection and avatars.
 */
ViewBase {
id: membersPage

commandsPanelVisible: false
contentColor: Style.baseColor
readonly property var tenantData: membersPage.model
property var stateManager: null
property var apiClient: null
property var userDataFactory: null

function updateGui() {}
function updateModel() {}

property string __editUserId: ""
property string __editUserName: ""
property string __editUserDescription: ""

readonly property bool __canManage: membersPage.stateManager ? membersPage.stateManager.canManageMembers : false

Connections {
target: membersPage.apiClient
function onUserDataReceived(data) {
if (membersPage.stateManager)
membersPage.stateManager.receivedUserData = data
}
function onUserCreated() {
if (membersPage.stateManager && membersPage.tenantData)
membersPage.stateManager.loadMembersFromModel()
}
}

StackViewHeader {
id: membersStackViewHeader
anchors.top: parent.top
anchors.horizontalCenter: parent.horizontalCenter
width: Math.min(parent.width - Style.marginXL * 2, 1000)
height: Style.controlHeightL
initialItemTitleVisible: true

onCloseClicked: {
membersStackView.previous()
membersStackViewHeader.popHeader()
}

Component.onCompleted: {
membersStackViewHeader.addHeader("members_list", qsTr("Members"))
}
}

Row {
id: membersHeaderButtons
visible: membersPage.__canManage && membersStackView.currentIndex === 0
anchors.right: membersStackViewHeader.right
anchors.verticalCenter: membersStackViewHeader.verticalCenter
spacing: Style.marginL

Text {
id: createUserBtn
text: "+ " + qsTr("Create User")
font.pixelSize: Style.fontSizeM
font.bold: true
color: Style.linkColor

MouseArea {
anchors.fill: parent
hoverEnabled: true
cursorShape: Qt.PointingHandCursor
onClicked: {
while (membersStackView.count > 1)
membersStackView.removePage(membersStackView.count - 1)
membersStackViewHeader.addHeader("create_user", qsTr("Create New User"))
membersStackView.addPage(userEditorView)
membersStackView.next()
}
}
}

Text {
id: inviteMemberBtn
text: "+ " + qsTr("Create Invitation")
font.pixelSize: Style.fontSizeM
font.bold: true
color: Style.linkColor

MouseArea {
anchors.fill: parent
hoverEnabled: true
cursorShape: Qt.PointingHandCursor
onClicked: {
var ids = []
var members = membersPage.stateManager ? membersPage.stateManager.pendingMembers : []
for (var i = 0; i < members.length; i++)
ids.push(members[i].id)
var point = inviteMemberBtn.mapToItem(null, 0, inviteMemberBtn.height)
ModalDialogManager.openDialog(membersSelectPopupComp, {
"x": point.x,
"y": point.y,
"preselectedIds": ids
})
}
}
}
}

BaseText {
id: membersDescription
anchors.top: membersStackViewHeader.bottom
anchors.topMargin: Style.marginS
anchors.horizontalCenter: parent.horizontalCenter
width: Math.min(parent.width - Style.marginXL * 2, 1000)
visible: membersStackView.currentIndex === 0
text: qsTr("Manage tenant members. Users created here automatically become members.")
font.pixelSize: Style.fontSizeS
color: Style.inactiveTextColor
}

StackView {
id: membersStackView
anchors.top: membersDescription.visible ? membersDescription.bottom : membersStackViewHeader.bottom
anchors.topMargin: Style.marginS
anchors.left: parent.left
anchors.right: parent.right
anchors.bottom: parent.bottom

Component.onCompleted: {
membersStackView.addPage(membersListView)
}
}

Component {
id: membersListView

Item {
TenantTableContainer {
anchors.top: parent.top
anchors.bottom: parent.bottom
anchors.topMargin: Style.marginM

IdSelectionManager {
id: membersSelectionManager
multiSelect: true
}

TenantTableHeader {
id: membersTableHeader
anchors.top: parent.top
anchors.left: parent.left
anchors.right: parent.right
selectedCount: membersSelectionManager.selectedIds.length
totalCount: membersListViewContent.count + invitationsListView.count
checkState: membersSelectionManager.selectedIds.length === 0
? Qt.Unchecked
: (membersSelectionManager.selectedIds.length === (membersListViewContent.count + invitationsListView.count)
? Qt.Checked : Qt.PartiallyChecked)

onSelectAllToggled: {
var totalItems = membersListViewContent.count + invitationsListView.count
if (membersSelectionManager.selectedIds.length === totalItems) {
membersSelectionManager.clear()
} else {
var allIds = []
var members = membersPage.stateManager ? membersPage.stateManager.pendingMembers : []
for (var i = 0; i < members.length; i++)
allIds.push(members[i].id)
var invitations = membersPage.stateManager ? membersPage.stateManager.pendingInvitations : []
for (var j = 0; j < invitations.length; j++)
allIds.push("inv_" + invitations[j].id)
membersSelectionManager.selectMultiple(allIds)
}
}
}

Flickable {
id: membersFlickable
anchors.top: membersTableHeader.bottom
anchors.left: parent.left
anchors.right: parent.right
anchors.bottom: parent.bottom
anchors.bottomMargin: Style.marginS
clip: true
contentHeight: membersColumn.height
boundsBehavior: Flickable.StopAtBounds

Column {
id: membersColumn
width: parent.width

// ---- Active Members ----
Repeater {
id: membersListViewContent
model: membersPage.stateManager ? membersPage.stateManager.pendingMembers : []

delegate: TenantMemberDelegate {
width: membersColumn.width
kind: "member"
memberData: modelData
tenantData: membersPage.tenantData
stateManager: membersPage.stateManager
canManageMembers: membersPage.__canManage
isOwner: membersPage.stateManager ? membersPage.stateManager.isOwner : false

onMemberActionsRequested: {
memberActionMenu.menuItems = menuItems
memberActionMenu.targetUserId = userId
memberActionMenu.targetUserName = userName
memberActionMenu.popup()
}
}
}

// ---- Invited Users Section ----
Rectangle {
visible: invitationsListView.count > 0
width: membersColumn.width
height: invitedHeader.implicitHeight + Style.marginM * 2
color: Style.backgroundColor2

BaseText {
id: invitedHeader
anchors.left: parent.left
anchors.leftMargin: Style.marginM
anchors.verticalCenter: parent.verticalCenter
text: qsTr("Invited (%1)").arg(invitationsListView.count)
font.pixelSize: Style.fontSizeS
font.bold: true
color: Style.inactiveTextColor
}
}

Repeater {
id: invitationsListView
model: membersPage.stateManager ? membersPage.stateManager.pendingInvitations : []

delegate: TenantMemberDelegate {
width: membersColumn.width
kind: "invitation"
memberData: modelData
tenantData: membersPage.tenantData
stateManager: membersPage.stateManager
canManageMembers: membersPage.__canManage
isOwner: membersPage.stateManager ? membersPage.stateManager.isOwner : false

onInviteActionsRequested: {
inviteActionMenu.menuItems = menuItems
inviteActionMenu.targetInvitationId = invitationId
inviteActionMenu.targetUserName = userName
inviteActionMenu.popup()
}
}
}

// Empty state
Item {
visible: membersListViewContent.count === 0 && invitationsListView.count === 0
width: membersColumn.width
height: Style.controlHeightL * 3

BaseText {
anchors.centerIn: parent
text: qsTr("No members found.")
font.pixelSize: Style.fontSizeM
color: Style.inactiveTextColor
}
}
}
}

// Member actions context menu
Menu {
id: memberActionMenu
property var menuItems: []
property string targetUserId: ""
property string targetUserName: ""

Instantiator {
model: memberActionMenu.menuItems
delegate: MenuItem {
text: modelData.text || ""
onTriggered: {
var action = modelData.action
if (action === "remove" && membersPage.apiClient) {
ModalDialogManager.showConfirmationDialog(
qsTr("Remove Member"),
qsTr("Are you sure you want to remove \"%1\"?").arg(memberActionMenu.targetUserName),
function(result) {
if (result === true)
membersPage.apiClient.removeUser(memberActionMenu.targetUserId)
}
)
} else if (action === "changeRole" && membersPage.apiClient) {
membersPage.apiClient.changeMemberRole(memberActionMenu.targetUserId)
} else if (action === "transfer" && membersPage.apiClient) {
membersPage.apiClient.transferOwnership(memberActionMenu.targetUserId)
} else if (action === "leave" && membersPage.apiClient) {
membersPage.apiClient.removeUser(memberActionMenu.targetUserId)
}
}
}
onObjectAdded: memberActionMenu.insertItem(index, object)
onObjectRemoved: memberActionMenu.removeItem(object)
}
}

// Invitation actions context menu
Menu {
id: inviteActionMenu
property var menuItems: []
property string targetInvitationId: ""
property string targetUserName: ""

Instantiator {
model: inviteActionMenu.menuItems
delegate: MenuItem {
text: modelData.text || ""
onTriggered: {
var action = modelData.action
if (action === "resend" && membersPage.apiClient)
membersPage.apiClient.resendInvitation(inviteActionMenu.targetInvitationId)
else if (action === "revoke" && membersPage.apiClient)
membersPage.apiClient.revokeInvitation(inviteActionMenu.targetInvitationId)
}
}
onObjectAdded: inviteActionMenu.insertItem(index, object)
onObjectRemoved: inviteActionMenu.removeItem(object)
}
}
}
}
}

Component {
id: userEditorView

Item {
UserView {
id: createUserView
anchors.fill: parent
commandsPanelVisible: false

Component.onCompleted: {
createUserView.model = membersPage.userDataFactory ? membersPage.userDataFactory() : null
createUserView.updateGui()
}
}

Row {
anchors.bottom: parent.bottom
anchors.bottomMargin: Style.marginXL
anchors.left: parent.left
anchors.leftMargin: Style.marginXL
spacing: Style.marginM

Button {
text: qsTr("Create")
onClicked: {
createUserView.updateModel()
var userData = createUserView.model
if (membersPage.apiClient)
membersPage.apiClient.insertUser(
userData ? userData.m_name : "",
userData ? userData.m_description : "")
membersStackViewHeader.popHeader()
membersStackView.previous()
}
}

Button {
text: qsTr("Cancel")
onClicked: {
membersStackViewHeader.popHeader()
membersStackView.previous()
}
}
}
}
}

// ===== Invite-user popup =====
Component {
id: membersSelectPopupComp

FilterableSelectPopup {
dataProvider: FilterableSelectGqlDataProvider {
collectionId: "Users"
multiSelect: true
}

itemWidth: 280
showCheckBox: true
showSelectedGroup: true
filterPlaceholder: qsTr("Type or choose a user to invite")

onSelectionChanged: {
var arr = []
for (var i = 0; i < selectedIds.length; i++) {
var selId = selectedIds[i]
var selName = dataProvider ? dataProvider.getSelectedItemText(selId) : ""
if (!selName)
selName = selId
arr.push({id: selId, name: selName})
}
if (membersPage.stateManager)
membersPage.stateManager.inviteSelectedUsers(arr)
}

Component.onDestruction: {
if (membersPage.stateManager && membersPage.stateManager.__membersModifiedLocally) {
membersPage.doUpdateModel()
membersPage.stateManager.__membersModifiedLocally = false
}
}
}
}
}
