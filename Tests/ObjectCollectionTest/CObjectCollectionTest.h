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
	void InsertNewObjectWithRequiredParamsTest_data();
	void InsertNewObjectWithRequiredParamsTest();
	void InsertNewObjectWithDataTest();
	void InsertNewObjectWithNonExistElementTest();
	void InsertNewObjectWithExistElementTest();
	void RemoveExistObjectTest();
	void RemoveNonExistObjectTest();
	void cleanupTestCase();
	void CheckSerializeTest();

private:
	istd::TDelPtr<ipackage::CComponentAccessor> m_compositePtr;
	QByteArray m_typeIdObjectCollection;
};
