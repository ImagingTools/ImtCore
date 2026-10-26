import QtQuick 2.12
import QtTest 1.12
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
}
