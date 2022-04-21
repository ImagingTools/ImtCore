#include "CMultiSelectionTest.h"


void CMultiSelectionTest::initTestCase()
{
	QByteArray firstVar("firstKey");
	QByteArray secondVar("secondKey");
	m_testMultiSelectedIds.append(firstVar);
	m_testMultiSelectedIds.append(secondVar);
	m_testSingleSelectedIds.append(secondVar);
}


void CMultiSelectionTest::SetSelectedIdsTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	imtbase::CSelection singleSelection2(imtbase::ISelection::SelectionMode::SM_SINGLE);
	bool checkMultiSelectData = multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	bool checkMultiSelectData2 = multiSelection2.SetSelectedIds(m_testEmptySelectedIds);
	bool checkSingleSelectData = singleSelection.SetSelectedIds(m_testSingleSelectedIds);
	bool checkSingleSelectData2 = singleSelection2.SetSelectedIds(m_testMultiSelectedIds);
	QVERIFY2(checkMultiSelectData, qPrintable(QString("Function CSelection::SetSelectedIds is failed in multi-select")));
	QVERIFY2(checkMultiSelectData2, qPrintable(QString("Function CSelection::SetSelectedIds is failed in multi-select with empty Ids list")));
	QVERIFY2(checkSingleSelectData, qPrintable(QString("Function CSelection::SetSelectedIds is failed in single-select")));
	QVERIFY2(!checkSingleSelectData2, qPrintable(QString("Function CSelection::SetSelectedIds is failed in single-select with many select keys")));
	QCOMPARE(multiSelection.GetSelectedIds(), m_testMultiSelectedIds);
	QCOMPARE(singleSelection.GetSelectedIds(), m_testSingleSelectedIds);
}


void CMultiSelectionTest::IsEqualTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection3(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	imtbase::CSelection singleSelection2(imtbase::ISelection::SelectionMode::SM_SINGLE);
	imtbase::CSelection singleSelection3(imtbase::ISelection::SelectionMode::SM_SINGLE);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	multiSelection2.SetSelectedIds(m_testMultiSelectedIds);
	singleSelection.SetSelectedIds(m_testSingleSelectedIds);
	singleSelection2.SetSelectedIds(m_testSingleSelectedIds);
	QVERIFY2((multiSelection.IsEqual(multiSelection2)), qPrintable(QString("Function CSelection::IsEqual is failed with compare multi-selects")));
	QVERIFY2(!(multiSelection.IsEqual(multiSelection3)), qPrintable(QString("Function CSelection::IsEqual is failed with compare multi-selects (null vs non-null)")));
	QVERIFY2(!(multiSelection.IsEqual(singleSelection)), qPrintable(QString("Function CSelection::IsEqual is failed with compare single-select and multi-select")));
	QVERIFY2((singleSelection.IsEqual(singleSelection2)), qPrintable(QString("Function CSelection::IsEqual is failed with compare single-selects")));
	QVERIFY2(!(singleSelection.IsEqual(singleSelection3)), qPrintable(QString("Function CSelection::IsEqual is failed with compare single-selects (null vs non-null)")));
	QVERIFY2(!(singleSelection3.IsEqual(multiSelection3)), qPrintable(QString("Function CSelection::IsEqual is failed with compare single-select and multi-select (null vs null)")));
}


void CMultiSelectionTest::CopyFromTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	imtbase::CSelection singleSelection2(imtbase::ISelection::SelectionMode::SM_SINGLE);
	imtbase::CSelection singleSelection3(imtbase::ISelection::SelectionMode::SM_SINGLE);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	singleSelection.SetSelectedIds(m_testSingleSelectedIds);
	multiSelection2.CopyFrom(multiSelection);
	singleSelection2.CopyFrom(singleSelection);
	imtbase::ISelection::Ids multiSelectionIds = multiSelection.GetSelectedIds();
	imtbase::ISelection::Ids multiSelection2Ids = multiSelection2.GetSelectedIds();
	imtbase::ISelection::Ids singleSelectionIds = singleSelection.GetSelectedIds();
	imtbase::ISelection::Ids singleSelection2Ids = singleSelection2.GetSelectedIds();
	QCOMPARE(singleSelectionIds, singleSelection2Ids);
	QCOMPARE(multiSelectionIds, multiSelection2Ids);
	singleSelection3.CopyFrom(multiSelection);
	imtbase::ISelection::Ids singleSelection3Ids = singleSelection3.GetSelectedIds();
	QCOMPARE(singleSelection3Ids, multiSelectionIds);
}


void CMultiSelectionTest::CloneMeTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	singleSelection.SetSelectedIds(m_testSingleSelectedIds);
	imtbase::CSelection* multiSelectionPtr = dynamic_cast<imtbase::CSelection*>(multiSelection.CloneMe());
	imtbase::CSelection* singleSelectionPtr = dynamic_cast<imtbase::CSelection*>(singleSelection.CloneMe());
	QVERIFY2(multiSelection.IsEqual(*multiSelectionPtr), qPrintable(QString("Function CSelection::CloneMe is failed with clone multi-select")));
	QVERIFY2(singleSelection.IsEqual(*singleSelectionPtr), qPrintable(QString("Function CSelection::CloneMe is failed with clone single-select")));
}


void CMultiSelectionTest::SerializeMemoryTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2;
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	iser::CMemoryWriteArchive memoryWritedArchive;
	bool checkSerializeMemory = multiSelection.Serialize(memoryWritedArchive);
	QString pathToInputFile = "../../Include/imtbase/Test/TestData/memory.txt";
	QFile memoryFile(pathToInputFile);
	memoryFile.open(QIODevice::ReadOnly);
	QByteArray dataFromFile = memoryFile.readAll();
	memoryFile.close();
	QByteArray dataFromWriteMemory(QByteArray::fromRawData((const char*)memoryWritedArchive.GetBuffer(), memoryWritedArchive.GetBufferSize()));
	QCOMPARE(dataFromFile, dataFromWriteMemory);
	QVERIFY2(checkSerializeMemory, qPrintable(QString("Function CSelection::Serialize is failed")));
	iser::CMemoryReadArchive memoryReadArchive(memoryWritedArchive);
	multiSelection2.Serialize(memoryReadArchive);
	QCOMPARE(multiSelection.GetSelectedIds(), multiSelection2.GetSelectedIds());
}


void CMultiSelectionTest::SerializeXmlTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2;
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	iser::CXmlStringWriteArchive xmlWritedArchive;
	bool checkSerializeXml = multiSelection.Serialize(xmlWritedArchive);
	QByteArray dataFromWriteXml = xmlWritedArchive.GetString();
	QString pathToInputFile = "../../Include/imtbase/Test/TestData/xml.xml";
	QFile xmlFile(pathToInputFile);
	xmlFile.open(QIODevice::ReadOnly);
	QByteArray dataFromFile = xmlFile.readAll();
	xmlFile.close();
	QCOMPARE(dataFromFile, dataFromWriteXml);
	QVERIFY2(checkSerializeXml, qPrintable(QString("Function CSelection::Serialize is failed")));
	iser::CXmlStringReadArchive xmlReadedArchive(xmlWritedArchive.GetString());
	multiSelection2.Serialize(xmlReadedArchive);
	QCOMPARE(multiSelection.GetSelectedIds(), multiSelection2.GetSelectedIds());
}


void CMultiSelectionTest::SerializeJsonTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2;
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	QByteArray inputData;
	iser::CJsonStringWriteArchive jsonWritedArchive(inputData);
	bool checkSerializeJson = multiSelection.Serialize(jsonWritedArchive);
}


void CMultiSelectionTest::ResetDataTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	multiSelection.ResetData();
	imtbase::ISelection::Ids multiSelectionIds = multiSelection.GetSelectedIds();
	QVERIFY2(multiSelectionIds.isEmpty(), qPrintable(QString("Function CSelection::ResetData is failed")));
}


void CMultiSelectionTest::cleanupTestCase()
{
	m_testMultiSelectedIds.clear();
}


I_ADD_TEST(CMultiSelectionTest);


