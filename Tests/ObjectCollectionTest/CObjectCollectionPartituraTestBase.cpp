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


void CObjectCollectionPartituraTestBase::InsertNewObjectWithRequiredParamsTest_data()
{
    initTestCase();
	QWARN(qPrintable(QString("Start tests for %1 accessor").arg(m_nameAccessor)));
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


void CObjectCollectionPartituraTestBase::InsertNewObjectWithRequiredParamsTest()
{
    // get values from rows
    QFETCH(QByteArray, typeId);
    QFETCH(QString, name);
    QFETCH(QString, description);
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

			// input params of meta
			imtauth::CAccountInfoMetaInfo inputMetaInfo;
			QString inputName = "AccountName";
			QString inputDescription = "AccountDescription";

			// set input params of meta
			inputMetaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_TYPE, QObject::tr("Private"));
			inputMetaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_NAME, inputName);
			inputMetaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_DESCRIPTION, inputDescription);

			// insert new object with meta
			QByteArray idNewObject = objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), QByteArray(), &inputMetaInfo);

			//
			if (!idNewObject.isEmpty()){
				imtbase::IObjectCollection::MetaInfoPtr referenceMetaInfoPtr;
				if (objectCollectionPtr->GetDataMetaInfo(idNewObject, referenceMetaInfoPtr)){
					QString referenceName = referenceMetaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_NAME).toString();
					QString referenceDescription = referenceMetaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_DESCRIPTION).toString();
					QVERIFY2(((inputName == referenceName) && (inputDescription == referenceDescription)), "Insert new object with meta data is failed");
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
            objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), "testId");

            // Reset collection
            objectCollectionPtr->ResetData();

            //Check remove inserted object from collection
            imtbase::IObjectCollection::Ids idsInObjectAfterReset = objectCollectionPtr->GetElementIds();
            bool checkDeleteAddedObject = !idsInObjectAfterReset.contains("testId");
            QVERIFY2(checkDeleteAddedObject, "Reset objects from collection with fixed object is failed");
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
				QString changedName = "ChangedNameObject";
				objectCollectionPtr->SetObjectName(idNewObject, changedName);
				QString objectName = objectCollectionPtr->GetElementInfo(idNewObject, imtbase::ICollectionInfoProvider::EIT_NAME).toString();
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
				objectCollectionPtr->SetObjectDescription(idNewObject, changedDescription);
				QString objectDescription = objectCollectionPtr->GetElementInfo(idNewObject, imtbase::ICollectionInfoProvider::EIT_DESCRIPTION).toString();
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
				imtbase::ICollectionInfoProvider::Id gettedObjectTypeId = objectCollectionPtr->GetObjectTypeId(idNewObject);
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

			// Set text filter
			imtbase::CCollectionFilter filter;
			filter.SetTextFilter(textFilter);
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
	QTest::newRow("count is negative") << 1 << -5 << true;
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
	QTest::addColumn<bool>("result");

	// set values and description of test
	QTest::newRow("filter is empty") << "" << false;
	QTest::newRow("filter with non-exist name") << "AnotherTestFilterName" << true;
	QTest::newRow("filter with correct name") << "TestFilterName1" << false;
}


void CObjectCollectionPartituraTestBase::FilterTest()
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

			// Set text filter
			imtbase::CCollectionFilter filter;
			filter.SetTextFilter(textFilter);
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


void CObjectCollectionPartituraTestBase::cleanupTestCase()
{
    m_typeIdObjectCollection.clear();
    m_imtCorePath.clear();
    m_registryFile.clear();
    m_configFile.clear();
	QWARN(qPrintable(QString("End tests for %1 accessor").arg(m_nameAccessor)));
	m_nameAccessor.clear();
}
