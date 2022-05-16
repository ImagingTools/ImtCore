#include "CRevisionControllerPartituraTestBase.h"
#include "imtbase/IRevisionController.h"

// ImtCore includes
#include <imtrepo/IFileObjectCollection.h>
#include <imtauth/CAccountInfo.h>
#include <imtrepo/CFileCollectionCompBase.h>


void CRevisionControllerPartituraTestBase::BackupObjectWithCorrectParamsTest()
{
	// get component object collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){
			objectCollectionPtr->ResetData();

			// get revision controller
			const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
			if (revisionControllerPtr != nullptr){

				// insert new object in object collection
				QByteArray idInsertObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestName", "TestDescription");
				if (!idInsertObject.isEmpty()){

					// create backup object and check him
					QString commentForRevision = "my new revision";
					int revisionOfObject = revisionControllerPtr->BackupObject(*objectCollectionPtr, idInsertObject, commentForRevision);
					if (revisionOfObject >= 0){
						imtbase::IRevisionController::RevisionInfoList revisionList = revisionControllerPtr->GetRevisionInfoList(*objectCollectionPtr, idInsertObject);
						bool checkExistRevision = false;
						for (int indexRevision = 0; indexRevision < revisionList.size(); indexRevision++){
							if ((revisionList[indexRevision].revision == revisionOfObject) && (revisionList[indexRevision].comment == commentForRevision)){
								checkExistRevision = true;
							}
						}
						QVERIFY2(checkExistRevision, "Backup object is failed");
					}
					else{
						QFAIL("Backup object don't created");
					}
				}
				else{
					QFAIL("New object don't inserted");
				}
			}
			else{
				QFAIL("Revision controller is nullptr");
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


void CRevisionControllerPartituraTestBase::BackupObjectWithIncorrectParamsTest_data()
{
	initTestCase();

	// variable decloration
	QTest::addColumn<bool>("isNullPtrObjectCollection");
	QTest::addColumn<QByteArray>("objectId");
	QTest::addColumn<QString>("comment");

	// set values and description of test
	QTest::newRow("all param is empty") << true << QByteArray("") << "";
	QTest::newRow("object collection is nullptr") << true << QByteArray("testObject") << "TestComment";
	QTest::newRow("object id is empty") << false << QByteArray("") << "TestComment";
	QTest::newRow("object id is non-exist") << false << QByteArray("anotherTestObject") << "TestComment";
}


void CRevisionControllerPartituraTestBase::BackupObjectWithIncorrectParamsTest()
{
	// get values from rows
	QFETCH(bool, isNullPtrObjectCollection);
	QFETCH(QByteArray, objectId);
	QFETCH(QString, comment);

	// get component object collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){
			objectCollectionPtr->ResetData();

			// get revision controller
			const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
			if (revisionControllerPtr != nullptr){

				// insert new object in object collection
				QByteArray idInsertObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestName", "TestDescription", imtbase::CObjectCollectionBase::DataPtr(), QByteArray("testObject"));
				if (!idInsertObject.isEmpty() ){

					// create backup object and check him
					int revisionOfObject = 0;
					imtbase::IObjectCollection* nullObjectCollectionPtr = nullptr;
					if (isNullPtrObjectCollection){
						revisionOfObject = revisionControllerPtr->BackupObject(*nullObjectCollectionPtr, objectId, comment);
					}
					else{
						revisionOfObject = revisionControllerPtr->BackupObject(*objectCollectionPtr, objectId, comment);
					}
					QVERIFY2((revisionOfObject == -1), "Backup object is failed");
				}
				else{
					QFAIL("New object don't inserted");
				}
			}
			else{
				QFAIL("Revision controller is nullptr");
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


void CRevisionControllerPartituraTestBase::RestoreObjectWithCorrectParamsTest()
{
	// get component object collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){
			objectCollectionPtr->ResetData();

			// get revision controller
			const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
			if (revisionControllerPtr != nullptr){

				// declaration input and reference data of object
				imtbase::IObjectCollection::DataPtr inputDataPtr = new imtauth::CAccountInfo();
				imtbase::IObjectCollection::DataPtr referenceDataPtr;

				// declaration values for object data in type AccountInfo in first revision
				QString nameAccount = "AccountName";
				QString descriptionAccount = "AccountDescription";

				// set values in input data
				imtauth::CAccountInfo* inputImplPtr = dynamic_cast<imtauth::CAccountInfo*>(inputDataPtr.GetPtr());
				inputImplPtr->SetAccountName(nameAccount);
				inputImplPtr->SetAccountDescription(descriptionAccount);

				// insert new object in object collection
				QByteArray idInsertObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestName", "TestDescription", inputDataPtr);
				if (!idInsertObject.isEmpty()){

					// create backup object for first revision
					QString commentForFirstRevision = "my first revision";
					QString commentForSecondRevision = "my second revision";
					int idFirstRevision = revisionControllerPtr->BackupObject(*objectCollectionPtr, idInsertObject, commentForFirstRevision);

					// change data for second revision
					inputImplPtr->SetAccountName("ChangedName");
					inputImplPtr->SetAccountDescription("ChangedDescription");
					bool checkSetDataForSecondRevision = objectCollectionPtr->SetObjectData(idInsertObject, *inputDataPtr);
					if (checkSetDataForSecondRevision){

						// create backup object for second revision
						int idSecondRevision = revisionControllerPtr->BackupObject(*objectCollectionPtr, idInsertObject, commentForSecondRevision);
						if (idFirstRevision >= 0 && idSecondRevision >= 0){

							// restore object data from first revision and check data
							bool checkRestoreFirstRevision = revisionControllerPtr->RestoreObject(*objectCollectionPtr, idInsertObject, idFirstRevision);
							if (checkRestoreFirstRevision){
								objectCollectionPtr->GetObjectData(idInsertObject, referenceDataPtr);
								imtauth::CAccountInfo* referenceImplPtr = dynamic_cast<imtauth::CAccountInfo*>(referenceDataPtr.GetPtr());
								QString referenceName = referenceImplPtr->GetAccountName();
								QString referenceDescription = referenceImplPtr->GetAccountDescription();
								QVERIFY2(((nameAccount == referenceName) && (descriptionAccount == referenceDescription)), "Object don't restored");
							}
							else{
								QFAIL("Object revision don't restored");
							}
						}
						else{
							QFAIL("Backups don't created");
						}
					}
					else{
						QFAIL("Data for second revision don't set");
					}
				}
				else{
					QFAIL("New object don't inserted");
				}
			}
			else{
				QFAIL("Revision controller is nullptr");
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


void CRevisionControllerPartituraTestBase::RestoreObjectWithIncorrectParamsTest_data()
{
	initTestCase();

	// variable decloration
	QTest::addColumn<QByteArray>("objectId");
	QTest::addColumn<int>("revisionId");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QTest::newRow("all param is empty") << QByteArray("") << NULL << false;
	QTest::newRow("object id is empty") << QByteArray("") << 1 << false;
	QTest::newRow("object id is non-exist") << QByteArray("anotherTestObject") << 1 << false;
	QTest::newRow("revision id is empty") << QByteArray("testObject") << NULL << false;
	QTest::newRow("revision id is non_exist") << QByteArray("testObject") << -1 << false;
}


void CRevisionControllerPartituraTestBase::RestoreObjectWithIncorrectParamsTest()
{
	// get values from rows
	QFETCH(QByteArray, objectId);
	QFETCH(int, revisionId);
	QFETCH(bool, result);

	// get component object collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){
			objectCollectionPtr->ResetData();

			// get revision controller
			const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
			if (revisionControllerPtr != nullptr){

				// declaration input data of object
				imtbase::IObjectCollection::DataPtr inputDataPtr = new imtauth::CAccountInfo();

				// declaration values for object data in type AccountInfo in first revision
				QString nameAccount = "AccountName";
				QString descriptionAccount = "AccountDescription";

				// set values in input data
				imtauth::CAccountInfo* inputImplPtr = dynamic_cast<imtauth::CAccountInfo*>(inputDataPtr.GetPtr());
				inputImplPtr->SetAccountName(nameAccount);
				inputImplPtr->SetAccountDescription(descriptionAccount);

				// insert new object in object collection
				QByteArray idInsertObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestName", "TestDescription", inputDataPtr, "testObject");
				if (!idInsertObject.isEmpty()){

					// create backup object for first revision
					QString commentForFirstRevision = "my first revision";
					QString commentForSecondRevision = "my second revision";
					int idFirstRevision = revisionControllerPtr->BackupObject(*objectCollectionPtr, idInsertObject, commentForFirstRevision);

					// change data for second revision
					inputImplPtr->SetAccountName("ChangedName");
					inputImplPtr->SetAccountDescription("ChangedDescription");
					bool checkSetDataForSecondRevision = objectCollectionPtr->SetObjectData(idInsertObject, *inputDataPtr);
					if (checkSetDataForSecondRevision){

						// create backup object for second revision
						int idSecondRevision = revisionControllerPtr->BackupObject(*objectCollectionPtr, idInsertObject, commentForSecondRevision);
						if (idFirstRevision >= 0 && idSecondRevision >= 0){
							bool checkRestoreFirstRevision = revisionControllerPtr->RestoreObject(*objectCollectionPtr, objectId, revisionId);
							QVERIFY2(checkRestoreFirstRevision == result, "Restore object data from revision don't work");
						}
						else{
							QFAIL("Backups don't created");
						}
					}
					else{
						QFAIL("Data for second revision don't set");
					}
				}
				else{
					QFAIL("New object don't inserted");
				}
			}
			else{
				QFAIL("Revision controller is nullptr");
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


void CRevisionControllerPartituraTestBase::ExportObjectWithCorrectParamsTest()
{
	// get component object collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){
			objectCollectionPtr->ResetData();

			// get revision controller
			const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
			if (revisionControllerPtr != nullptr){

				// get file collection
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

					// insert file in collection
					QString pathToImportFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/RevisionControllerTest/TestData/FileForInsert.xml";
					QByteArray idInsertObject = fileCollectionPtr->InsertFile(pathToImportFile, m_typeIdObjectCollection, "TestName", "TestDescription");
					if (!idInsertObject.isEmpty()){

						// create backup object for first revision
						QString commentForFirstRevision = "my first revision";
						int idFirstRevision = revisionControllerPtr->BackupObject(*objectCollectionPtr, idInsertObject, commentForFirstRevision);
						if (idFirstRevision >= 0){

							// export object to file
							QString pathToExportFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/RevisionControllerTest/TestData/FileForTestExport.xml";
							bool checkExportObject = revisionControllerPtr->ExportObject(*objectCollectionPtr, idInsertObject, idFirstRevision, pathToExportFile);
							if (checkExportObject){

								// read data of file for import
								QFile importFile(pathToImportFile);
								importFile.open(QIODevice::ReadOnly);
								QByteArray dataOfFileForImport = importFile.readAll();
								importFile.close();

								// read data of file for export
								QFile exportFile(pathToExportFile);
								exportFile.open(QIODevice::ReadOnly);
								QByteArray dataOfFileForExport = exportFile.readAll();
								exportFile.close();
								exportFile.remove();

								// check working export to file
								QVERIFY2((dataOfFileForImport == dataOfFileForExport), "Export object to file is failed");
							}
							else{
								QFAIL("Export object to file is failed");
							}
						}
						else{
							QFAIL("Backups don't created");
						}
					}
					else{
						QFAIL("Object don't inserted");
					}
				}
				else{
					QFAIL("File collection is nullptr");
				}
			}
			else{
				QFAIL("Revision controller is nullptr");
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


void CRevisionControllerPartituraTestBase::ExportObjectWithIncorrectParamsTest_data()
{
	initTestCase();

	// variable decloration
	QTest::addColumn<bool>("isNullPtrObjectCollection");
	QTest::addColumn<QByteArray>("objectId");
	QTest::addColumn<int>("revisionId");
	QTest::addColumn<QString>("pathToFile");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QString pathToExportFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/RevisionControllerTest/TestData/FileForTestExport.xml";
	QString incorrectPathToExportFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/RevisionControllerTest/|\?'eTestData/FileForTestExport.txt";
	QTest::newRow("all param is empty") << true << QByteArray("") << NULL << "" << false;
	QTest::newRow("object collection is nullptr") << true << QByteArray("testObject") << 1 << pathToExportFile << false;
	QTest::newRow("object id is empty")  << false << QByteArray("") << 1 << pathToExportFile << false;
	QTest::newRow("object id is non-exist") << false << QByteArray("anotherTestObject") << 1 << pathToExportFile << false;
	QTest::newRow("revision id is empty") << false << QByteArray("testObject") << NULL << pathToExportFile << false;
	QTest::newRow("revision id is non_exist") << false << QByteArray("testObject") << -1 << pathToExportFile << false;
	QTest::newRow("incorrect path to file") << false << QByteArray("testObject") << 1 << incorrectPathToExportFile << false;
}


void CRevisionControllerPartituraTestBase::ExportObjectWithIncorrectParamsTest()
{
	// get values from rows
	QFETCH(bool, isNullPtrObjectCollection);
	QFETCH(QByteArray, objectId);
	QFETCH(int, revisionId);
	QFETCH(QString, pathToFile);
	QFETCH(bool, result);

	// get component object collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){
			objectCollectionPtr->ResetData();

			// get revision controller
			const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
			if (revisionControllerPtr != nullptr){

				// get file collection
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

					// insert file in collection
					QString pathToImportFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/RevisionControllerTest/TestData/FileForInsert.xml";
					QByteArray idInsertObject = fileCollectionPtr->InsertFile(pathToImportFile, m_typeIdObjectCollection, "TestName", "TestDescription");
					if (!idInsertObject.isEmpty()){

						// create backup object for first revision
						QString commentForFirstRevision = "my first revision";
						int idFirstRevision = revisionControllerPtr->BackupObject(*objectCollectionPtr, idInsertObject, commentForFirstRevision);
						if (idFirstRevision >= 0){

							// export object to file
							QString pathToExportFile = qEnvironmentVariable("IMTCOREDIR")+"/Tests/RevisionControllerTest/TestData/FileForTestExport.xml";
							imtbase::IObjectCollection* nullObjectCollectionPtr = nullptr;
							bool checkExportObject = true;
							if (isNullPtrObjectCollection){
								checkExportObject = revisionControllerPtr->ExportObject(*nullObjectCollectionPtr, objectId, revisionId, pathToFile);
							}
							else{
								checkExportObject = revisionControllerPtr->ExportObject(*objectCollectionPtr, objectId, revisionId, pathToFile);
							}
							QVERIFY2(checkExportObject == result, "Export object to file is failed");

						}
						else{
							QFAIL("Backups don't created");
						}
					}
					else{
						QFAIL("Object don't inserted");
					}
				}
				else{
					QFAIL("File collection is nullptr");
				}
			}
			else{
				QFAIL("Revision controller is nullptr");
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


void CRevisionControllerPartituraTestBase::cleanupTestCase()
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
			filter.append("*TestName*");
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
