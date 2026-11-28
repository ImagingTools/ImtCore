// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtassistgui 1.0

/**
	AssistantSettingsPage

	Page "Assistant" of the administration: the assistant configuration of the organization
	(imtassistgql::CAssistantConfigurationControllerComp). The organization is the one of the
	session; the page reloads when it changes.

	The API key is never shown: the page only knows whether one is stored. A new key is sent
	once when saving; an empty field keeps the stored key.

	AdministrationView adds the page "Assistant" when the application has the assistant
	(AssistantCenter) and the user holds /Administration/Assistant/ViewPage.
*/
Item {
	id: settingsPage

	// Organization of the session; a change reloads the configuration.
	property string tenantId: AuthorizationController.currentTenantId
	property string permissionPath: ""
	// Whether the user may change the configuration; the server checks it again.
	property bool canEdit: PermissionsController.checkPermission("/Administration/Assistant/Edit")
	property color contentColor: Style.baseColor

	// The configuration as loaded: enabled, presetId, model, endpoint, region, project, resource,
	// hasApiKey, maxOutputTokens, additionalInstructions, updatedAt, allowsCustomEndpoint, presets
	property var configuration: ({})
	property var presets: []
	property bool loading: false
	property bool saving: false
	property string errorText: ""
	property string statusText: ""
	property bool removeApiKey: false

	readonly property var selectedPreset: settingsPage.findPreset(presetModel.idAt(providerCB.currentIndex))
	readonly property bool readOnly: !settingsPage.canEdit || settingsPage.loading || settingsPage.saving

	signal configurationLoaded()
	signal configurationSaved()

	Component.onCompleted: {
		settingsPage.load()
	}

	onTenantIdChanged: {
		settingsPage.load()
	}

	onConfigurationSaved: {
		// The button and the panel of the assistant follow the new state
		AssistantCenter.refresh()
	}

	function load(){
		settingsPage.loading = true
		settingsPage.errorText = ""
		settingsPage.statusText = ""
		getSender.send({})
	}

	function findPreset(presetId){
		for (let i = 0; i < settingsPage.presets.length; i++){
			if (settingsPage.presets[i].id === presetId){
				return settingsPage.presets[i]
			}
		}

		return null
	}

	function applyConfiguration(configuration){
		settingsPage.configuration = configuration
		settingsPage.presets = configuration.presets ? configuration.presets : []
		settingsPage.removeApiKey = false

		let items = [{"id": "", "name": qsTr("No provider")}]
		for (let i = 0; i < settingsPage.presets.length; i++){
			items.push({"id": settingsPage.presets[i].id, "name": settingsPage.presets[i].name})
		}

		providerCB.currentIndex = presetModel.fill(items, configuration.presetId ? configuration.presetId : "")
		enabledSwitch.checked = configuration.enabled === true
		modelInput.text = configuration.model ? configuration.model : ""
		endpointInput.text = configuration.endpoint ? configuration.endpoint : ""
		regionInput.text = configuration.region ? configuration.region : ""
		projectInput.text = configuration.project ? configuration.project : ""
		resourceInput.text = configuration.resource ? configuration.resource : ""
		apiKeyInput.text = ""
		maxTokensInput.value = configuration.maxOutputTokens > 0 ? configuration.maxOutputTokens : 1024
		maxTokensInput.text = String(maxTokensInput.value)
		instructionsInput.text = configuration.additionalInstructions ? configuration.additionalInstructions : ""
	}

	function createInput(){
		let apiKeyChange = "keep"
		if (apiKeyInput.text.trim() !== ""){
			apiKeyChange = "set"
		}
		else if (settingsPage.removeApiKey){
			apiKeyChange = "clear"
		}

		let maxTokens = parseInt(maxTokensInput.text)

		return {
			"enabled": enabledSwitch.checked,
			"presetId": presetModel.idAt(providerCB.currentIndex),
			"model": modelInput.text.trim(),
			"endpoint": endpointInput.visible ? endpointInput.text.trim() : "",
			"region": regionInput.visible ? regionInput.text.trim() : "",
			"project": projectInput.visible ? projectInput.text.trim() : "",
			"resource": resourceInput.visible ? resourceInput.text.trim() : "",
			"maxOutputTokens": isNaN(maxTokens) ? 0 : maxTokens,
			"additionalInstructions": instructionsInput.text.trim(),
			"apiKeyChange": apiKeyChange,
			"apiKey": apiKeyChange === "set" ? apiKeyInput.text.trim() : ""
		}
	}

	function save(){
		if (settingsPage.readOnly){
			return
		}

		settingsPage.saving = true
		settingsPage.errorText = ""
		settingsPage.statusText = ""
		setSender.send(settingsPage.createInput())
	}

	function readConfiguration(data){
		if (!data || !data.containsKey("configurationJson")){
			return null
		}

		let configuration = null
		try {
			configuration = JSON.parse(data.getData("configurationJson"))
		}
		catch (error){
			configuration = null
		}

		return configuration
	}

	TreeItemModel {
		id: presetModel

		function fill(items, selectedId){
			presetModel.clear()
			let selectedIndex = -1
			for (let i = 0; i < items.length; i++){
				let index = presetModel.insertNewItem()
				presetModel.setData("id", items[i].id, index)
				presetModel.setData("name", items[i].name, index)
				if (items[i].id === selectedId){
					selectedIndex = index
				}
			}

			return selectedIndex
		}

		function idAt(index){
			if (index < 0 || index >= presetModel.getItemsCount()){
				return ""
			}

			return String(presetModel.getData("id", index))
		}
	}

	GqlRequestSender {
		id: getSender

		requestType: 0
		gqlCommandId: "AssistantGetConfiguration"
		permissionPath: settingsPage.permissionPath

		function createQueryParams(query, params){
			query.AddParam(Gql.GqlObject("input"))
			query.AddField(Gql.GqlObject("configurationJson"))
		}

		function onResult(data){
			settingsPage.loading = false
			let configuration = settingsPage.readConfiguration(data)
			if (configuration === null){
				settingsPage.errorText = qsTr("The configuration could not be read.")

				return
			}

			settingsPage.applyConfiguration(configuration)
			settingsPage.configurationLoaded()
		}

		function onError(message, type){
			settingsPage.loading = false
			settingsPage.errorText = message !== "" ? message : qsTr("The configuration could not be read.")
		}
	}

	GqlRequestSender {
		id: setSender

		requestType: 1
		gqlCommandId: "AssistantSetConfiguration"
		permissionPath: settingsPage.permissionPath

		function createQueryParams(query, params){
			let input = Gql.GqlObject("input")
			input.InsertField("enabled", params.enabled)
			input.InsertField("presetId", params.presetId)
			input.InsertField("model", params.model)
			input.InsertField("endpoint", params.endpoint)
			input.InsertField("region", params.region)
			input.InsertField("project", params.project)
			input.InsertField("resource", params.resource)
			input.InsertField("maxOutputTokens", params.maxOutputTokens)
			input.InsertField("additionalInstructions", params.additionalInstructions)
			input.InsertField("apiKeyChange", params.apiKeyChange)
			input.InsertField("apiKey", params.apiKey)
			query.AddParam(input)
			query.AddField(Gql.GqlObject("success"))
			query.AddField(Gql.GqlObject("errorMessage"))
			query.AddField(Gql.GqlObject("configurationJson"))
		}

		function onResult(data){
			settingsPage.saving = false
			apiKeyInput.text = ""

			let success = data && data.containsKey("success") && data.getData("success") === true
			if (!success){
				let message = data && data.containsKey("errorMessage") ? data.getData("errorMessage") : ""
				settingsPage.errorText = message !== "" ? message : qsTr("The configuration could not be saved.")

				return
			}

			let configuration = settingsPage.readConfiguration(data)
			if (configuration !== null){
				settingsPage.applyConfiguration(configuration)
			}

			settingsPage.statusText = qsTr("The configuration was saved.")
			settingsPage.configurationSaved()
		}

		function onError(message, type){
			settingsPage.saving = false
			apiKeyInput.text = ""
			settingsPage.errorText = message !== "" ? message : qsTr("The configuration could not be saved.")
		}
	}

	Rectangle {
		anchors.fill: parent
		color: settingsPage.contentColor
	}

	CustomScrollbar {
		z: settingsFlickable.z + 1
		anchors.right: parent.right
		anchors.top: settingsFlickable.top
		anchors.bottom: settingsFlickable.bottom
		secondSize: Style.marginM
		targetItem: settingsFlickable
	}

	Flickable {
		id: settingsFlickable

		anchors.fill: parent
		anchors.margins: Style.marginXL
		contentHeight: settingsColumn.height
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: settingsColumn

			width: Math.min(settingsFlickable.width, Style.sizeHintXXL)
			spacing: Style.marginM

			BaseText {
				width: parent.width
				wrapMode: Text.WordWrap
				text: qsTr("The assistant answers questions and prepares changes in this organization. Every change is shown first and applied only after confirmation. The assistant can do only what the user may do.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			GroupElementView {
				width: parent.width

				SwitchElementView {
					id: enabledSwitch
					objectName: "AssistantEnabled"

					name: qsTr("Assistant enabled")
					description: qsTr("The members of the organization see the assistant in the application")
					readOnly: settingsPage.readOnly
				}

				ComboBoxElementView {
					id: providerCB
					objectName: "AssistantProvider"

					name: qsTr("Model provider")
					description: settingsPage.selectedPreset !== null ? settingsPage.selectedPreset.credentialHint : qsTr("The providers allowed on this server")
					model: presetModel
					changeable: !settingsPage.readOnly
				}

				TextInputElementView {
					id: modelInput
					objectName: "AssistantModel"

					name: qsTr("Model")
					placeHolderText: qsTr("Model ID or deployment name")
					maximumLength: 500
					readOnly: settingsPage.readOnly
				}

				TextInputElementView {
					id: regionInput
					objectName: "AssistantRegion"

					visible: settingsPage.selectedPreset !== null && settingsPage.selectedPreset.needsRegion === true
					name: qsTr("Region")
					placeHolderText: qsTr("e.g. eu-central-1")
					maximumLength: 128
					readOnly: settingsPage.readOnly
				}

				TextInputElementView {
					id: projectInput

					visible: settingsPage.selectedPreset !== null && settingsPage.selectedPreset.needsProject === true
					name: qsTr("Project")
					placeHolderText: qsTr("Google Cloud project")
					maximumLength: 128
					readOnly: settingsPage.readOnly
				}

				TextInputElementView {
					id: resourceInput

					visible: settingsPage.selectedPreset !== null && settingsPage.selectedPreset.needsResource === true
					name: qsTr("Resource")
					placeHolderText: qsTr("Azure OpenAI resource name")
					maximumLength: 128
					readOnly: settingsPage.readOnly
				}

				TextInputElementView {
					id: endpointInput
					objectName: "AssistantEndpoint"

					visible: settingsPage.configuration.allowsCustomEndpoint === true
					name: qsTr("Endpoint")
					placeHolderText: qsTr("Empty for the endpoint of the provider")
					maximumLength: 500
					readOnly: settingsPage.readOnly
				}
			}

			GroupElementView {
				width: parent.width

				TextInputElementView {
					id: apiKeyInput
					objectName: "AssistantApiKey"

					name: qsTr("API key")
					description: settingsPage.configuration.hasApiKey === true && !settingsPage.removeApiKey ? qsTr("A key is stored. Leave the field empty to keep it.") : qsTr("No key is stored.")
					placeHolderText: settingsPage.configuration.hasApiKey === true && !settingsPage.removeApiKey ? "********" : qsTr("Enter the key of the provider")
					echoMode: TextInput.Password
					maximumLength: 8192
					readOnly: settingsPage.readOnly
				}

				ButtonElementView {
					id: removeKeyButton
					objectName: "AssistantRemoveApiKey"

					visible: settingsPage.configuration.hasApiKey === true && !settingsPage.removeApiKey
					name: qsTr("Stored API key")
					description: qsTr("Removed when the configuration is saved")
					text: qsTr("Remove")
					buttonEnabled: !settingsPage.readOnly

					onClicked: {
						settingsPage.removeApiKey = true
						apiKeyInput.text = ""
					}
				}
			}

			GroupElementView {
				width: parent.width

				IntegerInputElementView {
					id: maxTokensInput

					name: qsTr("Maximum answer length")
					description: qsTr("Tokens of one answer of the model")
					bottomValue: 1
					topValue: 100000
					readOnly: settingsPage.readOnly
				}

				TextInputElementView {
					id: instructionsInput
					objectName: "AssistantInstructions"

					name: qsTr("Additional instructions")
					description: qsTr("e.g. the language or the tone of the answers")
					maximumLength: 4000
					readOnly: settingsPage.readOnly
				}
			}

			BaseText {
				width: parent.width
				visible: settingsPage.errorText !== ""
				wrapMode: Text.WordWrap
				text: settingsPage.errorText
				color: Style.errorTextColor
			}

			BaseText {
				width: parent.width
				visible: settingsPage.statusText !== ""
				wrapMode: Text.WordWrap
				text: settingsPage.statusText
				color: Style.inactiveTextColor
			}

			Button {
				id: saveButton
				objectName: "AssistantSave"

				visible: settingsPage.canEdit
				enabled: !settingsPage.readOnly
				text: settingsPage.saving ? qsTr("Saving...") : qsTr("Save")

				onClicked: {
					settingsPage.save()
				}
			}
		}
	}
}
