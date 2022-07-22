#include "CObjectCollectionPartituraTestBase.h"
#include "imtbase/CCollectionFilter.h"
#include <iprm/CParamsSet.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <imtauth/CAccountInfo.h>
#include <imtauth/CAccountInfoMetaInfo.h>
#include <imtauth/IAccountInfo.h>
#include <imtrepo/IFileObjectCollection.h>
#include <imtdb/IDatabaseObjectDelegate.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/IDatabaseLoginSettings.h>
#include <imttest/CTestInfo.h>
#include <imttest/ITestInfo.h>
#include <imttest/CTestMetaInfo.h>


void CObjectCollectionPartituraTestBase::cleanup()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		imtrepo::IFileObjectCollection* fileCollectionPtr = compositePtr->GetComponentInterface<imtrepo::IFileObjectCollection>();
		if (fileCollectionPtr != nullptr){
			QString folderPath = fileCollectionPtr->GetCollectionRootFolder();
			QDir folder(folderPath);
			QStringList filter;
			filter.append("*Test*");
			filter.append(" - *");
			folder.setNameFilters(filter);
			QStringList foldersForRemove = folder.entryList();
			for (int i = 0; i < foldersForRemove.size(); i++){
				QDir currentDir(folderPath + "/" + foldersForRemove[i]);
				currentDir.removeRecursively();
				folder.rmdir(foldersForRemove[i]);
			}
		}
		imtdb::IDatabaseEngine* databaseEnginePtr = compositePtr->GetComponentInterface<imtdb::IDatabaseEngine>();
		if (databaseEnginePtr != nullptr){
			QSqlError sqlError;
			QSqlQuery sqlQuery = databaseEnginePtr->ExecSqlQuery(QByteArray("DELETE FROM public.tests CASCADE;"), &sqlError);
		}
	}
}


void CObjectCollectionPartituraTestBase::InsertNewObjectWithRequiredParamsTest_data()
{
	initTestCase();
	QWARN(qPrintable(QString("Start tests for %1 accessor").arg(m_nameAccessor)));

	// variable decloration
	QTest::addColumn<QByteArray>("typeId");
	QTest::addColumn<QString>("name");
	QTest::addColumn<QString>("description");
	QTest::addColumn<QString>("nameInData");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QTest::newRow("all param is empty") << QByteArray("") << "" << "" << "" << true;
	QTest::newRow("type param is empty") << QByteArray("") << "TestObjectName" << "Insert object with empty type id" << "AccountName1" << true;
	QTest::newRow("name param is empty") << m_typeIdObjectCollection << "" << "Insert object with empty name" << "AccountName2" << false;
	QTest::newRow("description param is empty") << m_typeIdObjectCollection << "TestObjectName" << "" << "AccountName3" << false;
	QTest::newRow("type id is not exist") << QByteArray("AnotherTypeId") << "TestObjectName" << "Insert object with non-exist type id" << "AccountName4" << true;
	QTest::newRow("type id is exist") << m_typeIdObjectCollection << "TestObjectName" << "Insert object with exist type id and all params is setted" << "AccountName5" << false;
}


void CObjectCollectionPartituraTestBase::InsertNewObjectWithRequiredParamsTest()
{
	// get values from rows
	QFETCH(QByteArray, typeId);
	QFETCH(QString, name);
	QFETCH(QString, description);
	QFETCH(QString, nameInData);
	QFETCH(bool, result);
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// reset data from object collection
			objectCollectionPtr->ResetData();

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


void CObjectCollectionPartituraTestBase::InsertNewObjectWithNonExistElementTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// reset data from object collection
			objectCollectionPtr->ResetData();

			// insert object in collection
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::CObjectCollection::DataPtr(), "testId");

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


void CObjectCollectionPartituraTestBase::GetDataFromEmpyObjectIdTest()
{
    initTestCase();
    istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
    compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
    if (compositePtr.IsValid()){

        // get component object collection
        imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
        if (objectCollectionPtr != nullptr){

            // reset data from object collection
            objectCollectionPtr->ResetData();

			// insert objects in collection
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::CObjectCollection::DataPtr(), QByteArray(""));
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::CObjectCollection::DataPtr(), QByteArray("1_object"));

            imtbase::CObjectCollection::DataPtr dataPtr = nullptr;
			bool checkGetData = objectCollectionPtr->GetObjectData(QByteArray(""), dataPtr);

			QVERIFY2(!checkGetData, "GetData from object with empty Id is failed");
        }
    }
}


void CObjectCollectionPartituraTestBase::InsertNewObjectWithExistElementTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){

			// reset data from object collection
			objectCollectionPtr->ResetData();

			// insert first object in collection
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::CObjectCollection::DataPtr(), "testId");

			if (idNewObject == "testId"){

				// check contains first object in collection
				imtbase::IObjectCollection::Ids idsInObject = objectCollectionPtr->GetElementIds();
				bool checkInsertFirstObject = idsInObject.contains("testId");

				if(checkInsertFirstObject){

					// insert second object with exist id in collection
					QByteArray idSecondNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject2", "TestDescription2", imtbase::CObjectCollection::DataPtr(), "testId");

					if (!idSecondNewObject.isEmpty()){

						// check contains insert objects
						idsInObject = objectCollectionPtr->GetElementIds();
						checkInsertFirstObject = idsInObject.contains("testId");
						bool checkInsertSecondObject = idsInObject.contains(idSecondNewObject);

						// if one of conditions is false - test will be failed
						QVERIFY2((checkInsertFirstObject && checkInsertSecondObject), "Insert new object with set exist id is failed");
					}
					else{
						QFAIL("Insert second object in collection is failed");
					}
				}
				else{
					QFAIL("Insert first object in collection is failed");
				}
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


void CObjectCollectionPartituraTestBase::InsertNewObjectWithDataTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// reset data from object collection
			objectCollectionPtr->ResetData();

			// declaration input and reference data of object
			imtbase::IObjectCollection::DataPtr inputDataPtr;
			imtbase::IObjectCollection::DataPtr referenceDataPtr;
			if (m_typeIdObjectCollection == "AccountInfo"){
				inputDataPtr = new imtauth::CAccountInfo();
			}
			else{
				inputDataPtr = new imttest::CTestInfo();
			}

			// declaration values for object data in type AccountInfo
			QString nameAccount = "AccountName9";
			QString descriptionAccount = "AccountDescription";
			QString setNameAccount;
			QString setDescriptionAccount;

			// set values in input data
			if (m_typeIdObjectCollection == "AccountInfo"){
				imtauth::CAccountInfo* inputImplPtr = dynamic_cast<imtauth::CAccountInfo*>(inputDataPtr.GetPtr());
				inputImplPtr->SetAccountName(nameAccount);
				inputImplPtr->SetAccountDescription(descriptionAccount);
			}
			else{
				imttest::CTestInfo* inputImplPtr = dynamic_cast<imttest::CTestInfo*>(inputDataPtr.GetPtr());
				inputImplPtr->SetTestName(nameAccount);
				inputImplPtr->SetTestDescription(descriptionAccount);
			}

			// insert new object with input data in collection
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", inputDataPtr);
			if (!idNewObject.isEmpty()){

				// get reference data
				if(objectCollectionPtr->GetObjectData(idNewObject, referenceDataPtr)){

					// create reference Impl
					if (m_typeIdObjectCollection == "AccountInfo"){
						imtauth::CAccountInfo* referenceImplPtr = dynamic_cast<imtauth::CAccountInfo*>(referenceDataPtr.GetPtr());

						// get values from reference data
						setNameAccount = referenceImplPtr->GetAccountName();
						setDescriptionAccount = referenceImplPtr->GetAccountDescription();
					}

					else{
						imttest::CTestInfo* referenceImplPtr = dynamic_cast<imttest::CTestInfo*>(referenceDataPtr.GetPtr());
						// get values from reference data
						setNameAccount = referenceImplPtr->GetTestName();
						setDescriptionAccount = referenceImplPtr->GetTestDescription();
					}

					// compare input & reference data
					QVERIFY2(((nameAccount == setNameAccount) && (descriptionAccount == setDescriptionAccount)), "Insert new object with data is failed");

				}
				else{
					QFAIL("Data is null, Insert object with data failed");
				}
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


void CObjectCollectionPartituraTestBase::InsertNewObjectWithMetaObjectTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// reset data from object collection
			objectCollectionPtr->ResetData();

			QString inputName = "AccountName10";
			QString inputDescription = "AccountDescription";
			QByteArray idNewObject;

			if (m_typeIdObjectCollection == "AccountInfo"){

				// input params of meta
				imtauth::CAccountInfoMetaInfo inputMetaInfo;

				// set input params of meta
				inputMetaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_TYPE, QObject::tr("Private"));
				inputMetaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_NAME, inputName);
				inputMetaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_DESCRIPTION, inputDescription);
				idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::CObjectCollection::DataPtr(), QByteArray(), &inputMetaInfo);
			}
			else{
				imttest::CTestMetaInfo inputMetaInfo;

				inputMetaInfo.SetMetaInfo(imttest::ITestInfo::MIT_TEST_NAME, inputName);
				inputMetaInfo.SetMetaInfo(imttest::ITestInfo::MIT_TEST_DESCRIPTION, inputDescription);
				idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::CObjectCollection::DataPtr(), QByteArray(), &inputMetaInfo);
			}

			if (!idNewObject.isEmpty()){
				idoc::MetaInfoPtr referenceMetaInfoPtr = objectCollectionPtr->GetDataMetaInfo(idNewObject);
				QString referenceName;
				QString referenceDescription;
				if (referenceMetaInfoPtr.IsValid()){
					if (m_typeIdObjectCollection == "AccountInfo"){
						referenceName = referenceMetaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_NAME).toString();
						referenceDescription = referenceMetaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_DESCRIPTION).toString();
						QVERIFY2(((inputName == referenceName) && (inputDescription == referenceDescription)), "Insert new object with meta data is failed");
					}
					else{
						referenceName = referenceMetaInfoPtr->GetMetaInfo(imttest::ITestInfo::MIT_TEST_NAME).toString();
						referenceDescription = referenceMetaInfoPtr->GetMetaInfo(imttest::ITestInfo::MIT_TEST_DESCRIPTION).toString();
						QVERIFY2(((inputName == referenceName) && (inputDescription == referenceDescription)), "Insert new object with meta data is failed");
					}
				}
				else{
					QFAIL("Meta info not received");
				}
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


void CObjectCollectionPartituraTestBase::RemoveExistObjectTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();

		if (objectCollectionPtr != nullptr){
			// Reset collection:
			objectCollectionPtr->ResetData();

			// Insert two objects into the collection:
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::CObjectCollection::DataPtr(), "testId");
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject2", "TestDescription2", imtbase::CObjectCollection::DataPtr(), "testId2");

			// Remove object with the ID 'testId':
			objectCollectionPtr->RemoveElement("testId");

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


void CObjectCollectionPartituraTestBase::RemoveNonExistObjectTest()
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

			// Insert two objects into the collection:
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::CObjectCollection::DataPtr(), "testId");
			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject2", "TestDescription2", imtbase::CObjectCollection::DataPtr(), "testId2");

			// Remove object with non-exist id
			objectCollectionPtr->RemoveElement("testId3");

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


void CObjectCollectionPartituraTestBase::ResetCollectionWithoutFixedObjectsTest()
{
	initTestCase();
	// declaration component accessor
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::CObjectCollection::DataPtr(), "testId");

			// Reset collection
			objectCollectionPtr->ResetData();

			//Check remove inserted object from collection
			imtbase::IObjectCollection::Ids idsInObjectAfterReset = objectCollectionPtr->GetElementIds();
			bool checkDeleteAddedObject = !idsInObjectAfterReset.contains("testId");
			QVERIFY2(checkDeleteAddedObject, "Reset objects from collection without fixed object is failed");
		}
		else{
			QFAIL("Object collection is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


void CObjectCollectionPartituraTestBase::SetObjectNameTest()
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

			// insert new object, set new name and check
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription");
			if (!idNewObject.isEmpty()){
				QString changedName = "ChangedTestNameObject";
				objectCollectionPtr->SetElementName(idNewObject, changedName);
				QString objectName = objectCollectionPtr->GetElementInfo(idNewObject, imtbase::ICollectionInfo::EIT_NAME).toString();
				QVERIFY2(((changedName == objectName) && (objectName != "TestObject")), "Name object don't changed");
			}
			else{
				QFAIL("Object don't insert");
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

void CObjectCollectionPartituraTestBase::SetObjectDescriptionTest()
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

			// insert new object, set new description and check
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription");
			if (!idNewObject.isEmpty()){
				QString changedDescription = "ChangedDescriptionObject";
				objectCollectionPtr->SetElementDescription(idNewObject, changedDescription);
				QString objectDescription = objectCollectionPtr->GetElementInfo(idNewObject, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
				QVERIFY2(((changedDescription == objectDescription) && (objectDescription != "TestDescription")), "Description object don't changed");
			}
			else{
				QFAIL("Object don't insert");
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

void CObjectCollectionPartituraTestBase::GetObjectTypeIdTest()
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

			// insert new object and check type id
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription");
			if (!idNewObject.isEmpty()){
				imtbase::ICollectionInfo::Id gettedObjectTypeId = objectCollectionPtr->GetObjectTypeId(idNewObject);
				QVERIFY2((gettedObjectTypeId == m_typeIdObjectCollection), "Type id of object is incorrect");

			}
			else{
				QFAIL("Object don't insert");
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


//void CObjectCollectionPartituraTestBase::GetOperationFlagsInsertedObjectTest()
//{
//	initTestCase();
//	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
//	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
//	if (compositePtr.IsValid()){

//		// get component object collection
//		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
//		if (objectCollectionPtr != nullptr){

//			// Reset collection
//			objectCollectionPtr->ResetData();

//			// insert new object and check flags him
//			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription");
//			if (!idNewObject.isEmpty()){
//				int flagsInsertedObject = objectCollectionPtr->GetOperationFlags(idNewObject);
//				QVERIFY2((flagsInsertedObject == (imtbase::IObjectCollection::OF_ALL & ~imtbase::IObjectCollection::OF_SUPPORT_PAGINATION)), "Operation flags of inserted object is incorrect");
//			}
//			else{
//				QFAIL("Object don't insert");
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


void CObjectCollectionPartituraTestBase::GetElementsCountTest_data()
{
	initTestCase();

	// variable decloration
	QTest::addColumn<QString>("textFilter");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QTest::newRow("textFilter is empty") << "" << false;
	QTest::newRow("textFilter is non-exist") << "AnotherName" << true;
	QTest::newRow("textFilter exist") << "TestFilterName1" << false;
}


void CObjectCollectionPartituraTestBase::GetElementsCountTest()
{
	// get values from rows
	QFETCH(QString, textFilter);
	QFETCH(bool, result);

	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// Reset collection
			objectCollectionPtr->ResetData();

			if (m_typeIdObjectCollection == "TestInfo"){
				imtbase::IObjectCollection::Ids idsInObject = objectCollectionPtr->GetElementIds();
				if (!idsInObject.isEmpty()){
					for (int i = 0; i < idsInObject.count(); i++){
						objectCollectionPtr->RemoveElement(idsInObject[i]);
					}
				}
			}

			// Set text filter
			imtbase::CCollectionFilter filter;
			filter.SetTextFilter(textFilter);
			QByteArrayList filteringIds;
			filteringIds.append("Name");
			filter.SetFilteringInfoIds(filteringIds);
			iprm::CParamsSet filterParams;
			filterParams.SetEditableParameter("Filter", &filter);

			// Insert 20 objects in collection
			for (int i = 0; i < 20; i++){
				if (i%2 == 0){
					objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, QString("TestFilterName1 %1").arg(i), QString("TestDescription %1").arg(i));
				}
				else{
					objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, QString("TestFilterName2 %1").arg(i), QString("TestDescription %1").arg(i));
				}
			}

			// check count
			int count = objectCollectionPtr->GetElementsCount(&filterParams);
			if (count >= 0){
				if (count == 0){
					QVERIFY2(result, "Incorrect count of objects");
				}
				else{
					if (textFilter == ""){
						QVERIFY2((count == 20) , "Incorrect count of objects");
					}
					else if(textFilter == "TestFilterName1"){
						QVERIFY2((count == 10) , "Incorrect count of objects");
					}
					else{
						QFAIL("Incorrect count of objects");
					}
				}
			}
			else{
				QFAIL("Incorrect count of objects");
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


void CObjectCollectionPartituraTestBase::PaginationTest_data()
{
	initTestCase();

	// variable decloration
	QTest::addColumn<int>("offset");
	QTest::addColumn<int>("count");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QTest::newRow("offset is NULL") << NULL << 5 << false;
	QTest::newRow("offset out of size") << 100 << 5 << true;
	QTest::newRow("count is NULL")<< 1 << NULL << true;
	QTest::newRow("count out of size") << 1 << 100 << false;
	QTest::newRow("all params correct") << 1 << 100 << false;
//	QTest::newRow("count is negative") << 1 << -5 << true; for sql
//	QTest::newRow("offset is negative") << -5 << 5 << true;
}


void CObjectCollectionPartituraTestBase::PaginationTest()
{
	// get values from rows
	QFETCH(int, offset);
	QFETCH(int, count);
	QFETCH(bool, result);

	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// Reset collection
			objectCollectionPtr->ResetData();

			// Insert 10 objects in collection
			for (int i = 0; i < 10; i++){
				objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, QString("TestName %1").arg(i), QString("TestDescription %1").arg(i));
			}

			// check pagination
			imtbase::ICollectionInfo::Ids collectionIds = objectCollectionPtr->GetElementIds(offset, count);
			if (collectionIds.isEmpty()){
				QVERIFY2(result, "Pagination object collection is failed");
			}
			else{

				// check correct offset
				bool isCorrectOffset = false;
				imtbase::ICollectionInfo::Ids allCollectionIds = objectCollectionPtr->GetElementIds();
				if(allCollectionIds[offset] == collectionIds[0]){
					isCorrectOffset = true;
				}
				else{
					QFAIL("Offset in pagination is incorrect");
				}

				// check correct count
				bool isCorrectCount = false;
				if((count < 0) && (collectionIds.count() > 0)){
					QFAIL("Count objects is not null with set negative param 'count'");
				}
				else{
					if (collectionIds.count() <= count){
						isCorrectCount = true;
					}
					else{
						QFAIL("Count in pagination is incorrect");
					}
					QVERIFY2((isCorrectOffset && isCorrectCount), "Pagination object collection is failed");
				}
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


void CObjectCollectionPartituraTestBase::FilterTest_data()
{
	initTestCase();

	// variable decloration
	QTest::addColumn<QString>("textFilter");
	QTest::addColumn<QByteArray>("column");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QTest::newRow("all param is empty") << "" << QByteArray("") << false;
	QTest::newRow("filter is empty") << "" << QByteArray("Name") << false;
	QTest::newRow("filter with non-exist name") << "AnotherTestFilterName" << QByteArray("Name") << true;
	QTest::newRow("column is empty") << "TestFilterName1" << QByteArray("") << true;
	QTest::newRow("column with non-exist name") << "TestFilterName1" << QByteArray("AnotherTestColumn") << true;
	QTest::newRow("all params is correct") << "TestFilterName1" << QByteArray("Name") << false;
}


void CObjectCollectionPartituraTestBase::FilterTest()
{
	// get values from rows
	QFETCH(QString, textFilter);
	QFETCH(QByteArray, column);
	QFETCH(bool, result);

	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// Reset collection
			objectCollectionPtr->ResetData();

			if (m_typeIdObjectCollection == "TestInfo"){
				imtbase::IObjectCollection::Ids idsInObject = objectCollectionPtr->GetElementIds();
				if (!idsInObject.isEmpty()){
					for (int i = 0; i < idsInObject.count(); i++){
						objectCollectionPtr->RemoveElement(idsInObject[i]);
					}
				}
			}

			// Set text filter
			imtbase::CCollectionFilter filter;
			QByteArrayList columns;
			columns.append(column);
			filter.SetTextFilter(textFilter);
			filter.SetFilteringInfoIds(columns);
			iprm::CParamsSet filterParams;
			filterParams.SetEditableParameter("Filter", &filter);

			// Insert 20 objects in collection
			for (int i = 0; i < 20; i++){
				if (i%2 == 0){
					objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, QString("TestFilterName1 %1").arg(i), QString("TestDescription %1").arg(i));
				}
				else{
					objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, QString("TestFilterName2 %1").arg(i), QString("TestDescription %1").arg(i));
				}
			}

			// check filter
			imtbase::ICollectionInfo::Ids collectionIds = objectCollectionPtr->GetElementIds(0, 15, &filterParams);
			if (collectionIds.isEmpty()){
				QVERIFY2(result, "Text filter for object collection is failed");
			}
			else{
				if (collectionIds.count() == 10 && textFilter == "TestFilterName1"){
					for(int index = 0; index < collectionIds.size(); index++){
						QString objectName = objectCollectionPtr->GetElementInfo(collectionIds[index], imtbase::IObjectCollection::EIT_NAME).toString();
						QVERIFY2(objectName.contains(QString(textFilter+" %1").arg(index*2)), "Text filter for object collection is failed");
					}
				}
				else if (collectionIds.count() == 15 && textFilter == ""){
					for (int index = 0; index < collectionIds.count(); index++){
						QString objectName = objectCollectionPtr->GetElementInfo(collectionIds[index], imtbase::IObjectCollection::EIT_NAME).toString();
						if (index%2 == 0){
							QVERIFY2(objectName.contains(QString("TestFilterName1 %1").arg(index)), "Text filter for object collection is failed");
						}
						else{
							QVERIFY2(objectName.contains(QString("TestFilterName2 %1").arg(index)), "Text filter for object collection is failed");
						}
					}
				}
				else{
					QFAIL("Count of objects collection is incorrect");
				}
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


void CObjectCollectionPartituraTestBase::SortingTest_data()
{
	initTestCase();

	// variable decloration
	QTest::addColumn<QString>("sortOrder");
	QTest::addColumn<QByteArray>("column");

	// set values and description of test
	QTest::newRow("all params is null") << "NO ORDER" << QByteArray("");
	QTest::newRow("sortOrder is no-order") << "NO ORDER" << QByteArray("Name");
	QTest::newRow("sortOrder is ASC") << "ASC" << QByteArray("Name");
	QTest::newRow("sortOrder is DESC") << "DESC" << QByteArray("Name");
}


void CObjectCollectionPartituraTestBase::SortingTest()
{
	// get values from rows
	QFETCH(QString, sortOrder);
	QFETCH(QByteArray, column);

	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// Reset collection
			objectCollectionPtr->ResetData();

			if (m_typeIdObjectCollection == "TestInfo"){
				imtbase::IObjectCollection::Ids idsInObject = objectCollectionPtr->GetElementIds();
				if (!idsInObject.isEmpty()){
					for (int i = 0; i < idsInObject.count(); i++){
						objectCollectionPtr->RemoveElement(idsInObject[i]);
					}
				}
			}

			// Set sorting
			imtbase::CCollectionFilter filter;
			QByteArrayList columns;
			columns.append(column);
			filter.SetSortingOrder(sortOrder == "NO ORDER" ? imtbase::ICollectionFilter::SO_NO_ORDER : sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
			filter.SetSortingInfoIds(columns);
			iprm::CParamsSet filterParams;
			filterParams.SetEditableParameter("Filter", &filter);

			// Insert 20 objects in collection
			QStringList listObjects = {"bTest", "cTest", "1Test", "2Test", "aTest", "3Test", "11aTest", "2B2Test", "aaTest", "abTest", "a1Test", "aATest", "AaTest", "ABTest", "aBTest", "A2aTest", "a1ATest", "B1aTest", "cA1Test", "aaaTest"};
			for (int i = 0; i < 20; i++){
				QString nameOject = listObjects[i];
				objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, nameOject, "TestDescription");
			}
			imtbase::ICollectionInfo::Ids collectionIdsBeforeSort = objectCollectionPtr->GetElementIds();
			QStringList listObjectBeforeSort;

			for (int i = 0; i < collectionIdsBeforeSort.count(); i++){
				listObjectBeforeSort.append(objectCollectionPtr->GetElementInfo(collectionIdsBeforeSort[i], imtbase::IObjectCollection::EIT_NAME).toString());
			}
			// sorting list
			if (sortOrder == "ASC"){
				std::sort(listObjectBeforeSort.begin(), listObjectBeforeSort.end(), [](const QString &a, const QString &b){
					if (a.isEmpty())
						return true;
					if (b.isEmpty())
						return false;
					if (a.toUpper() == b.toUpper())
						return a > b;
					return a.toUpper() < b.toUpper();
				});
			}
			else if (sortOrder == "DESC"){
				std::sort(listObjectBeforeSort.rbegin(), listObjectBeforeSort.rend(), [](const QString &a, const QString &b){
					if (a.isEmpty())
						return true;
					if (b.isEmpty())
						return false;
					if (a.toUpper() == b.toUpper())
						return a > b;
					return a.toUpper() < b.toUpper();
				});
			}

			// check sorting
			imtbase::ICollectionInfo::Ids collectionIds = objectCollectionPtr->GetElementIds(0, 20, &filterParams);
			if (collectionIds.count() == 20){
				for (int index = 0; index < collectionIds.count(); index++){
					QString objectName = objectCollectionPtr->GetElementInfo(collectionIds[index], imtbase::IObjectCollection::EIT_NAME).toString();
					QVERIFY2(objectName.contains(listObjectBeforeSort[index]), "Sorting objects in collection is failed");
				}
			}
			else if (column == "AnotherName"){
				QVERIFY2(collectionIds.count() == 0, "Count of objects in collection is incorrect: non-exist column");
			}
			else{
				QFAIL("Count of objects in collection is incorrect");
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


void CObjectCollectionPartituraTestBase::GetElementIdsTest_data()
{
	initTestCase();

	// variable decloration
	QTest::addColumn<int>("offset");
	QTest::addColumn<int>("count");
	QTest::addColumn<QString>("textFilter");
	QTest::addColumn<QString>("sortOrder");
	QTest::addColumn<QByteArray>("sortColumn");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QTest::newRow("all params is empty") << NULL << NULL << "" << "NO ORDER" << QByteArray("Name") << true;
	QTest::newRow("offset is empty") << NULL << 15 << "TestFilterName1" << "ASC" << QByteArray("Name") << false;
	QTest::newRow("offset is out of size") << 100 << 15 << "TestFilterName1" << "ASC" << QByteArray("Name") << true;
//	QTest::newRow("offset is negative") << -10 << 15 << "TestFilterName1" << "ASC" << QByteArray("Name") << true;
	QTest::newRow("count is empty") << 3 << NULL << "TestFilterName1" << "ASC" << QByteArray("Name") << true;
	QTest::newRow("count is out of size") << 3 << 100 << "TestFilterName1" << "ASC" << QByteArray("Name") << false;
//	QTest::newRow("count is negative") << 3 << -10 << "TestFilterName1" << "ASC" << QByteArray("Name") << false; for sql
	QTest::newRow("testFilter is empty") << 3 << 15 << "" << "ASC" << QByteArray("Name") << false;
	QTest::newRow("testFilter is non-exist") << 3 << 15 << "AnotherTestFilter" << "ASC" << QByteArray("Name") << true;
	QTest::newRow("sortOrder is NO ORDER") << 3 << 15 << "TestFilterName1" << "NO ORDER" << QByteArray("Name") << false;
	QTest::newRow("sortOrder is ASC") << 3 << 15 << "TestFilterName1" << "ASC" << QByteArray("Name") << false;
	QTest::newRow("sortOrder is DESC") << 3 << 15 << "TestFilterName1" << "DESC" << QByteArray("Name") << false;
}


void CObjectCollectionPartituraTestBase::GetElementIdsTest()
{
	// get values from rows
	QFETCH(int, offset);
	QFETCH(int, count);
	QFETCH(QString, textFilter);
	QFETCH(QString, sortOrder);
	QFETCH(QByteArray, sortColumn);
	QFETCH(bool, result);

	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr != nullptr){

			// Reset collection
			objectCollectionPtr->ResetData();

			if (m_typeIdObjectCollection == "TestInfo"){
				imtbase::IObjectCollection::Ids idsInObject = objectCollectionPtr->GetElementIds();
				if (!idsInObject.isEmpty()){
					for (int i = 0; i < idsInObject.count(); i++){
						objectCollectionPtr->RemoveElement(idsInObject[i]);
					}
				}
			}

			// Set sorting and text filter
			imtbase::CCollectionFilter filter;
			QByteArrayList columns;
			columns.append(sortColumn);
			filter.SetTextFilter(textFilter);
			filter.SetSortingOrder(sortOrder == "NO ORDER" ? imtbase::ICollectionFilter::SO_NO_ORDER : sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
			filter.SetSortingInfoIds(columns);
			filter.SetFilteringInfoIds(columns);
			iprm::CParamsSet filterParams;
			filterParams.SetEditableParameter("Filter", &filter);

			// Insert 20 objects in collection
			for (int i = 0; i < 20; i++){
				if (i%2 == 0){
					objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, QString("TestFilterName1 %1").arg(i), QString("TestDescription %1").arg(i));
				}
				else{
					objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, QString("TestFilterName2 %1").arg(i), QString("TestDescription %1").arg(i));
				}
			}
			imtbase::ICollectionInfo::Ids collectionIds = objectCollectionPtr->GetElementIds(offset, count, &filterParams);
			imtbase::ICollectionInfo::Ids collectionIdsForTestOffset = objectCollectionPtr->GetElementIds(0, count, &filterParams);
			if (collectionIds.count() == 0){
				QVERIFY2(result, "GetElementIdsTest() is failed");
			}
			else{
				QStringList listObjects;
				if ((textFilter == "TestFilterName1" && collectionIds.count() == 10 && offset == 0) || (textFilter == "TestFilterName1" && collectionIds.count() == 7 && offset == 3)){

					// check filter
					bool checkFilter = true;
					for(int index = 0; index < collectionIds.count(); index++){
						QString objectName = objectCollectionPtr->GetElementInfo(collectionIds[index], imtbase::IObjectCollection::EIT_NAME).toString();
						listObjects.append(objectName);
						if (!objectName.contains(QString(textFilter))){
							checkFilter = false;
							break;
						}
					}
					if(!checkFilter){
						QEXPECT_FAIL("", "Filter is failed: name object don't contains text filter", Continue);
						QVERIFY(checkFilter);
					}
				}
				else if(textFilter == "" && collectionIds.count() == 15){
					for(int index = 0; index < collectionIds.count(); index++){
						QString objectName = objectCollectionPtr->GetElementInfo(collectionIds[index], imtbase::IObjectCollection::EIT_NAME).toString();
						listObjects.append(objectName);
					}
				}
				else{
					for(int index = 0; index < collectionIds.count(); index++){
						QString objectName = objectCollectionPtr->GetElementInfo(collectionIds[index], imtbase::IObjectCollection::EIT_NAME).toString();
						listObjects.append(objectName);
					}

					// sorting list
					if (sortOrder == "ASC"){
						std::sort(listObjects.begin(), listObjects.end(), [](const QString &a, const QString &b){
							if (a.isEmpty())
								return true;
							if (b.isEmpty())
								return false;
							if (a.toUpper() == b.toUpper())
								return a > b;
							return a.toUpper() < b.toUpper();
						});
					}
					else if (sortOrder == "DESC"){
						std::sort(listObjects.rbegin(), listObjects.rend(), [](const QString &a, const QString &b){
							if (a.isEmpty())
								return true;
							if (b.isEmpty())
								return false;
							if (a.toUpper() == b.toUpper())
								return a > b;
							return a.toUpper() < b.toUpper();
						});
					}

					// check sorting
					bool checkSorting = true;
					for(int index = 0; index < collectionIds.count(); index++){
						if (listObjects[index] != objectCollectionPtr->GetElementInfo(collectionIds[index], imtbase::IObjectCollection::EIT_NAME).toString()){
							checkSorting = false;
							break;
						}
					}
					if (!checkSorting){
						QEXPECT_FAIL("", "Sorting is failed", Continue);
						QVERIFY(checkSorting);
					}

					// check count in pagination
					if(count < collectionIds.count()){
						QEXPECT_FAIL("", "Pagination is failed: count objects is incorrect", Continue);
						QVERIFY(count >= collectionIds.count());
					}

					// check offset in pagination
					if(collectionIdsForTestOffset[offset] != collectionIds[0]){
						QEXPECT_FAIL("", "Pagination is failed: offset is incorrect", Continue);
						QVERIFY(collectionIdsForTestOffset[offset] == collectionIds[0]);
					}
					QFAIL("Filter is failed: count objects is incorrect");
				}

				// sorting list
				if (sortOrder == "ASC"){
					std::sort(listObjects.begin(), listObjects.end(), [](const QString &a, const QString &b){
						if (a.isEmpty())
							return true;
						if (b.isEmpty())
							return false;
						if (a.toUpper() == b.toUpper())
							return a > b;
						return a.toUpper() < b.toUpper();
					});
				}
				else if (sortOrder == "DESC"){
					std::sort(listObjects.rbegin(), listObjects.rend(), [](const QString &a, const QString &b){
						if (a.isEmpty())
							return true;
						if (b.isEmpty())
							return false;
						if (a.toUpper() == b.toUpper())
							return a > b;
						return a.toUpper() < b.toUpper();
					});
				}

				// check sorting
				bool checkSorting = true;
				for(int index = 0; index < collectionIds.count(); index++){
					if (listObjects[index] != objectCollectionPtr->GetElementInfo(collectionIds[index], imtbase::IObjectCollection::EIT_NAME).toString()){
						checkSorting = false;
						break;
					}
				}
				if(!checkSorting){
					QEXPECT_FAIL("", "Sorting is failed", Continue);
					QVERIFY(checkSorting);
				}

				// check count in pagination
				if(count < collectionIds.count()){
					QEXPECT_FAIL("", "Pagination is failed: count objects is incorrect", Continue);
					QVERIFY(count >= collectionIds.count());
				}

				// check offset in pagination
				if (collectionIdsForTestOffset[offset] != collectionIds[0]){
					QFAIL("Pagination is failed: offset is incorrect");
				}
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


void CObjectCollectionPartituraTestBase::cleanupTestCase()
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
			filter.append("*Test*");
			filter.append(" - *");
			folder.setNameFilters(filter);
			QStringList foldersForRemove = folder.entryList();
			for (int i = 0; i < foldersForRemove.size(); i++){
				QDir currentDir(folderPath + "/" + foldersForRemove[i]);
				currentDir.removeRecursively();
				folder.rmdir(foldersForRemove[i]);
			}
		}
		imtdb::IDatabaseEngine* databaseEnginePtr = compositePtr->GetComponentInterface<imtdb::IDatabaseEngine>();
		if (databaseEnginePtr != nullptr){
			QSqlError sqlError;
			QSqlQuery sqlQuery = databaseEnginePtr->ExecSqlQuery(QByteArray("DELETE FROM public.tests CASCADE;"), &sqlError);
			QSqlQuery sqlQuery2 = databaseEnginePtr->ExecSqlQuery(QByteArray("DROP DATABASE tests;"), &sqlError);

		}
	}
	m_typeIdObjectCollection.clear();
	m_imtCorePath.clear();
	m_registryFile.clear();
	m_configFile.clear();
	QWARN(qPrintable(QString("End tests for %1 accessor").arg(m_nameAccessor)));
	m_nameAccessor.clear();
}
