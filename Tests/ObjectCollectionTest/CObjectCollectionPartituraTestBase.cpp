#include "CObjectCollectionPartituraTestBase.h"


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


//void CObjectCollectionPartituraTestBase::ResetCollectionWithFixedObjectTest()
//{
//    initTestCase();
//    // declaration component accessor
//    istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
//    compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
//    if (compositePtr.IsValid()){

//        // get component object collection
//        imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
//        if (objectCollectionPtr != nullptr){
//            objectCollectionPtr->InsertNewObject(m_typeIdObjectCollection, "TestObject", "TestDescription", imtbase::IObjectCollection::DataPtr(), "testId");

//            // Reset collection
//            objectCollectionPtr->ResetData();

//            //Check exist fixed object and remove inserted object from collection
//            imtbase::IObjectCollection::Ids idsInObjectAfterReset = objectCollectionPtr->GetElementIds();
//            if (!idsInObjectAfterReset.isEmpty()){
//                bool checkExistFixedObject = idsInObjectAfterReset.contains("DefaultAccount");
//                bool checkDeleteAddedObject = !idsInObjectAfterReset.contains("testId");
//                QVERIFY2((checkExistFixedObject && checkDeleteAddedObject), "Reset objects from collection with fixed object is failed");
//            }
//            else{
//                QFAIL("Reset collection with fixed object is failed: collection is empty");
//            }
//        }
//        else{
//            QFAIL("Object collection is nullptr");
//        }
//    }
//    else{
//        QFAIL("Component is not initialized");
//    }
//}


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


void CObjectCollectionPartituraTestBase::cleanupTestCase()
{
    m_typeIdObjectCollection.clear();
    m_imtCorePath.clear();
    m_registryFile.clear();
    m_configFile.clear();
}
