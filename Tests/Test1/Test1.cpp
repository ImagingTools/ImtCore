#include "Test1.h"


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


void CObjectCollectionTest::CheckSerializeTest()
{
	if (m_compositePtr != nullptr){

		imtbase::IObjectCollection* objectCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){
			objectCollectionPtr->ResetData();

			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "Description object");

			if (!idNewObject.isEmpty()){

				objectCollectionPtr->SetObjectDescription(idNewObject, "Another description");
				objectCollectionPtr->SetObjectName(idNewObject, "AnotherName");

				QString pathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/Test1/TestData/xml.xml";
				ifile::CCompactXmlFileReadArchive readArchive(pathToFile);

				iser::ISerializable* data = m_compositePtr->GetComponentInterface<iser::ISerializable>();

				if (data != nullptr){
					data->Serialize(readArchive);
					objectCollectionPtr->SetObjectData(idNewObject, *data);

					imtauth::CAccountInfo accountInfo;
					accountInfo.SetAccountName("NameAccount");
					accountInfo.SetAccountDescription("DecriptionAccount");

					imtbase::IObjectCollection::DataPtr* dataPtr = new imtbase::IObjectCollection::DataPtr(accountInfo.CloneMe());
					imtbase::IObjectCollection::DataPtr* dataPtr2 = new imtbase::IObjectCollection::DataPtr;
					objectCollectionPtr->GetObjectData(idNewObject, *dataPtr2);
				}
			}
		}
	}
}


void CObjectCollectionTest::CreateNewObjectTest_data()
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


void CObjectCollectionTest::CreateNewObjectTest()
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
			QVERIFY2((idNewObject.isEmpty() == result), "Failed in insert new object");

		}
	}
}


void CObjectCollectionTest::cleanupTestCase()
{
}

I_ADD_TEST(CObjectCollectionTest);
