import QtQuick 2.12
import QtTest 1.12
import imtassistgui 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "AssistantSettingsPage"
	when: windowShown

	Component {
		id: pageComp

		AssistantSettingsPage {
			width: 800
			height: 900
			canEdit: true
		}
	}

	function init(){
		GqlTestTransport.reset()
	}

	function response(commandId, payload){
		let data = {}
		data[commandId] = payload

		return JSON.stringify({"data": data})
	}

	function configuration(fields){
		let result = {
			"enabled": true,
			"presetId": "bedrock",
			"model": "eu.anthropic.claude",
			"endpoint": "",
			"region": "eu-central-1",
			"project": "",
			"resource": "",
			"hasApiKey": true,
			"maxOutputTokens": 2048,
			"additionalInstructions": "Antworte auf Deutsch.",
			"updatedAt": "",
			"allowsCustomEndpoint": false,
			"presets": [
				{"id": "openai", "name": "OpenAI", "needsRegion": false, "needsProject": false, "needsResource": false, "needsEndpoint": false, "needsApiKey": true, "credentialHint": "OpenAI API key"},
				{"id": "bedrock", "name": "Amazon Bedrock", "needsRegion": true, "needsProject": false, "needsResource": false, "needsEndpoint": false, "needsApiKey": true, "credentialHint": "Bedrock API key"}
			]
		}

		for (let key in fields){
			result[key] = fields[key]
		}

		return result
	}

	function loadedPage(config){
		GqlTestTransport.queueResponse("AssistantGetConfiguration", response("AssistantGetConfiguration", {"configurationJson": JSON.stringify(config)}))

		let page = createTemporaryObject(pageComp, testCase.parent)
		tryCompare(page, "loading", false, 5000)

		return page
	}

	function lastInput(commandId){
		tryVerify(function(){ return GqlTestTransport.requestedCommands().indexOf(commandId) >= 0 }, 5000)

		let request = GqlTestTransport.parseLastRequest()
		verify(request.parsed, "server-side parser rejected: " + GqlTestTransport.lastRequestBody)
		compare(request.commandId, commandId)

		return request.params.input
	}

	function test_loadShowsConfigurationWithoutKey(){
		let page = loadedPage(configuration({}))

		compare(page.errorText, "")
		verify(findChild(page, "AssistantEnabled").checked)
		compare(findChild(page, "AssistantModel").text, "eu.anthropic.claude")
		verify(findChild(page, "AssistantRegion").visible)
		verify(!findChild(page, "AssistantEndpoint").visible)
		compare(findChild(page, "AssistantApiKey").text, "")
		verify(findChild(page, "AssistantRemoveApiKey").visible)
		compare(page.selectedPreset.id, "bedrock")
	}

	function test_saveKeepsTheStoredKey(){
		let page = loadedPage(configuration({}))

		GqlTestTransport.queueResponse("AssistantSetConfiguration", response("AssistantSetConfiguration", {"success": true, "configurationJson": JSON.stringify(configuration({}))}))
		page.save()

		let input = lastInput("AssistantSetConfiguration")
		compare(input.apiKeyChange, "keep")
		compare(input.apiKey, "")
		compare(input.presetId, "bedrock")
		compare(input.region, "eu-central-1")
		compare(input.maxOutputTokens, 2048)
		tryCompare(page, "saving", false, 5000)
		verify(page.statusText !== "")
	}

	function test_newKeyIsSentOnceAndCleared(){
		let page = loadedPage(configuration({"hasApiKey": false}))

		findChild(page, "AssistantApiKey").text = "sk-new-key"
		GqlTestTransport.queueResponse("AssistantSetConfiguration", response("AssistantSetConfiguration", {"success": true, "configurationJson": JSON.stringify(configuration({}))}))
		page.save()

		let input = lastInput("AssistantSetConfiguration")
		compare(input.apiKeyChange, "set")
		compare(input.apiKey, "sk-new-key")
		tryCompare(page, "saving", false, 5000)
		compare(findChild(page, "AssistantApiKey").text, "")
	}

	function test_removeKey(){
		let page = loadedPage(configuration({}))

		page.removeApiKey = true
		GqlTestTransport.queueResponse("AssistantSetConfiguration", response("AssistantSetConfiguration", {"success": true, "configurationJson": JSON.stringify(configuration({"hasApiKey": false}))}))
		page.save()

		compare(lastInput("AssistantSetConfiguration").apiKeyChange, "clear")
	}

	function test_refusalIsShown(){
		let page = loadedPage(configuration({}))

		GqlTestTransport.queueResponse("AssistantSetConfiguration", response("AssistantSetConfiguration", {"success": false, "errorMessage": "The provider is not available on this server"}))
		page.save()

		tryCompare(page, "saving", false, 5000)
		compare(page.errorText, "The provider is not available on this server")
	}

	function test_readOnlyWithoutPermission(){
		let page = loadedPage(configuration({}))
		page.canEdit = false

		page.save()

		verify(GqlTestTransport.requestedCommands().indexOf("AssistantSetConfiguration") < 0)
		verify(!findChild(page, "AssistantSave").visible)
	}
}
