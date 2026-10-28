// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QTemporaryDir>


namespace imtsdlgentest
{


/**
	Tests of the code, generated from the SDL schemas at build time.
	Unlike \c CSdlGenTest, it uses the generated classes together with the libraries, that are required by them (imtbase, imtgql, imtservergql).
*/
class CSdlGenIntegrationTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
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
	void TestRequestedFields_data();
	void TestRequestedFields();
	void TestRequestedFieldsWithComplexArguments();
	void TestRequestedFieldsOfMutation();
	void TestRequestedFieldsWithFieldNamedAsCommand();
	void TestRequestedFieldsOfBuiltRequest();

private:
	QTemporaryDir m_arraySerializationOutputDirectory;
};


} //namespace imtsdlgentest
