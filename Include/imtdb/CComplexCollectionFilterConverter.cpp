#include <imtdb/CComplexCollectionFilterConverter.h>



namespace imtdb
{


QSet<int> numericTypes({ QMetaType::LongLong, QMetaType::ULongLong, QMetaType::Double });
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
	{imtbase::IComplexCollectionFilter::FO_CONTAINS, "ILIKE"}});

QSet<int> boolTypes({ QMetaType::Bool });
QSet<imtbase::IComplexCollectionFilter::FieldOperation> boolOperations({
	imtbase::IComplexCollectionFilter::FO_EQUAL,
	imtbase::IComplexCollectionFilter::FO_NOT_EQUAL});


QString CComplexCollectionFilterConverter::CreateSqlSortQuery(const imtbase::IComplexCollectionFilter& filter)
{
	QVector<imtbase::IComplexCollectionFilter::FieldSortingInfo> infoList = filter.GetSortingInfo();
	QString retVal = !infoList.isEmpty() ? "ORDER BY" : "";

	for (int i = 0; i < infoList.count(); i++){
		const imtbase::IComplexCollectionFilter::FieldSortingInfo& info = infoList[i];

		if (info.fieldId.isEmpty()){
			return QString();
		}

		QString order;

		switch (info.sortingOrder){
		case imtbase::IComplexCollectionFilter::SO_ASC:
			order = "ASC";
			break;
		case imtbase::IComplexCollectionFilter::SO_DESC:
			order = "DESC";
			break;
		default:
			qWarning() << "Unexpected sortingOrder" << imtbase::IComplexCollectionFilter::ToString(info.sortingOrder);
			break;
		}

		if (order.isEmpty()){
			return QString();
		}

		retVal += QString("%0 \"%1\" %2").arg(i > 0 ? "," : "").arg(qPrintable(info.fieldId)).arg(order);
	}

	return retVal;
}


QString CComplexCollectionFilterConverter::CreateSqlFilterQuery(const imtbase::IComplexCollectionFilter& filter)
{
	QString retVal = ProcessGroup(filter.GetFieldsFilter());

	retVal = retVal.mid(1, retVal.length() - 2);

	return retVal;
}


QString CComplexCollectionFilterConverter::ProcessColumn(const imtbase::IComplexCollectionFilter::FieldFilter& filter)
{
	QString retVal;

	bool isOk = false;

	if (numericTypes.contains(filter.filterValue.type()) && numericOperations.contains(filter.filterOperation)){
		QString filterValue;

		switch (filter.filterValue.type()){
		case QMetaType::LongLong:
			filterValue = QString::number(filter.filterValue.toLongLong(&isOk));
			break;
		case QMetaType::ULongLong:
			filterValue = QString::number(filter.filterValue.toULongLong(&isOk));
			break;
		case QMetaType::Double:
			filterValue = QString::number(filter.filterValue.toDouble(&isOk));
			break;
		default:
			break;
		}

		if (isOk){
			retVal = QString("\"%1\" %2 %3").arg(qPrintable(filter.fieldId)).arg(numericOperations[filter.filterOperation]).arg(filterValue);
		}
	}
	else if (stringTypes.contains(filter.filterValue.type()) && stringOperations.contains(filter.filterOperation)){
		QString filterValue = filter.filterValue.toString();
		if (filter.filterOperation == imtbase::IComplexCollectionFilter::FO_CONTAINS){
			filterValue.prepend("%");
			filterValue.append("%");
		}
		retVal = QString("\"%1\" %2 '%3'").arg(qPrintable(filter.fieldId)).arg(stringOperations[filter.filterOperation]).arg(filterValue);
	}
	else if (boolTypes.contains(filter.filterValue.type()) && boolOperations.contains(filter.filterOperation)){
		bool value = filter.filterValue.toBool();

		retVal = QString("\"%1\" %2 %3").arg(qPrintable(filter.fieldId)).arg(filter.filterOperation).arg(value ? "true" : "false");
	}

	return retVal;
};


QString CComplexCollectionFilterConverter::ProcessGroup(const imtbase::IComplexCollectionFilter::GroupFilter& filter)
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
		QString retValPart = ProcessColumn(fieldFilter);
		if (retValPart.isEmpty()){
			continue;
		}

		retVal += retVal.isEmpty() ? retValPart : QString(" %1 %2").arg(logicOperation).arg(retValPart);
	}

	for (const imtbase::IComplexCollectionFilter::GroupFilter& groupFilter : filter.groupFilters){
		QString retValPart = ProcessGroup(groupFilter);
		if (retValPart.isEmpty()){
			continue;
		}

		retVal += retVal.isEmpty() ? retValPart : QString(" %1 %2").arg(logicOperation).arg(retValPart);
	}

	if (!retVal.isEmpty()){
		retVal.prepend("(");
		retVal.append(")");
	}

	return retVal;
};


} // namespace imtdb


