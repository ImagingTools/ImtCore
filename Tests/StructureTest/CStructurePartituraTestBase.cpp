#include "CStructurePartituraTestBase.h"
// #include "imtbase/CCollectionFilter.h"
#include <iprm/CParamsSet.h>


// ImtCore includes
// #include <imtbase/IObjectCollection.h>
// #include <ifile/CCompactXmlFileReadArchive.h>
// #include <ifile/CCompactXmlFileWriteArchive.h>
// #include <imtauth/CAccountInfo.h>
// #include <imtauth/CAccountInfoMetaInfo.h>
// #include <imtauth/IAccountInfo.h>
// #include <imtrepo/IFileObjectCollection.h>
// #include <imtdb/IDatabaseObjectDelegate.h>
#include <imtdb/IDatabaseEngine.h>
// #include <imtdb/IDatabaseLoginSettings.h>
#include <imtbase/ICollectionStructure.h>
#include <imtbase/ICollectionStructureController.h>
#include <imttest/CTestInfo.h>
#include <imttest/ITestInfo.h>
#include <imttest/CTestMetaInfo.h>



void CStructurePartituraTestBase::InsertNewNodeWithRequiredParamsTest_data()
{
	initTestCase();
	QWARN(qPrintable(QString("Start tests for %1 accessor").arg(m_nameAccessor)));
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtdb::IDatabaseEngine* databaseEnginePtr = compositePtr->GetComponentInterface<imtdb::IDatabaseEngine>();
		if (databaseEnginePtr != nullptr){
			QSqlError sqlError;
			QSqlQuery sqlQuery = databaseEnginePtr->ExecSqlQuery(QByteArray("DELETE FROM \"Nodes\" CASCADE;"), &sqlError);
		}
	}

	// variable decloration
	QTest::addColumn<QString>("name");
	QTest::addColumn<QString>("description");
	QTest::addColumn<QByteArray>("proposedNodeId");
	QTest::addColumn<QByteArray>("parentNodeId");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QTest::newRow("all param is empty") << "" << "" << QByteArray("") << QByteArray("") << false;
	QTest::newRow("name param is empty")  << "" << "Insert object with empty name" << QByteArray("781021ef-baac-40d5-8c97-336d837592b6") << QByteArray("") << false;
	QTest::newRow("node1")  << "node1" << "Insert node1" << QByteArray("781021ef-baac-40d5-8c97-336d837592b6") << QByteArray("") << true;
	QTest::newRow("node2")  << "node2" << "Insert node2" << QByteArray("ac0d4bcf-73bc-4bd7-9263-c914d41591ba") << QByteArray("") << true;
	QTest::newRow("node3")  << "node3" << "Insert node3" << QByteArray("83465a08-b435-44b8-99e3-b2e72e50b205") << QByteArray("781021ef-baac-40d5-8c97-336d837592b6") << true;
}


void CStructurePartituraTestBase::InsertNewNodeWithRequiredParamsTest()
{
	// get values from rows
	QFETCH(QString, name);
	QFETCH(QString, description);
	QFETCH(QByteArray, proposedNodeId);
	QFETCH(QByteArray, parentNodeId);
	QFETCH(bool, result);
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

	if (compositePtr.IsValid()){

		// get component hierarchical structure
		imtbase::ICollectionStructure* hierarchicalStructurePtr = compositePtr->GetComponentInterface<imtbase::ICollectionStructure>();
		if (hierarchicalStructurePtr != nullptr){

			// reset data from hierarchical structure
			hierarchicalStructurePtr->ResetData();

			QByteArray idNewNode = hierarchicalStructurePtr->InsertNewNode(name, description, proposedNodeId, parentNodeId);

			// check get correct result
			QVERIFY2((!idNewNode.isEmpty() == result), "Failed in insert new object with required params");
		}

		else{
			QFAIL("HierarchicalStructure is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


void CStructurePartituraTestBase::SetNodeNameTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

	if (compositePtr.IsValid()){

		// get component hierarchical structure
		imtbase::ICollectionStructure* hierarchicalStructurePtr = compositePtr->GetComponentInterface<imtbase::ICollectionStructure>();
		if (hierarchicalStructurePtr != nullptr){

			// reset data from hierarchical structure
			hierarchicalStructurePtr->ResetData();

			bool result = hierarchicalStructurePtr->SetNodeName("781021ef-baac-40d5-8c97-336d837592b6", "node1 test");

			// check get correct result
			QVERIFY2(result == true, "Failed in insert new object with required params");
		}

		else{
			QFAIL("HierarchicalStructure is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


void CStructurePartituraTestBase::SetNodeDescriptionTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

	if (compositePtr.IsValid()){

		// get component hierarchical structure
		imtbase::ICollectionStructure* hierarchicalStructurePtr = compositePtr->GetComponentInterface<imtbase::ICollectionStructure>();
		if (hierarchicalStructurePtr != nullptr){

			// reset data from hierarchical structure
			hierarchicalStructurePtr->ResetData();

			bool result = hierarchicalStructurePtr->SetNodeDescription("781021ef-baac-40d5-8c97-336d837592b6", "node1 description");

			// check get correct result
			QVERIFY2(result == true, "Failed in insert new object with required params");
		}

		else{
			QFAIL("HierarchicalStructure is nullptr");
		}
	}
	else{
		QFAIL("Component is not initialized");
	}
}


void CStructurePartituraTestBase::MoveNodeTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

	if (compositePtr.IsValid()) {

		// get component hierarchical structure
		imtbase::ICollectionStructure* hierarchicalStructurePtr = compositePtr->GetComponentInterface<imtbase::ICollectionStructure>();
		if (hierarchicalStructurePtr != nullptr) {

			// reset data from hierarchical structure
			hierarchicalStructurePtr->ResetData();

			bool result = hierarchicalStructurePtr->MoveNode("ac0d4bcf-73bc-4bd7-9263-c914d41591ba","781021ef-baac-40d5-8c97-336d837592b6");

			// check get correct result
			QVERIFY2(result == true, "Failed in insert new object with required params");
		}

		else {
			QFAIL("HierarchicalStructure is nullptr");
		}
	}
	else {
		QFAIL("Component is not initialized");
	}
}


void CStructurePartituraTestBase::RemoveNodeTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

	if (compositePtr.IsValid()) {

		// get component hierarchical structure
		imtbase::ICollectionStructure* hierarchicalStructurePtr = compositePtr->GetComponentInterface<imtbase::ICollectionStructure>();
		if (hierarchicalStructurePtr != nullptr) {

			// reset data from hierarchical structure
			hierarchicalStructurePtr->ResetData();

			bool result = hierarchicalStructurePtr->RemoveNode("ac0d4bcf-73bc-4bd7-9263-c914d41591ba");

			// check get correct result
			QVERIFY2(result == true, "Failed in insert new object with required params");
		}

		else {
			QFAIL("HierarchicalStructure is nullptr");
		}
	}
	else {
		QFAIL("Component is not initialized");
	}
}


void CStructurePartituraTestBase::InsertObjectWithRequiredParamsTest_data()
{
	initTestCase();
	QWARN(qPrintable(QString("Start tests for %1 accessor").arg(m_nameAccessor)));

	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

	if (compositePtr.IsValid()) {
		imtdb::IDatabaseEngine* databaseEnginePtr = compositePtr->GetComponentInterface<imtdb::IDatabaseEngine>();
		if (databaseEnginePtr != nullptr){
			QSqlError sqlError;
			QSqlQuery sqlQuery = databaseEnginePtr->ExecSqlQuery(QByteArray("DELETE FROM \"Tests\" CASCADE;"), &sqlError);
			sqlQuery = databaseEnginePtr->ExecSqlQuery(QByteArray("DELETE FROM \"NodeDocuments\" CASCADE;"), &sqlError);
		}
	}

	// variable decloration
	QTest::addColumn<QByteArray>("nodeId");
	QTest::addColumn<QByteArray>("typeId");
	QTest::addColumn<QString>("name");
	QTest::addColumn<QString>("description");
	QTest::addColumn<QString>("nameInData");
	QTest::addColumn<QByteArray>("objectId");
	QTest::addColumn<bool>("result");

	// set values and description of test
	QTest::newRow("all param is empty") << QByteArray("") <<  QByteArray("") << "" << "" << "" << QByteArray("") << false;
	QTest::newRow("type param is empty") << QByteArray("83465a08-b435-44b8-99e3-b2e72e50b205") << QByteArray("") << "TestObjectName" << "Insert object with empty type id" << "AccountName1" << QByteArray("2378168c-c55f-4212-ba35-d1880a21bce7") << false;
	QTest::newRow("node id is empty") << QByteArray("") << QByteArray("TestInfo") << "TestObjectName" << "Insert object with empty type id" << "AccountName1" << QByteArray("2478168c-c55f-4212-ba35-d1880a21bce7") << true;
	QTest::newRow("name param is empty") << QByteArray("83465a08-b435-44b8-99e3-b2e72e50b205") << QByteArray("TestInfo") << "" << "Insert object with empty name" << "AccountName2" << QByteArray("2578168c-c55f-4212-ba35-d1880a21bce7") << false;
	QTest::newRow("type id is not exist") << QByteArray("83465a08-b435-44b8-99e3-b2e72e50b205") << QByteArray("AnotherTypeId") << "TestObjectName" << "Insert object with non-exist type id" << "AccountName4" << QByteArray("2278168c-c55f-4212-ba35-d1880a21bce7") << false;
	QTest::newRow("description param is empty") << QByteArray("83465a08-b435-44b8-99e3-b2e72e50b205") << QByteArray("TestInfo") << "TestObjectName" << "" << "AccountName3" << QByteArray("2678168c-c55f-4212-ba35-d1880a21bce7") << true;
	QTest::newRow("object id is exist") << QByteArray("83465a08-b435-44b8-99e3-b2e72e50b205") << QByteArray("TestInfo") << "TestObjectName" << "Insert object with exist type id and all params is setted" << "AccountName5" << QByteArray("2678168c-c55f-4212-ba35-d1880a21bce7") << true;

}


void CStructurePartituraTestBase::InsertObjectWithRequiredParamsTest()
{
	// get values from rows
	QFETCH(QByteArray, nodeId);
	QFETCH(QByteArray, typeId);
	QFETCH(QString, name);
	QFETCH(QString, description);
	QFETCH(QString, nameInData);
	QFETCH(QByteArray, objectId);
	QFETCH(bool, result);
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

	if (compositePtr.IsValid()) {

		// get component object collection
		imtbase::IObjectCollection* objectCollectionPtr = compositePtr->GetComponentInterface<imtbase::IObjectCollection>();
		if (objectCollectionPtr == nullptr){
			QFAIL("Object collection is nullptr");
		}

		// get component hierarchical structure
		imtbase::ICollectionStructureController* collectionStructureControllerPtr = compositePtr->GetComponentInterface<imtbase::ICollectionStructureController>();
		if (collectionStructureControllerPtr != nullptr) {

			QByteArray idNewObject = collectionStructureControllerPtr->InsertNewObjectIntoCollection(objectCollectionPtr, nodeId, typeId, name, description, imtbase::IObjectCollection::DataPtr(), objectId);
			// check contains object in collection
			imtbase::IObjectCollection::Ids idsInObject = objectCollectionPtr->GetElementIds();
			bool checkIdIsExist = idsInObject.contains(idNewObject);

			// if collection don't contains object test will be failed
			QVERIFY2(checkIdIsExist == result, "Insert object with set non-exist id is failed");
		}
		else {
			QFAIL("HierarchicalStructure is nullptr");
		}
	}
	else {
		QFAIL("Component is not initialized");
	}
}


void CStructurePartituraTestBase::MoveObjectTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

	if (compositePtr.IsValid()) {

		// get component hierarchical structure
		imtbase::ICollectionStructure* hierarchicalStructurePtr = compositePtr->GetComponentInterface<imtbase::ICollectionStructure>();
		if (hierarchicalStructurePtr != nullptr) {

			// reset data from hierarchical structure
			hierarchicalStructurePtr->ResetData();

			bool result = hierarchicalStructurePtr->MoveObject("2678168c-c55f-4212-ba35-d1880a21bce7", "83465a08-b435-44b8-99e3-b2e72e50b205", "781021ef-baac-40d5-8c97-336d837592b6");

			// check get correct result
			QVERIFY2(result == true, "Failed in move object");
		}

		else {
			QFAIL("HierarchicalStructure is nullptr");
		}
	}
	else {
		QFAIL("Component is not initialized");
	}
}


void CStructurePartituraTestBase::RemoveObjectTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

	if (compositePtr.IsValid()) {

		// get component hierarchical structure
		imtbase::ICollectionStructure* hierarchicalStructurePtr = compositePtr->GetComponentInterface<imtbase::ICollectionStructure>();
		if (hierarchicalStructurePtr != nullptr) {

			// reset data from hierarchical structure
			hierarchicalStructurePtr->ResetData();

			bool result = hierarchicalStructurePtr->RemoveObject("2678168c-c55f-4212-ba35-d1880a21bce7", "781021ef-baac-40d5-8c97-336d837592b6");

			// check get correct result
			QVERIFY2(result == true, "Failed in remove object");
		}

		else {
			QFAIL("HierarchicalStructure is nullptr");
		}
	}
	else {
		QFAIL("Component is not initialized");
	}
}


void CStructurePartituraTestBase::GetNodePathTest()
{
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

	if (compositePtr.IsValid()) {

		// get component hierarchical structure
		imtbase::ICollectionStructure* hierarchicalStructurePtr = compositePtr->GetComponentInterface<imtbase::ICollectionStructure>();
		if (hierarchicalStructurePtr != nullptr) {

			// reset data from hierarchical structure
			hierarchicalStructurePtr->ResetData();

			imtbase::ICollectionStructureInfo::NodePath nodePath = hierarchicalStructurePtr->GetNodePath("83465a08-b435-44b8-99e3-b2e72e50b205");

			// check get correct result
			QVERIFY2(nodePath.count() == 2, "Failed in get node path");
		}

		else {
			QFAIL("HierarchicalStructure is nullptr");
		}
	}
	else {
		QFAIL("Component is not initialized");
	}
}


void CStructurePartituraTestBase::cleanupTestCase()
{
	// get component object collection
	initTestCase();
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	if (compositePtr.IsValid()){
		imtdb::IDatabaseEngine* databaseEnginePtr = compositePtr->GetComponentInterface<imtdb::IDatabaseEngine>();
		if (databaseEnginePtr != nullptr){
			QSqlError sqlError;
//			QSqlQuery sqlQuery = databaseEnginePtr->ExecSqlQuery(QByteArray("DELETE FROM public.tests CASCADE;"), &sqlError);
//			//QSqlQuery sqlQuery = databaseEnginePtr->ExecSqlQuery(QByteArray("DELETE FROM \"Nodes\" CASCADE;"), &sqlError);
//			QSqlQuery sqlQuery2 = databaseEnginePtr->ExecSqlQuery(QByteArray("DROP DATABASE tests;"), &sqlError);

		}
	}
	m_imtCorePath.clear();
	m_registryFile.clear();
	m_configFile.clear();
	QWARN(qPrintable(QString("End tests for %1 accessor").arg(m_nameAccessor)));
	m_nameAccessor.clear();
}
