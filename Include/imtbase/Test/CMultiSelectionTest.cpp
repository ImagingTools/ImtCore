#include "CMultiSelectionTest.h"


void CMultiSelectionTest::initTestCase()
{
	QByteArray firstVar("firstKey");
	QByteArray thirdVar("thirdKey");
	m_testSelectedIds.append(firstVar);
	m_testSelectedIds.append(thirdVar);
}


void CMultiSelectionTest::SetSelectedIdsTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	bool checkSelectData = multiSelection.SetSelectedIds(m_testSelectedIds);
	QVERIFY2(checkSelectData, qPrintable(QString("Function CSelection::SetSelectedIds is failed")));
	QCOMPARE(multiSelection.GetSelectedIds(), m_testSelectedIds);
}

void CMultiSelectionTest::IsEqualTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::ISelection::Ids selectedIds;
	QByteArray firstVar("firstKey");
	selectedIds.append(firstVar);
	multiSelection.SetSelectedIds(m_testSelectedIds);
	multiSelection2.SetSelectedIds(selectedIds);
	QVERIFY2((!multiSelection.IsEqual(multiSelection2)), qPrintable(QString("Function CSelection::IsEqual is failed")));
}

void CMultiSelectionTest::CopyFromTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testSelectedIds);
	multiSelection2.CopyFrom(multiSelection);
	imtbase::ISelection::Ids multiSelectionIds = multiSelection.GetSelectedIds();
	imtbase::ISelection::Ids multiSelection2Ids = multiSelection2.GetSelectedIds();
	QCOMPARE(multiSelectionIds, multiSelection2Ids);
	}

void CMultiSelectionTest::CloneMeTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testSelectedIds);
	imtbase::CSelection* multiSelectionPtr = dynamic_cast<imtbase::CSelection*>(multiSelection.CloneMe());
	QVERIFY2(multiSelection.IsEqual(*multiSelectionPtr), qPrintable(QString("Function CSelection::CloneMe is failed")));
}

void CMultiSelectionTest::SerializeTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	imtbase::CSelection multiSelection2;

	multiSelection.SetSelectedIds(m_testSelectedIds);
	iser::CMemoryWriteArchive memoryWritedArchive;
	bool checkSerializeMemory = multiSelection.Serialize(memoryWritedArchive);
	QVERIFY2(checkSerializeMemory, qPrintable(QString("Function CSelection::Serialize is failed")));
	iser::CMemoryReadArchive memoryReadArchive(memoryWritedArchive);
	multiSelection2.Serialize(memoryReadArchive);
	QCOMPARE(multiSelection.GetSelectedIds(), multiSelection2.GetSelectedIds());
	multiSelection2.ResetData();
	iser::CXmlStringWriteArchive xmlWritedArchive;
	bool checkSerializeXml = multiSelection.Serialize(xmlWritedArchive);
	QByteArray test = xmlWritedArchive.GetString();
	QVERIFY2(checkSerializeXml, qPrintable(QString("Function CSelection::Serialize is failed")));
	iser::CXmlStringReadArchive xmlReadedArchive(xmlWritedArchive.GetString());
	multiSelection2.Serialize(xmlReadedArchive);
	QCOMPARE(multiSelection.GetSelectedIds(), multiSelection2.GetSelectedIds());
}
void CMultiSelectionTest::ResetDataTest()
{
	imtbase::CSelection multiSelection(imtbase::ISelection::SelectionMode::SM_MULTI);
	multiSelection.SetSelectedIds(m_testSelectedIds);
	multiSelection.ResetData();
	imtbase::ISelection::Ids multiSelectionIds = multiSelection.GetSelectedIds();
	QVERIFY2(multiSelectionIds.isEmpty(), qPrintable(QString("Function CSelection::ResetData is failed")));
}


void CMultiSelectionTest::cleanupTestCase()
{
	m_testSelectedIds.clear();
}


I_ADD_TEST(CMultiSelectionTest);


