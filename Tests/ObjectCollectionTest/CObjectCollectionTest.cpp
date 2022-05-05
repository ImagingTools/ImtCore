#include "CObjectCollectionTest.h"

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <imtauth/CAccountInfo.h>
#include <imtauth/CAccountInfoMetaInfo.h>
#include <imtauth/IAccountInfo.h>


//I_ADD_TEST(CObjectCollectionTest);

const char partitura[] = "ObjectCollectionWithoutFixedTypeIdsTest";
typedef CObjectCollectionPartituraTestBaseRegister<partitura> partituraClass;
I_ADD_TEST(partituraClass);

const char partitura1[] = "ObjectCollectionWithFixedTypeIdsTest";
typedef CObjectCollectionPartituraTestBaseRegister<partitura1> partituraClass1;
I_ADD_TEST(partituraClass1);

