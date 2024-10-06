#include "CGqlRequestTest.h"


// ImtCore includes
#include <imtgql/CGqlRequest.h>

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


const char* complexQuery = R"(
{"query": "mutation TestMutation {
	TestMutation(
		input: {
			IntValue: 1,
			DoubleValue: 1.1,
			StringValue: \"1\",

			ObjectValue: {
				IntValue: 1,
				DoubleValue: 1.1,
				StringValue: \"1\"
			},

			ArrayOfInts: [1,2,3],
			ArrayOfDoubles: [0.1,0.2,0.3],
			ArrayOfStrings: [\"1\",\"2\",\"3\"],

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

void CGqlRequestTest::initTestCase()
{

}


void CGqlRequestTest::ParseObjectQuery()
{
	int errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(objectQuery, errorPosition);
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
}


void CGqlRequestTest::ParseArrayQuery()
{
	int errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(arrayQuery, errorPosition);
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
}


void CGqlRequestTest::ParseArrayOfObjectArraysQuery()
{
	int errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(arrayOfObjectArraysQuery, errorPosition);
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
}


void CGqlRequestTest::ParseComplexTest()
{
	int errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(complexQuery, errorPosition);
	QByteArray resultQuery = request.GetQuery();
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
}


void CGqlRequestTest::cleanupTestCase()
{
}


I_ADD_TEST(CGqlRequestTest);
