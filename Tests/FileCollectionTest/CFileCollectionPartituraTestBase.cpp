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
	QString incorrectPathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/|\?'eTestData/FileForTestInsert.txt";
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

			// check 'xml' persistence in accessor
			const ifile::IFileResourceTypeConstraints *constraints = fileCollectionPtr->GetFileTypeConstraints();
			const ifile::IFileTypeInfo *typeInfo = constraints->GetFileTypeInfo(0);
			QStringList typesList;
			typeInfo->GetFileExtensions(typesList);
			if (!typesList.contains("xml")){
				QSKIP("Accessor don't have 'xml' extension. Add him to Persistence in accessor");
			}

			// check insert file
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


void CFileCollectionPartituraTestBase::GetFileWithCorrectParamsTest()
{
	// get component file collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
		if (fileCollectionPtr != nullptr){

			// reset data
			fileCollectionPtr->ResetData();

			// check 'xml' persistence in accessor
			const ifile::IFileResourceTypeConstraints *constraints = fileCollectionPtr->GetFileTypeConstraints();
			const ifile::IFileTypeInfo *typeInfo = constraints->GetFileTypeInfo(0);
			QStringList typesList;
			typeInfo->GetFileExtensions(typesList);
			if (!typesList.contains("xml")){
				QSKIP("Accessor don't have 'xml' extension. Add him to Persistence in accessor");
			}

			// announcement paths to files for tests
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


void CFileCollectionPartituraTestBase::GetFileWithIncorrectParamsTest_data()
{
	initTestCase();
	// variable decloration
	QTest::addColumn<QByteArray>("objectId");
	QTest::addColumn<QString>("filePath");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QString pathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestInsert.xml";
	QString incorrectPathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/|\?'eTestData/FileForTestInsert.txt";
	QTest::newRow("all param is empty") << QByteArray("") << "" << true;
	QTest::newRow("empty id object") << QByteArray("") << pathToFile << true;
	QTest::newRow("non exist id object") << QByteArray("anotherTestObject") << pathToFile << true;
	QTest::newRow("incorrect path") << QByteArray("TestObject") << incorrectPathToFile << true;
}


void CFileCollectionPartituraTestBase::GetFileWithIncorrectParamsTest()
{
	// get values from rows
	QFETCH(QByteArray, objectId);
	QFETCH(QString, filePath);
	QFETCH(bool, result);

	// get component file collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
		if (fileCollectionPtr != nullptr){

			// reset data
			fileCollectionPtr->ResetData();

			// check 'xml' persistence in accessor
			const ifile::IFileResourceTypeConstraints *constraints = fileCollectionPtr->GetFileTypeConstraints();
			const ifile::IFileTypeInfo *typeInfo = constraints->GetFileTypeInfo(0);
			QStringList typesList;
			typeInfo->GetFileExtensions(typesList);
			if (!typesList.contains("xml")){
				QSKIP("Accessor don't have 'xml' extension. Add him to Persistence in accessor");
			}

			// announcement paths to files for tests
			QString pathToInsertFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestInsert.xml";
			if (QFile::exists(pathToInsertFile)){
				QString pathToGetFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestGet.xml";

				// insert file in file collection
				QByteArray idInsertedObject = fileCollectionPtr->InsertFile(pathToInsertFile, m_typeIdObjectCollection, "TestName", "TestDescription", QByteArray("TestObject"));
				if (!idInsertedObject.isEmpty()){

					//remove last getted file is exist
					if (QFile::exists(pathToGetFile)){
						QFile fileForTest(pathToGetFile);
						fileForTest.remove();
					}

					// check result of GetFile()
					QString pathReferenceFile = fileCollectionPtr->GetFile(objectId, filePath);
					QVERIFY2((pathReferenceFile.isEmpty() == result), "GetFile with non-exist object is failed");
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


void CFileCollectionPartituraTestBase::UpdateFileWithCorrectParamsTest()
{
	// get component file collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
		if (fileCollectionPtr != nullptr){

			// reset data
			fileCollectionPtr->ResetData();

			// check 'xml' persistence in accessor
			const ifile::IFileResourceTypeConstraints *constraints = fileCollectionPtr->GetFileTypeConstraints();
			const ifile::IFileTypeInfo *typeInfo = constraints->GetFileTypeInfo(0);
			QStringList typesList;
			typeInfo->GetFileExtensions(typesList);
			if (!typesList.contains("xml")){
				QSKIP("Accessor don't have 'xml' extension. Add him to Persistence in accessor");
			}

			// announcement paths to files for tests
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


void CFileCollectionPartituraTestBase::UpdateFileWithIncorrectParamsTest_data()
{
	initTestCase();
	// variable decloration
	QTest::addColumn<QString>("filePath");
	QTest::addColumn<QByteArray>("objectId");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QString pathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestUpdateFile.xml";
	QString incorrectPathToFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/|\?'eTestData/FileForTestInsert.txt";
	QTest::newRow("all param is empty") << "" << QByteArray("") << false;
	QTest::newRow("empty path") << "" << QByteArray("TestObject") << false;
	QTest::newRow("empty id object") << pathToFile  << QByteArray("") << false;
	QTest::newRow("incorrect path") << incorrectPathToFile  << QByteArray("TestObject") << false;
	QTest::newRow("non-exist id object") << incorrectPathToFile << QByteArray("anotherTestObject") << false;
}


void CFileCollectionPartituraTestBase::UpdateFileWithIncorrectParamsTest()
{
	// get values from rows
	QFETCH(QString, filePath);
	QFETCH(QByteArray, objectId);
	QFETCH(bool, result);

	// get component file collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
		if (fileCollectionPtr != nullptr){

			// reset data
			fileCollectionPtr->ResetData();

			// check 'xml' persistence in accessor
			const ifile::IFileResourceTypeConstraints *constraints = fileCollectionPtr->GetFileTypeConstraints();
			const ifile::IFileTypeInfo *typeInfo = constraints->GetFileTypeInfo(0);
			QStringList typesList;
			typeInfo->GetFileExtensions(typesList);
			if (!typesList.contains("xml")){
				QSKIP("Accessor don't have 'xml' extension. Add him to Persistence in accessor");
			}

			// announcement paths to files for tests
			QString pathToInsertFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/FileCollectionTest/TestData/FileForTestInsert.xml";
			if (QFile::exists(pathToInsertFile)){

				// insert file in file collection
				QByteArray idInsertedObject = fileCollectionPtr->InsertFile(pathToInsertFile, m_typeIdObjectCollection, "TestName", "TestDescription", "TestObject");
				if (!idInsertedObject.isEmpty()){

					// update file in file collection and check this
					bool checkUpdateFile = fileCollectionPtr->UpdateFile(filePath, objectId);
					QCOMPARE(checkUpdateFile, result);
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


void CFileCollectionPartituraTestBase::GetFileInfoTest()
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

			// check 'xml' persistence in accessor
			const ifile::IFileResourceTypeConstraints *constraints = fileCollectionPtr->GetFileTypeConstraints();
			const ifile::IFileTypeInfo *typeInfo = constraints->GetFileTypeInfo(0);
			QStringList typesList;
			typeInfo->GetFileExtensions(typesList);
			if (!typesList.contains("xml")){
				QSKIP("Accessor don't have 'xml' extension. Add him to Persistence in accessor");
			}

			// insert file in file collection
			QString nameInsertFile = "TestName";
			QByteArray idInsertedObject = fileCollectionPtr->InsertFile(pathToInsertFile, m_typeIdObjectCollection, nameInsertFile, "TestDescription");
			if (!idInsertedObject.isEmpty()){

				// get info of inserted file
				imtrepo::IFileObjectCollection::FileInfo fileInfo = fileCollectionPtr->GetFileInfo(idInsertedObject);
				if (QFile::exists(fileInfo.filePath)){

					// read data of file for insert
					QFile fileForInsert(pathToInsertFile);
					fileForInsert.open(QIODevice::ReadOnly);
					QByteArray dataOfFileForInsert = fileForInsert.readAll();
					fileForInsert.close();

					// read data of file from fileinfo
					QFile referenceFile(fileInfo.filePath);
					referenceFile.open(QIODevice::ReadOnly);
					QByteArray dataReferenceFile = referenceFile.readAll();
					referenceFile.close();

					// check GetFileInfo()
					QCOMPARE(dataReferenceFile, dataOfFileForInsert);
					QVERIFY2(fileInfo.fileName.contains(nameInsertFile), "FileInfo have incorrect filename, GetFileInfo failed");
				}
				else{
					QFAIL("Inserted file have incorrect path to file in info, GetFileInfo() is failed");
				}
			}
			else{
				QFAIL("Insert file is failed");
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

void CFileCollectionPartituraTestBase::cleanupTestCase()
{
	// get component object collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get file collection
		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
		if (fileCollectionPtr != nullptr){
			QString folderPath = fileCollectionPtr->GetCollectionRootFolder();
			QDir folder(folderPath);
			QStringList filter;
			filter.append("*TestFileName*");
			filter.append("*TestName*");
			filter.append(" *");
			folder.setNameFilters(filter);
			QStringList foldersForRemove = folder.entryList();
			for (int i = 0; i < foldersForRemove.size(); i++){
				QDir currentDir(folderPath + "/" + foldersForRemove[i]);
				currentDir.removeRecursively();
				folder.rmdir(foldersForRemove[i]);
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
