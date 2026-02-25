#include "CFileCollectionTest.h"

// Partitura test registrations for FileCollectionTest
const char partituraFileCollectionTest[] = "FileCollectionTest";
typedef CFileCollectionPartituraTestBaseRegister<partituraFileCollectionTest> partituraFileCollectionTestClass;
I_ADD_TEST(partituraFileCollectionTestClass);
