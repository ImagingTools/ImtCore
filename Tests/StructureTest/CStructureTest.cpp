#include "CStructureTest.h"

// // ImtCore includes
// #include <imtbase/ICollectionStructure.h>

// // #include <imtbase/IObjectCollection.h>
// #include <ifile/CCompactXmlFileReadArchive.h>
// #include <ifile/CCompactXmlFileWriteArchive.h>
// #include <imtauth/CAccountInfo.h>
// #include <imtauth/CAccountInfoMetaInfo.h>
// #include <imtauth/IAccountInfo.h>
#include <GeneratedFiles/StructureTest/CGenerateTest.h>


// void CStructureTest::initTestCase()
// {
// 	// paths to test component accessor and config for him
// 	m_imtCorePath = qEnvironmentVariable("IMTCOREDIR");
// 	m_registryFile = m_imtCorePath + QString("/Tests/StructureTest/Partitura/StructureTest.acc");
// 	m_configFile = m_imtCorePath + "/Config/ImtCoreTest.awc";
// 	m_nameAccessor = "StructureTest";
// 	// set type id for tests
// 	// m_typeIdObjectCollection = "AccountInfo";
// }



// void CStructureTest::InsertNewNodeWithRequiredParamsTest_data()
// {
// 	initTestCase();
// 	QWARN(qPrintable(QString("Start tests for %1 accessor").arg(m_nameAccessor)));

// 	// variable decloration
// 	QTest::addColumn<QString>("name");
// 	QTest::addColumn<QString>("description");
// 	QTest::addColumn<QByteArray>("proposedNodeId");
// 	QTest::addColumn<QByteArray>("parentNodeId");
// 	QTest::addColumn<bool>("result");

// 	// set values and description of test
// 	QTest::newRow("all param is empty") << "" << "" << QByteArray("") << QByteArray("") << false;
// 	QTest::newRow("name param is empty")  << "" << "Insert object with empty name" << QByteArray("781021ef-baac-40d5-8c97-336d837592b6") << QByteArray("") << false;
// 	QTest::newRow("node1")  << "node1" << "Insert node1" << QByteArray("781021ef-baac-40d5-8c97-336d837592b6") << QByteArray("") << true;
// 	QTest::newRow("node2")  << "node2" << "Insert node2" << QByteArray("ac0d4bcf-73bc-4bd7-9263-c914d41591ba") << QByteArray("") << false;
// 	QTest::newRow("node3")  << "node3" << "Insert node3" << QByteArray("83465a08-b435-44b8-99e3-b2e72e50b205") << QByteArray("781021ef-baac-40d5-8c97-336d837592b6") << false;
// }


// void CStructureTest::InsertNewNodeWithRequiredParamsTest()
// {
// 	// get values from rows
// 	QFETCH(QString, name);
// 	QFETCH(QString, description);
// 	QFETCH(QByteArray, proposedNodeId);
// 	QFETCH(QByteArray, parentNodeId);
// 	QFETCH(bool, result);
// 	initTestCase();
// 	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
// 	compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));

// 	if (compositePtr.IsValid()){

// 		// get component hierarchical structure
// 		imtbase::ICollectionStructure* hierarchicalStructurePtr = compositePtr->GetComponentInterface<imtbase::ICollectionStructure>();
// 		if (hierarchicalStructurePtr != nullptr){

// 			// reset data from hierarchical structure
// 			hierarchicalStructurePtr->ResetData();

// 			QByteArray idNewNode = hierarchicalStructurePtr->InsertNewNode(name, description, proposedNodeId, parentNodeId);

// 			// check get correct result
// 			QVERIFY2((idNewNode.isEmpty() == result), "Failed in insert new object with required params");
// 		}

// 		else{
// 			QFAIL("HierarchicalStructure is nullptr");
// 		}
// 	}
// 	else{
// 		QFAIL("Component is not initialized");
// 	}
// }


// void CStructureTest::cleanupTestCase()
// {
// 	m_imtCorePath.clear();
// 	m_registryFile.clear();
// 	m_configFile.clear();
// 	m_nameAccessor.clear();
// }

// I_ADD_TEST(CStructureTest);
