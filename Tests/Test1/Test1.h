#pragma once

// std includes
#include <memory>

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>
#include <ipackage/CComponentAccessor.h>
#include <istd/TDelPtr.h>


class CObjectCollectionTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void CheckSerializeTest();
	void CreateNewObjectTest_data();
	void CreateNewObjectTest();
	void cleanupTestCase();

private:
	istd::TDelPtr<ipackage::CComponentAccessor> m_compositePtr;
	QByteArray m_typeIdObjectCollection;
};
