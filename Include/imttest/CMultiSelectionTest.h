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
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/IMultiSelection.h>

class CMultiSelectionTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void SetSelectedIdsTest();
	void IsEqualWrongTest();
	void ResetDataWrongTest();
	void cleanupTestCase();

private:
	imtbase::IMultiSelection::Ids m_selectedIds;

};
