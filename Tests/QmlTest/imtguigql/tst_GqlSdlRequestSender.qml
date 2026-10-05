import QtQuick 2.12
import QtTest 1.12
import imtguigql 1.0
import imtqmltest 1.0
import imtbaseFileSystemSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

TestCase {
	id: testCase

	name: "GqlSdlRequestSender"

	SignalSpy {
		id: finishedSpy

		signalName: "finished"
	}

	GqlSdlRequestSender {
		id: measurementSender

		gqlCommandId: "GetMeasurementViewData"

		function getRequestedFields(){
			let fields = Gql.GqlObject("MeasurementViewData")
			fields.InsertField("colorantList")
			fields.InsertField("ppc")
			fields.InsertField("details")

			return fields
		}
	}

	GetFileSystemEntriesInput {
		id: fileSystemInput
	}

	GqlSdlRequestSender {
		id: fileSystemSender

		property string errorMessage
		property string errorType

		gqlCommandId: ImtbaseFileSystemSdlCommandIds.s_getFileSystemEntries

		sdlObjectComp: Component {
			GetFileSystemEntriesPayload {
			}
		}

		function getRequestedFields(){
			let payload = Gql.GqlObject("payload")
			payload.InsertField("path")

			let entries = Gql.GqlObject("entries")
			entries.InsertField("name")
			entries.InsertField("entryType")
			payload.InsertFieldObject(entries)

			payload.InsertField("totalCount")

			return payload
		}

		function onError(message, type){
			fileSystemSender.errorMessage = message
			fileSystemSender.errorType = type
			fileSystemSender.finished(-1)
		}
	}

	GqlSdlRequestSender {
		id: arraySender

		requestType: 1
		gqlCommandId: "SaveValues"

		function getRequestedFields(){
			let fields = Gql.GqlObject("")
			fields.InsertField("id")
			fields.InsertField("name")

			return fields
		}
	}

	ComplexCollectionFilter {
		id: complexFilter
	}

	FieldFilter {
		id: invalidFieldFilter
	}

	GqlSdlRequestSender {
		id: filterSender

		property string errorMessage

		gqlCommandId: "FilterItems"

		function getRequestedFields(){
			let address = Gql.GqlObject("address")
			address.InsertField("city")
			let contact = Gql.GqlObject("contact")
			contact.InsertFieldObject(address)
			let owner = Gql.GqlObject("owner")
			owner.InsertFieldObject(contact)
			let info = Gql.GqlObject("info")
			info.InsertFieldObject(owner)

			let items = Gql.GqlObject("items")
			items.InsertField("id")
			items.InsertField("name")
			items.InsertFieldObject(info)

			let notification = Gql.GqlObject("notification")
			notification.InsertField("text")
			notification.InsertField("level")

			let payload = Gql.GqlObject("payload")
			payload.InsertFieldObject(items)
			payload.InsertField("totalCount")
			payload.InsertFieldObject(notification)

			return payload
		}

		function onError(message, type){
			filterSender.errorMessage = message
			filterSender.finished(-1)
		}
	}

	GqlSdlRequestSender {
		id: inputSourceSender

		gqlCommandId: "Inputs"

		inputObjectComp: Component {
			TextFilter {
				m_text: "from component"
			}
		}
	}

	TextFilter {
		id: sdlInput

		m_text: "from argument"
		m_fieldIds: ["a"]
	}

	GqlSdlRequestSender {
		id: paramsSender

		gqlCommandId: "Inputs"
	}

	GqlSdlRequestSender {
		id: subscriptionSender

		requestType: 2
		gqlCommandId: "OnItemChanged"

		function getRequestedFields(){
			let fields = Gql.GqlObject("payload")
			fields.InsertField("itemId")

			return fields
		}
	}

	GqlSdlRequestSender {
		id: invalidTypeSender

		requestType: 5
		gqlCommandId: "Invalid"
	}

	GqlSdlRequestSender {
		id: invalidCommandSender
	}

	GqlSdlRequestSender {
		id: headersSender

		gqlCommandId: "WithHeaders"
		context: "context-1"

		function getHeaders(){
			let headers = {}
			headers["X-Custom"] = "custom value"

			return headers
		}
	}

	GqlSdlRequestSender {
		id: pingSender

		gqlCommandId: "Ping"
	}

	function init(){
		GqlTestTransport.reset()
	}

	function startSending(sender){
		finishedSpy.target = sender
		finishedSpy.clear()
	}

	function waitFinished(expectedStatus){
		if (finishedSpy.count === 0){
			finishedSpy.wait()
		}

		compare(finishedSpy.count, 1)
		compare(finishedSpy.signalArguments[0][0], expectedStatus)
	}

	function compareSentQuery(expectedQuery){
		compare(GqlTestTransport.requestCount, 1)
		compare(GqlTestTransport.lastRequestBody, '{"query":' + JSON.stringify(expectedQuery) + '}')
	}

	function parseSentQuery(){
		let request = GqlTestTransport.parseLastRequest()
		verify(request.parsed, "server-side parser rejected: " + GqlTestTransport.lastRequestBody)

		return request
	}

	function test_requestedFieldsContainerIsNotSelected(){
		startSending(measurementSender)
		measurementSender.addInputParam("__typename", "MeasurementViewDataRequest")
		measurementSender.addInputParam("id", "d5531383-f90a-418b-900a-9dc7e086f327")
		measurementSender.addInputParam("colorants", [])
		measurementSender.addInputParam("requestType", "ALL")
		measurementSender.addInputParam("tonalValueCalcParams", null)
		measurementSender.addInputParam("isNewMeasurement", false)

		measurementSender.send()

		compareSentQuery('query GetMeasurementViewData {GetMeasurementViewData(input: {__typename: "MeasurementViewDataRequest", id: "d5531383-f90a-418b-900a-9dc7e086f327", colorants: [], requestType: "ALL", tonalValueCalcParams: null, isNewMeasurement: false}) {colorantList ppc details}}')

		let request = parseSentQuery()
		compare(request.requestType, "query")
		compare(request.commandId, "GetMeasurementViewData")
		compare(request.fields, {colorantList: true, ppc: true, details: true})
		compare(request.params.input.id, "d5531383-f90a-418b-900a-9dc7e086f327")
		compare(request.params.input.colorants, [])
		compare(request.params.input.requestType, "ALL")
		compare(request.params.input.isNewMeasurement, false)
		compare(request.params.input.tonalValueCalcParams, null)

		waitFinished(-1)
	}

	function test_nestedRequestedFieldsWithSdlInput(){
		startSending(fileSystemSender)
		GqlTestTransport.responseBody = '{"data":{"GetFileSystemEntries":{"path":"C:/Data","entries":[{"name":"a.txt","entryType":"File"},{"name":"b","entryType":"Directory"}],"totalCount":2}}}'

		fileSystemInput.m_path = 'C:\\Data "x"\nnext'
		fileSystemInput.m_offset = 10
		fileSystemInput.m_limit = 20

		fileSystemSender.send(fileSystemInput)

		compareSentQuery('query GetFileSystemEntries {GetFileSystemEntries(input: {__typename: "GetFileSystemEntriesInput", path: "C:\\\\Data \\"x\\"\\nnext", offset: 10, limit: 20, nameFilter: "", extensionFilter: "", sortBy: "", sortAscending: false}) {path entries {name entryType} totalCount}}')

		let request = parseSentQuery()
		compare(request.params.input.path, 'C:\\Data "x"\nnext')
		compare(request.params.input.offset, 10)
		compare(request.params.input.sortAscending, false)
		compare(request.fields, {path: true, entries: {name: true, entryType: true}, totalCount: true})

		waitFinished(1)

		let payload = fileSystemSender.sdlObject
		compare(payload.m_path, "C:/Data")
		compare(payload.m_totalCount, 2)
		compare(payload.m_entries.count, 2)
		compare(payload.m_entries.get(1).item.m_name, "b")
		compare(payload.m_entries.get(1).item.m_entryType, "Directory")
	}

	function test_anonymousContainerAndArrays(){
		startSending(arraySender)

		let firstItem = Gql.GqlObject("item")
		firstItem.InsertField("id", "a")

		arraySender.addInputParam("names", ['quote"', 'C:\\temp', 'line\nnext'])
		arraySender.addInputParam("items", [firstItem, null])
		arraySender.addInputParam("filter", null)

		arraySender.send()

		compareSentQuery('mutation SaveValues {SaveValues(input: {names: ["quote\\"", "C:\\\\temp", "line\\nnext"], items: [{id: "a"}, null], filter: null}) {id name}}')

		let request = parseSentQuery()
		compare(request.requestType, "mutation")
		compare(request.params.input.names, ['quote"', 'C:\\temp', 'line\nnext'])
		compare(request.params.input.items, [{id: "a"}, null])
		compare(request.fields, {id: true, name: true})

		waitFinished(-1)
	}

	function test_resendDoesNotAccumulate(){
		startSending(arraySender)
		arraySender.send()
		waitFinished(-1)
		let firstBody = GqlTestTransport.lastRequestBody

		startSending(arraySender)
		arraySender.send()
		waitFinished(-1)

		compare(GqlTestTransport.requestCount, 2)
		compare(GqlTestTransport.lastRequestBody, firstBody)
	}

	function test_complexSdlFilter(){
		startSending(filterSender)
		verify(complexFilter.createFromJson('{"sortingInfo":[{"fieldId":"name","sortingOrder":"ASC"},{"fieldId":"date","sortingOrder":"DESC"}],'
				+ '"fieldsFilter":{"fieldFilters":['
				+ '{"__typename":"FieldFilter","fieldId":"name","filterValue":"O\'Brien \\"Jr\\" \\\\ {x}","filterValueType":"String","filterOperations":["Contains","Not"]},'
				+ '{"__typename":"ArrayFieldFilter","fieldId":"tags","filterValues":["a","b, c"],"filterValueType":"String","filterOperations":["ArrayHasAny"]}],'
				+ '"groupFilters":[{"fieldFilters":[{"__typename":"FieldFilter","fieldId":"age","filterValue":"18","filterValueType":"Integer","filterOperations":["Greater"]}],"groupFilters":[],"logicalOperation":"Or"}],'
				+ '"logicalOperation":"And"},'
				+ '"textFilter":{"text":"Привет (мир)","fieldIds":["name","description"]},'
				+ '"distinctFields":["status"]}'))

		filterSender.send(complexFilter)

		compareSentQuery('query FilterItems {FilterItems(input: {__typename: "ComplexCollectionFilter", '
				+ 'sortingInfo: [{__typename:"FieldSortingInfo",fieldId:"name",sortingOrder:"ASC"},{__typename:"FieldSortingInfo",fieldId:"date",sortingOrder:"DESC"}], '
				+ 'fieldsFilter: {__typename:"GroupFilter",fieldFilters:['
				+ '{__typename:"FieldFilter",fieldId:"name",filterValue:"O\'Brien \\"Jr\\" \\\\ {x}",filterValueType:"String",filterOperations:["Contains", "Not"]},'
				+ '{__typename:"ArrayFieldFilter",fieldId:"tags",filterValues:["a", "b, c"],filterValueType:"String",filterOperations:["ArrayHasAny"]}],'
				+ 'groupFilters:[{__typename:"GroupFilter",fieldFilters:[{__typename:"FieldFilter",fieldId:"age",filterValue:"18",filterValueType:"Integer",filterOperations:["Greater"]}],groupFilters:[],logicalOperation:"Or"}],'
				+ 'logicalOperation:"And"}, '
				+ 'timeFilter: null, '
				+ 'textFilter: {__typename:"TextFilter",text:"Привет (мир)",fieldIds:["name", "description"]}, '
				+ 'distinctFields: ["status"]}) '
				+ '{items {id name info {owner {contact {address {city}}}}} totalCount notification {text level}}}')

		let request = parseSentQuery()
		compare(request.fields, {items: {id: true, name: true, info: {owner: {contact: {address: {city: true}}}}}, totalCount: true, notification: {text: true, level: true}})
		compare(request.params.input, {
			__typename: "ComplexCollectionFilter",
			sortingInfo: [
				{__typename: "FieldSortingInfo", fieldId: "name", sortingOrder: "ASC"},
				{__typename: "FieldSortingInfo", fieldId: "date", sortingOrder: "DESC"}
			],
			fieldsFilter: {
				__typename: "GroupFilter",
				fieldFilters: [
					{__typename: "FieldFilter", fieldId: "name", filterValue: 'O\'Brien "Jr" \\ {x}', filterValueType: "String", filterOperations: ["Contains", "Not"]},
					{__typename: "ArrayFieldFilter", fieldId: "tags", filterValues: ["a", "b, c"], filterValueType: "String", filterOperations: ["ArrayHasAny"]}
				],
				groupFilters: [
					{
						__typename: "GroupFilter",
						fieldFilters: [{__typename: "FieldFilter", fieldId: "age", filterValue: "18", filterValueType: "Integer", filterOperations: ["Greater"]}],
						groupFilters: [],
						logicalOperation: "Or"
					}
				],
				logicalOperation: "And"
			},
			timeFilter: null,
			textFilter: {__typename: "TextFilter", text: "Привет (мир)", fieldIds: ["name", "description"]},
			distinctFields: ["status"]
		})

		waitFinished(-1)
	}

	function test_invalidNestedSdlIsNotSent(){
		verify(complexFilter.createFromJson('{"fieldsFilter":{"fieldFilters":[{"__typename":"FieldFilter","fieldId":"name","filterValue":"x","filterValueType":"String","filterOperations":["Equal"]}],"logicalOperation":"And"}}'))
		complexFilter.m_fieldsFilter.m_fieldFilters.get(0).item.m_filterOperations = null

		startSending(filterSender)
		filterSender.errorMessage = ""
		filterSender.send(complexFilter)

		compare(GqlTestTransport.requestCount, 0)
		compare(filterSender.errorMessage, "Unable to create GraphQL value of the field 'fieldsFilter'")
		waitFinished(-1)
	}

	function test_invalidSdlInputIsNotSent(){
		invalidFieldFilter.m_fieldId = "name"
		invalidFieldFilter.m_filterOperations = null

		startSending(filterSender)
		filterSender.errorMessage = ""
		filterSender.send(invalidFieldFilter)

		compare(GqlTestTransport.requestCount, 0)
		compare(filterSender.errorMessage, "Invalid array value of the field 'filterOperations'")
		waitFinished(-1)
	}

	function test_unsupportedValueIsNotSent(){
		startSending(paramsSender)
		paramsSender.addInputParam("createdAt", new Date(0))

		paramsSender.send()

		compare(GqlTestTransport.requestCount, 0)
		waitFinished(-1)

		paramsSender.inputParams.RemoveField("createdAt")
	}

	function test_inputSources(){
		startSending(inputSourceSender)
		inputSourceSender.send(sdlInput)
		compareSentQuery('query Inputs {Inputs(input: {__typename: "TextFilter", text: "from argument", fieldIds: ["a"]}) {}}')
		waitFinished(-1)

		GqlTestTransport.reset()
		startSending(inputSourceSender)
		inputSourceSender.send()
		compareSentQuery('query Inputs {Inputs(input: {__typename: "TextFilter", text: "from component", fieldIds: null}) {}}')
		compare(parseSentQuery().params.input.text, "from component")
		waitFinished(-1)

		GqlTestTransport.reset()
		startSending(paramsSender)
		paramsSender.send()
		compareSentQuery('query Inputs {Inputs(input: {}) {}}')
		compare(parseSentQuery().params.input, {})
		waitFinished(-1)
	}

	function test_subscription(){
		startSending(subscriptionSender)
		subscriptionSender.send()

		compareSentQuery('subscription OnItemChanged {OnItemChanged(input: {}) {itemId}}')

		let request = parseSentQuery()
		compare(request.requestType, "subscription")
		compare(request.fields, {itemId: true})
		waitFinished(-1)
	}

	function test_invalidConfigurationIsReported(){
		startSending(invalidTypeSender)
		invalidTypeSender.send()
		compare(GqlTestTransport.requestCount, 0)
		waitFinished(-1)

		startSending(invalidCommandSender)
		invalidCommandSender.send()
		compare(GqlTestTransport.requestCount, 0)
		waitFinished(-1)
	}

	function test_headers(){
		startSending(headersSender)
		headersSender.send()

		compareSentQuery('query WithHeaders {WithHeaders(input: {}) {}}')
		compare(GqlTestTransport.lastRequestHeaders["x-custom"], "custom value")
		compare(GqlTestTransport.lastRequestHeaders["context"], "context-1")
		compare(GqlTestTransport.lastRequestHeaders["content-type"], "application/x-www-form-urlencoded")
		waitFinished(-1)

		GqlTestTransport.reset()
		startSending(pingSender)
		pingSender.send()

		compareSentQuery('query Ping {Ping(input: {}) {}}')
		verify(!("context" in GqlTestTransport.lastRequestHeaders))
		waitFinished(-1)
	}

	function test_responses_data(){
		let noDataMessage = "The server returned no data. Please try again."
		let entries = '{"path":"C:/","entries":[],"totalCount":0}'

		return [
			{tag: "data", status: 200, body: '{"data":{"GetFileSystemEntries":' + entries + '}}', result: 1, message: "", type: ""},
			{tag: "graphql error with type", status: 200, body: '{"errors":[{"message":"Access denied","extensions":{"type":"Warning"}}]}', result: -1, message: "Access denied", type: "Warning"},
			{tag: "graphql error without message", status: 200, body: '{"errors":[{"extensions":null}]}', result: -1, message: "", type: ""},
			{tag: "empty errors", status: 200, body: '{"errors":[]}', result: -1, message: "Unknown error", type: "Error"},
			{tag: "errors win over data", status: 200, body: '{"data":{"GetFileSystemEntries":' + entries + '},"errors":[{"message":"partial"}]}', result: -1, message: "partial", type: ""},
			{tag: "null command result", status: 200, body: '{"data":{"GetFileSystemEntries":null}}', result: -1, message: noDataMessage, type: ""},
			{tag: "result of another command", status: 200, body: '{"data":{"Other":' + entries + '}}', result: -1, message: noDataMessage, type: ""},
			{tag: "null data", status: 200, body: '{"data":null}', result: -1, message: noDataMessage, type: ""},
			{tag: "neither data nor errors", status: 200, body: '{}', result: -1, message: noDataMessage, type: ""},
			{tag: "invalid json", status: 200, body: '{"data":', result: -1, message: "Unable to read the server response", type: ""},
			{tag: "json null", status: 200, body: 'null', result: -1, message: "Unable to read the server response", type: ""},
			{tag: "empty body", status: 200, body: '', result: -1, message: "Network error", type: ""},
			{tag: "server error", status: 500, body: '{"data":{"GetFileSystemEntries":' + entries + '}}', result: -1, message: "Network error", type: ""},
			{tag: "not found", status: 404, body: '', result: -1, message: "Network error", type: ""},
			{tag: "unauthorized", status: 401, body: '', result: -1, message: "", type: ""},
			{tag: "forbidden", status: 403, body: '', result: -1, message: "", type: ""}
		]
	}

	function test_responses(data){
		GqlTestTransport.responseStatus = data.status
		GqlTestTransport.responseBody = data.body
		fileSystemSender.errorMessage = ""
		fileSystemSender.errorType = ""

		startSending(fileSystemSender)
		fileSystemSender.send(fileSystemInput)
		waitFinished(data.result)

		compare(fileSystemSender.errorMessage, data.message)
		compare(fileSystemSender.errorType, data.type)
	}
}
