#include "CObjectCollectionTest.h"

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <imtauth/CAccountInfo.h>
#include <imtauth/CAccountInfoMetaInfo.h>
#include <imtauth/IAccountInfo.h>


void CObjectCollectionTest::InsertNewObjectWithDataTest()
{
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFileWithoutFixedTypeIds, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// reset data from object collection
			objectCollectionPtr->ResetData();

			// declaration input and reference data of object
			imtbase::IObjectCollection::DataPtr inputDataPtr = new imtauth::CAccountInfo();
			imtbase::IObjectCollection::DataPtr referenceDataPtr;

			// declaration values for object data in type AccountInfo
			QString nameAccount = "AccountName";
			QString descriptionAccount = "AccountDescription";
			QString setNameAccount;
			QString setDescriptionAccount;

			// set values in input data
			imtauth::CAccountInfo* inputImplPtr = dynamic_cast<imtauth::CAccountInfo*>(inputDataPtr.GetPtr());
			inputImplPtr->SetAccountName(nameAccount);
			inputImplPtr->SetAccountDescription(descriptionAccount);

			// insert new object with input data in collection
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", inputDataPtr);
			if (!idNewObject.isEmpty()){

				// get reference data
				objectCollectionPtr->GetObjectData(idNewObject, referenceDataPtr);

				// create reference Impl
				imtauth::CAccountInfo* referenceImplPtr = dynamic_cast<imtauth::CAccountInfo*>(referenceDataPtr.GetPtr());

				// get values from reference data
				setNameAccount = referenceImplPtr->GetAccountName();
				setDescriptionAccount = referenceImplPtr->GetAccountDescription();

				// compare input & reference data
				QVERIFY2(((nameAccount == setNameAccount) && (descriptionAccount == setDescriptionAccount)), "Insert new object with data is failed");
			}
			else{
				QFAIL("Object is not insert in collection");
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
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFileWithoutFixedTypeIds, m_configFile));
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


//I_ADD_TEST(CObjectCollectionTest);

const char partitura[] = "ObjectCollectionWithoutFixedTypeIdsTest";
typedef CObjectCollectionPartituraTestBaseRegister<partitura> partituraClass;
I_ADD_TEST(partituraClass);

const char partitura1[] = "ObjectCollectionWithFixedTypeIdsTest";
typedef CObjectCollectionPartituraTestBaseRegister<partitura1> partituraClass1;
I_ADD_TEST(partituraClass1);

