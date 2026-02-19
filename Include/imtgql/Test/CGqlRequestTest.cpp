// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CGqlRequestTest.h"


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlFieldFragment.h>


const char* emptyQuery = R"(
{"query": "query TestQuery {
		TestQuery {}
	}
"}
)";

static const QByteArray emptyQueryResult = R"({"query": "query TestQuery {TestQuery{}}"})";


const char* simpleQuery = R"(
{"query": "query TestQuery {
		TestQuery
		{
			RootField
		}
	}
"}
)";


static const QByteArray simpleQueryResult = R"({"query": "query TestQuery {TestQuery {RootField}}"})";


const char* fieldsQuery = R"(
{"query": "query TestQuery {
		TestQuery
		{
			FirstField
			SecondField ThirdField
		}
	}
"}
)";

static const QByteArray fieldsQueryResult = R"({"query": "query TestQuery {TestQuery {FirstField SecondField ThirdField}}"})";


const char* complexFieldsQuery = R"(
{"query": "query TestQuery {
		TestQuery
		{
			FirstField {first second {first2 second2}}
			SecondField {first3 second3}
		}
	}
"}
)";

static const QByteArray complexFieldsQueryResult = R"({"query": "query TestQuery {TestQuery {FirstField {first second {first2 second2}} SecondField {first3 second3}}}"})";


const char* arrayQuery = R"(
{"query": "mutation TestMutation {
	TestMutation(
		input: {
			ArrayOfInts: [1,2,3],
			ArrayOfDoubles: [0.1,0.2,0.3],
			ArrayOfStrings: [\"1\",\"2\",\"3\"],
			ArrayOfObjects: [
				{Value: 1},
				{Value: 1.1},
				{Value:\"1\"},
			]
		}
	)
	{
		RootField {
			Field
		}
	}
}"}
)";

static const QByteArray arrayQueryResult = R"({"query": "mutation TestMutation {TestMutation(input: {ArrayOfDoubles: [0.1, 0.2, 0.3], ArrayOfInts: [1, 2, 3], ArrayOfStrings: [\"1\", \"2\", \"3\"], ArrayOfObjects :[ {Value: 1}, {Value: 1.1}, {Value: \"1\"} ]}) {RootField {Field}}}"})";


const char* objectQuery = R"(
{"query": "mutation TestMutation {
	TestMutation(
		input: {
			object1: {Value: 1},
			object2: {Value: 1.1},
			object3: {
				Value: { objectValue: \"1\"}
			}
		}
	)
	{
		RootField {
			Field
		}
	}
}"}
)";

static const QByteArray objectQueryResult = R"({"query": "mutation TestMutation {TestMutation(input: {object1: {Value: 1}, object2: {Value: 1.1}, object3: {Value: {objectValue: \"1\"}}}) {RootField {Field}}}"})";


const char* arrayOfObjectArraysQuery = R"(
{"query": "mutation TestMutation {
	TestMutation(
		input: {
			ArrayOfObjectArrays: [
				{
					object: {
						ArrayOfObjectsWithArray: [
							{
								Array: [1, 2, 3]
							},
							{
								Array: [0.1, 0.2, 0.3]
							},
							{
								Array: [\"1\", \"2\", \"3\"]
							}
						]
					}
				},
				{
					object2: {
						ArrayOfObjectsWithArray: [
							{
								Array: [1, 2, 3]
							}
						]
					}
				}
			]
		}
	)
	{
		RootField {
			Field
		}
	}
}"}
)";

static const QByteArray arrayOfObjectArraysQueryResult = R"({"query": "mutation TestMutation {TestMutation(input: {ArrayOfObjectArrays :[ {object: {ArrayOfObjectsWithArray :[ {Array: [1, 2, 3]}, {Array: [0.1, 0.2, 0.3]}, {Array: [\"1\", \"2\", \"3\"]} ]}}, {object2: {ArrayOfObjectsWithArray :[ {Array: [1, 2, 3]} ]}} ]}) {RootField {Field}}}"})";


const char* complexQuery = R"(
{"query": "mutation TestMutation {
	TestMutation(
		input: {
			IntValue: 1,
			DoubleValue: 1.1,
			StringValue: \"1\",
			BoolValueYes: true,
			BoolValueNo: false,

			ObjectValue: {
				IntValue: 1,
				DoubleValue: 1.1,
				StringValue: \"1\"
			},

			ArrayOfInts: [1,2,3],
			ArrayOfDoubles: [0.1,0.2,0.3],
			ArrayOfStrings: [\"1\",\"2\",\"3\"],
			ArrayOfBools: [true,false,true],


			ArrayOfObjects: [
				{Value: 1},
				{Value: 1.1},
				{Value:\"1\"}
			],

			ArrayOfObjectArrays: [
				{
					object: {
						ArrayOfObjectsWithArray: [
							{
								Array: [1, 2, 3]
							},
							{
								Array: [0.1, 0.2, 0.3]
							},
							{
								Array: [\"1\", \"2\", \"3\"]
							}
						]
					}
				},
				{
					object2: {
						ArrayOfObjectsWithArray: [
							{
								Array: [1, 2, 3]
							}
						]
					}
				}
			]
		}

	)

	{
		RootField {
			Field
		}
	}
}"}
)";


static const QByteArray complexQueryResult = R"({"query": "mutation TestMutation {TestMutation(input: {ArrayOfBools: [true, false, true], ArrayOfDoubles: [0.1, 0.2, 0.3], ArrayOfInts: [1, 2, 3], ArrayOfStrings: [\"1\", \"2\", \"3\"], BoolValueNo: false, BoolValueYes: true, DoubleValue: 1.1, IntValue: 1, StringValue: \"1\", ObjectValue: {DoubleValue: 1.1, IntValue: 1, StringValue: \"1\"}, ArrayOfObjectArrays :[ {object: {ArrayOfObjectsWithArray :[ {Array: [1, 2, 3]}, {Array: [0.1, 0.2, 0.3]}, {Array: [\"1\", \"2\", \"3\"]} ]}}, {object2: {ArrayOfObjectsWithArray :[ {Array: [1, 2, 3]} ]}} ], ArrayOfObjects :[ {Value: 1}, {Value: 1.1}, {Value: \"1\"} ]}) {RootField {Field}}}"})";

const char* substratQuery = R"(
{"query": "query SubstratesList {
	SubstratesList(input:
		{viewParams:
			{count: 25, offset: 0,
				filterModel: {
					sortingInfo:[
						{fieldId:\"Name\",sortingOrder:\"ASC\"}
					],
					fieldsFilter:{
						fieldFilters:[],
						groupFilters:[
							{
								fieldFilters:[
									{
										fieldId:\"Name\",
										filterValue:\"cc\",
										filterValueType:\"string\",
										filterOperation:\"CONTAINS\"
									},
									{
										fieldId:\"TypeId\",
										filterValue:\"cc\",
										filterValueType:\"string\",
										filterOperation:\"CONTAINS\"
									},
									{
										fieldId:\"SubstrateClassName\",
										filterValue:\"cc\",
										filterValueType:\"string\",
										filterOperation:\"CONTAINS\"
									},
									{
										fieldId:\"Manufacturer\",
										filterValue:\"cc\",
										filterValueType:\"string\",
										filterOperation:\"CONTAINS\"
									},
									{
										fieldId:\"Article\",
										filterValue:\"cc\",
										filterValueType:\"string\",
										filterOperation:\"CONTAINS\"
									}
								],
								groupFilters:[],
								logicalOperation:\"OR\"
							}
						],
						logicalOperation:\"AND\"
					},
					timeFilter:{
						timeRange:{Begin:\"\",End:\"\"},
						timeUnit:\"\",
						interpretationMode:\"\",
						unitMultiplier:0
					}
				}
			}
	}) {items {id name lab classDeltaE typeId substrateClassName manufacturer article added lastModified}}}"}
)";


static const QByteArray substratQueryResult = R"({"query": "query SubstratesList {SubstratesList(input: {viewParams: {count: 25, offset: 0, filterModel: {fieldsFilter: {logicalOperation: \"AND\", fieldFilters :[ ], groupFilters :[ {logicalOperation: \"OR\", fieldFilters :[ {fieldId: \"Name\", filterOperation: \"CONTAINS\", filterValue: \"cc\", filterValueType: \"string\"}, {fieldId: \"TypeId\", filterOperation: \"CONTAINS\", filterValue: \"cc\", filterValueType: \"string\"}, {fieldId: \"SubstrateClassName\", filterOperation: \"CONTAINS\", filterValue: \"cc\", filterValueType: \"string\"}, {fieldId: \"Manufacturer\", filterOperation: \"CONTAINS\", filterValue: \"cc\", filterValueType: \"string\"}, {fieldId: \"Article\", filterOperation: \"CONTAINS\", filterValue: \"cc\", filterValueType: \"string\"} ], groupFilters :[ ]} ]}, timeFilter: {interpretationMode: \"\", timeUnit: \"\", unitMultiplier: 0, timeRange: {Begin: \"\", End: \"\"}}, sortingInfo :[ {fieldId: \"Name\", sortingOrder: \"ASC\"} ]}}}) {items {id name lab classDeltaE typeId substrateClassName manufacturer article added lastModified}}}"})";

const char* unionFieldsQuery = R"(
{"query": "query TestUnionQuery {
		TestUnionQuery
		{
			FirstField {first second {... on Person { name } ... on Photo { height } }}
			SecondField {
				first
				second {
					... on Person {
						name
					}
					... on Photo {
						height
						status {
							isOpen
							... on Device {
								id
								type
							}
						}
					}
				}
			}
		}
	}
"}
)";


const char* arrayQuery2 = R"(
{"query": "mutation TestMutation {
	TestMutation(
		input: {
			ArrayOfInts: [1,2,3],
			ArrayOfDoubles: [0.1,0.2,0.3],
			ArrayOfStrings: [\"1\",\"2\",\"3\"],
			ArrayOfObjects: [
				{Value: 1},
				{Value: 1.1},
				{Value:\"1\"},
			]
		}
	)
	{
		RootField {
			Field
		}
	}
}"}
)";

static const QByteArray unionFieldsQueryResult = R"({"query": "query TestUnionQuery {TestUnionQuery {FirstField {first second { ... on Person {name}  ... on Photo {height}}} SecondField {first second { ... on Person {name}  ... on Photo {height status {isOpen  ... on Device {id type}}}}}}}"})";


void CGqlRequestTest::initTestCase()
{

}

void CGqlRequestTest::CreateArrayQuery()
{
	imtgql::CGqlRequest request(imtgql::IGqlRequest::RT_MUTATION);
	request.SetCommandId("TestMutation");
	imtgql::CGqlFieldObject rootField;
	rootField.InsertField("Field");
	request.AddField("RootField", rootField);

	imtgql::CGqlParamObject inputObject;
	QList<int> arrayOfInts = {1,2,3};
	inputObject.InsertParam("ArrayOfInts", QVariant::fromValue(arrayOfInts));
	QList<double> arrayOfDoubles = {0.1,0.2,0.3};
	inputObject.InsertParam("ArrayOfDoubles", QVariant::fromValue(arrayOfDoubles));
	QList<QString> arrayOfStrings = {"1","2","3"};
	inputObject.InsertParam("ArrayOfStrings", QVariant::fromValue(arrayOfStrings));

	imtgql::CGqlParamObject valueObject;
	valueObject.InsertParam("Value", 1);
	inputObject.AppendParamToArray("ArrayOfObjects",valueObject);
	valueObject.InsertParam("Value", 1.1);
	inputObject.AppendParamToArray("ArrayOfObjects",valueObject);
	valueObject.InsertParam("Value", "1");
	inputObject.AppendParamToArray("ArrayOfObjects",valueObject);

	request.AddParam("input", inputObject);

	QByteArray resultQuery = request.GetQuery();
	QCOMPARE(resultQuery, arrayQueryResult);
}


void CGqlRequestTest::CreateUnionRequest()
{
	imtgql::CGqlRequest request;
	request.SetCommandId("TestUnionQuery");

	imtgql::CGqlFieldObject firstField;
	firstField.InsertField("first");
	imtgql::CGqlFieldObject secondInternalField;
	imtgql::CGqlFieldFragment fragmentPerson;
	fragmentPerson.InsertField("name");
	secondInternalField.InsertFragment("Person", fragmentPerson);
	imtgql::CGqlFieldFragment fragmentPhoto;
	fragmentPhoto.InsertField("height");
	secondInternalField.InsertFragment("Photo", fragmentPhoto);
	firstField.InsertField("second", secondInternalField);
	request.AddField("FirstField", firstField);

	imtgql::CGqlFieldObject secondField;
	secondInternalField.ResetData();
	secondField.InsertField("first");
	secondInternalField.InsertFragment("Person", fragmentPerson);
	imtgql::CGqlFieldObject fieldStatus;
	fieldStatus.InsertField("isOpen");
	imtgql::CGqlFieldFragment fragmentDevice;
	fragmentDevice.InsertField("id");
	fragmentDevice.InsertField("type");
	fieldStatus.InsertFragment("Device", fragmentDevice);
	fragmentPhoto.InsertField("status", fieldStatus);
	secondInternalField.InsertFragment("Photo", fragmentPhoto);
	secondField.InsertField("second", secondInternalField);
	request.AddField("SecondField", secondField);

	QByteArray resultQuery = request.GetQuery();
	QCOMPARE(resultQuery, unionFieldsQueryResult);
}


void CGqlRequestTest::ParseUnionFields()
{
	qsizetype errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(unionFieldsQuery, errorPosition);
	QByteArray resultQuery = request.GetQuery();
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
	QCOMPARE(resultQuery, unionFieldsQueryResult);
}


void CGqlRequestTest::ParseEmpty()
{
	qsizetype errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(emptyQuery, errorPosition);
	QByteArray resultQuery = request.GetQuery();
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
	QCOMPARE(resultQuery, emptyQueryResult);
}


void CGqlRequestTest::ParseSimple()
{
	qsizetype errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(simpleQuery, errorPosition);
	QByteArray resultQuery = request.GetQuery();
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
	QCOMPARE(resultQuery, simpleQueryResult);
}


void CGqlRequestTest::ParseFields()
{
	qsizetype errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(fieldsQuery, errorPosition);
	QByteArray resultQuery = request.GetQuery();
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
	QCOMPARE(resultQuery, fieldsQueryResult);
}


void CGqlRequestTest::ParseComplexFields()
{
	qsizetype errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(complexFieldsQuery, errorPosition);
	QByteArray resultQuery = request.GetQuery();
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
	QCOMPARE(resultQuery, complexFieldsQueryResult);
}

void CGqlRequestTest::ParseSubstrateTest()
{
	qsizetype errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(substratQuery, errorPosition);
	QByteArray resultQuery = request.GetQuery();
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
	QCOMPARE(resultQuery, substratQueryResult);
}


void CGqlRequestTest::ParseObjectQuery()
{
	qsizetype errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(objectQuery, errorPosition);
	QByteArray resultQuery = request.GetQuery();
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
	QCOMPARE(resultQuery, objectQueryResult);
}


void CGqlRequestTest::ParseArrayQuery()
{
	qsizetype errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(arrayQuery, errorPosition);
	QByteArray resultQuery = request.GetQuery();
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
	QCOMPARE(resultQuery, arrayQueryResult);
}


void CGqlRequestTest::ParseArrayOfObjectArraysQuery()
{
	qsizetype errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(arrayOfObjectArraysQuery, errorPosition);
	QByteArray resultQuery = request.GetQuery();
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
	QCOMPARE(resultQuery, arrayOfObjectArraysQueryResult);
}


void CGqlRequestTest::ParseComplexTest()
{
	qsizetype errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(complexQuery, errorPosition);
	QByteArray resultQuery = request.GetQuery();
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
	QCOMPARE(resultQuery, complexQueryResult);
}


void CGqlRequestTest::ParseQueryWithVariables()
{
	const char* queryWithVariables = R"(
	{
		"query": "query TestQuery { TestQuery { Field } }",
		"variables": {
			"var1": "value1",
			"var2": 123,
			"var3": true
		}
	}
	)";

	qsizetype errorPosition = -1;
	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(queryWithVariables, errorPosition);

	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);

	const imtgql::CGqlParamObject& vars = request.GetVariables();
	QVERIFY(vars.GetParamArgumentValue("var1").toString() == "value1");
	QVERIFY(vars.GetParamArgumentValue("var2").toInt() == 123);
	QVERIFY(vars.GetParamArgumentValue("var3").toBool() == true);
}


void CGqlRequestTest::ParseQueryWithOperationName()
{
	const char* queryWithOperationName = R"(
	{
		"query": "mutation StartSession($input: DeviceIdInput!) { BeginMeasurementSession(input: $input) { __typename id } }",
		"operationName": "StartSession",
		"variables": {
			"input": {
				"id": "X-Rite i1Pro 2 SN-Virtual-1234#Agent-2"
			}
		}
	}
	)";

	qsizetype errorPosition = -1;
	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(queryWithOperationName, errorPosition);

	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);

	const imtgql::CGqlParamObject& vars = request.GetVariables();
	const imtgql::CGqlParamObject* inputPtr = vars.GetParamArgumentObjectPtr("input");
	QVERIFY(inputPtr != nullptr);
	QCOMPARE(inputPtr->GetParamArgumentValue("id").toString(),
			 QString("X-Rite i1Pro 2 SN-Virtual-1234#Agent-2"));

	// // Objects
	const imtgql::CGqlParamObject* rootInput = request.GetParamObject("input");
	QVERIFY(rootInput != nullptr);
	QCOMPARE(rootInput->GetParamArgumentValue("id").toString(),
			 QString("X-Rite i1Pro 2 SN-Virtual-1234#Agent-2"));
}


void CGqlRequestTest::ParseStartPatchMeasurement()
{
	const char* payload = R"(
	{
	  "query": "mutation StartPatchMeasurement($input: BeginMeasuringPatchInput!) { BeginMeasuringPatch(input: $input) { __typename id } }",
	  "operationName": "StartPatchMeasurement",
	  "variables": {
		"input": {
		  "sessionId": "S:4fe8fe7c-afc7-436a-8021-7c4021e7d652#D:X-Rite i1Pro 2 SN-Virtual-1234#A:Agent-2",
		  "fieldIndex": 1,
		  "rowIndex": 2,
		  "patchIndex": 8
		}
	  }
	}
	)";

	qsizetype errorPosition = -1;
	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(payload, errorPosition);

	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);

	const imtgql::CGqlParamObject& vars = request.GetVariables();
	const imtgql::CGqlParamObject* inputPtr = vars.GetParamArgumentObjectPtr("input");
	QVERIFY(inputPtr != nullptr);

	QCOMPARE(inputPtr->GetParamArgumentValue("sessionId").toString(),
			 QStringLiteral("S:4fe8fe7c-afc7-436a-8021-7c4021e7d652#D:X-Rite i1Pro 2 SN-Virtual-1234#A:Agent-2"));
	QCOMPARE(inputPtr->GetParamArgumentValue("fieldIndex").toInt(), 1);
	QCOMPARE(inputPtr->GetParamArgumentValue("rowIndex").toInt(), 2);
	QCOMPARE(inputPtr->GetParamArgumentValue("patchIndex").toInt(), 8);

	// Objects
	const imtgql::CGqlParamObject* rootInput = request.GetParamObject("input");
	QVERIFY(rootInput != nullptr);
	QCOMPARE(rootInput->GetParamArgumentValue("sessionId").toString(),
			 QStringLiteral("S:4fe8fe7c-afc7-436a-8021-7c4021e7d652#D:X-Rite i1Pro 2 SN-Virtual-1234#A:Agent-2"));
	QCOMPARE(rootInput->GetParamArgumentValue("fieldIndex").toInt(), 1);
	QCOMPARE(rootInput->GetParamArgumentValue("rowIndex").toInt(), 2);
	QCOMPARE(rootInput->GetParamArgumentValue("patchIndex").toInt(), 8);
}


void CGqlRequestTest::ParseStartPatchMeasurementComplex()
{
	const char* payload = R"(
	{
	  "query": "mutation StartPatchMeasurement($input: BeginMeasuringPatchInput!) { BeginMeasuringPatch(input: $input) { __typename id } }",
	  "operationName": "StartPatchMeasurement",
	  "variables": {
		"input": {
		  "sessionId": {
			"id": "S:4fe8fe7c-afc7-436a-8021-7c4021e7d652#D:X-Rite i1Pro 2 SN-Virtual-1234#A:Agent-2"
		  },
		  "array": [
			{ "test": 3 },
			{ "test1": 4 }
		  ],
		  "fieldIndex": 1,
		  "rowIndex": 2,
		  "patchIndex": 8
		}
	  }
	}
	)";

	qsizetype errorPosition = -1;
	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(payload, errorPosition);

	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);

	const imtgql::CGqlParamObject& vars = request.GetVariables();
	const imtgql::CGqlParamObject* inputPtr = vars.GetParamArgumentObjectPtr("input");
	QVERIFY(inputPtr != nullptr);

	const imtgql::CGqlParamObject* sessionPtr = inputPtr->GetParamArgumentObjectPtr("sessionId");
	QVERIFY(inputPtr != nullptr);
	QCOMPARE(sessionPtr->GetParamArgumentValue("id").toString(),
			 QStringLiteral("S:4fe8fe7c-afc7-436a-8021-7c4021e7d652#D:X-Rite i1Pro 2 SN-Virtual-1234#A:Agent-2"));

	const QList<const imtgql::CGqlParamObject *> arrayListPtr = inputPtr->GetParamArgumentObjectPtrList("array");
	QVERIFY(!arrayListPtr.isEmpty());
	QCOMPARE(arrayListPtr.size(), 2);
	QVERIFY(arrayListPtr[0] != nullptr);
	QCOMPARE(arrayListPtr[0]->GetParamArgumentValue("test").toInt(), 3);
	QVERIFY(arrayListPtr[1] != nullptr);
	QCOMPARE(arrayListPtr[1]->GetParamArgumentValue("test1").toInt(), 4);

	QCOMPARE(inputPtr->GetParamArgumentValue("fieldIndex").toInt(), 1);
	QCOMPARE(inputPtr->GetParamArgumentValue("rowIndex").toInt(), 2);
	QCOMPARE(inputPtr->GetParamArgumentValue("patchIndex").toInt(), 8);
}


void CGqlRequestTest::TestVariableObjectList()
{
	const char* payload = R"(
	{
	  "query": "mutation BulkUpdate($items: [ItemInput!]!) { updateItems(input: $items) { success } }",
	  "operationName": "BulkUpdate",
	  "variables": {
		"items": [
		  { "code": 101, "status": "active" },
		  { "code": 102, "status": "pending" }
		]
	  }
	}
	)";

	imtgql::CGqlRequest request;
	qsizetype errorPos = -1;
	bool success = request.ParseQuery(payload, errorPos);

	QVERIFY(success);
	QVERIFY(errorPos < 0);

	const imtgql::CGqlParamObject& vars = request.GetVariables();
	QVERIFY(vars.IsObjectList("items"));
	QCOMPARE(vars.GetObjectsCount("items"), 2);

	const imtgql::CGqlParamObject& params = request.GetParams();
	QVERIFY(params.IsObjectList("input"));
	QCOMPARE(params.GetObjectsCount("input"), 2);

	const imtgql::CGqlParamObject* item1 = params.GetParamArgumentObjectPtr("input", 0);
	const imtgql::CGqlParamObject* item2 = params.GetParamArgumentObjectPtr("input", 1);
	QVERIFY(item1 != nullptr);
	QVERIFY(item2 != nullptr);

	QCOMPARE(item1->GetParamArgumentValue("code").toInt(), 101);
	QCOMPARE(item1->GetParamArgumentValue("status").toString(), QStringLiteral("active"));
	QCOMPARE(item2->GetParamArgumentValue("code").toInt(), 102);
	QCOMPARE(item2->GetParamArgumentValue("status").toString(), QStringLiteral("pending"));
}


void CGqlRequestTest::TestVariablePrimitivesAndLists()
{
	const char* payload = R"(
	{
	  "query": "query GetDevices($id: ID!, $tags: [String]) { brand(ownerId: $id, filter: $tags) { name } }",
	  "variables": {
		"id": "S::X-Rite i1Pro 2 SN-Virtual-6509#A:Agent",
		"tags": ["dev", "X-Rite i1Pro2"]
	  }
	}
	)";

	imtgql::CGqlRequest request;
	qsizetype errorPos = -1;
	bool success = request.ParseQuery(payload, errorPos);

	QVERIFY(success);
	QVERIFY(errorPos < 0);

	const imtgql::CGqlParamObject& params = request.GetParams();
	QCOMPARE(params.GetParamArgumentValue("ownerId").toString(),
			 QStringLiteral("S::X-Rite i1Pro 2 SN-Virtual-6509#A:Agent"));
	QVariant filterVar = params.GetParamArgumentValue("filter");
	QVERIFY(filterVar.typeId() == QMetaType::QVariantList);

	QVariantList filterList = filterVar.toList();
	QCOMPARE(filterList.size(), 2);
	QCOMPARE(filterList.at(0).toString(), QStringLiteral("dev"));
	QCOMPARE(filterList.at(1).toString(), QStringLiteral("X-Rite i1Pro2"));
}


void CGqlRequestTest::cleanupTestCase()
{
}


I_ADD_TEST(CGqlRequestTest);
