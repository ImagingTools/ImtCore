import QtQuick 2.12
import QtTest 1.12
import imtcontrols 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import imtbaseFileSystemSdl 1.0

TestCase {
	id: testCase

	name: "BaseModel"

	GroupFilter {
		id: groupFilter
	}

	GroupFilter {
		id: restoredFilter
	}

	GetFileSystemEntriesPayload {
		id: entriesPayload
	}

	function createFilters(){
		verify(groupFilter.createFromJson('{"fieldFilters":['
				+ '{"__typename":"FieldFilter","fieldId":"name","filterValue":"a\\"b\\\\c\\nd\\te","filterValueType":"String","filterOperations":["Contains","Not"]},'
				+ '{"__typename":"ArrayFieldFilter","fieldId":"tags","filterValues":["x\\"y","line\\nnext"],"filterValueType":"String","filterOperations":["ArrayHasAny"]}],'
				+ '"logicalOperation":"And"}'))

		return groupFilter.m_fieldFilters
	}

	function test_toGraphQL(){
		let model = createFilters()

		compare(model.toGraphQL(), '[{__typename:"FieldFilter",fieldId:"name",filterValue:"a\\"b\\\\c\\nd\\te",filterValueType:"String",filterOperations:["Contains", "Not"]},'
				+ '{__typename:"ArrayFieldFilter",fieldId:"tags",filterValues:["x\\"y", "line\\nnext"],filterValueType:"String",filterOperations:["ArrayHasAny"]}]')
	}

	function test_toJsonRoundTrip(){
		let model = createFilters()
		let json = model.toJson()

		let parsed = JSON.parse(json)
		compare(parsed[0].filterValue, 'a"b\\c\nd\te')
		compare(parsed[1].filterValues, ['x"y', 'line\nnext'])

		verify(restoredFilter.createFromJson('{"fieldFilters":' + json + ',"logicalOperation":"Or"}'))
		compare(restoredFilter.m_fieldFilters.toJson(), json)
	}

	function test_optionalNullIsSkipped(){
		verify(groupFilter.createFromJson('{"groupFilters":[{"fieldFilters":[],"groupFilters":[],"logicalOperation":"Or"}],"logicalOperation":"And"}'))
		let model = groupFilter.m_groupFilters
		model.get(0).item.m_fieldFilters = null

		compare(model.toGraphQL(), '[{__typename:"GroupFilter",groupFilters:[],logicalOperation:"Or"}]')
		compare(JSON.parse(model.toJson()), [{__typename: "GroupFilter", groupFilters: [], logicalOperation: "Or"}])
	}

	function test_nullElement(){
		let model = createFilters()
		model.append({item: null})

		verify(model.hasNullElements())
		verify(model.toGraphQL().endsWith(",null]"))
		compare(JSON.parse(model.toJson())[2], null)
	}

	function test_requiredArrayOfItemIsValidated(){
		let model = createFilters()
		model.get(1).item.m_filterOperations = null

		compare(model.toGraphQL(), "")
		compare(model.toJson(), "")
		compare(groupFilter.toGraphQL(), "")
	}

	function test_requiredModelElementsAreValidated(){
		verify(entriesPayload.createFromJson('{"path":"C:/","entries":[{"name":"a","path":"C:/a","entryType":"File"}],"totalCount":1,"hasMore":false}'))
		verify(entriesPayload.toGraphQL() !== "")

		entriesPayload.m_entries.append({item: null})

		compare(entriesPayload.toGraphQL(), "")
		compare(entriesPayload.toJson(), "")
	}

	function test_emptyModel(){
		verify(groupFilter.createFromJson('{"fieldFilters":[],"logicalOperation":"And"}'))

		compare(groupFilter.m_fieldFilters.toGraphQL(), "[]")
		compare(groupFilter.m_fieldFilters.toJson(), "[]")
	}
}
