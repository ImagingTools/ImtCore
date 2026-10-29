import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "GqlBasedTenantManagementApiClient"
	when: windowShown

	Component {
		id: clientComp

		GqlBasedTenantManagementApiClient {
		}
	}

	Component {
		id: spyComp

		SignalSpy {
		}
	}

	function init(){
		GqlTestTransport.reset()
	}

	function membershipResponse(membershipId, userId){
		return JSON.stringify({"data": {"FindMembership": {"membership": {"id": membershipId, "userId": userId, "tenantId": "tenant"}}}})
	}

	// Several members excluded at once are removed one after another, each reported under its own user.
	function test_excludedMembersAreRemovedOneByOne(){
		let client = createTemporaryObject(clientComp, testCase)
		let removedSpy = createTemporaryObject(spyComp, testCase, {"target": client, "signalName": "memberRemoved"})
		GqlTestTransport.queueResponse("FindMembership", membershipResponse("membership-1", "user-1"))
		GqlTestTransport.queueResponse("FindMembership", membershipResponse("membership-2", "user-2"))
		GqlTestTransport.queueResponse("RemoveMembership", JSON.stringify({"data": {"RemoveMembership": {"success": true}}}))
		GqlTestTransport.queueResponse("RemoveMembership", JSON.stringify({"data": {"RemoveMembership": {"success": true}}}))

		client.removeMember("tenant", "user-1")
		client.removeMember("tenant", "user-2")

		tryCompare(removedSpy, "count", 2, 5000)
		compare(removedSpy.signalArguments[0][0], "user-1")
		compare(removedSpy.signalArguments[1][0], "user-2")

		let commands = GqlTestTransport.requestedCommands().filter(function(command){
			return command === "FindMembership" || command === "RemoveMembership"
		})
		compare(commands, ["FindMembership", "RemoveMembership", "FindMembership", "RemoveMembership"])
	}

	// A failed removal does not stop the members queued after it.
	function test_failedRemovalContinuesWithTheNextMember(){
		let client = createTemporaryObject(clientComp, testCase)
		let removedSpy = createTemporaryObject(spyComp, testCase, {"target": client, "signalName": "memberRemoved"})
		GqlTestTransport.queueResponse("FindMembership", JSON.stringify({"data": {"FindMembership": {"errorMessage": "Membership not found"}}}))
		GqlTestTransport.queueResponse("FindMembership", membershipResponse("membership-2", "user-2"))
		GqlTestTransport.queueResponse("RemoveMembership", JSON.stringify({"data": {"RemoveMembership": {"success": true}}}))

		client.removeMember("tenant", "user-1")
		client.removeMember("tenant", "user-2")

		tryCompare(removedSpy, "count", 1, 5000)
		compare(removedSpy.signalArguments[0][0], "user-2")
	}
}
