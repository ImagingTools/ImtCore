#include "CRevisionControllerPartituraTestBase.h"
#include "imtbase/IRevisionController.h"

// ImtCore includes
#include <imtrepo/IFileObjectCollection.h>
#include <imtauth/CAccountInfo.h>


void CRevisionControllerPartituraTestBase::BackupObjectTestWithCorrectParams()
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


void CRevisionControllerPartituraTestBase::BackupObjectTestWithIncorrectParams_data()
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


void CRevisionControllerPartituraTestBase::BackupObjectTestWithIncorrectParams()
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


//void CRevisionControllerPartituraTestBase::RestoreObjectTestWithCorrectParams()
//{
//	// get component object collection
//	initTestCase();
//	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
//	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
//	if (compositePtr.IsValid()){
//		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
//		if (objectCollectionPtr != nullptr){
//			objectCollectionPtr->ResetData();

//			// get revision controller
//			const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
//			if (revisionControllerPtr != nullptr){

//				// insert new object in object collection
//				QByteArray idInsertObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestName", "TestDescription");
//				if (!idInsertObject.isEmpty()){

//					// create backup object and check him
//					QString commentForFirstRevision = "my first revision";
//					QString commentForSecondRevision = "my second revision";
//					int idFirstRevision = revisionControllerPtr->BackupObject(*objectCollectionPtr, idInsertObject, commentForFirstRevision);
//					objectCollectionPtr->SetObjectName(idInsertObject, "ChangedName");
//					int idSecondRevision = revisionControllerPtr->BackupObject(*objectCollectionPtr, idInsertObject, commentForSecondRevision);
//					if (idFirstRevision >= 0 && idSecondRevision >= 0){
//						bool checkRestoreFirstRevision = revisionControllerPtr->RestoreObject(*objectCollectionPtr, idInsertObject, idSecondRevision);
//						if (checkRestoreFirstRevision){
//							imtbase::IRevisionController::RevisionInfoList revisionList = revisionControllerPtr->GetRevisionInfoList(*objectCollectionPtr, idInsertObject);
//							bool checkExistFirstRevision = false;
//							bool checkExistSecondRevision = false;
//							for (int indexRevision = 0; indexRevision < revisionList.size(); indexRevision++){
//								if ((revisionList[indexRevision].revision == idFirstRevision) && (revisionList[indexRevision].comment == commentForFirstRevision)){
//									checkExistFirstRevision = true;
//								}
//								if ((revisionList[indexRevision].revision == idSecondRevision) && (revisionList[indexRevision].comment == commentForSecondRevision)){
//									checkExistSecondRevision = true;
//								}
//							}
//							QVERIFY2((!checkExistFirstRevision && checkExistSecondRevision), "Backup object is failed");
//						}
//						else{
//							QFAIL("Object revision don't restored");
//						}
//					}
//					else{
//						QFAIL("Backups don't created");
//					}
//				}
//				else{
//					QFAIL("New object don't inserted");
//				}
//			}
//			else{
//				QFAIL("Revision controller is nullptr");
//			}
//		}
//		else{
//			QFAIL("Object collection is nullptr");
//		}
//	}
//	else{
//		QFAIL("Component is not initialized");
//	}
//}
