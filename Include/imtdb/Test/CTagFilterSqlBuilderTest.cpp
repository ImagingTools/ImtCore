// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTagFilterSqlBuilderTest.h"


// Qt includes
#include <QtCore/QFile>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>


typedef imtbase::IComplexCollectionFilter Filter;


static Filter::FieldFilter TagFilter(Filter::FieldOperation operation, const QStringList& tagIds)
{
	QVariantList values;
	for (const QString& tagId : tagIds){
		values << tagId;
	}

	return Filter::FieldFilter(imtdb::CTagFilterSqlBuilder::s_tagsFieldId, values, operation);
}


// private slots

void CTagFilterSqlBuilderTest::initTestCase()
{
	Q_INIT_RESOURCE(imtdb);
	Q_INIT_RESOURCE(imttagdb);

	m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("CTagFilterSqlBuilderTest"));
	m_database.setDatabaseName(QStringLiteral(":memory:"));
	QVERIFY2(m_database.open(), qPrintable(m_database.lastError().text()));

	QVERIFY(ExecuteScript(QStringLiteral(":/SQL/SQLite/CreateCollectionTable.sql"), QStringLiteral("Tags")));
	QVERIFY(ExecuteScript(QStringLiteral(":/SQL/SQLite/CreateCollectionTable.sql"), QStringLiteral("Things")));
	QVERIFY(ExecuteScript(QStringLiteral(":/SQL/SQLite/CreateTagAssignmentsTable.sql"), QString()));

	// Tag "d" is deleted; its assignments must be invisible to the empty/non-empty checks.
	const QList<QPair<QString, QString>> tags = {{"a", "Active"}, {"b", "Active"}, {"c", "Active"}, {"d", "Disabled"}};
	for (const QPair<QString, QString>& tag : tags){
		QVERIFY(Execute(QStringLiteral(R"(INSERT INTO "Tags" ("DocumentId", "Document", "Name", "TimeStamp", "State") VALUES ('%1', '{}', '%1', '2026-01-01', '%2'))")
					.arg(tag.first, tag.second)));
	}

	for (int i = 1; i <= 5; ++i){
		QVERIFY(Execute(QStringLiteral(R"(INSERT INTO "Things" ("DocumentId", "Document", "Name", "TimeStamp", "State") VALUES ('e%1', '{}', 'thing%1', '2026-01-01', 'Active'))")
					.arg(i)));
	}

	// e1 {a, b}, e2 {a}, e3 {}, e4 {d (deleted)}, e5 {c}; the "Other" type must never leak into "Things".
	const QList<QStringList> assignments = {
		{"Things", "e1", "a"}, {"Things", "e1", "b"}, {"Things", "e2", "a"},
		{"Things", "e4", "d"}, {"Things", "e5", "c"}, {"Other", "e2", "b"}, {"Other", "e3", "a"}
	};
	int assignmentIndex = 0;
	for (const QStringList& assignment : assignments){
		QVERIFY(Execute(QStringLiteral(R"(INSERT INTO "TagAssignments" ("Id", "TagId", "EntityType", "EntityId", "CreatedAt") VALUES ('%1', '%2', '%3', '%4', '2026-01-01'))")
					.arg(QString::number(++assignmentIndex), assignment[2], assignment[0], assignment[1])));
	}

	m_config.assignmentsTable = QStringLiteral(R"("TagAssignments")");
	m_config.tagsTable = QStringLiteral(R"("Tags")");
	m_config.entityType = QStringLiteral("Things");
	m_config.entityIdExpression = QStringLiteral(R"(root."DocumentId")");
	m_config.isSqlite = true;
}


void CTagFilterSqlBuilderTest::testSingleCondition_data()
{
	QTest::addColumn<int>("operation");
	QTest::addColumn<QStringList>("tagIds");
	QTest::addColumn<QByteArrayList>("expected");

	QTest::newRow("label:a") << int(Filter::FO_ARRAY_HAS_ANY) << QStringList{"a"} << QByteArrayList{"e1", "e2"};
	QTest::newRow("label:b,c") << int(Filter::FO_ARRAY_HAS_ANY) << QStringList{"b", "c"} << QByteArrayList{"e1", "e5"};
	QTest::newRow("label:a label:b") << int(Filter::FO_ARRAY_HAS_ALL) << QStringList{"a", "b"} << QByteArrayList{"e1"};
	QTest::newRow("label:a label:a") << int(Filter::FO_ARRAY_HAS_ALL) << QStringList{"a", "a"} << QByteArrayList{"e1", "e2"};
	QTest::newRow("-label:a") << int(Filter::FO_ARRAY_NOT_HAS_ANY) << QStringList{"a"} << QByteArrayList{"e3", "e4", "e5"};
	QTest::newRow("not all of a,b") << int(Filter::FO_ARRAY_NOT_HAS_ALL) << QStringList{"a", "b"} << QByteArrayList{"e2", "e3", "e4", "e5"};
	QTest::newRow("no:label") << int(Filter::FO_ARRAY_IS_EMPTY) << QStringList() << QByteArrayList{"e3", "e4"};
	QTest::newRow("has any label") << int(Filter::FO_ARRAY_NOT_IS_EMPTY) << QStringList() << QByteArrayList{"e1", "e2", "e5"};
}


void CTagFilterSqlBuilderTest::testSingleCondition()
{
	QFETCH(int, operation);
	QFETCH(QStringList, tagIds);
	QFETCH(QByteArrayList, expected);

	const QString condition = imtdb::CTagFilterSqlBuilder::CreateCondition(TagFilter(Filter::FieldOperation(operation), tagIds), m_config);
	QVERIFY(!condition.isEmpty());

	QCOMPARE(SelectEntities(condition), expected);
}


void CTagFilterSqlBuilderTest::testEmptyTagListSelectsNothing()
{
	QVERIFY(imtdb::CTagFilterSqlBuilder::CreateCondition(TagFilter(Filter::FO_ARRAY_HAS_ANY, QStringList()), m_config).isEmpty());
	QVERIFY(imtdb::CTagFilterSqlBuilder::CreateCondition(TagFilter(Filter::FO_ARRAY_HAS_ALL, QStringList()), m_config).isEmpty());

	// An ignored tag field must not turn the whole filter into a condition over a JSON field "Tags".
	Filter::FilterExpression expression;
	expression.fieldFilters << TagFilter(Filter::FO_ARRAY_HAS_ANY, QStringList());
	expression.fieldFilters << Filter::FieldFilter("Name", QStringLiteral("thing3"));

	QCOMPARE(SelectEntities(expression), QByteArrayList{"e3"});
}


void CTagFilterSqlBuilderTest::testCombinedGroups()
{
	// label:a -label:b
	Filter::FilterExpression andExpression;
	andExpression.fieldFilters << TagFilter(Filter::FO_ARRAY_HAS_ANY, {"a"});
	andExpression.fieldFilters << TagFilter(Filter::FO_ARRAY_NOT_HAS_ANY, {"b"});
	QCOMPARE(SelectEntities(andExpression), QByteArrayList{"e2"});

	// Name = thing5 OR (label:a label:b)
	Filter::FilterExpression orExpression;
	orExpression.logicalOperation = Filter::LO_OR;
	orExpression.fieldFilters << Filter::FieldFilter("Name", QStringLiteral("thing5"));
	orExpression.fieldFilters << TagFilter(Filter::FO_ARRAY_HAS_ALL, {"a", "b"});
	QCOMPARE(SelectEntities(orExpression), (QByteArrayList{"e1", "e5"}));

	// Nested group: no:label AND (Name = thing3 OR Name = thing1)
	Filter::FilterExpression nameGroup;
	nameGroup.logicalOperation = Filter::LO_OR;
	nameGroup.fieldFilters << Filter::FieldFilter("Name", QStringLiteral("thing3"));
	nameGroup.fieldFilters << Filter::FieldFilter("Name", QStringLiteral("thing1"));

	Filter::FilterExpression nestedExpression;
	nestedExpression.fieldFilters << TagFilter(Filter::FO_ARRAY_IS_EMPTY, QStringList());
	nestedExpression.filterExpressions << nameGroup;
	QCOMPARE(SelectEntities(nestedExpression), QByteArrayList{"e3"});
}


void CTagFilterSqlBuilderTest::testHostileTagIds()
{
	// Quotes, a forged token and an argument placeholder must stay inert literals.
	Filter::FilterExpression expression;
	expression.fieldFilters << TagFilter(Filter::FO_ARRAY_HAS_ANY, {"x' OR '1'='1", "__TAG_FILTER_0__", "%1"});
	QCOMPARE(SelectEntities(expression), QByteArrayList());

	Filter::FilterExpression textExpression;
	textExpression.logicalOperation = Filter::LO_OR;
	textExpression.fieldFilters << Filter::FieldFilter("Name", QStringLiteral("__TAG_FILTER_0__"));
	textExpression.fieldFilters << TagFilter(Filter::FO_ARRAY_HAS_ANY, {"c"});
	QCOMPARE(SelectEntities(textExpression), QByteArrayList{"e5"});
}


void CTagFilterSqlBuilderTest::cleanupTestCase()
{
	m_database.close();
	m_database = QSqlDatabase();
	QSqlDatabase::removeDatabase(QStringLiteral("CTagFilterSqlBuilderTest"));
}


// private methods

QByteArrayList CTagFilterSqlBuilderTest::SelectEntities(const QString& whereClause) const
{
	QByteArrayList retVal;

	const QString queryText = QStringLiteral(R"(SELECT root."DocumentId" FROM "Things" root WHERE %1 ORDER BY root."DocumentId")").arg(whereClause);

	QSqlQuery query(m_database);
	if (!query.exec(queryText)){
		qWarning() << query.lastError().text() << queryText;

		return QByteArrayList() << QByteArrayLiteral("<SQL error>");
	}

	while (query.next()){
		retVal << query.value(0).toByteArray();
	}

	return retVal;
}


QByteArrayList CTagFilterSqlBuilderTest::SelectEntities(const Filter::FilterExpression& expression) const
{
	imtbase::CComplexCollectionFilter filter;
	filter.SetFilterExpression(expression);

	const QString whereClause = imtdb::CTagFilterSqlBuilder::CreateFilterQuery(filter, m_config);
	if (whereClause.isEmpty()){
		return QByteArrayList() << QByteArrayLiteral("<empty filter>");
	}

	return SelectEntities(whereClause);
}


bool CTagFilterSqlBuilderTest::ExecuteScript(const QString& resourcePath, const QString& tableName)
{
	QFile scriptFile(resourcePath);
	if (!scriptFile.open(QFile::ReadOnly)){
		qWarning() << "Script not found:" << resourcePath;

		return false;
	}

	QString script = QString::fromUtf8(scriptFile.readAll());
	script.replace(QStringLiteral("${TableName}"), tableName);

	for (const QString& statement : script.split(';')){
		if (!statement.trimmed().isEmpty() && !Execute(statement)){
			return false;
		}
	}

	return true;
}


bool CTagFilterSqlBuilderTest::Execute(const QString& queryText)
{
	QSqlQuery query(m_database);
	if (!query.exec(queryText)){
		qWarning() << query.lastError().text() << queryText;

		return false;
	}

	return true;
}


I_ADD_TEST(CTagFilterSqlBuilderTest);


