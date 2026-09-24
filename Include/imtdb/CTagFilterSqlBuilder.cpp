// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTagFilterSqlBuilder.h>


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>

// ImtCore includes
#include <imtdb/CComplexCollectionFilterConverter.h>
#include <imtdb/imtdb.h>


namespace imtdb
{


const QByteArray CTagFilterSqlBuilder::s_tagsFieldId = QByteArrayLiteral("Tags");


// public static methods

QString CTagFilterSqlBuilder::CreateCondition(const imtbase::IComplexCollectionFilter::FieldFilter& filter, const Config& config)
{
	const QString assignmentsSource = QStringLiteral(R"(SELECT %1 FROM %2 ta WHERE ta."EntityType" = '%3' AND ta."EntityId" = %4)");
	const QString entityType = SqlEncode(config.entityType);

	QStringList tagIds;
	for (const QVariant& value : filter.filterValue.toList()){
		const QString tagId = value.toString();
		if (!tagId.isEmpty()){
			tagIds << QStringLiteral("'%1'").arg(SqlEncode(tagId));
		}
	}

	tagIds.removeDuplicates();

	const QString tagIdCondition = QStringLiteral(R"( AND ta."TagId" IN (%1))").arg(tagIds.join(QStringLiteral(", ")));

	switch (filter.filterOperation){
	case imtbase::IComplexCollectionFilter::FO_ARRAY_HAS_ANY:
	case imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_HAS_ANY:{
		if (tagIds.isEmpty()){
			return QString();
		}

		const QString exists = QStringLiteral("EXISTS (%1)").arg(
					assignmentsSource.arg(QStringLiteral("1"), config.assignmentsTable, entityType, config.entityIdExpression) + tagIdCondition);

		return (filter.filterOperation == imtbase::IComplexCollectionFilter::FO_ARRAY_HAS_ANY) ? exists : QStringLiteral("NOT ") + exists;
	}

	case imtbase::IComplexCollectionFilter::FO_ARRAY_HAS_ALL:
	case imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_HAS_ALL:{
		if (tagIds.isEmpty()){
			return QString();
		}

		const QString count = QStringLiteral("(%1) = %2").arg(
					assignmentsSource.arg(QStringLiteral(R"(COUNT(DISTINCT ta."TagId"))"), config.assignmentsTable, entityType, config.entityIdExpression) + tagIdCondition,
					QString::number(tagIds.count()));

		return (filter.filterOperation == imtbase::IComplexCollectionFilter::FO_ARRAY_HAS_ALL) ? count : QStringLiteral("NOT (%1)").arg(count);
	}

	case imtbase::IComplexCollectionFilter::FO_ARRAY_IS_EMPTY:
	case imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_IS_EMPTY:{
		// Assignments of deleted tags do not count.
		const QString tagIdColumn = config.isSqlite ? QStringLiteral(R"(t."DocumentId")") : QStringLiteral(R"(t."DocumentId"::text)");
		const QString activeTagCondition = QStringLiteral(R"( AND EXISTS (SELECT 1 FROM %1 t WHERE %2 = ta."TagId" AND t."State" = 'Active'))")
					.arg(config.tagsTable, tagIdColumn);

		const QString exists = QStringLiteral("EXISTS (%1)").arg(
					assignmentsSource.arg(QStringLiteral("1"), config.assignmentsTable, entityType, config.entityIdExpression) + activeTagCondition);

		return (filter.filterOperation == imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_IS_EMPTY) ? exists : QStringLiteral("NOT ") + exists;
	}

	default:
		return QString();
	}
}


QString CTagFilterSqlBuilder::CreateFilterQuery(
			const imtbase::IComplexCollectionFilter& collectionFilter,
			const Config& config,
			const std::function<void(QString&)>& postProcessor)
{
	const CComplexCollectionFilterConverter::SqlContext sqlContext = config.isSqlite
				? CComplexCollectionFilterConverter::SC_GENERAL
				: CComplexCollectionFilterConverter::SC_POSTGRES;

	// Tag conditions enter the query as plain tokens, so the post-processing never rewrites their identifiers.
	// The nonce keeps filter values supplied by the client from forging a token.
	const QString tokenPrefix = QStringLiteral("__TAG_FILTER_%1_").arg(QUuid::createUuid().toString(QUuid::Id128));
	QStringList tagConditions;
	const CComplexCollectionFilterConverter::FieldRenderer tagRenderer =
				[&tagConditions, &config, &tokenPrefix](const imtbase::IComplexCollectionFilter::FieldFilter& filter, QString& sql)
	{
		if (filter.fieldId != s_tagsFieldId){
			return false;
		}

		const QString condition = CreateCondition(filter, config);
		if (!condition.isEmpty()){
			sql = tokenPrefix + QString::number(tagConditions.count()) + QStringLiteral("__");
			tagConditions << condition;
		}

		return true;
	};

	QString filterQuery = CComplexCollectionFilterConverter::CreateSqlFilterQuery(collectionFilter, sqlContext, tagRenderer);
	if (!filterQuery.isEmpty() && postProcessor){
		postProcessor(filterQuery);
	}

	if (tagConditions.isEmpty()){
		return filterQuery;
	}

	// Single pass, so text inside an inserted condition is never treated as a token.
	const QRegularExpression tokenExpression(QRegularExpression::escape(tokenPrefix) + QStringLiteral("(\\d+)__"));

	QString retVal;
	int position = 0;
	QRegularExpressionMatchIterator matchIterator = tokenExpression.globalMatch(filterQuery);
	while (matchIterator.hasNext()){
		const QRegularExpressionMatch match = matchIterator.next();

		retVal += filterQuery.mid(position, match.capturedStart() - position);
		retVal += QStringLiteral("(") + tagConditions.value(match.captured(1).toInt()) + QStringLiteral(")");
		position = match.capturedEnd();
	}

	retVal += filterQuery.mid(position);

	return retVal;
}


} // namespace imtdb


