import QtQuick 2.12
import QtTest 1.12
import sdlgentestunionScalarUnionSdl 1.0
import sdlgentestunioncpp 1.0

// The transfer of the union of all scalar types between C++ and QML.
// The JSON of the values is the same as in the C++ test 'CSdlGenScalarUnionTest.cpp', keep both tests in sync
TestCase {
	id: testCase

	name: "ScalarUnion"

	Component {
		id: qmlPayloadComp

		ScalarUnionPayload {}
	}

	Component {
		id: cppPayloadComp

		CppScalarUnionPayload {}
	}

	function test_values_data(){
		return [
			{tag: "Double", value: 2.5, json: "2.5"},
			{tag: "negative Double", value: -0.125, json: "-0.125"},
			{tag: "String", value: "text", json: '"text"'},
			{tag: "empty String", value: "", json: '""'},
			{tag: "numeric String", value: "42", json: '"42"'},
			{tag: "boolean String", value: "true", json: '"true"'},
			{tag: "Int", value: 42, json: "42"},
			{tag: "negative Int", value: -7, json: "-7"},
			{tag: "zero Int", value: 0, json: "0"},
			{tag: "Bool true", value: true, json: "true"},
			{tag: "Bool false", value: false, json: "false"}
		]
	}

	function createPayloadJson(jsonValue){
		return '{"__typename":"ScalarUnionPayload","requiredValue":' + jsonValue + ',"optionalValue":' + jsonValue + '}'
	}

	function checkValue(actual, expected, fieldName){
		compare(typeof actual, typeof expected, fieldName + " has the wrong type")
		compare(actual, expected, fieldName + " has the wrong value")
	}

	function checkPayload(payload, expected){
		checkValue(payload.m_requiredValue, expected, "requiredValue")
		checkValue(payload.m_optionalValue, expected, "optionalValue")
	}

	function test_values(data){
		let expectedJson = JSON.parse(createPayloadJson(data.json))

		// C++ -> QML
		let cppSource = createTemporaryObject(cppPayloadComp, testCase)
		verify(cppSource)
		cppSource.m_requiredValue = data.value
		cppSource.m_optionalValue = data.value
		checkPayload(cppSource, data.value)
		let cppJson = cppSource.toJson()
		compare(JSON.parse(cppJson), expectedJson)

		let qmlTarget = createTemporaryObject(qmlPayloadComp, testCase)
		verify(qmlTarget)
		verify(qmlTarget.createFromJson(cppJson))
		checkPayload(qmlTarget, data.value)

		// QML -> C++
		let qmlSource = createTemporaryObject(qmlPayloadComp, testCase)
		verify(qmlSource)
		qmlSource.m_requiredValue = data.value
		qmlSource.m_optionalValue = data.value
		let qmlJson = qmlSource.toJson()
		compare(JSON.parse(qmlJson), expectedJson)

		let cppTarget = createTemporaryObject(cppPayloadComp, testCase)
		verify(cppTarget)
		verify(cppTarget.createFromJson(qmlJson))
		checkPayload(cppTarget, data.value)
		compare(JSON.parse(cppTarget.toJson()), expectedJson)
	}

	function test_optionalValueMissing(){
		let json = '{"__typename":"ScalarUnionPayload","requiredValue":"text"}'

		let qmlPayload = createTemporaryObject(qmlPayloadComp, testCase)
		verify(qmlPayload)
		verify(qmlPayload.createFromJson(json))
		checkValue(qmlPayload.m_requiredValue, "text", "requiredValue")
		verify(qmlPayload.m_optionalValue === null || qmlPayload.m_optionalValue === undefined)
		compare(JSON.parse(qmlPayload.toJson()), JSON.parse(json))

		let cppPayload = createTemporaryObject(cppPayloadComp, testCase)
		verify(cppPayload)
		verify(cppPayload.createFromJson(json))
		checkValue(cppPayload.m_requiredValue, "text", "requiredValue")
		verify(!cppPayload.hasOptionalValue())
		compare(JSON.parse(cppPayload.toJson()), JSON.parse(json))
	}
}
