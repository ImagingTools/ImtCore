import QtQuick 2.12
import QtTest 1.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "UserPanelOrganizations"
	when: windowShown

	Component {
		id: panelComp

		UserPanel {
			width: 300
			height: 60
		}
	}

	// The rail the panel sits in: the account row shows its texts only there
	Component {
		id: railComp

		Item {
			id: rail

			width: 300
			height: 60

			// The pages of the rail, as the menu panel lists them
			property var pageIds: ["Search", "Tenants"]
			property var model: QtObject {
				function getItemsCount(){ return rail.pageIds.length }
				function getData(key, index){ return rail.pageIds[index] }
			}

			function showHint(text, y){}
			function hideHint(){}
		}
	}

	property bool previousTenantsEnabled: false
	property string previousUserId: ""
	property string previousTenantId: ""
	property string previousTenantName: ""

	function init(){
		GqlTestTransport.reset()
		previousTenantsEnabled = ApplicationInfoProvider.tenantsEnabled
		previousUserId = AuthorizationController.userTokenProvider.userId
		previousTenantId = AuthorizationController.currentTenantId
		previousTenantName = AuthorizationController.currentTenantName

		ApplicationInfoProvider.tenantsEnabled = true
		AuthorizationController.userTokenProvider.userId = "user-id"
		AuthorizationController.currentTenantId = "tenant-id"
		AuthorizationController.currentTenantName = "Old name"
	}

	function cleanup(){
		ApplicationInfoProvider.tenantsEnabled = previousTenantsEnabled
		AuthorizationController.userTokenProvider.userId = previousUserId
		AuthorizationController.currentTenantId = previousTenantId
		AuthorizationController.currentTenantName = previousTenantName
	}

	function requestCount(commandId){
		return GqlTestTransport.requestedCommands().filter(function(command){ return command === commandId }).length
	}

	// Reasons that come together, like a login that selects an organization, read the organizations once
	function test_aBurstOfReasonsReadsTheOrganizationsOnce(){
		let panel = createTemporaryObject(panelComp, testCase)
		wait(300)
		GqlTestTransport.reset()

		AuthorizationController.notifyOrganizationsChanged()
		AuthorizationController.notifyOrganizationsChanged()
		AuthorizationController.tenantMembershipRemoved("tenant-id", "user-id")

		tryCompare(testCase, "organizationsRequestCount", 1, 3000)
		wait(300)
		compare(requestCount("GetUserOrganizations"), 1)
	}

	property int organizationsRequestCount: 0

	Timer {
		interval: 20
		repeat: true
		running: true
		onTriggered: testCase.organizationsRequestCount = testCase.requestCount("GetUserOrganizations")
	}

	// A renamed current organization shows its new name in the panel
	function test_theCurrentOrganizationTakesItsNewName(){
		let panel = createTemporaryObject(panelComp, testCase)
		wait(300)
		GqlTestTransport.reset()

		GqlTestTransport.queueResponse("GetUserOrganizations", JSON.stringify({"data": {"GetUserOrganizations": {"organizations": [
			{"id": "tenant-id", "name": "New name", "isActive": true, "isOwner": true, "role": "Owner", "isDelegated": false}]}}}))

		AuthorizationController.notifyOrganizationsChanged()

		tryCompare(AuthorizationController, "currentTenantName", "New name", 3000)
	}
	// The pending invitations of the user, answered as the server does, reach the panel: the menu lists them and the account shows a mark
	function test_thePendingInvitationsReachThePanel(){
		let panel = createTemporaryObject(panelComp, testCase.parent, {"menuPanelRef": createTemporaryObject(railComp, testCase.parent)})
		wait(300)
		GqlTestTransport.reset()
		let previousToken = AuthorizationController.userTokenProvider.accessToken
		AuthorizationController.userTokenProvider.accessToken = "access-token"
		GqlTestTransport.queueResponse("GetMyTenantInvitations", JSON.stringify({"data": {"GetMyTenantInvitations": {"__typename": "GetMyTenantInvitationsPayload", "invitations": [
			{"__typename": "TenantInvitationData", "acceptedAt": "", "createdAt": "2026-11-23T15:26:06.528Z", "expiresAt": "2026-11-30T15:26:06.528Z", "id": "invitation-id",
				"invitedByUserId": "su-id", "lastSentAt": "2026-11-23T15:26:06.528Z", "rejectedAt": "", "revokedAt": "", "status": "Pending",
				"tenantId": "other-tenant-id", "tenantName": "IronBit", "userId": "user-id"}]}}}))

		AuthorizationController.refreshPendingInvitations()
		tryCompare(AuthorizationController, "pendingInvitationsCount", 1, 3000)
		AuthorizationController.userTokenProvider.accessToken = previousToken

		let request = GqlTestTransport.requests().filter(function(r){ return r.commandId === "GetMyTenantInvitations" })[0]
		verify(request.body.indexOf("Pending") >= 0, "Only the pending invitations are asked: " + request.body)
		compare(AuthorizationController.pendingInvitations[0].tenantName, "IronBit")

		// Without opening the menu: the count before the gear, as a page of the rail shows its badge
		let count = findChild(panel, "InvitationCount")
		let dot = findChild(panel, "InvitationDot")
		verify(count && count.visible, "The account row shows the count")
		verify(!dot.visible, "The open rail shows no dot")
		compare(count.children[0].text, "1")

		// Folded, the rail has no room for the count: a dot on the avatar
		panel.collapsed = true
		verify(dot.visible, "The folded rail shows the dot")
		verify(!count.visible)
		panel.collapsed = false

		AuthorizationController.__updatePendingInvitations(null)
		verify(!count.visible && !dot.visible, "Answered invitations leave no mark")
	}

	// The menu reads the invitations when it opens: a missed notification hides none
	function test_theMenuReadsTheInvitationsWhenItOpens(){
		let panel = createTemporaryObject(panelComp, testCase)
		wait(300)
		let previousToken = AuthorizationController.userTokenProvider.accessToken
		AuthorizationController.userTokenProvider.accessToken = "access-token"
		GqlTestTransport.reset()

		panel.openMenu()
		let count = requestCount("GetMyTenantInvitations")
		AuthorizationController.userTokenProvider.accessToken = previousToken
		while (ModalDialogManager.count > 0)
			ModalDialogManager.closeDialog()

		compare(count, 1)
	}

	// "All organizations" is offered only with the organizations page: without the permission for it a command does nothing
	function test_allOrganizationsNeedsTheirPage(){
		let rail = createTemporaryObject(railComp, testCase.parent)
		let panel = createTemporaryObject(panelComp, testCase.parent, {"menuPanelRef": rail})
		verify(panel.__hasPage("Tenants"))

		rail.pageIds = ["Search"]
		verify(!panel.__hasPage("Tenants"))
	}
}
