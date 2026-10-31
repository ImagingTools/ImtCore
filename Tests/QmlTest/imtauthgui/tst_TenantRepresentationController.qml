import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "TenantRepresentationController"
	when: windowShown

	Component {
		id: controllerComp

		TenantRepresentationController {
			collectionId: "Tenants"
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

	function createController(){
		let controller = createTemporaryObject(controllerComp, testCase)
		controller.documentId = "doc"
		controller.representationModel.m_name = "Typed"

		return controller
	}

	function requestCount(commandId){
		let commands = GqlTestTransport.requestedCommands()
		let count = 0
		for (let i = 0; i < commands.length; i++){
			if (commands[i] === commandId){
				count++
			}
		}

		return count
	}

	// A refused save reports the reason of the server; reading the organization again is left to DocumentDecorator
	function test_refusedUpdateIsReported_data(){
		return [
			{
				tag: "error",
				response: {"errors": [{"message": "Permission '/SensorManagement/Bind' is not available to the parent organization", "extensions": {"type": "Warning"}}]},
				message: "Permission '/SensorManagement/Bind' is not available to the parent organization"
			},
			{
				tag: "status with message",
				response: {"data": {"UpdateTenantFromRepresentation": {"status": "Failed", "message": "Organization name must be unique"}}},
				message: "Organization name must be unique"
			},
			{
				tag: "status without message",
				response: {"data": {"UpdateTenantFromRepresentation": {"status": "InvalidDocumentId"}}},
				message: "The organization could not be saved"
			}
		]
	}

	function test_refusedUpdateIsReported(data){
		let controller = createController()
		let failedSpy = createTemporaryObject(spyComp, testCase, {target: controller, signalName: "updateDocumentFailed"})
		let updatedSpy = createTemporaryObject(spyComp, testCase, {target: controller, signalName: "documentUpdated"})

		GqlTestTransport.queueResponse("UpdateTenantFromRepresentation", JSON.stringify(data.response))
		controller.updateDocumentFromRepresentation()

		tryCompare(failedSpy, "count", 1, 5000)
		compare(failedSpy.signalArguments[0][0], "doc")
		compare(failedSpy.signalArguments[0][1], data.message)
		compare(updatedSpy.count, 0, "A refused save is no saved document")

		wait(300)
		compare(requestCount("UpdateTenantFromRepresentation"), 1, "The refused change is sent once")
		compare(requestCount("GetTenantRepresentation"), 0, "The controller does not read the organization itself")
		verify(controller.readBackOnRefusal, "DocumentDecorator reads the editor again after a refusal")
		compare(failedSpy.count, 1)
	}

	// The editor of a new organization shows its parent and limits the scope to the parent's before the first save
	function test_newOrganizationReadsItsParent(){
		let controller = createController()
		verify(controller.requestRepresentationOnCreate, "A new organization reads its representation")

		GqlTestTransport.queueResponse("GetTenantRepresentation", JSON.stringify({"data": {"GetTenantRepresentation": {
			"id": "", "name": "", "isActive": true, "parentTenantId": "parent-id",
			"canEditTenantPermissions": true, "parentTenantPermissions": ["/Sensors/View", "/Sensors/Edit"]}}}))

		controller.updateRepresentationFromDocument()

		tryVerify(function(){ return controller.representationModel.m_parentTenantId === "parent-id" }, 5000)
		compare(controller.representationModel.m_parentTenantPermissions.length, 2)
		compare(String(controller.representationModel.m_parentTenantPermissions[0]), "/Sensors/View")

		wait(300)
		compare(requestCount("GetTenantRepresentation"), 1, "The new organization is read once")
		compare(requestCount("UpdateTenantFromRepresentation"), 0, "Reading it sends no change")
	}

	function test_savedUpdateIsNotReloaded(){
		let controller = createController()
		let failedSpy = createTemporaryObject(spyComp, testCase, {target: controller, signalName: "updateDocumentFailed"})
		let updatedSpy = createTemporaryObject(spyComp, testCase, {target: controller, signalName: "documentUpdated"})

		GqlTestTransport.queueResponse("UpdateTenantFromRepresentation", JSON.stringify({"data": {"UpdateTenantFromRepresentation": {"status": "Success"}}}))

		controller.updateDocumentFromRepresentation()

		tryCompare(updatedSpy, "count", 1, 5000)
		compare(failedSpy.count, 0)
		compare(GqlTestTransport.requestedCommands().indexOf("GetTenantRepresentation"), -1, "A saved organization is not read again")
		compare(requestCount("UpdateTenantFromRepresentation"), 1, "The change is sent once")
		compare(controller.representationModel.m_name, "Typed")
	}
}
