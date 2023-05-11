#include "CSelectionTest.h"


void CSelectionTest::initTestCase()
{
	QByteArray firstVar("firstKey");
	QByteArray secondVar("secondKey");
	QByteArray emptyKey("");
	m_testMultiSelectedIds += (firstVar);
	m_testMultiSelectedIds += (secondVar);
	m_testSingleSelectedIds += (secondVar);
	m_testSelectedWithEmptyKeysIds += (emptyKey);
}


void CSelectionTest::SetSelectedIdsInMultiSelectTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	bool checkMultiSelectData = multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	QVERIFY2(checkMultiSelectData, qPrintable(QString("Function CSelection::SetSelectedIds is failed in multi-select")));
	QCOMPARE(multiSelection.GetSelectedIds(), m_testMultiSelectedIds);
}


void CSelectionTest::SetSelectedIdsInSingleSelectTest()
{
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	bool checkSingleSelectData = singleSelection.SetSelectedIds(m_testSingleSelectedIds);
	QVERIFY2(checkSingleSelectData, qPrintable(QString("Function CSelection::SetSelectedIds is failed in single-select")));
	QCOMPARE(singleSelection.GetSelectedIds(), m_testSingleSelectedIds);
}


void CSelectionTest::SetSelectedEmptyIdsInMultiSelectTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	bool checkMultiSelectWithEmptyData = multiSelection.SetSelectedIds(m_testSelectedWithEmptyKeysIds);
	QVERIFY2(!checkMultiSelectWithEmptyData, qPrintable(QString("Function CSelection::SetSelectedIds is failed in multi-select with empty Ids list")));
}


void CSelectionTest::SetSelectedEmptyIdsInSingleSelectTest()
{
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	bool checkSingleSelectWithEmptyData = singleSelection.SetSelectedIds(m_testSelectedWithEmptyKeysIds);
	QVERIFY2(!checkSingleSelectWithEmptyData, qPrintable(QString("Function CSelection::SetSelectedIds is failed in multi-select with empty Ids list")));
}


void CSelectionTest::SetSelectedManyIdsInSingleSelectTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	bool checkSingleSelectWithManyData = singleSelection.SetSelectedIds(m_testMultiSelectedIds);
	QVERIFY2(!checkSingleSelectWithManyData, qPrintable(QString("Function CSelection::SetSelectedIds is failed in single-select with many select keys")));
}


void CSelectionTest::IsEqualMultiSelectTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	multiSelection2.SetSelectedIds(m_testMultiSelectedIds);
	QVERIFY2((multiSelection.IsEqual(multiSelection2)), qPrintable(QString("Function CSelection::IsEqual is failed with compare multi-selects")));
}


void CSelectionTest::IsNonEqualMultiSelectTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	QVERIFY2(!(multiSelection.IsEqual(multiSelection2)), qPrintable(QString("Function CSelection::IsEqual is failed with compare multi-selects (null vs non-null)")));
}


void CSelectionTest::IsNonEqualMultiSelectAndSingleSelectTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	singleSelection.SetSelectedIds(m_testSingleSelectedIds);
	QVERIFY2(!(multiSelection.IsEqual(singleSelection)), qPrintable(QString("Function CSelection::IsEqual is failed with compare single-select and multi-select")));
}


void CSelectionTest::IsEqualSingleSelectTest()
{
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	imtbase::CSelection singleSelection2(imtbase::ISelection::SelectionMode::SM_SINGLE);
	singleSelection.SetSelectedIds(m_testSingleSelectedIds);
	singleSelection2.SetSelectedIds(m_testSingleSelectedIds);
	QVERIFY2((singleSelection.IsEqual(singleSelection2)), qPrintable(QString("Function CSelection::IsEqual is failed with compare single-selects")));
}


void CSelectionTest::IsNonEqualSingleSelectTest()
{
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	imtbase::CSelection singleSelection2(imtbase::ISelection::SelectionMode::SM_SINGLE);
	singleSelection.SetSelectedIds(m_testSingleSelectedIds);
	QVERIFY2(!(singleSelection.IsEqual(singleSelection2)), qPrintable(QString("Function CSelection::IsEqual is failed with compare single-selects (null vs non-null)")));
}


void CSelectionTest::IsNonEqualEmptiesSingleSelectAndMultiSelectTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	QVERIFY2(!(singleSelection.IsEqual(multiSelection)), qPrintable(QString("Function CSelection::IsEqual is failed with compare single-select and multi-select (null vs null)")));
}


void CSelectionTest::CopyFromMultiSelectToMultiSelectTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	multiSelection2.CopyFrom(multiSelection);
	imtbase::ISelection::Ids multiSelectionIds = multiSelection.GetSelectedIds();
	imtbase::ISelection::Ids multiSelection2Ids = multiSelection2.GetSelectedIds();
	QCOMPARE(multiSelectionIds, multiSelection2Ids);
}


void CSelectionTest::CopyFromSingleSelectToSingleSelectTest()
{
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	imtbase::CSelection singleSelection2(imtbase::ISelection::SelectionMode::SM_SINGLE);
	singleSelection.SetSelectedIds(m_testSingleSelectedIds);
	singleSelection2.CopyFrom(singleSelection);
	imtbase::ISelection::Ids singleSelectionIds = singleSelection.GetSelectedIds();
	imtbase::ISelection::Ids singleSelection2Ids = singleSelection2.GetSelectedIds();
	QCOMPARE(singleSelectionIds, singleSelection2Ids);
}


void CSelectionTest::CopyFromMultiSelectToSingleSelectTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	singleSelection.CopyFrom(multiSelection);
	imtbase::ISelection::Ids singleSelectionIds = singleSelection.GetSelectedIds();
	imtbase::ISelection::Ids multiSelectionIds = multiSelection.GetSelectedIds();
	QCOMPARE(singleSelectionIds, multiSelectionIds);
}


void CSelectionTest::CloneMeSingleSelectTest()
{
	imtbase::CSelection singleSelection(imtbase::ISelection::SelectionMode::SM_SINGLE);
	singleSelection.SetSelectedIds(m_testSingleSelectedIds);
	imtbase::CSelection* singleSelectionPtr = dynamic_cast<imtbase::CSelection*>(singleSelection.CloneMe());
	QVERIFY2(singleSelection.IsEqual(*singleSelectionPtr), qPrintable(QString("Function CSelection::CloneMe is failed with clone single-select")));
}


void CSelectionTest::CloneMeMultiSelectTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	imtbase::CSelection* multiSelectionPtr = dynamic_cast<imtbase::CSelection*>(multiSelection.CloneMe());
	QVERIFY2(multiSelection.IsEqual(*multiSelectionPtr), qPrintable(QString("Function CSelection::CloneMe is failed with clone multi-select")));
}


void CSelectionTest::CheckSerializeMemoryTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	iser::CMemoryWriteArchive memoryWritedArchive;
	bool checkSerializeMemory = multiSelection.Serialize(memoryWritedArchive);
	QVERIFY2(checkSerializeMemory, qPrintable(QString("Function CSelection::Serialize is failed")));
}


void CSelectionTest::CompareSerializeMemoryDataWithInputFileDataTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	iser::CMemoryWriteArchive memoryWritedArchive;
	bool checkSerializeMemory = multiSelection.Serialize(memoryWritedArchive);
	QString pathToInputFile = qEnvironmentVariable("IMTCOREDIR")+"/Include/imtbase/Test/TestData/memory.txt";
	QFile memoryFile(pathToInputFile);
	memoryFile.open(QIODevice::ReadOnly);
	QByteArray dataFromFile = memoryFile.readAll();
	memoryFile.close();
	QByteArray dataFromWriteMemory(QByteArray::fromRawData((const char*)memoryWritedArchive.GetBuffer(), memoryWritedArchive.GetBufferSize()));
	QCOMPARE(dataFromFile, dataFromWriteMemory);
}


void CSelectionTest::SerializeMemoryTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2;
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	iser::CMemoryWriteArchive memoryWritedArchive;
	bool checkSerializeMemory = multiSelection.Serialize(memoryWritedArchive);
	iser::CMemoryReadArchive memoryReadArchive(memoryWritedArchive);
	multiSelection2.Serialize(memoryReadArchive);
	QCOMPARE(multiSelection.GetSelectedIds(), multiSelection2.GetSelectedIds());
}


void CSelectionTest::CheckSerializeXmlTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	iser::CXmlStringWriteArchive xmlWritedArchive;
	bool checkSerializeXml = multiSelection.Serialize(xmlWritedArchive);
	QVERIFY2(checkSerializeXml, qPrintable(QString("Function CSelection::Serialize is failed")));
}


void CSelectionTest::CompareSerializeXmlDataWithInputFileDataTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	iser::CXmlStringWriteArchive xmlWritedArchive;
	bool checkSerializeXml = multiSelection.Serialize(xmlWritedArchive);
	QByteArray dataFromWriteXml = xmlWritedArchive.GetString();
	QString pathToInputFile = qEnvironmentVariable("IMTCOREDIR")+"/Include/imtbase/Test/TestData/xml.xml";
	QFile xmlFile(pathToInputFile);
	xmlFile.open(QIODevice::ReadOnly);
	QByteArray dataFromFile = xmlFile.readAll();
	xmlFile.close();
	QCOMPARE(dataFromFile, dataFromWriteXml);
}


void CSelectionTest::SerializeXmlTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2;
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	iser::CXmlStringWriteArchive xmlWritedArchive;
	bool checkSerializeXml = multiSelection.Serialize(xmlWritedArchive);
	iser::CXmlStringReadArchive xmlReadedArchive(xmlWritedArchive.GetString());
	multiSelection2.Serialize(xmlReadedArchive);
	QCOMPARE(multiSelection.GetSelectedIds(), multiSelection2.GetSelectedIds());
}


void CSelectionTest::ResetDataTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testMultiSelectedIds);
	multiSelection.ResetData();
	imtbase::ISelection::Ids multiSelectionIds = multiSelection.GetSelectedIds();
	QVERIFY2(multiSelectionIds.isEmpty(), qPrintable(QString("Function CSelection::ResetData is failed")));
}


void CSelectionTest::cleanupTestCase()
{
	m_testMultiSelectedIds.clear();
}


I_ADD_TEST(CSelectionTest);
