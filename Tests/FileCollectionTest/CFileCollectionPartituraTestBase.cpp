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
	QTest::newRow("objectName is empty") << pathToFile << m_typeIdObjectCollection << "" << "TestFileDescription" << QByteArray("testIdObject") << false;
	QTest::newRow("objectDescription is empty") << pathToFile << m_typeIdObjectCollection << "TestFileName" << "" << QByteArray("testIdObject") << false;
	QTest::newRow("proposedObjectId is empty") << pathToFile << m_typeIdObjectCollection << "TestFileName" << "TestFileDescription" << QByteArray("") << false;
	QTest::newRow("all param setted") << pathToFile << m_typeIdObjectCollection << "TestFileName" << "TestFileDescription" << QByteArray("testIdObject") << false;
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

	// get component file collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
		if (fileCollectionPtr != nullptr){

			// reset data and check working of insert file with difference values
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


void CFileCollectionPartituraTestBase::GetFileTest()
{
	// get component file collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
		if (fileCollectionPtr != nullptr){

			// reset data and announcement paths to files for tests
			fileCollectionPtr->ResetData();
			QString pathToInsertFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestInsert.xml";
			if (QFile::exists(pathToInsertFile)){
				QString pathToGetFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestGet.xml";

				// read data of file for insert
				QFile fileForInsert(pathToInsertFile);
				fileForInsert.open(QIODevice::ReadOnly);
				QByteArray dataOfFileForInsert = fileForInsert.readAll();
				fileForInsert.close();

				// insert file in file collection
				QByteArray idInsertedObject = fileCollectionPtr->InsertFile(pathToInsertFile, m_typeIdObjectCollection, "TestName", "TestDescription");
				if (!idInsertedObject.isEmpty()){

					//remove last getted file is exist
					if (QFile::exists(pathToGetFile)){
						QFile fileForTest(pathToGetFile);
						fileForTest.remove();
					}

					// check result of GetFile()
					QString pathReferenceFile = fileCollectionPtr->GetFile(idInsertedObject, pathToGetFile);
					if (pathReferenceFile == pathToGetFile){
						if (QFile::exists(pathReferenceFile)){

							// read data of reference file
							QFile referenceFile(pathReferenceFile);
							referenceFile.open(QIODevice::ReadOnly);
							QByteArray dataOfReferenceFile = referenceFile.readAll();
							referenceFile.close();
							QVERIFY2((dataOfReferenceFile == dataOfFileForInsert), "Getted file not equal inserted file, GetFile() is failed");

						}
						else{
							QFAIL("File not exist, GetFile() is failed");
						}
					}
					else {
						QFAIL("Path of getted file is incorrect, GetFile() is failed");
					}
					QFile::remove(pathReferenceFile);
				}
				else{
					QFAIL("Insert file is failed");
				}
			}
			else{
				QFAIL("File for insert in collection in test GetFile not exist, check FileForTestInsert.xml in TestData");
			}
		}
		else{
			QFAIL("File collection is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


void CFileCollectionPartituraTestBase::UpdateFileTest()
{
	// get component file collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
		if (fileCollectionPtr != nullptr){

			// reset data and announcement paths to files for tests
			fileCollectionPtr->ResetData();
			QString pathToInsertFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestInsert.xml";
			QString pathToUpdateFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestUpdateFile.xml";
			QString pathToGetFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestGet.xml";
			if (QFile::exists(pathToInsertFile) && QFile::exists(pathToUpdateFile)){

				// insert file in file collection
				QByteArray idInsertedObject = fileCollectionPtr->InsertFile(pathToInsertFile, m_typeIdObjectCollection, "TestName", "TestDescription");
				if (!idInsertedObject.isEmpty()){

					// update file in file collection and check this
					bool checkUpdateFile = fileCollectionPtr->UpdateFile(pathToUpdateFile, idInsertedObject);
					if (checkUpdateFile){

						// read data of file for update
						QFile fileForUpdate(pathToUpdateFile);
						fileForUpdate.open(QIODevice::ReadOnly);
						QByteArray dataOfFileForUpdate = fileForUpdate.readAll();
						fileForUpdate.close();

						// remove last getted file is exist
						if (QFile::exists(pathToGetFile)){
							QFile fileForTest(pathToGetFile);
							fileForTest.remove();
						}

						// get file from collection, read data and check update him
						QString pathToReferenceFile = fileCollectionPtr->GetFile(idInsertedObject, pathToGetFile);
						if ((pathToReferenceFile == pathToGetFile) && QFile::exists(pathToReferenceFile)){
							QFile referenceFile(pathToReferenceFile);
							referenceFile.open(QIODevice::ReadOnly);
							QByteArray dataOfReferenceFile = referenceFile.readAll();
							referenceFile.close();
							QVERIFY2((dataOfReferenceFile == dataOfFileForUpdate), "File not updated");
							referenceFile.remove();
						}
						else{
							QFAIL("Get file is failed");
						}
					}
					else{
						QFAIL("Update file is failed");
					}
				}
				else{
					QFAIL("Insert file is failed");
				}
			}
			else{
				QFAIL("Files for test UpdateFile not exist, check FileForTestInsert.xml and FileForTestUpdateFile.xml in TestData");
			}
		}
		else{
			QFAIL("File collection is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


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
//		ifile::IFilePersistence* filePersistance = compositePtr->GetComponentInterface<ifile::IFilePersistence>();
//		if (fileCollectionPtr != nullptr){
//			fileCollectionPtr->ResetData();
//			imtbase::IObjectCollection::DataPtr inputDataPtr = new imtauth::CAccountInfo();

//			// declaration values for object data in type AccountInfo
//			QString nameAccount = "AccountName";
//			QString descriptionAccount = "AccountDescription";

//			// set values in input data
//			imtauth::CAccountInfo* inputImplPtr = dynamic_cast<imtauth::CAccountInfo*>(inputDataPtr.GetPtr());
//			inputImplPtr->SetAccountName(nameAccount);
//			inputImplPtr->SetAccountDescription(descriptionAccount);
//			QString pathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestInsert.xml";
//			filePersistance->SaveToFile(*inputDataPtr.GetPtr(), pathToFile);
//			QByteArray idNewObject = fileCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestName", "TestDescription", inputDataPtr);
//			fileCollectionPtr->GetFile(idNewObject, pathToFile);
//			fileCollectionPtr->UpdateFile(pathToFile2, idNewObject);
//			fileCollectionPtr->GetFile(idNewObject, pathToFile2);

//		}
//		else{
//			QFAIL("File collection is nullptr");
//		}
//	}
//	else{
//		QFAIL("Component is not initialized");
//	}
//}
