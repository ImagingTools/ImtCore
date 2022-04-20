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
#include <imtbase/IMultiSelection.h>
#include <imtbase/CMultiSelection.h>

class CMultiSelectionTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void SetSelectedIdsTest();
	void IsEqualTest();
	void CopyFromTest();
	void CloneMeTest();
	void SerializeTest();
	void ResetDataTest();
	void cleanupTestCase();

private:
	imtbase::IMultiSelection::Ids m_testSelectedIds;
};
