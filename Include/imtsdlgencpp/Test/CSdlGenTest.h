// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDir>

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
	void PrinterTest();

	void cleanup();
	void cleanupTestCase();

private:
	QDir m_tempOutputDir;
	bool m_isAllTestsPassed;

};


} //namespace imtsdlgentest
