#include "Test1.h"


// ImtCore includes
#include <imtbase/IObjectCollection.h>


void Test1::InitTestCase()
{
	QString imtCorePath = qEnvironmentVariable("IMTCOREDIR");
	QString registryFile = imtCorePath + "/Tests/Partitura/ImtTestVoce.arp/Test1.acc";
	QString configFile = imtCorePath + "/Config/ImtCoreTest.awc";

	compositePtr = new ipackage::CComponentAccessor(registryFile, configFile);
}


void Test1::TestCase1()
{
	if (compositePtr != nullptr){
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){
			objectCollectionPtr->ResetData();
		}
	}
}

void Test1::CleanupTestCase()
{
}

I_ADD_TEST(Test1);
