// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/Test/CDuckSqlResultTest.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtduckdb/CDuckSqlDriver.h>


void CDuckSqlResultTest::initTestCase()
{
	QVERIFY(duckdb_open(nullptr, &m_database) == DuckDBSuccess);
	QVERIFY(duckdb_connect(m_database, &m_connection) == DuckDBSuccess);
}


void CDuckSqlResultTest::cleanupTestCase()
{
	duckdb_disconnect(&m_connection);
	duckdb_close(&m_database);
}


void CDuckSqlResultTest::exactIntegers()
{
	imtduckdb::CDuckSqlDriver driver(m_connection);
	QSqlQuery query(driver.createResult());
	QVERIFY2(query.exec(QStringLiteral(
				"SELECT '-9223372036854775808'::HUGEINT, '9223372036854775809'::HUGEINT, "
				"'18446744073709551615'::UHUGEINT, '-9223372036854775809'::HUGEINT, "
				"'18446744073709551616'::HUGEINT, '340282366920938463463374607431768211455'::UHUGEINT")), qPrintable(query.lastError().text()));
	QVERIFY(query.next());
	QCOMPARE(query.value(0).typeId(), static_cast<int>(QMetaType::LongLong));
	QCOMPARE(query.value(0).toLongLong(), INT64_MIN);
	QCOMPARE(query.value(1).typeId(), static_cast<int>(QMetaType::ULongLong));
	QCOMPARE(query.value(1).toULongLong(), UINT64_C(9223372036854775809));
	QCOMPARE(query.value(2).typeId(), static_cast<int>(QMetaType::ULongLong));
	QCOMPARE(query.value(2).toULongLong(), UINT64_MAX);
	QCOMPARE(query.value(3).toString(), QStringLiteral("-9223372036854775809"));
	QCOMPARE(query.value(4).toString(), QStringLiteral("18446744073709551616"));
	QCOMPARE(query.record().value(5).toString(), QStringLiteral("340282366920938463463374607431768211455"));
}


void CDuckSqlResultTest::timestamps()
{
	imtduckdb::CDuckSqlDriver driver(m_connection);
	QSqlQuery query(driver.createResult());
	QVERIFY2(query.exec(QStringLiteral(
				"SELECT TIMESTAMP '1969-12-31 23:59:59.999999', TIMESTAMP_NS '1969-12-31 23:59:59.999999999', "
				"TIMESTAMP '1970-01-01 00:00:00.000001', TIMESTAMP_S 'infinity', "
				"TIMESTAMP_MS '-infinity', TIMESTAMPTZ 'infinity', NULL::TIMESTAMP")), qPrintable(query.lastError().text()));
	QVERIFY(query.next());
	QCOMPARE(query.value(0).toDateTime().toMSecsSinceEpoch(), -1);
	QCOMPARE(query.value(1).toDateTime().toMSecsSinceEpoch(), -1);
	QVERIFY(query.value(2).toDateTime().isValid());
	QCOMPARE(query.value(2).toDateTime().toMSecsSinceEpoch(), 0);
	QCOMPARE(query.value(3).toString(), QStringLiteral("infinity"));
	QCOMPARE(query.value(4).toString(), QStringLiteral("-infinity"));
	QCOMPARE(query.value(5).toString(), QStringLiteral("infinity"));
	QVERIFY(!query.isNull(3));
	QVERIFY(query.isNull(6));
}


void CDuckSqlResultTest::unsupportedSchema()
{
	imtduckdb::CDuckSqlDriver driver(m_connection);
	QSqlQuery query(driver.createResult());
	for (const QString& expression : {QStringLiteral("INTERVAL '1 day'"), QStringLiteral("[]::INTERVAL[]"),
				QStringLiteral("NULL::STRUCT(a INTERVAL[2])")}){
		for (int limit : {0, 1}){
			QVERIFY(!query.exec(QStringLiteral("SELECT %1 AS unsupported_column LIMIT %2").arg(expression).arg(limit)));
			QVERIFY(query.lastError().text().contains(QStringLiteral("Column 'unsupported_column'")));
		}
	}

	QVERIFY(query.exec(QStringLiteral("SELECT [1, 2] AS supported LIMIT 0")));
	QCOMPARE(query.record().count(), 1);
	QCOMPARE(query.size(), 0);
	QVERIFY(!query.next());
}


void CDuckSqlResultTest::navigation_data()
{
	QTest::addColumn<bool>("forwardOnly");
	QTest::addColumn<bool>("readSize");
	QTest::newRow("sequential") << true << false;
	QTest::newRow("sequential with size") << true << true;
	QTest::newRow("scrollable") << false << false;
	QTest::newRow("scrollable with size") << false << true;
}


void CDuckSqlResultTest::navigation()
{
	QFETCH(bool, forwardOnly);
	QFETCH(bool, readSize);

	imtduckdb::CDuckSqlDriver driver(m_connection);
	QSqlQuery query(driver.createResult());
	query.setForwardOnly(forwardOnly);
	QVERIFY(query.exec(QStringLiteral("SELECT i, {'items': [i, NULL]} AS nested FROM range(10000) t(i) ORDER BY i")));
	QVERIFY(query.next());
	if (readSize){
		QCOMPARE(query.size(), 10000);
		QCOMPARE(query.at(), 0);
	}
	QCOMPARE(query.value(0).toInt(), 0);
	QVERIFY(query.seek(7000));
	const QVariantList items = query.record().value(1).toMap().value(QStringLiteral("items")).toList();
	QCOMPARE(items.size(), 2);
	QCOMPARE(items.at(0).toInt(), 7000);
	QVERIFY(!items.at(1).isValid());
	QVERIFY(query.last());
	QCOMPARE(query.value(0).toInt(), 9999);
	QCOMPARE(query.size(), 10000);
	if (!forwardOnly){
		QVERIFY(query.seek(3));
		QCOMPARE(query.value(0).toInt(), 3);
	}
}


void CDuckSqlResultTest::preparedQueryReuse()
{
	imtduckdb::CDuckSqlDriver driver(m_connection);
	QSqlQuery query(driver.createResult());
	QVERIFY(query.prepare(QStringLiteral("SELECT i FROM range(CAST(:count AS BIGINT)) t(i) ORDER BY i")));
	query.bindValue(QStringLiteral(":count"), 10000);
	QVERIFY(query.exec());
	QVERIFY(query.next());
	query.finish();
	query.bindValue(QStringLiteral(":count"), 3);
	QVERIFY(query.exec());
	QVERIFY(query.last());
	QCOMPARE(query.value(0).toInt(), 2);
	QCOMPARE(query.size(), 3);
}


I_ADD_TEST(CDuckSqlResultTest);

