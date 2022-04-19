#include "CMultiSelectionTest.h"


void CMultiSelectionTest::initTestCase()
{
	QByteArray firstVar("firstKey");
	QByteArray secondVar("secondKey");
	QByteArray thirdVar("thirdKey");
	m_selectedIds.append(firstVar);
	m_selectedIds.append(secondVar);
	m_selectedIds.append(thirdVar);
}


void CMultiSelectionTest::SetSelectedIdsTest()
{
	imtbase::IMultiSelection::Ids selectedIds;
	QByteArray firstVar("firstKey");
	QByteArray thirdVar("thirdKey");
	selectedIds.append(firstVar);
	selectedIds.append(thirdVar);
	if (m_selectedIds != selectedIds){
		m_selectedIds = selectedIds;
	}
	QVERIFY2((m_selectedIds == selectedIds), qPrintable(QString("Selected ids is not set")));
}

void CMultiSelectionTest::IsEqualWrongTest()
{
	imtbase::IMultiSelection::Ids selectedIds;
	QByteArray firstVar("firstKey");
	selectedIds.append(firstVar);
	QCOMPARE(m_selectedIds, selectedIds);
}


void CMultiSelectionTest::ResetDataWrongTest()
{
	if (!m_selectedIds.isEmpty()){
		m_selectedIds.clear();
	}
	QByteArray firstVar("firstKey");
	m_selectedIds.append(firstVar);
	QVERIFY2(m_selectedIds.isEmpty(), qPrintable(QString("Data from m_selectedIds is not deleted")));
}


void CMultiSelectionTest::cleanupTestCase()
{
	m_selectedIds.clear();
}

I_ADD_TEST(CMultiSelectionTest);
