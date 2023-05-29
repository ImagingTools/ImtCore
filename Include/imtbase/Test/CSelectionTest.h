#pragma once

// std includes
#include <memory>

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iprm/IOptionsList.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <iser/CXmlStringWriteArchive.h>
#include <iser/CXmlStringReadArchive.h>
#include <iser/CReadArchiveBase.h>
#include <iser/CWriteArchiveBase.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/CSelection.h>


class CSelectionTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void SetSelectedIdsInMultiSelectTest();
	void SetSelectedIdsInSingleSelectTest();
	void SetSelectedEmptyIdsInMultiSelectTest();
	void SetSelectedEmptyIdsInSingleSelectTest();
	void SetSelectedManyIdsInSingleSelectTest();
	void IsEqualMultiSelectTest();
	void IsNonEqualMultiSelectTest();
	void IsNonEqualMultiSelectAndSingleSelectTest();
	void IsEqualSingleSelectTest();
	void IsNonEqualSingleSelectTest();
	void IsNonEqualEmptiesSingleSelectAndMultiSelectTest();
	void CopyFromMultiSelectToMultiSelectTest();
	void CopyFromSingleSelectToSingleSelectTest();
	void CopyFromMultiSelectToSingleSelectTest();
	void CloneMeSingleSelectTest();
	void CloneMeMultiSelectTest();
	void CheckSerializeMemoryTest();
	void CompareSerializeMemoryDataWithInputFileDataTest();
	void SerializeMemoryTest();
	void CheckSerializeXmlTest();
	void CompareSerializeXmlDataWithInputFileDataTest();
	void SerializeXmlTest();
	void ResetDataTest();
	void cleanupTestCase();

private:
	imtbase::ISelection::Ids m_testMultiSelectedIds;
	imtbase::ISelection::Ids m_testSingleSelectedIds;
	imtbase::ISelection::Ids m_testEmptySelectedIds;
	imtbase::ISelection::Ids m_testSelectedWithEmptyKeysIds;
};


