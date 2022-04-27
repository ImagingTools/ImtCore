#include "CObjectCollectionTest.h"


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <imtauth/CAccountInfo.h>


void CObjectCollectionTest::initTestCase()
{
	// paths to test component accessor and config for him
	QString imtCorePath = qEnvironmentVariable("IMTCOREDIR");
	QString registryFileWithoutFixedTypeIds = imtCorePath + "/Tests/Partitura/ImtTestVoce.arp/ObjectCollectionWithoutFixedTypeIdsTest.acc";
	QString registryFileWithFixedTypeIds = imtCorePath + "/Tests/Partitura/ImtTestVoce.arp/ObjectCollectionWithFixedTypeIdsTest.acc";
	QString configFile = imtCorePath + "/Config/ImtCoreTest.awc";

	// set type id for tests and declaration component accessor
	m_typeIdObjectCollection = "AccountInfo";
	m_compositePtr = new ipackage::CComponentAccessor(registryFileWithoutFixedTypeIds, configFile);
	m_compositeWithFixedTypeIdPtr = new ipackage::CComponentAccessor(registryFileWithFixedTypeIds, configFile);
}


// variable decloration for Unit-test InsertNewObjectWithRequiredParamsTest()
void CObjectCollectionTest::InsertNewObjectWithRequiredParamsTest_data()
{
	// variable decloration
	QTest::addColumn<QByteArray>("typeId");
	QTest::addColumn<QString>("name");
	QTest::addColumn<QString>("description");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QTest::newRow("all param is empty") << QByteArray("") << "" << "" << true;
	QTest::newRow("type param is empty") << QByteArray("") << "testObjectName" << "Insert object with empty type id" << true;
	QTest::newRow("name param is empty") << m_typeIdObjectCollection << "" << "Insert object with empty name" << false;
	QTest::newRow("description param is empty") << m_typeIdObjectCollection << "testObjectName" << "" << false;
	QTest::newRow("type id is not exist") << QByteArray("AnotherTypeId") << "testObjectName" << "Insert object with non-exist type id" << true;
	QTest::newRow("type id is exist") << m_typeIdObjectCollection << "testObjectName" << "Insert object with exist type id and all params is setted" << false;
}


void CObjectCollectionTest::InsertNewObjectWithRequiredParamsTest()
{
	// get values from rows
	QFETCH(QByteArray, typeId);
	QFETCH(QString, name);
	QFETCH(QString, description);
	QFETCH(bool, result);

	if (m_compositePtr != nullptr){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){

			// reset data from object collection
			objectCollectionPtr->ResetData();

			// insert object in collection
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(typeId, name, description);

			// check get correct result
			QVERIFY2((idNewObject.isEmpty() == result), "Failed in insert new object with required params");
		}

		else{
			QFAIL("Object collection is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


void CObjectCollectionTest::InsertNewObjectWithDataTest()
{
	if (m_compositePtr != nullptr){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
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


void CObjectCollectionTest::InsertNewObjectWithNonExistElementTest()
{
	if (m_compositePtr != nullptr){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// reset data from object collection
			objectCollectionPtr->ResetData();

			// insert object in collection
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), "testId");

			// check contains object in collection
			imtbase::IObjectCollection::Ids idsInObject = objectCollectionPtr->GetElementIds();
			bool checkIdIsExist = idsInObject.contains(idNewObject);

			// if collection don't contains object test will be failed
			QVERIFY2(checkIdIsExist, "Insert object with set non-exist id is failed");
		}

		else{
			QFAIL("Object collection is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


void CObjectCollectionTest::InsertNewObjectWithExistElementTest()
{
	if (m_compositePtr != nullptr){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){

			// reset data from object collection
			objectCollectionPtr->ResetData();

			// insert first object in collection
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), "testId");

			// check contains first object in collection
			imtbase::IObjectCollection::Ids idsInObject = objectCollectionPtr->GetElementIds();
			bool checkInsertFirstObject = idsInObject.contains("testId");

			if(checkInsertFirstObject){

				// insert second object with exist id in collection
				QByteArray idSecondNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject2", "TestDescription2", imtbase::IObjectCollection::DataPtr(), "testId");

				// check contains insert objects
				idsInObject = objectCollectionPtr->GetElementIds();
				checkInsertFirstObject = idsInObject.contains("testId");
				bool checkInsertSecondObject = idsInObject.contains(idSecondNewObject);

				// if one of conditions is false - test will be failed
				QVERIFY2((checkInsertFirstObject && checkInsertSecondObject), "Insert new object with set exist id is failed");
			}

			else{
				QFAIL("Insert first object in collection is failed");
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


void CObjectCollectionTest::RemoveExistObjectTest()
{
	if (m_compositePtr != nullptr){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){
			// Reset collection:
			objectCollectionPtr->ResetData();

			// Insert two objects into the collection:
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), "testId");
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject2", "TestDescription2", imtbase::IObjectCollection::DataPtr(), "testId2");

			// Remove object with the ID 'testId':
			objectCollectionPtr->RemoveObject("testId");

			// Get IDs in the collection after removing of the 'testId':
			imtbase::IObjectCollection::Ids idsInObjectAfterRemove = objectCollectionPtr->GetElementIds();

			// Check if the removed object is not existing any more:
			bool wasObjectRemoved = !idsInObjectAfterRemove.contains("testId");
			QVERIFY2(wasObjectRemoved, "Object could not be removed");

			// Check if the not removed object exists:
			bool checkExistAnotherId = idsInObjectAfterRemove.contains("testId2");
			QVERIFY2(checkExistAnotherId, "Not affected object was removed");
		}
		else{
			QFAIL("Object collection is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


void CObjectCollectionTest::RemoveNonExistObjectTest()
{
	if (m_compositePtr != nullptr){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// Reset collection
			objectCollectionPtr->ResetData();

			// Insert two objects into the collection:
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), "testId");
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject2", "TestDescription2", imtbase::IObjectCollection::DataPtr(), "testId2");

			// Remove object with non-exist id
			objectCollectionPtr->RemoveObject("testId3");

			// Check contains removed object and another objects
			imtbase::IObjectCollection::Ids idsInObjectAfterRemove = objectCollectionPtr->GetElementIds();
			bool checkContainsRemovedObject = !idsInObjectAfterRemove.contains("testId3");
			bool checkExistAnotherId = (idsInObjectAfterRemove.contains("testId2") && idsInObjectAfterRemove.contains("testId"));

			// if one of conditios is false - test failed
			QVERIFY2((checkContainsRemovedObject && checkExistAnotherId), "Remove exist object is failed");
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
	if (m_compositePtr != nullptr){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// Reset collection
			objectCollectionPtr->ResetData();

			// declaration dataPtr for collection
			iser::ISerializable* dataPtr = m_compositePtr->GetComponentInterface<iser::ISerializable>();

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
	m_typeIdObjectCollection.clear();

	m_compositePtr.Reset();
}


I_ADD_TEST(CObjectCollectionTest);


