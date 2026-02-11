// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDir>

// ImtCore includes
#include <imtsdl/ISdlEditableProcessArgumentsParser.h>


namespace imtsdlgenqmltest
{


class CSdlGenQmlTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();

	void TestComplexCollectionFilter();
	void TestGenerationResultSerialization();
	void TestGenerationResultFileOperations();
	void TestAppendFoldersWithAutomaticTimestamp();
	void TestAppendFoldersWithSpecificTimestamp();
	void TestGenerationResultJsonFormat();

	void cleanup();
	void cleanupTestCase();

private:
	QDir m_tempOutputDir;
	bool m_isAllTestsPassed;
};


} //namespace imtsdlgenqmltest
