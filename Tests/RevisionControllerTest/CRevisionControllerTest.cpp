#include "CRevisionControllerTest.h"

// Partitura test registrations for RevisionControllerTest
const char partituraFileCollectionTest[] = "FileCollectionTest";
typedef CRevisionControllerPartituraTestBaseRegister<partituraFileCollectionTest> partituraFileCollectionTestClass;
I_ADD_TEST(partituraFileCollectionTestClass);
