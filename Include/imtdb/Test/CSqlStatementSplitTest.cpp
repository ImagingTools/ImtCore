// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSqlStatementSplitTest.h"


// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtdb/imtdb.h>


void CSqlStatementSplitTest::SplitTest_data()
{
	QTest::addColumn<QString>("query");
	QTest::addColumn<QStringList>("statements");

	QTest::newRow("statements")
				<< QStringLiteral("DELETE FROM a;\nDELETE FROM b;")
				<< QStringList{QStringLiteral("DELETE FROM a"), QStringLiteral("\nDELETE FROM b")};
	QTest::newRow("no terminating semicolon")
				<< QStringLiteral("DELETE FROM a")
				<< QStringList{QStringLiteral("DELETE FROM a")};
	QTest::newRow("semicolon in a literal")
				<< QStringLiteral("UPDATE t SET d='a; DROP TABLE t; b' WHERE id='1';DELETE FROM b;")
				<< QStringList{QStringLiteral("UPDATE t SET d='a; DROP TABLE t; b' WHERE id='1'"), QStringLiteral("DELETE FROM b")};
	QTest::newRow("escaped quote in a literal")
				<< QStringLiteral("UPDATE t SET d='it''s; ok';DELETE FROM b")
				<< QStringList{QStringLiteral("UPDATE t SET d='it''s; ok'"), QStringLiteral("DELETE FROM b")};
	QTest::newRow("literal that is only an escaped quote")
				<< QStringLiteral("UPDATE t SET d='''';DELETE FROM b")
				<< QStringList{QStringLiteral("UPDATE t SET d=''''"), QStringLiteral("DELETE FROM b")};
	QTest::newRow("semicolon in an identifier")
				<< QStringLiteral("SELECT \"a;\"\"b\" FROM t;DELETE FROM b")
				<< QStringList{QStringLiteral("SELECT \"a;\"\"b\" FROM t"), QStringLiteral("DELETE FROM b")};
	QTest::newRow("quote and semicolon in a line comment")
				<< QStringLiteral("-- don't; split\nDELETE FROM a;DELETE FROM b")
				<< QStringList{QStringLiteral("-- don't; split\nDELETE FROM a"), QStringLiteral("DELETE FROM b")};
	QTest::newRow("quote and semicolon in a block comment")
				<< QStringLiteral("/* it's; one */DELETE FROM a;DELETE FROM b")
				<< QStringList{QStringLiteral("/* it's; one */DELETE FROM a"), QStringLiteral("DELETE FROM b")};
	QTest::newRow("block comment that starts with a slash")
				<< QStringLiteral("/*/; still a comment */DELETE FROM a;DELETE FROM b")
				<< QStringList{QStringLiteral("/*/; still a comment */DELETE FROM a"), QStringLiteral("DELETE FROM b")};
	QTest::newRow("empty statements")
				<< QStringLiteral(";; \n;DELETE FROM a;\n")
				<< QStringList{QStringLiteral("DELETE FROM a")};
	QTest::newRow("empty query")
				<< QString()
				<< QStringList();
}


void CSqlStatementSplitTest::SplitTest()
{
	QFETCH(QString, query);
	QFETCH(QStringList, statements);

	QCOMPARE(imtdb::SplitSqlStatements(query), statements);
}


void CSqlStatementSplitTest::EscapedValueKeepsSemicolonTest()
{
	const QString description = QStringLiteral("Step 1; step 2; O'Reilly");
	const QString query = QStringLiteral("UPDATE \"Tickets\" SET \"Description\"='%1' WHERE \"Id\"='1';DELETE FROM b;")
				.arg(imtdb::EscapeSql(description));

	const QStringList statements = imtdb::SplitSqlStatements(query);

	QCOMPARE(statements.count(), 2);
	QCOMPARE(statements.first(), QStringLiteral("UPDATE \"Tickets\" SET \"Description\"='Step 1; step 2; O''Reilly' WHERE \"Id\"='1'"));
}


I_ADD_TEST(CSqlStatementSplitTest);


