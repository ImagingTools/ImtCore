#include <imtbase/ComplexCollectionFilterHelper.h>



namespace imtbase
{


IComplexCollectionFilter::SortingOrder GetSortingOrder(const IComplexCollectionFilter& filter, const QByteArrayList& fieldIds)
{
	for (const IComplexCollectionFilter::FieldSortingInfo& info : filter.GetSortingInfo()){
		if (fieldIds.contains(info.fieldId) || fieldIds.isEmpty()){
			return info.sortingOrder;
		}
	}

	return IComplexCollectionFilter::SO_NO_ORDER;
}


void SetSortingOrder(IComplexCollectionFilter& filter, const QByteArrayList& fieldIds, IComplexCollectionFilter::SortingOrder order)
{
	IComplexCollectionFilter::FieldSortingInfoList infoList;

	for (const QByteArray& id : fieldIds){
		IComplexCollectionFilter::FieldSortingInfo info;

		info.fieldId = id;
		info.sortingOrder = order;

		infoList.append(info);
	}

	filter.SetSortingInfo(infoList);
}


QSet<QByteArray> GetSortingFieldIds(const IComplexCollectionFilter& filter)
{
	QSet<QByteArray> retVal;

	for (const IComplexCollectionFilter::FieldSortingInfo& info : filter.GetSortingInfo()){
		retVal += info.fieldId;
	}

	return retVal;
}


QSet<QByteArray> GetFilteringFieldIds(const IComplexCollectionFilter::GroupFilter& filter)
{
	QSet<QByteArray> retVal;

	for (const IComplexCollectionFilter::FieldFilter& fieldFilter : filter.fieldFilters){
		retVal += fieldFilter.fieldId;
	}

	for (const IComplexCollectionFilter::GroupFilter& groupFilter : filter.groupFilters){
		retVal += GetFilteringFieldIds(groupFilter);
	}

	return retVal;
}


QString GetTextFilter(const imtbase::IComplexCollectionFilter::GroupFilter& filter)
{
	for (const IComplexCollectionFilter::FieldFilter& fieldFilter : filter.fieldFilters){
		return fieldFilter.filterValue.toString();
	}

	for (const IComplexCollectionFilter::GroupFilter& groupFilter : filter.groupFilters){
		QString retVal = GetTextFilter(groupFilter);

		if (!retVal.isEmpty()){
			return retVal;
		}
	}

	return QString();
}


void SetTextFilter(imtbase::IComplexCollectionFilter& filter, const QByteArrayList& fieldIds, const QString& text)
{
	imtbase::IComplexCollectionFilter::GroupFilter groupFilter;

	QVector<imtbase::IComplexCollectionFilter::FieldFilter> fieldFilters;
	for (const QByteArray& fieldId : fieldIds){
		imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;

		fieldFilter.fieldId = fieldId;
		fieldFilter.filterValue = text;
		fieldFilter.filterOperation = IComplexCollectionFilter::FO_CONTAINS;

		fieldFilters.append(fieldFilter);
	}
	groupFilter.fieldFilters = fieldFilters;

	filter.SetFieldsFilter(groupFilter);
}


QSet<int> numericTypes({ QMetaType::LongLong, QMetaType::ULongLong, QMetaType::Double });
QMap<IComplexCollectionFilter::FieldOperation, QString> numericOperations({
	{IComplexCollectionFilter::FO_EQUAL, "="},
	{IComplexCollectionFilter::FO_NOT_EQUAL, "!="},
	{IComplexCollectionFilter::FO_LESS, "<"},
	{IComplexCollectionFilter::FO_GREATER, ">"},
	{IComplexCollectionFilter::FO_NOT_LESS, ">="},
	{IComplexCollectionFilter::FO_NOT_GREATER, "<="}});

QSet<int> stringTypes({ QMetaType::QString, QMetaType::QByteArray });
QMap<IComplexCollectionFilter::FieldOperation, QString> stringOperations({
	{IComplexCollectionFilter::FO_EQUAL, "="},
	{IComplexCollectionFilter::FO_NOT_EQUAL, "!="},
	{IComplexCollectionFilter::FO_LESS, "<"},
	{IComplexCollectionFilter::FO_GREATER, ">"},
	{IComplexCollectionFilter::FO_NOT_LESS, ">="},
	{IComplexCollectionFilter::FO_NOT_GREATER, "<="},
	{IComplexCollectionFilter::FO_CONTAINS, "ILIKE"}});

QSet<int> boolTypes({ QMetaType::Bool });
QSet<IComplexCollectionFilter::FieldOperation> boolOperations({
	IComplexCollectionFilter::FO_EQUAL,
	IComplexCollectionFilter::FO_NOT_EQUAL});


QString processColumn(const IComplexCollectionFilter::FieldFilter& filter)
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
		if (filter.filterOperation == IComplexCollectionFilter::FO_CONTAINS){
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


QString processGroup(const IComplexCollectionFilter::GroupFilter& filter)
{
	QString retVal;
	QString logicOperation;

	switch (filter.logicalOperation){
	case IComplexCollectionFilter::LO_AND:
		logicOperation = "AND";
		break;
	case IComplexCollectionFilter::LO_OR:
		logicOperation = "OR";
		break;
	}

	if (logicOperation.isEmpty()){
		return retVal;
	}

	for (const IComplexCollectionFilter::FieldFilter& fieldFilter : filter.fieldFilters){
		QString retValPart = processColumn(fieldFilter);
		if (retValPart.isEmpty()){
			return QString();
		}

		retVal += retVal.isEmpty() ? retValPart : QString(" %1 %2").arg(logicOperation).arg(retValPart);
	}

	for (const IComplexCollectionFilter::GroupFilter& groupFilter : filter.groupFilters){
		QString retValPart = processGroup(groupFilter);
		if (retValPart.isEmpty()){
			return QString();
		}

		retVal += retVal.isEmpty() ? retValPart : QString(" %1 %2").arg(logicOperation).arg(retValPart);
	}

	retVal.prepend("(");
	retVal.append(")");

	return retVal;
};



QString CreateDefaultSqlSortingQuery(const imtbase::IComplexCollectionFilter& filter)
{
	QVector<IComplexCollectionFilter::FieldSortingInfo> infoList = filter.GetSortingInfo();
	QString retVal = !infoList.isEmpty() ? "ORDER BY" : "";

	for (int i = 0; i < infoList.count(); i++){
		const IComplexCollectionFilter::FieldSortingInfo& info = infoList[i];

		if (info.fieldId.isEmpty()){
			return QString();
		}

		QString order;

		switch (info.sortingOrder){
		case IComplexCollectionFilter::SO_ASC:
			order = "ASC";
			break;
		case IComplexCollectionFilter::SO_DESC:
			order = "DESC";
			break;
		}

		if (order.isEmpty()){
			return QString();
		}

		retVal += QString("%0 \"%1\" %2").arg(i > 0 ? "," : "").arg(qPrintable(info.fieldId)).arg(order);
	}

	return retVal;
}


QString CreateDefaultSqlFilterQuery(const imtbase::IComplexCollectionFilter& filter)
{
	QString retVal = processGroup(filter.GetFieldsFilter());

	retVal = retVal.mid(1, retVal.count() - 2);

	return retVal;
}


} // namespace imtbase


