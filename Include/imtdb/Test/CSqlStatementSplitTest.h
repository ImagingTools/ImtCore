// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of imtdb::SplitSqlStatements(): the queries of the SQL delegates are split into statements before they run.
*/
class CSqlStatementSplitTest: public QObject
{
	Q_OBJECT
private slots:
	void SplitTest_data();
	void SplitTest();
	void EscapedValueKeepsSemicolonTest();
};


