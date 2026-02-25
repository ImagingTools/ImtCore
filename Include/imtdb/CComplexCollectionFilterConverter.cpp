// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CComplexCollectionFilterConverter.h>


namespace imtdb
{


QSet<int> numericTypes({ QMetaType::LongLong, QMetaType::ULongLong, QMetaType::Double, QMetaType::Int });
QMap<imtbase::IComplexCollectionFilter::FieldOperation, QString> numericOperations({
	{imtbase::IComplexCollectionFilter::FO_EQUAL, "="},
	{imtbase::IComplexCollectionFilter::FO_NOT_EQUAL, "!="},
	{imtbase::IComplexCollectionFilter::FO_LESS, "<"},
	{imtbase::IComplexCollectionFilter::FO_GREATER, ">"},
	{imtbase::IComplexCollectionFilter::FO_NOT_LESS, ">="},
	{imtbase::IComplexCollectionFilter::FO_NOT_GREATER, "<="}});

QSet<int> stringTypes({ QMetaType::QString, QMetaType::QByteArray });
QMap<imtbase::IComplexCollectionFilter::FieldOperation, QString> stringOperations({
	{imtbase::IComplexCollectionFilter::FO_EQUAL, "="},
	{imtbase::IComplexCollectionFilter::FO_NOT_EQUAL, "!="},
	{imtbase::IComplexCollectionFilter::FO_LESS, "<"},
	{imtbase::IComplexCollectionFilter::FO_GREATER, ">"},
	{imtbase::IComplexCollectionFilter::FO_NOT_LESS, ">="},
	{imtbase::IComplexCollectionFilter::FO_NOT_GREATER, "<="},
	{imtbase::IComplexCollectionFilter::FO_CONTAINS, "LIKE"}});

QSet<int> boolTypes({ QMetaType::Bool });
QMap<imtbase::IComplexCollectionFilter::FieldOperation, QString> boolOperations({
	{imtbase::IComplexCollectionFilter::FO_EQUAL, "="},
	{imtbase::IComplexCollectionFilter::FO_NOT_EQUAL, "!="}});

QSet<imtbase::IComplexCollectionFilter::FieldOperation> arrayOperations({
	imtbase::IComplexCollectionFilter::FO_ARRAY_HAS_ANY,
	imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_HAS_ANY,
	imtbase::IComplexCollectionFilter::FO_ARRAY_HAS_ALL,
	imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_HAS_ALL,
	imtbase::IComplexCollectionFilter::FO_ARRAY_IS_EMPTY,
	imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_IS_EMPTY,
	imtbase::IComplexCollectionFilter::FO_ARRAY_ILIKE_ANY,
	imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_ILIKE_ANY
});


QString CComplexCollectionFilterConverter::CreateSqlSortQuery(const imtbase::IComplexCollectionFilter& filter)
{
	QStringList orderParts;

	const imtbase::IComplexCollectionFilter::Fields& fields = filter.GetFields();

	for (const imtbase::IComplexCollectionFilter::FieldInfo& info : fields){
		if (!(info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_SORT)){
			continue;
		}

		if (info.metaInfo.sortingOrder == imtbase::IComplexCollectionFilter::SO_NO_ORDER){
			continue;
		}

		if (info.id.isEmpty()){
			qWarning() << "Empty field id in sort filter";
			continue;
		}

		QString order;
		switch (info.metaInfo.sortingOrder){
			case imtbase::IComplexCollectionFilter::SO_ASC:
				order = QStringLiteral("ASC");
				break;

			case imtbase::IComplexCollectionFilter::SO_DESC:
				order = QStringLiteral("DESC");
				break;

			default:
				qWarning() << "Unexpected sortingOrder" << imtbase::IComplexCollectionFilter::ToString(info.metaInfo.sortingOrder);
				continue;
		}

		orderParts << QStringLiteral("\"%1\" %2").arg(QString::fromUtf8(info.id), order);
	}

	if (orderParts.isEmpty()){
		return QString();
	}

	return QStringLiteral("ORDER BY %1").arg(orderParts.join(", "));
}



QString CComplexCollectionFilterConverter::CreateSqlFilterQuery(const imtbase::IComplexCollectionFilter& filter, SqlContext sqlContext)
{
	QString retVal = ProcessGroup(filter.GetFilterExpression(), sqlContext);

	retVal = retVal.mid(1, retVal.length() - 2);

	return retVal;
}


QString CComplexCollectionFilterConverter::ProcessColumn(const imtbase::IComplexCollectionFilter::FieldFilter& filter, SqlContext sqlContext)
{
	QString retVal;
	bool isOk = false;

	if (numericTypes.contains(filter.filterValue.typeId()) && numericOperations.contains(filter.filterOperation)){
		QString filterValue;
		QString type;

		switch (filter.filterValue.typeId()){
		case QMetaType::Int:
			filterValue = QString::number(filter.filterValue.toInt(&isOk));
			type = "int";
			break;
		case QMetaType::LongLong:
			filterValue = QString::number(filter.filterValue.toLongLong(&isOk));
			type = "int";
			break;
		case QMetaType::ULongLong:
			filterValue = QString::number(filter.filterValue.toULongLong(&isOk));
			type = "int";
			break;
		case QMetaType::Double:
			filterValue = QString::number(filter.filterValue.toDouble(&isOk));
			type = "decimal";
			break;
		default:
			break;
		}

		if (isOk){
			QString columnExpr = sqlContext == SC_POSTGRES ? QString("(\"%1\")::%2").arg(qPrintable(filter.fieldId), type)
											: QString("\"%1\"").arg(qPrintable(filter.fieldId));
			retVal = QString("%1 %2 %3").arg(columnExpr, numericOperations[filter.filterOperation], filterValue);
		}
	}
	else if (stringTypes.contains(filter.filterValue.typeId()) && stringOperations.contains(filter.filterOperation)){
		QString filterValue = filter.filterValue.toString();
		if (filter.filterOperation == imtbase::IComplexCollectionFilter::FO_CONTAINS){
			filterValue.prepend("%");
			filterValue.append("%");

			switch (sqlContext){
			case SC_GENERAL:
				stringOperations[filter.filterOperation] = "LIKE";
				break;
			case SC_POSTGRES:
				stringOperations[filter.filterOperation] = "ILIKE";
				break;
			}
		}

		retVal = sqlContext == SC_POSTGRES ? QString("(\"%1\")::text %2 '%3'").arg(qPrintable(filter.fieldId), stringOperations[filter.filterOperation], filterValue)
							: QString("\"%1\" %2 '%3'").arg(qPrintable(filter.fieldId), stringOperations[filter.filterOperation], filterValue);
	}
	else if (boolTypes.contains(filter.filterValue.typeId()) && boolOperations.contains(filter.filterOperation)){
		bool value = filter.filterValue.toBool();
		QString columnExpr = sqlContext == SC_POSTGRES ? QString("coalesce((\"%1\")::bool, false)").arg(qPrintable(filter.fieldId))
										: QString("\"%1\"").arg(qPrintable(filter.fieldId));
		retVal = QString("%1 %2 %3").arg(columnExpr, boolOperations[filter.filterOperation], value ? "true" : "false");
	}
	else if (sqlContext == SC_POSTGRES && arrayOperations.contains(filter.filterOperation)){
		const QString columnExpr = QString("\"%1\"").arg(qPrintable(filter.fieldId));

		switch (filter.filterOperation){
		case imtbase::IComplexCollectionFilter::FO_ARRAY_IS_EMPTY:
			return QString("coalesce(jsonb_array_length(%1), 0) = 0").arg(columnExpr);

		case imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_IS_EMPTY:
			return QString("jsonb_array_length(%1) > 0").arg(columnExpr);

		case imtbase::IComplexCollectionFilter::FO_ARRAY_HAS_ANY:
		case imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_HAS_ANY:
		case imtbase::IComplexCollectionFilter::FO_ARRAY_HAS_ALL:
		case imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_HAS_ALL: {
			const QVariantList values = filter.filterValue.toList();
			if (values.isEmpty()){
				return {};
			}

			const QString sqlArray = ToSqlArray(values);

			QString expr;

			if (filter.filterOperation == imtbase::IComplexCollectionFilter::FO_ARRAY_HAS_ANY ||
				filter.filterOperation == imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_HAS_ANY){
				expr = QString("%1 ?| %2").arg(columnExpr, sqlArray);
			}
			else{
				expr = QString("%1 ?& %2").arg(columnExpr, sqlArray);
			}

			if (filter.filterOperation == imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_HAS_ANY ||
				filter.filterOperation == imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_HAS_ALL){
				expr.prepend("NOT (").append(")");
			}

			return expr;
		}
		case imtbase::IComplexCollectionFilter::FO_ARRAY_ILIKE_ANY:
			case imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_ILIKE_ANY: {
				const QVariantList values = filter.filterValue.toList();
				if (values.isEmpty()){
					return {};
				}

				QStringList likeConditions;
				for (const QVariant& v : values){
					QString pattern = v.toString().replace("'", "''");
					likeConditions << QString("elem ILIKE '%%%1%%'").arg(pattern);
				}

				QString expr = QString(
					"EXISTS ("
					"SELECT 1 "
					"FROM jsonb_array_elements_text(%1) AS elem "
					"WHERE %2"
					")"
				).arg(columnExpr, likeConditions.join(" OR "));

				if (filter.filterOperation == imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_ILIKE_ANY){
					expr.prepend("NOT (").append(")");
				}

				return expr;
			}

		default:
			break;
		}
	}

	return retVal;
};


QString CComplexCollectionFilterConverter::ProcessGroup(const imtbase::IComplexCollectionFilter::FilterExpression& filter, SqlContext sqlContext)
{
	QString retVal;
	QString logicOperation;

	switch (filter.logicalOperation){
	case imtbase::IComplexCollectionFilter::LO_AND:
		logicOperation = "AND";
		break;
	case imtbase::IComplexCollectionFilter::LO_OR:
		logicOperation = "OR";
		break;
	}

	if (logicOperation.isEmpty()){
		return retVal;
	}

	for (const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : filter.fieldFilters){
		QString retValPart = ProcessColumn(fieldFilter, sqlContext);
		if (retValPart.isEmpty()){
			continue;
		}

		retVal += retVal.isEmpty() ? retValPart : QString(" %1 %2").arg(logicOperation, retValPart);
	}

	for (const imtbase::IComplexCollectionFilter::FilterExpression& groupFilter : filter.filterExpressions){
		QString retValPart = ProcessGroup(groupFilter, sqlContext);
		if (retValPart.isEmpty()){
			continue;
		}

		retVal += retVal.isEmpty() ? retValPart : QString(" %1 %2").arg(logicOperation, retValPart);
	}

	if (!retVal.isEmpty()){
		retVal.prepend("(");
		retVal.append(")");
	}

	return retVal;
};


QString CComplexCollectionFilterConverter::ToSqlArray(const QVariantList& values)
{
	QStringList parts;
	for (const QVariant& v : values){
		if (v.typeId() == QMetaType::QString || v.typeId() == QMetaType::QByteArray){
			parts << QString("'%1'").arg(v.toString().replace("'", "''"));
		}
		else{
			parts << v.toString();
		}
	}
	return QString("ARRAY[%1]").arg(parts.join(", "));
};


} // namespace imtdb


