#pragma once


// Qt includes
#include <QtTest/QtTest>
#include <itest/CStandardTestExecutor.h>


class CGqlRequestTest : public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();

	void ParseQueryTest();

	void cleanupTestCase();

private:
};


