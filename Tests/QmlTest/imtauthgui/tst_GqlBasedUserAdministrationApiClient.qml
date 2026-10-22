import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "GqlBasedUserAdministrationApiClient"
	when: windowShown

	Component {
		id: clientComp

		GqlBasedUserAdministrationApiClient {
		}
	}

	function init(){
		GqlTestTransport.reset()
	}

	// The representation of each editor is sent and read under the ID of the collection element, so the
	// server stores the object under that ID and the History tab finds its revisions.
	function editors_data(){
		return [
			{tag: "Role", typeId: "Role", input: "role", update: "UpdateRoleFromRepresentation", get: "GetRoleRepresentation"},
			{tag: "Group", typeId: "Group", input: "group", update: "UpdateGroupFromRepresentation", get: "GetGroupRepresentation"},
			{tag: "User", typeId: "User", input: "user", update: "UpdateUserFromRepresentation", get: "GetUserRepresentation"}
		]
	}

	function editorParts(client, typeId){
		if (typeId === "Role"){
			return {service: client.roleDocumentManager, controllerComp: client.__roleControllerComp}
		}
		if (typeId === "Group"){
			return {service: client.groupDocumentManager, controllerComp: client.__groupControllerComp}
		}

		return {service: client.userDocumentManager, controllerComp: client.__userControllerComp}
	}

	function test_newDocumentIsSavedUnderTheProposedId_data(){
		return editors_data()
	}

	function test_newDocumentIsSavedUnderTheProposedId(data){
		let client = createTemporaryObject(clientComp, testCase)
		let parts = editorParts(client, data.typeId)

		// The NewDocumentCreated notification may arrive before the CreateNewDocument response.
		parts.service.handleDocumentCreated("doc", data.typeId, "", true, "", true, true)
		let controller = createTemporaryObject(parts.controllerComp, testCase)
		controller.documentId = "doc"

		parts.service.handleDocumentCreated("doc", data.typeId, "", true, "proposed-id", true, true)
		controller.updateDocumentFromRepresentation()

		tryVerify(function(){ return GqlTestTransport.requestedCommands().indexOf(data.update) >= 0 }, 5000)
		let request = GqlTestTransport.parseLastRequest()
		verify(request.parsed, "server-side parser rejected: " + GqlTestTransport.lastRequestBody)
		compare(request.params.input[data.input].id, "proposed-id", "The representation is sent under the proposed element ID")
	}

	function test_storedDocumentKeepsTheElementId_data(){
		return editors_data()
	}

	function test_storedDocumentKeepsTheElementId(data){
		let client = createTemporaryObject(clientComp, testCase)
		let parts = editorParts(client, data.typeId)

		parts.service.handleDocumentOpened("doc", "element-id", data.typeId, "Name", true, false)
		let controller = createTemporaryObject(parts.controllerComp, testCase)
		controller.documentId = "doc"

		let representation = {}
		representation[data.get] = {"id": "editor-id", "name": "Name"}
		GqlTestTransport.queueResponse(data.get, JSON.stringify({"data": representation}))

		controller.updateRepresentationFromDocument()

		tryVerify(function(){ return controller.representationModel.m_name === "Name" }, 5000, "The representation is read")
		compare(controller.representationModel.m_id, "element-id", "An object stored under another UUID is shown under its element ID")
	}
}
