#include "CGqlRequestTest.h"


// ImtCore includes
#include <imtgql/CGqlRequest.h>


const char* testQuery = R"(
{"query": "mutation TestMutation {
	TestMutation(
		input: {
			IntValue: 1,
			DoubleValue: 1.1,
			StringValue: \"1\",

			ObjectValue: {
				IntValue: 1,
				DoubleValue: 1.1,
				StringValue: \"1\",
			}

			ArrayOfInts: [1,2,3],
			ArrayOfDoubles: [0.1,0.2,0.3],
			ArrayOfStrings: [\"1\",\"2\",\"3\"],

			ArrayOfObjects: [
				{Value: 1},
				{Value: 1.1},
				{Value:\"1\"},
			],

			ArrayOfArrays: [
				[1, 2, 3],
				[0.1, 0.2, 0.3],
				[\"1\", \"2\", \"3\"]
			]

			ArrayOfObjectsWithArray: [
				{
					Array: [1, 2, 3]
				}
				{
					Array: [0.1, 0.2, 0.3]
				}
				{
					Array: [\"1\", \"2\", \"3\"]
				}
			]
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


void CGqlRequestTest::ParseQueryTest()
{
	int errorPosition = -1;

	imtgql::CGqlRequest request;
	bool retVal = request.ParseQuery(testQuery, errorPosition);
	QVERIFY(retVal);
	QVERIFY(errorPosition < 0);
}

void CGqlRequestTest::cleanupTestCase()
{
}


I_ADD_TEST(CGqlRequestTest);
