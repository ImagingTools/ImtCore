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


void CGqlRequestTest::cleanupTestCase()
{
}


I_ADD_TEST(CGqlRequestTest);
