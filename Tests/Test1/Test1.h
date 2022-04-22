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


class Test1: public QObject
{
	Q_OBJECT

private slots:
	void InitTestCase();
	void TestCase1();
	void CleanupTestCase();

private:
	istd::TDelPtr<ipackage::CComponentAccessor> compositePtr;
};
