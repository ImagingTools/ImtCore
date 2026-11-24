import QtQuick 2.12
import QtTest 1.12
import Acf 1.0
import imtauthgui 1.0

TestCase {
	id: testCase

	name: "TenantMembersLevels"
	when: windowShown

	readonly property var administratorPermissions: ["/OrganizationManagement/Members/View", "/OrganizationManagement/Members/Edit/Invite"]

	Component {
		id: stateManagerComp

		QtObject {
			property bool canViewOrganizationPermissions: true
			property bool canEditOrganizationMemberPermissions: true
			property bool canInviteOrganizationMember: false
			property bool canExcludeOrganizationMember: false
			property bool canChangeOrganizationMember: false
			property bool isOwner: false
			property var pendingMembers: []
			property var pendingInvitations: []
			property bool __membersModifiedLocally: false

			function hasAnyPermission(ids){ return false }
			function hasPermission(id){ return false }
			function isInvitationExpired(expiresAt){ return false }
			function formatInvitationInfo(invitedByName, expiresAt){ return "" }
			function loadMembersFromModel(){}
			function setMemberOrganizationPermissions(userId, permissions){
				let members = pendingMembers.slice()
				for (let i = 0; i < members.length; ++i){
					if (members[i].id === userId){
						members[i] = {"id": userId, "name": members[i].name, "organizationPermissions": permissions}
					}
				}
				pendingMembers = members
			}
		}
	}

	// Records what the page sends; the tree and the level Administrator come as from the server
	Component {
		id: apiClientComp

		QtObject {
			property var userDocumentManager: null
			property string userObjectId: ""
			property var administratorOrganizationPermissions: []
			property var sent: []

			signal organizationPermissionsReceived()
			signal memberPermissionsUpdated()
			signal requestFailed(string message)
			signal memberRemoved(string userId)
			signal invitationRevoked(string invitationId)

			function fetchOrganizationPermissions(tenantId, userId){}
			function setMemberOrganizationPermissionsByUser(tenantId, userId, permissions){
				sent.push({"tenantId": tenantId, "userId": userId, "permissions": permissions})
			}
		}
	}

	Component {
		id: pageComp

		TenantMembersPage {
			width: 800
			height: 600
		}
	}

	function createPage(stateProperties){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		let properties = {"pendingMembers": [
			{"id": "owner", "name": "Owner", "organizationPermissions": []},
			{"id": "admin", "name": "Admin", "organizationPermissions": [administratorPermissions[1], administratorPermissions[0]]},
			{"id": "plain", "name": "Plain", "organizationPermissions": []},
			{"id": "special", "name": "Special", "organizationPermissions": [administratorPermissions[0]]}
		]}
		for (let key in stateProperties){
			properties[key] = stateProperties[key]
		}
		let stateManager = createTemporaryObject(stateManagerComp, testCase, properties)
		let page = createTemporaryObject(pageComp, testCase.parent, {
			"apiClient": apiClient,
			"stateManager": stateManager,
			"model": {"m_id": "tenant", "m_ownerId": "owner", "m_creatorId": "", "m_currentUserId": "owner"}
		})
		wait(50)

		// The tree arrives with the level Administrator
		apiClient.administratorOrganizationPermissions = administratorPermissions
		apiClient.organizationPermissionsReceived()

		return {"page": page, "apiClient": apiClient, "stateManager": stateManager}
	}

	function roleOf(page, userId){
		let items = page.__combinedModel
		for (let i = 0; i < items.length; ++i){
			if (items[i].id === userId){
				return page.createRow(items[i]).role
			}
		}
		return null
	}

	// The level is told by the permissions: the set of an administrator in any order, none, or another set
	function test_theLevelOfEveryMember(){
		let context = createPage({})

		compare(roleOf(context.page, "owner"), "Owner")
		compare(roleOf(context.page, "admin"), "Administrator")
		compare(roleOf(context.page, "plain"), "Member")
		compare(roleOf(context.page, "special"), "Special permissions")
	}

	// The menu offers the levels the member does not have, and the tree for the fine tuning
	function test_theMenuOffersTheOtherLevels(){
		let context = createPage({})

		compare(context.page.__memberMenuItems("plain"), ["makeAdministrator", "managePermissions"])
		compare(context.page.__memberMenuItems("admin"), ["makeMember", "managePermissions"])
		compare(context.page.__memberMenuItems("special"), ["makeAdministrator", "makeMember", "managePermissions"])
		compare(context.page.__memberMenuItems("owner"), [], "The owner has no level to change")
	}

	// Who may not change the permissions gets no level commands
	function test_noLevelsWithoutTheChangeOfThePermissions(){
		let context = createPage({"canEditOrganizationMemberPermissions": false})

		compare(context.page.__memberMenuItems("plain"), [])
	}

	// A member is made an administrator with the set of the server; the row shows it once the server agrees
	function test_makeAdministratorSendsTheSetOfTheServer(){
		let context = createPage({})

		context.page.applyMemberPermissions("plain", context.apiClient.administratorOrganizationPermissions)
		compare(context.apiClient.sent.length, 1)
		compare(context.apiClient.sent[0].tenantId, "tenant")
		compare(context.apiClient.sent[0].userId, "plain")
		compare(context.apiClient.sent[0].permissions, administratorPermissions)
		compare(roleOf(context.page, "plain"), "Member", "Nothing changes before the answer")

		context.apiClient.memberPermissionsUpdated()
		compare(roleOf(context.page, "plain"), "Administrator")
	}

	// A refused change leaves the member as it was
	function test_aRefusedChangeKeepsTheLevel(){
		let context = createPage({})

		context.page.applyMemberPermissions("admin", [])
		context.apiClient.requestFailed("Access denied")
		context.apiClient.memberPermissionsUpdated()

		compare(roleOf(context.page, "admin"), "Administrator")
	}

	// Without the set of the server there is nothing to make an administrator with
	function test_noAdministratorBeforeTheTree(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		let stateManager = createTemporaryObject(stateManagerComp, testCase, {"pendingMembers": [{"id": "plain", "name": "Plain", "organizationPermissions": []}]})
		let page = createTemporaryObject(pageComp, testCase.parent, {
			"apiClient": apiClient,
			"stateManager": stateManager,
			"model": {"m_id": "tenant", "m_ownerId": "owner", "m_creatorId": "", "m_currentUserId": "owner"}
		})
		wait(50)

		compare(page.__memberMenuItems("plain"), ["managePermissions"])
	}
}
