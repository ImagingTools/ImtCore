#pragma once

// std includes
#include <memory>

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes


class Test2: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();

private:

};
