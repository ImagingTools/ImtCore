#include "CFileCollectionPartituraTestBase.h"

// ImtCore includes
#include <imtrepo/IFileObjectCollection.h>
#include <imtauth/CAccountInfo.h>


void CFileCollectionPartituraTestBase::InsertFileTest_data()
{
	initTestCase();
	// variable decloration
	QTest::addColumn<QString>("filePath");
	QTest::addColumn<QByteArray>("objectTypeId");
	QTest::addColumn<QString>("objectName");
	QTest::addColumn<QString>("objectDescription");
	QTest::addColumn<QByteArray>("proposedObjectId");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QString pathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestInsert.xml";
	QString incorrectPathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestInsert.txt";
	QTest::newRow("all param is empty") << "" << QByteArray("") << "" << "" << QByteArray("") << true;
	QTest::newRow("filePath is empty") << "" << m_typeIdObjectCollection << "TestFileName" << "TestFileDescription" << QByteArray("testIdObject") << true;
	QTest::newRow("objectTypeId is empty") << pathToFile << QByteArray("") << "TestFileName" << "TestFileDescription" << QByteArray("testIdObject") << false;
	QTest::newRow("objectName is empty") << pathToFile << m_typeIdObjectCollection << "" << "TestFileDescription" << QByteArray("testIdObject") << true;
	QTest::newRow("objectDescription is empty") << pathToFile << m_typeIdObjectCollection << "TestFileName" << "" << QByteArray("testIdObject") << true;
	QTest::newRow("proposedObjectId is empty") << pathToFile << m_typeIdObjectCollection << "TestFileName" << "TestFileDescription" << QByteArray("") << true;
	QTest::newRow("all param setted") << pathToFile << m_typeIdObjectCollection << "TestFileName" << "TestFileDescription" << QByteArray("testIdObject") << true;
	QTest::newRow("incorect path to file") << incorrectPathToFile << m_typeIdObjectCollection << "TestFileName" << "TestFileDescription" << QByteArray("testIdObject") << true;
}

void CFileCollectionPartituraTestBase::InsertFileTest()
{
	// get values from rows
	QFETCH(QString, filePath);
	QFETCH(QByteArray, objectTypeId);
	QFETCH(QString, objectName);
	QFETCH(QString, objectDescription);
	QFETCH(QByteArray, proposedObjectId);
	QFETCH(bool, result);
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
		if (fileCollectionPtr != nullptr){
			fileCollectionPtr->ResetData();
			QByteArray idNewObject = fileCollectionPtr->InsertFile(filePath, objectTypeId, objectName, objectDescription, proposedObjectId);
			QCOMPARE(idNewObject.isEmpty(), result);
		}
		else{
			QFAIL("File collection is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


//void CFileCollectionPartituraTestBase::GetFileTest()
//{
//	initTestCase();
//	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
//	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
//	if (compositePtr.IsValid()){
//		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
//		if (fileCollectionPtr != nullptr){

//		}
//		else{
//			QFAIL("File collection is nullptr");
//		}
//	}
//	else{
//		QFAIL("Component is not initialized");
//	}
//}


//void CFileCollectionPartituraTestBase::SetFileTest()
//{
//	initTestCase();
//	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
//	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
//	if (compositePtr.IsValid()){
//		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
//		if (fileCollectionPtr != nullptr){

//		}
//		else{
//			QFAIL("File collection is nullptr");
//		}
//	}
//	else{
//		QFAIL("Component is not initialized");
//	}
//}


//void CFileCollectionPartituraTestBase::GetFileInfoTest()
//{
//	initTestCase();
//	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
//	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
//	if (compositePtr.IsValid()){
//		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
//		if (fileCollectionPtr != nullptr){

//		}
//		else{
//			QFAIL("File collection is nullptr");
//		}
//	}
//	else{
//		QFAIL("Component is not initialized");
//	}
//}


//void CFileCollectionPartituraTestBase::GetFileConstraitsTest()
//{
//	initTestCase();
//	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
//	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
//	if (compositePtr.IsValid()){
//		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
//		if (fileCollectionPtr != nullptr){

//		}
//		else{
//			QFAIL("File collection is nullptr");
//		}
//	}
//	else{
//		QFAIL("Component is not initialized");
//	}
//}


//void CFileCollectionPartituraTestBase::UpdateFileTest()
//{
//	initTestCase();
//	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
//	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
//	if (compositePtr.IsValid()){
//		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
//		if (fileCollectionPtr != nullptr){

//		}
//		else{
//			QFAIL("File collection is nullptr");
//		}
//	}
//	else{
//		QFAIL("Component is not initialized");
//	}
//}


//void CFileCollectionPartituraTestBase::GetCollectionFileLayoutTest()
//{
//	initTestCase();
//	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
//	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
//	if (compositePtr.IsValid()){
//		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
//		if (fileCollectionPtr != nullptr){

//		}
//		else{
//			QFAIL("File collection is nullptr");
//		}
//	}
//	else{
//		QFAIL("Component is not initialized");
//	}
//}

//void CFileCollectionPartituraTestBase::Test()
//{
//	initTestCase();
//	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
//	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
//	if (compositePtr.IsValid()){
//		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
//		if (fileCollectionPtr != nullptr){
//			fileCollectionPtr->ResetData();
//			imtbase::IObjectCollection::DataPtr inputDataPtr = new imtauth::CAccountInfo();

//			// declaration values for object data in type AccountInfo
//			QString nameAccount = "AccountName";
//			QString descriptionAccount = "AccountDescription";
//			QString setNameAccount;
//			QString setDescriptionAccount;

//			// set values in input data
//			imtauth::CAccountInfo* inputImplPtr = dynamic_cast<imtauth::CAccountInfo*>(inputDataPtr.GetPtr());
//			inputImplPtr->SetAccountName(nameAccount);
//			inputImplPtr->SetAccountDescription(descriptionAccount);
//			QString pathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestInsert.xml";
//			QByteArray idNewObject = fileCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestName", "TestDescription", inputDataPtr);
//			fileCollectionPtr->GetFile(idNewObject, pathToFile);
//		}
//		else{
//			QFAIL("File collection is nullptr");
//		}
//	}
//	else{
//		QFAIL("Component is not initialized");
//	}
//}
