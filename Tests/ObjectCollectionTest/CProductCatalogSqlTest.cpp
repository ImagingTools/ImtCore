// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CProductCatalogSqlTest.h"


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ImtCore includes
#include <imtauthdb/ProductCatalogSql.h>
#include <imtdb/CSqlDatabaseObjectCollectionIterator.h>


void CProductCatalogSqlTest::init()
{
	m_database = QSqlDatabase::addDatabase("QSQLITE", "ProductCatalogSqlTest");
	m_database.setDatabaseName(":memory:");
	QVERIFY2(m_database.open(), qPrintable(m_database.lastError().text()));
}


void CProductCatalogSqlTest::cleanup()
{
	m_database.close();
	m_database = QSqlDatabase();
	QSqlDatabase::removeDatabase("ProductCatalogSqlTest");
}


void CProductCatalogSqlTest::ActiveProductUniqueness()
{
	QSqlQuery query(m_database);
	QVERIFY(query.exec("CREATE TABLE products (\"ProductId\" TEXT, \"State\" TEXT)"));
	QVERIFY(query.exec(imtauthdb::ProductCatalogSql::ActiveProductIndex("\"main\".\"unique_product\"", "\"products\"")));
	QVERIFY(query.exec("INSERT INTO products VALUES ('one', 'Active')"));
	QVERIFY(!query.exec("INSERT INTO products VALUES ('one', 'Active')"));
	QVERIFY(query.exec("INSERT INTO products VALUES ('one', 'InActive'), ('one', 'Disabled'), ('two', 'Active')"));

	QVERIFY(m_database.transaction());
	QVERIFY(query.exec("UPDATE products SET \"State\" = 'InActive' WHERE \"ProductId\" = 'one' AND \"State\" = 'Active'"));
	QVERIFY(query.exec("INSERT INTO products VALUES ('one', 'Active')"));
	QVERIFY(m_database.commit());
	QVERIFY(!query.exec("UPDATE products SET \"State\" = 'Active' WHERE \"ProductId\" = 'one' AND \"State\" = 'Disabled'"));
	QVERIFY(query.exec("SELECT COUNT(*) FROM products WHERE \"ProductId\" = 'one'"));
	QVERIFY(query.next());
	QCOMPARE(query.value(0).toInt(), 4);
}


void CProductCatalogSqlTest::DuplicateMigrationPreservesData()
{
	QSqlQuery query(m_database);
	QVERIFY(query.exec("CREATE TABLE products (\"ProductId\" TEXT, \"State\" TEXT)"));
	QVERIFY(query.exec("INSERT INTO products VALUES ('duplicate', 'Active'), ('duplicate', 'Active')"));
	QVERIFY(m_database.transaction());
	QVERIFY(!query.exec(imtauthdb::ProductCatalogSql::ActiveProductIndex("\"unique_product\"", "products")));
	QVERIFY(m_database.rollback());
	QVERIFY(query.exec("SELECT COUNT(*) FROM products"));
	QVERIFY(query.next());
	QCOMPARE(query.value(0).toInt(), 2);
}


void CProductCatalogSqlTest::IteratorCompleteness()
{
	QSqlQuery query(m_database);
	QVERIFY(query.exec("SELECT 1 UNION ALL SELECT 2"));
	imtdb::CSqlDatabaseObjectCollectionIterator unpaged(query, nullptr);
	QCOMPARE(unpaged.GetElementsCount(), 2);
	QVERIFY(unpaged.Next());
	QVERIFY(unpaged.Next());
	QVERIFY(!unpaged.Next());

	QVERIFY(query.exec("SELECT 1"));
	imtdb::CSqlDatabaseObjectCollectionIterator paged(query, nullptr, 8);
	QCOMPARE(paged.GetElementsCount(), 8);

	QVERIFY(query.exec("SELECT json_extract(value, '$.x') FROM (SELECT '{\"x\":1}' AS value UNION ALL SELECT 'invalid')"));
	imtdb::CSqlDatabaseObjectCollectionIterator incomplete(query, nullptr);
	QCOMPARE(incomplete.GetElementsCount(), -1);
	QVERIFY(!incomplete.Next());
}


#ifdef PRODUCT_CATALOG_STANDALONE_TEST
QTEST_GUILESS_MAIN(CProductCatalogSqlTest)
#else
#include <itest/CStandardTestExecutor.h>
I_ADD_TEST(CProductCatalogSqlTest);
#endif
