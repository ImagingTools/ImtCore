// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDir>
#include <QtCore/QTemporaryDir>

// ImtCore includes
#include <imtsdl/ISdlEditableProcessArgumentsParser.h>


namespace imtsdlgentest
{


class CSdlGenTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();

	void TestBasicSchema();
	void TestComplexCollectionFilter();
	void TestUnion();
	void TestComplexUnion();
	void TestArrayNullabilityParsing();
	void TestImportedEnumArrayQObjectSetter();
	void TestTreeModelExplicitNullKey();
	void TestArrayBackendsRoundTrip_data();
	void TestArrayBackendsRoundTrip();
	void BenchmarkArrayGraphQlWriteMillionScalars();
	void BenchmarkArrayGraphQlReadMillionScalars();
	void TestArrayReadersRejectInvalidValues_data();
	void TestArrayReadersRejectInvalidValues();
	void TestArrayWritersRejectInvalidValues_data();
	void TestArrayWritersRejectInvalidValues();
	void TestArrayOptionalReadersPreserveMissingFields();
	void TestArrayQObjectBridge();
	void TestArrayJsonFileRoundTrip();
	void TestArrayGraphQlRequestFileRoundTrip();
	void TestNestedFieldNameCollision();
	void PrinterTest();
	void SubstrateSpecifications();

	void cleanup();
	void cleanupTestCase();

private:
	QDir m_tempOutputDir;
	QTemporaryDir m_arraySerializationOutputDirectory;
	bool m_isAllTestsPassed;

};


} //namespace imtsdlgentest
