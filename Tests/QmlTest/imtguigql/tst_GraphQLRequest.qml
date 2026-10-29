import QtQuick 2.12
import QtTest 1.12
import imtguigql 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "GraphQLRequest"

	QtObject {
		id: emptyGraphQlValue

		function toGraphQL(){
			return ""
		}
	}

	QtObject {
		id: numberGraphQlValue

		function toGraphQL(){
			return 5
		}
	}

	QtObject {
		id: literalGraphQlValue

		function toGraphQL(){
			return '{id:"x",tags:["a"]}'
		}
	}

	function createBody(query){
		return '{"query":' + JSON.stringify(query) + '}'
	}

	function createValueRequest(value){
		let request = Gql.GqlRequest("query", "Values")
		request.AddField(Gql.GqlObject("id"))

		let input = Gql.GqlObject("input")
		input.InsertField("value", value)
		request.AddParam(input)

		return request
	}

	// spec: {field: true | {nested spec}}; "" is an anonymous container
	function createFields(name, spec){
		let fields = Gql.GqlObject(name)
		for (let key in spec){
			if (spec[key] === true){
				fields.InsertField(key)
			}
			else{
				fields.InsertFieldObject(createFields(key, spec[key]))
			}
		}

		return fields
	}

	function parse(body){
		let request = GqlTestTransport.parseRequest(body)
		verify(request.parsed, "server-side parser rejected: " + body)

		return request
	}

	function test_values_data(){
		return [
			{tag: "empty string", value: "", literal: '""', serverValue: ""},
			{tag: "quote and backslash", value: 'a"b\\c', literal: '"a\\"b\\\\c"', serverValue: 'a"b\\c'},
			{tag: "escape sequence as text", value: 'line\\nnext \\u0041', literal: '"line\\\\nnext \\\\u0041"', serverValue: 'line\\nnext \\u0041'},
			{tag: "control characters", value: "\t\r\n\b\f", literal: '"\\t\\r\\n\\b\\f"', serverValue: "\t\r\n\b\f"},
			{tag: "graphql syntax inside of string", value: '{a} (b) [c], d: e ... on X #1', literal: '"{a} (b) [c], d: e ... on X #1"', serverValue: '{a} (b) [c], d: e ... on X #1'},
			{tag: "unicode", value: "Привет, 世界 \u00e9 😀", literal: '"Привет, 世界 \u00e9 😀"', serverValue: "Привет, 世界 \u00e9 😀"},
			{tag: "zero", value: 0, literal: "0", serverValue: 0},
			{tag: "negative zero", value: -0, literal: "0", serverValue: 0},
			{tag: "negative integer", value: -42, literal: "-42", serverValue: -42},
			{tag: "float", value: 3.25, literal: "3.25", serverValue: 3.25},
			{tag: "negative float", value: -0.5, literal: "-0.5", serverValue: -0.5},
			{tag: "max safe integer", value: 9007199254740991, literal: "9007199254740991", serverValue: 9007199254740991},
			{tag: "large exponent", value: 1e21, literal: "1e+21", serverValue: 1e21},
			{tag: "small exponent", value: 1e-7, literal: "1e-7", serverValue: 1e-7},
			{tag: "NaN", value: NaN, literal: "null", serverValue: null},
			{tag: "infinity", value: -Infinity, literal: "null", serverValue: null},
			{tag: "true", value: true, literal: "true", serverValue: true},
			{tag: "false", value: false, literal: "false", serverValue: false},
			{tag: "null", value: null, literal: "null", serverValue: null},
			{tag: "undefined", value: undefined, literal: "null", serverValue: null},
			{tag: "empty array", value: [], literal: "[]", serverValue: []},
			{tag: "strings with separators", value: ["a,b", "[x]", "}", ""], literal: '["a,b", "[x]", "}", ""]', serverValue: ["a,b", "[x]", "}", ""]},
			{tag: "strings and nulls", value: ["a", null, undefined, "b"], literal: '["a", null, null, "b"]', serverValue: ["a", null, null, "b"]},
			{tag: "numbers and booleans", value: [1, -2.5, true, false], literal: "[1, -2.5, true, false]", serverValue: [1, -2.5, true, false]},
			{tag: "exponents", value: [1e21, -3e-7], literal: "[1e+21, -3e-7]", serverValue: [1e21, -3e-7]},
			{tag: "objects and nulls", value: JSON.parse('[{"id":1},null,{"id":2,"tags":["x"]}]'), literal: '[{id: 1}, null, {id: 2, tags: ["x"]}]', serverValue: [{id: 1}, null, {id: 2, tags: ["x"]}]},
			{tag: "empty object", value: JSON.parse('{}'), literal: "{}", serverValue: {}},
			{tag: "deep plain object", value: JSON.parse('{"a":{"b":{"c":{"d":[{"e":"f"},{"e":"g"}]}}},"g":null}'), literal: '{a: {b: {c: {d: [{e: "f"}, {e: "g"}]}}}, g: null}', serverValue: {a: {b: {c: {d: [{e: "f"}, {e: "g"}]}}}, g: null}},
			{tag: "serialized model", value: literalGraphQlValue, literal: '{id:"x",tags:["a"]}', serverValue: {id: "x", tags: ["a"]}},
			{tag: "serialized models in array", value: [literalGraphQlValue, null], literal: '[{id:"x",tags:["a"]}, null]', serverValue: [{id: "x", tags: ["a"]}, null]}
		]
	}

	function test_values(data){
		let body = createValueRequest(data.value).GetQuery()
		compare(body, createBody("query Values {Values(input: {value: " + data.literal + "}) {id}}"))

		let request = parse(body)
		compare(request.params.input.value, data.serverValue)
	}

	function test_unsupportedValues_data(){
		return [
			{tag: "date", value: new Date(0), error: "Unsupported GraphQL value of the field 'value'"},
			{tag: "function", value: function(){}, error: "Unsupported GraphQL value of the field 'value'"},
			{tag: "qml object", value: testCase, error: "Unsupported GraphQL value of the field 'value'"},
			{tag: "unsupported array element", value: [1, [new Date(0)]], error: "Unsupported GraphQL value of the field 'value'"},
			{tag: "unsupported plain object member", value: [JSON.parse('{"id":1}'), {created: new Date(0)}], error: "Unsupported GraphQL value of the field 'created'"},
			{tag: "empty serialized model", value: emptyGraphQlValue, error: "Unable to create GraphQL value of the field 'value'"},
			{tag: "serialized model is not a string", value: numberGraphQlValue, error: "Unable to create GraphQL value of the field 'value'"}
		]
	}

	function test_unsupportedValues(data){
		let request = createValueRequest(data.value)
		let errorMessage = ""
		try {
			request.GetQuery()
		}
		catch (error){
			errorMessage = error.message
		}

		compare(errorMessage, data.error)
	}

	// SDL has no lists of lists, the server parser does not accept them
	function test_nestedArrays(){
		compare(createValueRequest([[1, 2], [], ["a"]]).GetQuery(), createBody('query Values {Values(input: {value: [[1, 2], [], ["a"]]}) {id}}'))
	}

	function test_selections_data(){
		return [
			{
				tag: "scalars",
				spec: {id: true, name: true},
				selection: "id name",
				serverFields: {id: true, name: true}
			},
			{
				tag: "six levels",
				spec: {items: {info: {owner: {contact: {address: {city: true, geo: {lat: true, lon: true}}}}}}, total: true},
				selection: "items {info {owner {contact {address {city geo {lat lon}}}}}} total",
				serverFields: {items: {info: {owner: {contact: {address: {city: true, geo: {lat: true, lon: true}}}}}}, total: true}
			},
			{
				tag: "object without fields is a scalar",
				spec: {payload: {}, name: true},
				selection: "payload name",
				serverFields: {payload: true, name: true}
			},
			{
				tag: "anonymous containers are inlined",
				spec: {"": {id: true, "": {name: true}}, items: {"": {id: true}, total: true}},
				selection: "id name items {id total}",
				serverFields: {id: true, name: true, items: {id: true, total: true}}
			},
			{
				tag: "field named as command",
				spec: {Report: {title: true}, summary: true},
				selection: "Report {title} summary",
				serverFields: {Report: {title: true}, summary: true}
			},
			{
				tag: "names of object members",
				spec: {__typename: true, constructor: true, toString: {valueOf: true}},
				selection: "__typename constructor toString {valueOf}",
				serverFields: {__typename: true, constructor: true, toString: {valueOf: true}}
			},
			{
				tag: "empty container",
				spec: {},
				selection: "",
				serverFields: {}
			}
		]
	}

	function test_selections(data){
		let request = Gql.GqlRequest("query", "Values")
		request.AddField(createFields("", data.spec))

		let body = request.GetQuery()
		compare(body, createBody("query Values {Values {" + data.selection + "}}"))

		// Own members like toString shadow Object.prototype and break QTest's value formatting
		compare(canonical(parse(body).fields), canonical(data.serverFields))
	}

	// The server side map is sorted by key, so the order of the fields is not compared
	function canonical(fields){
		let keys = Object.keys(fields).sort()
		let parts = []
		for (let i = 0; i < keys.length; ++i){
			let value = fields[keys[i]]
			parts.push(JSON.stringify(keys[i]) + ":" + (value === true ? "true" : canonical(value)))
		}

		return "{" + parts.join(",") + "}"
	}

	function test_manyFields(){
		let fields = Gql.GqlObject("items")
		let names = []
		for (let i = 0; i < 200; ++i){
			fields.InsertField("field" + i)
			names.push("field" + i)
		}

		let request = Gql.GqlRequest("query", "Values")
		request.AddField(fields)

		let body = request.GetQuery()
		compare(body, createBody("query Values {Values {items {" + names.join(" ") + "}}}"))
		compare(Object.keys(parse(body).fields.items).length, 200)
	}

	function test_params(){
		let request = Gql.GqlRequest("mutation", "Save")
		request.AddField(Gql.GqlObject("id"))

		let input = Gql.GqlObject("input")
		input.InsertField("name", "n")
		input.InsertField("unset")

		let level3 = Gql.GqlObject("level3")
		level3.InsertField("value", [1, 2])
		let level2 = Gql.GqlObject("level2")
		level2.InsertFieldObject(level3)
		let level1 = Gql.GqlObject("level1")
		level1.InsertFieldObject(level2)
		input.InsertFieldObject(level1)
		input.InsertFieldObject(Gql.GqlObject("empty"))
		request.AddParam(input)

		let options = Gql.GqlObject("")
		options.InsertField("dryRun", true)
		options.InsertField("limit", 5)
		request.AddParam(options)
		request.AddParam(Gql.GqlObject(""))

		let body = request.GetQuery()
		compare(body, createBody('mutation Save {Save(input: {name: "n", unset: null, level1: {level2: {level3: {value: [1, 2]}}}, empty: {}}, dryRun: true, limit: 5) {id}}'))

		let parsed = parse(body)
		compare(parsed.requestType, "mutation")
		compare(parsed.params.input.unset, null)
		compare(parsed.params.input.level1.level2.level3.value, [1, 2])
		compare(parsed.params.input.empty, {})
		compare(parsed.params.dryRun, true)
		compare(parsed.params.limit, 5)
	}

	function test_requestTypes_data(){
		return [
			{tag: "query", requestType: "query"},
			{tag: "mutation", requestType: "mutation"},
			{tag: "subscription", requestType: "subscription"}
		]
	}

	function test_requestTypes(data){
		let request = Gql.GqlRequest(data.requestType, "Changed")
		request.AddField(Gql.GqlObject("id"))

		let body = request.GetQuery()
		compare(body, createBody(data.requestType + " Changed {Changed {id}}"))

		let parsed = parse(body)
		compare(parsed.requestType, data.requestType)
		compare(parsed.commandId, "Changed")
	}

	function test_withoutFieldsAndParams(){
		let body = Gql.GqlRequest("query", "Ping").GetQuery()

		compare(body, createBody("query Ping {Ping {}}"))
		compare(parse(body).fields, {})
	}

	function test_objectEditing(){
		let object = Gql.GqlObject("input")
		object.InsertField("first", 1)
		object.InsertField("second", 2)
		object.InsertField("third")

		object.InsertField("first", 10)
		object.InsertField("second")
		compare(object.GetFieldIds(), ["first", "second", "third"])
		compare(object.GetFieldArgumentValue("first"), 10)
		compare(object.GetFieldArgumentValue("second"), 2)
		compare(object.GetFieldArgumentValue("third"), null)

		let nested = Gql.GqlObject("second")
		nested.InsertField("value", "x")
		object.InsertFieldObject(nested)
		compare(object.GetFieldIds(), ["first", "second", "third"])
		verify(object.IsObject("second"))
		compare(object.GetFieldArgumentObjectPtr("second"), nested)
		compare(nested.m_parentPtr, object)

		object.InsertField("second", 3)
		verify(!object.IsObject("second"))

		let ids = object.GetFieldIds()
		ids.push("external")
		compare(object.GetFieldIds().length, 3)

		object.RemoveField("first")
		object.RemoveField("missing")
		compare(object.GetFieldIds(), ["second", "third"])
		verify(!object.HasField("first"))
		compare(object.GetFieldArgumentValue("first"), null)
		compare(object.GetFieldArgumentObjectPtr("first"), null)

		object.Clear()
		compare(object.GetFieldIds(), [])
	}

	function test_requestEditing(){
		let request = Gql.GqlRequest("query", "First")
		let id = Gql.GqlObject("id")
		let name = Gql.GqlObject("name")
		let input = Gql.GqlObject("input")
		input.InsertField("value", 1)

		request.AddField(id)
		request.AddField(name)
		request.AddParam(input)
		request.RemoveField(id)
		request.RemoveField(Gql.GqlObject("id"))
		request.SetCommandId("Second")
		request.SetRequestType("mutation")

		compare(request.GetCommandId(), "Second")
		compare(request.GetRequestType(), "mutation")
		compare(request.GetQuery(), createBody("mutation Second {Second(input: {value: 1}) {name}}"))

		request.RemoveParam(input)
		compare(request.GetQuery(), createBody("mutation Second {Second {name}}"))

		request.Clear()
		compare(request.GetQuery(), createBody("mutation Second {Second {}}"))
	}

	function test_fromJson(){
		let input = Gql.GqlObject("input")
		input.fromJson('{"text":"a\\"b","count":3,"empty":null,"nested":{"items":[{"id":1},null],"flags":[true,false],"deep":{"x":"y"}}}')

		compare(input.GetFieldIds(), ["text", "count", "empty", "nested"])
		verify(input.IsObject("nested"))
		verify(input.GetFieldArgumentObjectPtr("nested").IsObject("deep"))

		let request = Gql.GqlRequest("query", "Values")
		request.AddParam(input)

		let body = request.GetQuery()
		compare(body, createBody('query Values {Values(input: {text: "a\\"b", count: 3, empty: null, nested: {items: [{id: 1}, null], flags: [true, false], deep: {x: "y"}}}) {}}'))

		let parsed = parse(body)
		compare(parsed.params.input.text, 'a"b')
		compare(parsed.params.input.nested.items, [{id: 1}, null])
		compare(parsed.params.input.nested.deep.x, "y")
	}
}
