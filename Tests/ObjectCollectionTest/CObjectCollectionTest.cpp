#include "CObjectCollectionTest.h"

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <imtauth/CAccountInfo.h>
#include <imtauth/CAccountInfoMetaInfo.h>
#include <imtauth/IAccountInfo.h>
#include <GeneratedFiles/ObjectCollectionTest/CGenerateTest.h>


void CObjectCollectionTest::initTestCase()
{
	// paths to test component accessor and config for him
	m_imtCorePath = qEnvironmentVariable("IMTCOREDIR");
	m_registryFile = m_imtCorePath + QString("/Tests/ObjectCollectionTest/PrivatePartitura/ObjectCollectionWithFixedTypeIdsTest.acc");
	m_configFile = m_imtCorePath + "/Config/ImtCoreTest.awc";
	// set type id for tests
	m_typeIdObjectCollection = "AccountInfo";
}

void CObjectCollectionTest::ResetCollectionWithFixedObjectTest()
{
	initTestCase();
	// declaration component accessor
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// Reset collection
			objectCollectionPtr->ResetData();

			imtbase::IObjectCollection::Ids idsInObjectBeforeReset = objectCollectionPtr->GetElementIds();
			if (idsInObjectBeforeReset.count() > 0){
				objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), "testId");

				// Reset collection
				objectCollectionPtr->ResetData();

				//Check exist fixed object and remove inserted object from collection
				imtbase::IObjectCollection::Ids idsInObjectAfterReset = objectCollectionPtr->GetElementIds();
				if (!idsInObjectAfterReset.isEmpty()){
					bool checkExistFixedObject = idsInObjectAfterReset.contains(idsInObjectBeforeReset[0]);
					bool checkDeleteAddedObject = !idsInObjectAfterReset.contains("testId");
					QVERIFY2((checkExistFixedObject && checkDeleteAddedObject), "Reset objects from collection with fixed object is failed");
				}
				else{
					QFAIL("Reset collection with fixed object is failed: collection is empty");
				}
			}
			else{
				QSKIP("Object collection don't have fixed objects");
			}

		}
		else{
			QFAIL("Object collection is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


void CObjectCollectionTest::GetOperationFlagsFixedObjectTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// Reset collection
			objectCollectionPtr->ResetData();

			// check exist fixed object and check flags him
			if (objectCollectionPtr->GetElementsCount() > 0){
				int flagsFixedObject = objectCollectionPtr->GetOperationFlags("DefaultAccount");
				QVERIFY2((flagsFixedObject == (imtbase::IObjectCollection::OF_ALL & ~imtbase::IObjectCollection::OF_SUPPORT_DELETE & ~imtbase::IObjectCollection::OF_SUPPORT_PAGINATION)), "Operation flags of fixed object is incorrect");
			}
			else{
				QSKIP("Object collection don't have fixed objects");
			}
		}
		else{
			QFAIL("Object collection is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


void CObjectCollectionTest::CheckSerializeTest()
{
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// Reset collection
			objectCollectionPtr->ResetData();

			// declaration dataPtr for collection
			iser::ISerializable* dataPtr = compositePtr->GetComponentInterface<iser::ISerializable>();

			// set path to file with test data and serialize in dataPtr
			QString pathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/ObjectCollectionTest/TestData/testData.xml";
			ifile::CCompactXmlFileReadArchive readArchive(pathToFile);
			dataPtr->Serialize(readArchive);

			// get data from object and compare with input data
			imtbase::IObjectCollection::DataPtr inputDataPtr;
			if (objectCollectionPtr->GetObjectData("{98c7bd83-2541-427a-a9d2-2427e1d32618}", inputDataPtr)){
				imtauth::CAccountInfo* inputImplPtr = dynamic_cast<imtauth::CAccountInfo*>(inputDataPtr.GetPtr());
				QString nameAccount = inputImplPtr->GetAccountName();
				QString descriptionAccount = inputImplPtr->GetAccountDescription();
				QVERIFY2(((nameAccount == "AccountName") && (descriptionAccount == "AccountDescription")), "Serialize data from file in ibject collection is failed");
			}
			else{
				QFAIL("Data from file is not readed");
			}
		}
		else{
			QFAIL("Object collection is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


void CObjectCollectionTest::cleanupTestCase()
{
	m_imtCorePath.clear();
	m_registryFile.clear();
	m_configFile.clear();
	m_typeIdObjectCollection.clear();
}

I_ADD_TEST(CObjectCollectionTest);
