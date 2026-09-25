// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>

// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <imtdb/CTagFilterSqlBuilder.h>


/**
	Runs the SQL produced by CTagFilterSqlBuilder against an in-memory SQLite database
	created with the real table scripts of imtdb and imttagdb.
*/
class CTagFilterSqlBuilderTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void testSingleCondition_data();
	void testSingleCondition();
	void testEmptyTagListSelectsNothing();
	void testCombinedGroups();
	void testHostileTagIds();
	void cleanupTestCase();

private:
	QByteArrayList SelectEntities(const QString& whereClause) const;
	QByteArrayList SelectEntities(const imtbase::IComplexCollectionFilter::FilterExpression& expression) const;
	bool ExecuteScript(const QString& resourcePath, const QString& tableName);
	bool Execute(const QString& query);

	QSqlDatabase m_database;
	imtdb::CTagFilterSqlBuilder::Config m_config;
};


