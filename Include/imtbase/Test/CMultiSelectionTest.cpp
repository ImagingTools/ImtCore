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
	imtbase::CMultiSelection multiSelection;
	bool checkSelectData = multiSelection.SetSelectedIds(m_testSelectedIds);
	QVERIFY2(checkSelectData, qPrintable(QString("Function CMultiSelection::SetSelectedIds is failed")));
	QCOMPARE(multiSelection.GetSelectedIds(), m_testSelectedIds);
}

void CMultiSelectionTest::IsEqualTest()
{
	imtbase::CMultiSelection multiSelection;
	imtbase::CMultiSelection multiSelection2;
	imtbase::IMultiSelection::Ids selectedIds;
	QByteArray firstVar("firstKey");
	selectedIds.append(firstVar);
	multiSelection.SetSelectedIds(m_testSelectedIds);
	multiSelection2.SetSelectedIds(selectedIds);
	QVERIFY2((!multiSelection.IsEqual(multiSelection2)), qPrintable(QString("Function CMultiSelection::IsEqual is failed")));
}

void CMultiSelectionTest::CopyFromTest()
{
	imtbase::CMultiSelection multiSelection;
	imtbase::CMultiSelection multiSelection2;
	multiSelection.SetSelectedIds(m_testSelectedIds);
	multiSelection2.CopyFrom(multiSelection);
	imtbase::IMultiSelection::Ids multiSelectionIds = multiSelection.GetSelectedIds();
	imtbase::IMultiSelection::Ids multiSelection2Ids = multiSelection2.GetSelectedIds();
	QCOMPARE(multiSelectionIds, multiSelection2Ids);
}

void CMultiSelectionTest::CloneMeTest()
{
	imtbase::CMultiSelection multiSelection;
	multiSelection.SetSelectedIds(m_testSelectedIds);
	imtbase::CMultiSelection* multiSelectionPtr = dynamic_cast<imtbase::CMultiSelection*>(multiSelection.CloneMe());
	QVERIFY2(multiSelection.IsEqual(*multiSelectionPtr), qPrintable(QString("Function CMultiSelection::CloneMe is failed")));
}
void CMultiSelectionTest::ResetDataTest()
{
	imtbase::CMultiSelection multiSelection;
	multiSelection.SetSelectedIds(m_testSelectedIds);
	multiSelection.ResetData();
	imtbase::IMultiSelection::Ids multiSelectionIds = multiSelection.GetSelectedIds();
	QVERIFY2(multiSelectionIds.isEmpty(), qPrintable(QString("Function CMultiSelection::ResetData is failed")));
}


void CMultiSelectionTest::cleanupTestCase()
{
	m_testSelectedIds.clear();
}

I_ADD_TEST(CMultiSelectionTest);
