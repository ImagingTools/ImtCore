#include "CObjectCollectionTest.h"


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <imtauth/CAccountInfo.h>


void CObjectCollectionTest::initTestCase()
{
	QString imtCorePath = qEnvironmentVariable("IMTCOREDIR");
	QString registryFile = imtCorePath + "/Tests/Partitura/ImtTestVoce.arp/Test1.acc";
	QString configFile = imtCorePath + "/Config/ImtCoreTest.awc";

	m_typeIdObjectCollection = "AccountInfo";
	m_compositePtr = new ipackage::CComponentAccessor(registryFile, configFile);
}


void CObjectCollectionTest::InsertNewObjectWithRequiredParamsTest_data()
{
	QTest::addColumn<QByteArray>("typeId");
	QTest::addColumn<QString>("name");
	QTest::addColumn<QString>("description");
	QTest::addColumn<bool>("result");

	QTest::newRow("all param is empty") << QByteArray("") << "" << "" << true;
	QTest::newRow("type param is empty") << QByteArray("") << "testObjectName" << "Insert object with empty type id" << true;
	QTest::newRow("name param is empty") << m_typeIdObjectCollection << "" << "Insert object with empty name" << false;
	QTest::newRow("description param is empty") << m_typeIdObjectCollection << "testObjectName" << "" << false;
	QTest::newRow("type id is not exist") << QByteArray("AnotherTypeId") << "testObjectName" << "Insert object with non-exist type id" << true;
	QTest::newRow("type id is exist") << m_typeIdObjectCollection << "testObjectName" << "Insert object with exist type id and all params is setted" << false;
}


void CObjectCollectionTest::InsertNewObjectWithRequiredParamsTest()
{
	QFETCH(QByteArray, typeId);
	QFETCH(QString, name);
	QFETCH(QString, description);
	QFETCH(bool, result);

	if (m_compositePtr != nullptr){

		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){

			objectCollectionPtr->ResetData();
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(typeId, name, description);
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

		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){

			objectCollectionPtr->ResetData();
			imtbase::IObjectCollection::DataPtr dataInObject = new imtauth::CAccountInfo();
			imtbase::IObjectCollection::DataPtr dataInObject2 = new imtauth::CAccountInfo();
			imtauth::CAccountInfo* accountInfoPtr = dynamic_cast<imtauth::CAccountInfo*>(dataInObject.GetPtr());
			imtauth::CAccountInfo* accountInfo2Ptr = dynamic_cast<imtauth::CAccountInfo*>(dataInObject2.GetPtr());

			QString nameAccount = "AccountName";
			QString descriptionAccount = "AccountDescription";
			QString setNameAccount;
			QString setDescriptionAccount;
			accountInfoPtr->SetAccountName(nameAccount);
			accountInfoPtr->SetAccountDescription(descriptionAccount);

			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", dataInObject);

			if (!idNewObject.isEmpty()){

				objectCollectionPtr->GetObjectData(idNewObject, dataInObject2);
				setNameAccount = accountInfo2Ptr->GetAccountName();
				setDescriptionAccount = accountInfo2Ptr->GetAccountDescription();
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

		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){

			objectCollectionPtr->ResetData();
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), "testId");
			QVector<QByteArray> idsInObject = objectCollectionPtr->GetElementIds();
			bool checkIdIsExist = idsInObject.contains(idNewObject);

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

		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){

			objectCollectionPtr->ResetData();
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), "testId");
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject2", "TestDescription2", imtbase::IObjectCollection::DataPtr(), "testId");
			QVector<QByteArray> idsInObject = objectCollectionPtr->GetElementIds();

			QVERIFY2((idsInObject.size() == 2), "Insert new object with set exist id is failed");
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

		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){

			objectCollectionPtr->ResetData();
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), "testId");
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject2", "TestDescription2", imtbase::IObjectCollection::DataPtr(), "testId2");
			objectCollectionPtr->RemoveObject("testId");

			QVector<QByteArray> idsInObjectAfterRemove = objectCollectionPtr->GetElementIds();
			bool checkExistAnotherId = idsInObjectAfterRemove.contains("testId2");

			QVERIFY2(((idsInObjectAfterRemove.size() == 1) && checkExistAnotherId), "Remove exist object is failed");
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

		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){

			objectCollectionPtr->ResetData();
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), "testId");
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject2", "TestDescription2", imtbase::IObjectCollection::DataPtr(), "testId2");
			objectCollectionPtr->RemoveObject("testId3");

			QVector<QByteArray> idsInObjectAfterRemove = objectCollectionPtr->GetElementIds();
			bool checkExistAnotherId = (idsInObjectAfterRemove.contains("testId2") && idsInObjectAfterRemove.contains("testId"));

			QVERIFY2(((idsInObjectAfterRemove.size() == 2) && checkExistAnotherId), "Remove exist object is failed");
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

		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){

			objectCollectionPtr->ResetData();

			iser::ISerializable* dataPtr = m_compositePtr->GetComponentInterface<iser::ISerializable>();
			QString pathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/ObjectCollectionTest/TestData/testData.xml";
			ifile::CCompactXmlFileReadArchive readArchive(pathToFile);
			dataPtr->Serialize(readArchive);

			imtbase::IObjectCollection::DataPtr dataInObject = new imtauth::CAccountInfo();

			if (objectCollectionPtr->GetObjectData("{98c7bd83-2541-427a-a9d2-2427e1d32618}", dataInObject)){

				imtauth::CAccountInfo* accountInfoPtr = dynamic_cast<imtauth::CAccountInfo*>(dataInObject.GetPtr());
				QString nameAccount = accountInfoPtr->GetAccountName();
				QString descriptionAccount = accountInfoPtr->GetAccountDescription();
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
