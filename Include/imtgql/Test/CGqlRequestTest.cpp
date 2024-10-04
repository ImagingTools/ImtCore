#include "CGqlRequestTest.h"


// ImtCore includes
#include <imtgql/CGqlRequest.h>


const char* testQuery = R"(
{"query": "mutation TestMutation {
	TestMutation(
		input: {
			intValue: 1,
			doubleValue: 1.1,
			stringValue: \"1\",

			objectValue: {
				intValue: 1,
				doubleValue: 1.1,
				stringValue: \"1\",
			}

			ArrayOfInts: [1,2,3],
			ArrayOfDoubles: [0.1,0.2,0.3],
			ArrayOfStrings: [\"1\",\"2\",\"3\"],

			ArrayOfObjects: [
				{value: 1},
				{value: 1.1},
				{value:\"1\"},
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
