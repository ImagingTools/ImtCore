#pragma once


// Qt includes
#include <QtTest/QtTest>
#include <itest/CStandardTestExecutor.h>


class CGqlRequestTest : public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();

	void CreateArrayQuery();
	void CreateUnionRequest();
	void ParseUnionFields();

	void ParseFields();
	void ParseComplexFields();
	void ParseSimple();
	void ParseEmpty();
	void ParseSubstrateTest();
	void ParseArrayOfObjectArraysQuery();
	void ParseObjectQuery();
	void ParseArrayQuery();
	void ParseComplexTest();

	void cleanupTestCase();

private:
};


