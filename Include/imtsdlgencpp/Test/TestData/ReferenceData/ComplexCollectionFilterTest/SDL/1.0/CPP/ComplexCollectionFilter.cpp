#include "ComplexCollectionFilter.h"


namespace sdl::V1_0::imtbase
{


QByteArray CTimeFilter::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CTimeFilter::operator==(const CTimeFilter& other) const
{
	return 
				timeRange == other.timeRange &&
				timeUnit == other.timeUnit &&
				interpretationMode == other.interpretationMode &&
				unitMultiplier == other.unitMultiplier;
}


bool V1_0::imtbase::CTimeFilter::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "TimeFilter", modelIndex);


	if (timeRange){
		::imtbase::CTreeItemModel* timeRangeNewModelPtr = model.AddTreeModel("timeRange", modelIndex);
		const bool isTimeRangeAdded = timeRange->WriteToModel(*timeRangeNewModelPtr, 0);
		if (!isTimeRangeAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

			return false;
		}

	}
	if (timeUnit){
		model.SetData("timeUnit", *timeUnit, modelIndex);
	}

	if (interpretationMode){
		model.SetData("interpretationMode", *interpretationMode, modelIndex);
	}

	if (unitMultiplier){
		model.SetData("unitMultiplier", *unitMultiplier, modelIndex);
	}


	return true;
}


bool V1_0::imtbase::CTimeFilter::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* timeRangeDataModelPtr = model.GetTreeItemModel("timeRange", modelIndex);
	if (timeRangeDataModelPtr != nullptr){
		timeRange = V1_0::imtbase::CTimeRange();
		const bool isTimeRangeRead = timeRange->ReadFromModel(*timeRangeDataModelPtr, modelIndex);
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

			return false;
		}
	}

	QVariant timeUnitData = model.GetData("timeUnit", modelIndex);
	if (!timeUnitData.isNull()){
		timeUnit = timeUnitData.toString();
	}

	QVariant interpretationModeData = model.GetData("interpretationMode", modelIndex);
	if (!interpretationModeData.isNull()){
		interpretationMode = interpretationModeData.toString();
	}

	QVariant unitMultiplierData = model.GetData("unitMultiplier", modelIndex);
	if (!unitMultiplierData.isNull()){
		unitMultiplier = unitMultiplierData.toInt();
	}

	return true;
}


bool V1_0::imtbase::CTimeFilter::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* timeRangeDataModelPtr = model.GetTreeItemModel("timeRange", modelIndex);
	if (timeRangeDataModelPtr != nullptr){
		timeRange = V1_0::imtbase::CTimeRange();
		const bool isTimeRangeRead = timeRange->ReadFromModel(*timeRangeDataModelPtr, modelIndex);
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

			return false;
		}
	}

	QVariant timeUnitData = model.GetData("timeUnit", modelIndex);
	if (!timeUnitData.isNull()){
		timeUnit = timeUnitData.toString();
	}

	QVariant interpretationModeData = model.GetData("interpretationMode", modelIndex);
	if (!interpretationModeData.isNull()){
		interpretationMode = interpretationModeData.toString();
	}

	QVariant unitMultiplierData = model.GetData("unitMultiplier", modelIndex);
	if (!unitMultiplierData.isNull()){
		unitMultiplier = unitMultiplierData.toInt();
	}

	return true;
}


bool V1_0::imtbase::CTimeFilter::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (timeRange){
		::imtgql::CGqlParamObject timeRangeGqlObject;
		const bool isTimeRangeAdded = timeRange->WriteToGraphQlObject(timeRangeGqlObject);
		if (!isTimeRangeAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("timeRange", timeRangeGqlObject);
	}

	if (timeUnit){
		gqlObject.InsertParam("timeUnit", QVariant(*timeUnit));
	}

	if (interpretationMode){
		gqlObject.InsertParam("interpretationMode", QVariant(*interpretationMode));
	}

	if (unitMultiplier){
		gqlObject.InsertParam("unitMultiplier", QVariant(*unitMultiplier));
	}

	gqlObject.InsertParam("__typename", QVariant("TimeFilter"));

	return true;
}


bool V1_0::imtbase::CTimeFilter::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("timeRange") && (gqlObject.GetParamArgumentObjectPtr("timeRange") != nullptr)){
		timeRange = V1_0::imtbase::CTimeRange();
		const bool isTimeRangeRead = timeRange->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("timeRange"));
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("timeUnit") && (gqlObject["timeUnit"].userType() == QMetaType::QString || gqlObject["timeUnit"].userType() == QMetaType::QByteArray)){
		timeUnit = gqlObject["timeUnit"].toString();
	}

	if (gqlObject.ContainsParam("interpretationMode") && (gqlObject["interpretationMode"].userType() == QMetaType::QString || gqlObject["interpretationMode"].userType() == QMetaType::QByteArray)){
		interpretationMode = gqlObject["interpretationMode"].toString();
	}

	if (gqlObject.ContainsParam("unitMultiplier") && (gqlObject["unitMultiplier"].userType() == QMetaType::Int || gqlObject["unitMultiplier"].userType() == QMetaType::UInt || gqlObject["unitMultiplier"].userType() == QMetaType::LongLong || gqlObject["unitMultiplier"].userType() == QMetaType::ULongLong || gqlObject["unitMultiplier"].userType() == QMetaType::Long || gqlObject["unitMultiplier"].userType() == QMetaType::Short || gqlObject["unitMultiplier"].userType() == QMetaType::ULong || gqlObject["unitMultiplier"].userType() == QMetaType::UShort || gqlObject["unitMultiplier"].userType() == QMetaType::UChar)){
		unitMultiplier = gqlObject["unitMultiplier"].toInt();
	}

	return true;
}


bool V1_0::imtbase::CTimeFilter::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("timeRange") && (gqlObject.GetParamArgumentObjectPtr("timeRange") != nullptr)){
		timeRange = V1_0::imtbase::CTimeRange();
		const bool isTimeRangeRead = timeRange->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("timeRange"));
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("timeUnit") && (gqlObject["timeUnit"].userType() == QMetaType::QString || gqlObject["timeUnit"].userType() == QMetaType::QByteArray)){
		timeUnit = gqlObject["timeUnit"].toString();
	}

	if (gqlObject.ContainsParam("interpretationMode") && (gqlObject["interpretationMode"].userType() == QMetaType::QString || gqlObject["interpretationMode"].userType() == QMetaType::QByteArray)){
		interpretationMode = gqlObject["interpretationMode"].toString();
	}

	if (gqlObject.ContainsParam("unitMultiplier") && (gqlObject["unitMultiplier"].userType() == QMetaType::Int || gqlObject["unitMultiplier"].userType() == QMetaType::UInt || gqlObject["unitMultiplier"].userType() == QMetaType::LongLong || gqlObject["unitMultiplier"].userType() == QMetaType::ULongLong || gqlObject["unitMultiplier"].userType() == QMetaType::Long || gqlObject["unitMultiplier"].userType() == QMetaType::Short || gqlObject["unitMultiplier"].userType() == QMetaType::ULong || gqlObject["unitMultiplier"].userType() == QMetaType::UShort || gqlObject["unitMultiplier"].userType() == QMetaType::UChar)){
		unitMultiplier = gqlObject["unitMultiplier"].toInt();
	}

	return true;
}


bool V1_0::imtbase::CTimeFilter::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (timeRange){
		QJsonObject timeRangeJsonObject;
		const bool isTimeRangeAdded = timeRange->WriteToJsonObject(timeRangeJsonObject);
		if (!isTimeRangeAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["timeRange"] = timeRangeJsonObject;
	}

	if (timeUnit){
		jsonObject["timeUnit"] = QJsonValue::fromVariant(*timeUnit);
	}

	if (interpretationMode){
		jsonObject["interpretationMode"] = QJsonValue::fromVariant(*interpretationMode);
	}

	if (unitMultiplier){
		jsonObject["unitMultiplier"] = QJsonValue::fromVariant(*unitMultiplier);
	}

	jsonObject["__typename"] = "TimeFilter";

	return true;
}


bool V1_0::imtbase::CTimeFilter::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("timeRange") && jsonObject["timeRange"].isObject()){
		timeRange = V1_0::imtbase::CTimeRange();
		const bool isTimeRangeRead = timeRange->ReadFromJsonObject(jsonObject["timeRange"].toObject());
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("timeUnit") && jsonObject["timeUnit"].isString()){
		timeUnit = jsonObject["timeUnit"].toString();
	}

	if (jsonObject.contains("interpretationMode") && jsonObject["interpretationMode"].isString()){
		interpretationMode = jsonObject["interpretationMode"].toString();
	}

	if (jsonObject.contains("unitMultiplier") && jsonObject["unitMultiplier"].isDouble()){
		unitMultiplier = jsonObject["unitMultiplier"].toInt();
	}

	return true;
}


bool V1_0::imtbase::CTimeFilter::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("timeRange") && jsonObject["timeRange"].isObject()){
		timeRange = V1_0::imtbase::CTimeRange();
		const bool isTimeRangeRead = timeRange->OptReadFromJsonObject(jsonObject["timeRange"].toObject());
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("timeUnit") && jsonObject["timeUnit"].isString()){
		timeUnit = jsonObject["timeUnit"].toString();
	}

	if (jsonObject.contains("interpretationMode") && jsonObject["interpretationMode"].isString()){
		interpretationMode = jsonObject["interpretationMode"].toString();
	}

	if (jsonObject.contains("unitMultiplier") && jsonObject["unitMultiplier"].isDouble()){
		unitMultiplier = jsonObject["unitMultiplier"].toInt();
	}

	return true;
}




QByteArray CFieldSortingInfo::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CFieldSortingInfo::operator==(const CFieldSortingInfo& other) const
{
	return 
				fieldId == other.fieldId &&
				sortingOrder == other.sortingOrder;
}


bool V1_0::imtbase::CFieldSortingInfo::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "FieldSortingInfo", modelIndex);

	if (!fieldId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("fieldId", *fieldId, modelIndex);

	if (!sortingOrder){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("sortingOrder", *sortingOrder, modelIndex);


	return true;
}


bool V1_0::imtbase::CFieldSortingInfo::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant fieldIdData = model.GetData("fieldId", modelIndex);
	if (fieldIdData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = fieldIdData.toString();

	QVariant sortingOrderData = model.GetData("sortingOrder", modelIndex);
	if (sortingOrderData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

		return false;
	}
	sortingOrder = sortingOrderData.toString();

	return true;
}


bool V1_0::imtbase::CFieldSortingInfo::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant fieldIdData = model.GetData("fieldId", modelIndex);
	if (!fieldIdData.isNull()){
		fieldId = fieldIdData.toString();
	}

	QVariant sortingOrderData = model.GetData("sortingOrder", modelIndex);
	if (!sortingOrderData.isNull()){
		sortingOrder = sortingOrderData.toString();
	}

	return true;
}


bool V1_0::imtbase::CFieldSortingInfo::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!fieldId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("fieldId", QVariant(*fieldId));

	if (!sortingOrder){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("sortingOrder", QVariant(*sortingOrder));

	gqlObject.InsertParam("__typename", QVariant("FieldSortingInfo"));

	return true;
}


bool V1_0::imtbase::CFieldSortingInfo::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("fieldId") || (gqlObject["fieldId"].userType() != QMetaType::QString && gqlObject["fieldId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = gqlObject["fieldId"].toString();

	if (!gqlObject.ContainsParam("sortingOrder") || (gqlObject["sortingOrder"].userType() != QMetaType::QString && gqlObject["sortingOrder"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

		return false;
	}
	sortingOrder = gqlObject["sortingOrder"].toString();

	return true;
}


bool V1_0::imtbase::CFieldSortingInfo::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("fieldId") && (gqlObject["fieldId"].userType() == QMetaType::QString || gqlObject["fieldId"].userType() == QMetaType::QByteArray)){
		fieldId = gqlObject["fieldId"].toString();
	}

	if (gqlObject.ContainsParam("sortingOrder") && (gqlObject["sortingOrder"].userType() == QMetaType::QString || gqlObject["sortingOrder"].userType() == QMetaType::QByteArray)){
		sortingOrder = gqlObject["sortingOrder"].toString();
	}

	return true;
}


bool V1_0::imtbase::CFieldSortingInfo::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!fieldId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["fieldId"] = QJsonValue::fromVariant(*fieldId);

	if (!sortingOrder){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["sortingOrder"] = QJsonValue::fromVariant(*sortingOrder);

	jsonObject["__typename"] = "FieldSortingInfo";

	return true;
}


bool V1_0::imtbase::CFieldSortingInfo::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("fieldId") || ! jsonObject["fieldId"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = jsonObject["fieldId"].toString();

	if (!jsonObject.contains("sortingOrder") || ! jsonObject["sortingOrder"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

		return false;
	}
	sortingOrder = jsonObject["sortingOrder"].toString();

	return true;
}


bool V1_0::imtbase::CFieldSortingInfo::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("fieldId") && jsonObject["fieldId"].isString()){
		fieldId = jsonObject["fieldId"].toString();
	}

	if (jsonObject.contains("sortingOrder") && jsonObject["sortingOrder"].isString()){
		sortingOrder = jsonObject["sortingOrder"].toString();
	}

	return true;
}




QByteArray CFieldFilter::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CFieldFilter::operator==(const CFieldFilter& other) const
{
	return 
				fieldId == other.fieldId &&
				filterValue == other.filterValue &&
				filterValueType == other.filterValueType &&
				filterOperations == other.filterOperations;
}


bool V1_0::imtbase::CFieldFilter::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "FieldFilter", modelIndex);

	if (!fieldId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("fieldId", *fieldId, modelIndex);

	if (!filterValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("filterValue", *filterValue, modelIndex);

	if (!filterValueType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

		return false;
	}
	QString filterValueTypeStringValue;
	switch (*filterValueType){
	case ValueType::Integer:
		filterValueTypeStringValue = "Integer";
		break;
	case ValueType::Number:
		filterValueTypeStringValue = "Number";
		break;
	case ValueType::String:
		filterValueTypeStringValue = "String";
		break;
	case ValueType::Bool:
		filterValueTypeStringValue = "Bool";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("filterValueType", filterValueTypeStringValue, modelIndex);

	if (!filterOperations){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterOperations").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* newFilterOperationsModelPtr = model.AddTreeModel("filterOperations", modelIndex);
	newFilterOperationsModelPtr->setIsArray(true);
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperations->size(); ++filterOperationsIndex){
		if (!filterOperations->at(filterOperationsIndex)){
			newFilterOperationsModelPtr->InsertNewItem();
			newFilterOperationsModelPtr->SetData(QByteArray(), QVariant(), filterOperationsIndex);
			continue;
		}
		QString filterOperationsStringValue;
		switch (*filterOperations->at(filterOperationsIndex)){
		case FilterOperation::Not:
			filterOperationsStringValue = "Not";
			break;
		case FilterOperation::Equal:
			filterOperationsStringValue = "Equal";
			break;
		case FilterOperation::Less:
			filterOperationsStringValue = "Less";
			break;
		case FilterOperation::Greater:
			filterOperationsStringValue = "Greater";
			break;
		case FilterOperation::Contains:
			filterOperationsStringValue = "Contains";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		newFilterOperationsModelPtr->InsertNewItem();
		newFilterOperationsModelPtr->SetData(QByteArray(), filterOperationsStringValue, filterOperationsIndex);
	}


	return true;
}


bool V1_0::imtbase::CFieldFilter::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant fieldIdData = model.GetData("fieldId", modelIndex);
	if (fieldIdData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = fieldIdData.toByteArray();

	QVariant filterValueData = model.GetData("filterValue", modelIndex);
	if (filterValueData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	filterValue = filterValueData.toString();

	QVariant filterValueTypeData = model.GetData("filterValueType", modelIndex);
	if (filterValueTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

		return false;
	}
	QString filterValueTypeStringValue = filterValueTypeData.toString();
	if(filterValueTypeStringValue == "Integer"){
		filterValueType = ValueType::Integer;
	}
	else if(filterValueTypeStringValue == "Number"){
		filterValueType = ValueType::Number;
	}
	else if(filterValueTypeStringValue == "String"){
		filterValueType = ValueType::String;
	}
	else if(filterValueTypeStringValue == "Bool"){
		filterValueType = ValueType::Bool;
	}
	else {
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* filterOperationsModel = model.GetTreeItemModel("filterOperations", modelIndex);
	if (filterOperationsModel == nullptr){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' is missing, but required").arg(__FILE__, QString::number(__LINE__), "filterOperations").toLocal8Bit().constData();)

		return false;
	}
	int filterOperationsCount = filterOperationsModel->GetItemsCount();
	imtsdl::TElementList<V1_0::imtbase::FilterOperation> filterOperationsList;
	for (int filterOperationsIndex = 0; filterOperationsIndex < filterOperationsCount; ++filterOperationsIndex){
		if (!filterOperationsModel->GetData(QByteArray(), filterOperationsIndex).isValid()){
			filterOperationsList.AppendNull();
			continue;
		}
		V1_0::imtbase::FilterOperation filterOperationsData;
		QString filterOperationsStringValue = filterOperationsModel->GetData(QByteArray(), filterOperationsIndex).toString();
		if(filterOperationsStringValue == "Not"){
			filterOperationsData = FilterOperation::Not;
		}
		else if(filterOperationsStringValue == "Equal"){
			filterOperationsData = FilterOperation::Equal;
		}
		else if(filterOperationsStringValue == "Less"){
			filterOperationsData = FilterOperation::Less;
		}
		else if(filterOperationsStringValue == "Greater"){
			filterOperationsData = FilterOperation::Greater;
		}
		else if(filterOperationsStringValue == "Contains"){
			filterOperationsData = FilterOperation::Contains;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", filterOperationsStringValue);)

			return false;
		}

		filterOperationsList << filterOperationsData;
	}
	filterOperations = filterOperationsList;

	return true;
}


bool V1_0::imtbase::CFieldFilter::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant fieldIdData = model.GetData("fieldId", modelIndex);
	if (!fieldIdData.isNull()){
		fieldId = fieldIdData.toByteArray();
	}

	QVariant filterValueData = model.GetData("filterValue", modelIndex);
	if (!filterValueData.isNull()){
		filterValue = filterValueData.toString();
	}

	QVariant filterValueTypeData = model.GetData("filterValueType", modelIndex);
	if (!filterValueTypeData.isNull()){
		QString filterValueTypeStringValue = filterValueTypeData.toString();
		if(filterValueTypeStringValue == "Integer"){
			filterValueType = ValueType::Integer;
		}
		else if(filterValueTypeStringValue == "Number"){
			filterValueType = ValueType::Number;
		}
		else if(filterValueTypeStringValue == "String"){
			filterValueType = ValueType::String;
		}
		else if(filterValueTypeStringValue == "Bool"){
			filterValueType = ValueType::Bool;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* filterOperationsModel = model.GetTreeItemModel("filterOperations", modelIndex);
	if (filterOperationsModel != nullptr){
		int filterOperationsCount = filterOperationsModel->GetItemsCount();
		imtsdl::TElementList<V1_0::imtbase::FilterOperation> filterOperationsList;
		for (int filterOperationsIndex = 0; filterOperationsIndex < filterOperationsCount; ++filterOperationsIndex){
			if (!filterOperationsModel->GetData(QByteArray(), filterOperationsIndex).isValid()){
				filterOperationsList.AppendNull();
				continue;
			}
			V1_0::imtbase::FilterOperation filterOperationsData;
			QString filterOperationsStringValue = filterOperationsModel->GetData(QByteArray(), filterOperationsIndex).toString();
			if(filterOperationsStringValue == "Not"){
				filterOperationsData = FilterOperation::Not;
			}
			else if(filterOperationsStringValue == "Equal"){
				filterOperationsData = FilterOperation::Equal;
			}
			else if(filterOperationsStringValue == "Less"){
				filterOperationsData = FilterOperation::Less;
			}
			else if(filterOperationsStringValue == "Greater"){
				filterOperationsData = FilterOperation::Greater;
			}
			else if(filterOperationsStringValue == "Contains"){
				filterOperationsData = FilterOperation::Contains;
			}
			else {
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", filterOperationsStringValue);)

				return false;
			}

			filterOperationsList << filterOperationsData;
		}
		filterOperations = filterOperationsList;

	}

	return true;
}


bool V1_0::imtbase::CFieldFilter::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!fieldId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("fieldId", QVariant(*fieldId));

	if (!filterValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("filterValue", QVariant(*filterValue));

	if (!filterValueType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

		return false;
	}
	QString filterValueTypeStringValue;
	switch (*filterValueType){
	case ValueType::Integer:
		filterValueTypeStringValue = "Integer";
		break;
	case ValueType::Number:
		filterValueTypeStringValue = "Number";
		break;
	case ValueType::String:
		filterValueTypeStringValue = "String";
		break;
	case ValueType::Bool:
		filterValueTypeStringValue = "Bool";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("filterValueType", QVariant(filterValueTypeStringValue));

	if (!filterOperations){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterOperations").toLocal8Bit().constData();)

		return false;
	}
	QVariantList filterOperationsDataObjectList;
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperations->size(); ++filterOperationsIndex){
		if (!filterOperations->at(filterOperationsIndex)){
			filterOperationsDataObjectList << QVariant();
			continue;
		}
		QString filterOperationsStringValue;
		switch (*filterOperations->at(filterOperationsIndex)){
		case FilterOperation::Not:
			filterOperationsStringValue = "Not";
			break;
		case FilterOperation::Equal:
			filterOperationsStringValue = "Equal";
			break;
		case FilterOperation::Less:
			filterOperationsStringValue = "Less";
			break;
		case FilterOperation::Greater:
			filterOperationsStringValue = "Greater";
			break;
		case FilterOperation::Contains:
			filterOperationsStringValue = "Contains";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		filterOperationsDataObjectList << filterOperationsStringValue;
	}
	gqlObject.InsertParam("filterOperations", filterOperationsDataObjectList);

	gqlObject.InsertParam("__typename", QVariant("FieldFilter"));

	return true;
}


bool V1_0::imtbase::CFieldFilter::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("fieldId") || (gqlObject["fieldId"].userType() != QMetaType::QString && gqlObject["fieldId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = gqlObject["fieldId"].toByteArray();

	if (!gqlObject.ContainsParam("filterValue") || (gqlObject["filterValue"].userType() != QMetaType::QString && gqlObject["filterValue"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	filterValue = gqlObject["filterValue"].toString();

	if (!gqlObject.ContainsParam("filterValueType") || (gqlObject["filterValueType"].userType() != QMetaType::QString && gqlObject["filterValueType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

		return false;
	}
	const QString filterValueTypeStringValue = gqlObject["filterValueType"].toString();
	if(filterValueTypeStringValue == "Integer"){
		filterValueType = ValueType::Integer;
	}
	else if(filterValueTypeStringValue == "Number"){
		filterValueType = ValueType::Number;
	}
	else if(filterValueTypeStringValue == "String"){
		filterValueType = ValueType::String;
	}
	else if(filterValueTypeStringValue == "Bool"){
		filterValueType = ValueType::Bool;
	}
	else {
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("filterOperations") || (gqlObject["filterOperations"].isNull())){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterOperations").toLocal8Bit().constData();)

		return false;
	}
	const QVariant filterOperationsData = gqlObject["filterOperations"];
	const QVariantList filterOperationsDataList = filterOperationsData.toList();
	const qsizetype filterOperationsElementsCount = filterOperationsDataList.size();
	filterOperations = imtsdl::TElementList<V1_0::imtbase::FilterOperation>();
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperationsElementsCount; ++filterOperationsIndex){
		if (!filterOperationsDataList[filterOperationsIndex].isValid() || filterOperationsDataList[filterOperationsIndex].isNull()){
			filterOperations->AppendNull();
			continue;
		}
		const QString tempFilterOperations = filterOperationsDataList[filterOperationsIndex].toString();
		V1_0::imtbase::FilterOperation filterOperationsDataValue;
		if(tempFilterOperations == "Not"){
			filterOperationsDataValue = FilterOperation::Not;
		}
		else if(tempFilterOperations == "Equal"){
			filterOperationsDataValue = FilterOperation::Equal;
		}
		else if(tempFilterOperations == "Less"){
			filterOperationsDataValue = FilterOperation::Less;
		}
		else if(tempFilterOperations == "Greater"){
			filterOperationsDataValue = FilterOperation::Greater;
		}
		else if(tempFilterOperations == "Contains"){
			filterOperationsDataValue = FilterOperation::Contains;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", tempFilterOperations);)

			return false;
		}
		filterOperations->append(filterOperationsDataValue);
	}

	return true;
}


bool V1_0::imtbase::CFieldFilter::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("fieldId") && (gqlObject["fieldId"].userType() == QMetaType::QString || gqlObject["fieldId"].userType() == QMetaType::QByteArray)){
		fieldId = gqlObject["fieldId"].toByteArray();
	}

	if (gqlObject.ContainsParam("filterValue") && (gqlObject["filterValue"].userType() == QMetaType::QString || gqlObject["filterValue"].userType() == QMetaType::QByteArray)){
		filterValue = gqlObject["filterValue"].toString();
	}

	if (gqlObject.ContainsParam("filterValueType") && (gqlObject["filterValueType"].userType() == QMetaType::QString || gqlObject["filterValueType"].userType() == QMetaType::QByteArray)){
		const QString filterValueTypeStringValue = gqlObject["filterValueType"].toString();
		if(filterValueTypeStringValue == "Integer"){
			filterValueType = ValueType::Integer;
		}
		else if(filterValueTypeStringValue == "Number"){
			filterValueType = ValueType::Number;
		}
		else if(filterValueTypeStringValue == "String"){
			filterValueType = ValueType::String;
		}
		else if(filterValueTypeStringValue == "Bool"){
			filterValueType = ValueType::Bool;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("filterOperations") && !(gqlObject["filterOperations"].isNull())){
		const QVariant filterOperationsData = gqlObject["filterOperations"];
		const QVariantList filterOperationsDataList = filterOperationsData.toList();
		const qsizetype filterOperationsElementsCount = filterOperationsDataList.size();
		filterOperations = imtsdl::TElementList<V1_0::imtbase::FilterOperation>();
		for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperationsElementsCount; ++filterOperationsIndex){
			if (!filterOperationsDataList[filterOperationsIndex].isValid() || filterOperationsDataList[filterOperationsIndex].isNull()){
				filterOperations->AppendNull();
				continue;
			}
			const QString tempFilterOperations = filterOperationsDataList[filterOperationsIndex].toString();
			V1_0::imtbase::FilterOperation filterOperationsDataValue;
			if(tempFilterOperations == "Not"){
				filterOperationsDataValue = FilterOperation::Not;
			}
			else if(tempFilterOperations == "Equal"){
				filterOperationsDataValue = FilterOperation::Equal;
			}
			else if(tempFilterOperations == "Less"){
				filterOperationsDataValue = FilterOperation::Less;
			}
			else if(tempFilterOperations == "Greater"){
				filterOperationsDataValue = FilterOperation::Greater;
			}
			else if(tempFilterOperations == "Contains"){
				filterOperationsDataValue = FilterOperation::Contains;
			}
			else {
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", tempFilterOperations);)

				return false;
			}
			filterOperations->append(filterOperationsDataValue);
		}
	}

	return true;
}


bool V1_0::imtbase::CFieldFilter::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!fieldId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["fieldId"] = QString::fromUtf8(*fieldId);

	if (!filterValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["filterValue"] = QJsonValue::fromVariant(*filterValue);

	if (!filterValueType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

		return false;
	}
	QString filterValueTypeStringValue;
	switch (*filterValueType){
	case ValueType::Integer:
		filterValueTypeStringValue = "Integer";
		break;
	case ValueType::Number:
		filterValueTypeStringValue = "Number";
		break;
	case ValueType::String:
		filterValueTypeStringValue = "String";
		break;
	case ValueType::Bool:
		filterValueTypeStringValue = "Bool";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["filterValueType"] = QJsonValue::fromVariant(filterValueTypeStringValue);

	if (!filterOperations){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterOperations").toLocal8Bit().constData();)

		return false;
	}
	QJsonArray newFilterOperationsArray;
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperations->size(); ++filterOperationsIndex){
		if (!filterOperations->at(filterOperationsIndex)){
			newFilterOperationsArray << QJsonValue(QJsonValue::Null);
			continue;
		}
		QString filterOperationsStringValue;
		switch (*filterOperations->at(filterOperationsIndex)){
		case FilterOperation::Not:
			filterOperationsStringValue = "Not";
			break;
		case FilterOperation::Equal:
			filterOperationsStringValue = "Equal";
			break;
		case FilterOperation::Less:
			filterOperationsStringValue = "Less";
			break;
		case FilterOperation::Greater:
			filterOperationsStringValue = "Greater";
			break;
		case FilterOperation::Contains:
			filterOperationsStringValue = "Contains";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		newFilterOperationsArray << filterOperationsStringValue;
	}
	jsonObject["filterOperations"] = newFilterOperationsArray;

	jsonObject["__typename"] = "FieldFilter";

	return true;
}


bool V1_0::imtbase::CFieldFilter::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("fieldId") || ! jsonObject["fieldId"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = jsonObject["fieldId"].toString().toUtf8();

	if (!jsonObject.contains("filterValue") || ! jsonObject["filterValue"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	filterValue = jsonObject["filterValue"].toString();

	if (!jsonObject.contains("filterValueType") || ! jsonObject["filterValueType"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

		return false;
	}
	const QString filterValueTypeStringValue = jsonObject["filterValueType"].toString();
	if(filterValueTypeStringValue == "Integer"){
		filterValueType = ValueType::Integer;
	}
	else if(filterValueTypeStringValue == "Number"){
		filterValueType = ValueType::Number;
	}
	else if(filterValueTypeStringValue == "String"){
		filterValueType = ValueType::String;
	}
	else if(filterValueTypeStringValue == "Bool"){
		filterValueType = ValueType::Bool;
	}
	else {
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

		return false;
	}

	if (!jsonObject.contains("filterOperations") || ! jsonObject["filterOperations"].isArray()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterOperations").toLocal8Bit().constData();)

		return false;
	}
	const QJsonArray filterOperationsJsonArray = jsonObject["filterOperations"].toArray();
	const qsizetype filterOperationsArrayCount = filterOperationsJsonArray.size();
	filterOperations = imtsdl::TElementList<V1_0::imtbase::FilterOperation>();
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperationsArrayCount; ++filterOperationsIndex){
		if (filterOperationsJsonArray[filterOperationsIndex].isNull()){
			filterOperations->AppendNull();
			continue;
		}
		const QString tempFilterOperations = filterOperationsJsonArray[filterOperationsIndex].toString();
		V1_0::imtbase::FilterOperation filterOperationsDataValue;
		if(tempFilterOperations == "Not"){
			filterOperationsDataValue = FilterOperation::Not;
		}
		else if(tempFilterOperations == "Equal"){
			filterOperationsDataValue = FilterOperation::Equal;
		}
		else if(tempFilterOperations == "Less"){
			filterOperationsDataValue = FilterOperation::Less;
		}
		else if(tempFilterOperations == "Greater"){
			filterOperationsDataValue = FilterOperation::Greater;
		}
		else if(tempFilterOperations == "Contains"){
			filterOperationsDataValue = FilterOperation::Contains;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", tempFilterOperations);)

			return false;
		}
		filterOperations->append(filterOperationsDataValue);
	}

	return true;
}


bool V1_0::imtbase::CFieldFilter::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("fieldId") && jsonObject["fieldId"].isString()){
		fieldId = jsonObject["fieldId"].toString().toUtf8();
	}

	if (jsonObject.contains("filterValue") && jsonObject["filterValue"].isString()){
		filterValue = jsonObject["filterValue"].toString();
	}

	if (jsonObject.contains("filterValueType") && jsonObject["filterValueType"].isString()){
		const QString filterValueTypeStringValue = jsonObject["filterValueType"].toString();
		if(filterValueTypeStringValue == "Integer"){
			filterValueType = ValueType::Integer;
		}
		else if(filterValueTypeStringValue == "Number"){
			filterValueType = ValueType::Number;
		}
		else if(filterValueTypeStringValue == "String"){
			filterValueType = ValueType::String;
		}
		else if(filterValueTypeStringValue == "Bool"){
			filterValueType = ValueType::Bool;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("filterOperations") && jsonObject["filterOperations"].isArray()){
		const QJsonArray filterOperationsJsonArray = jsonObject["filterOperations"].toArray();
		const qsizetype filterOperationsArrayCount = filterOperationsJsonArray.size();
		filterOperations = imtsdl::TElementList<V1_0::imtbase::FilterOperation>();
		for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperationsArrayCount; ++filterOperationsIndex){
			if (filterOperationsJsonArray[filterOperationsIndex].isNull()){
				filterOperations->AppendNull();
				continue;
			}
			const QString tempFilterOperations = filterOperationsJsonArray[filterOperationsIndex].toString();
			V1_0::imtbase::FilterOperation filterOperationsDataValue;
			if(tempFilterOperations == "Not"){
				filterOperationsDataValue = FilterOperation::Not;
			}
			else if(tempFilterOperations == "Equal"){
				filterOperationsDataValue = FilterOperation::Equal;
			}
			else if(tempFilterOperations == "Less"){
				filterOperationsDataValue = FilterOperation::Less;
			}
			else if(tempFilterOperations == "Greater"){
				filterOperationsDataValue = FilterOperation::Greater;
			}
			else if(tempFilterOperations == "Contains"){
				filterOperationsDataValue = FilterOperation::Contains;
			}
			else {
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", tempFilterOperations);)

				return false;
			}
			filterOperations->append(filterOperationsDataValue);
		}
	}

	return true;
}




QByteArray CGroupFilter::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CGroupFilter::operator==(const CGroupFilter& other) const
{
	return 
				fieldFilters == other.fieldFilters &&
				groupFilters == other.groupFilters &&
				logicalOperation == other.logicalOperation;
}


bool V1_0::imtbase::CGroupFilter::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "GroupFilter", modelIndex);

	if (fieldFilters){
		::imtbase::CTreeItemModel* newFieldFiltersModelPtr = model.AddTreeModel("fieldFilters", modelIndex);
		newFieldFiltersModelPtr->setIsArray(true);
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFilters->size(); ++fieldFiltersIndex){
			if (!fieldFilters->at(fieldFiltersIndex)){
				newFieldFiltersModelPtr->InsertNewItem();
				newFieldFiltersModelPtr->SetData(QByteArray(), QVariant(), fieldFiltersIndex);
				continue;
			}
			newFieldFiltersModelPtr->InsertNewItem();
			if (!(fieldFilters->at(fieldFiltersIndex)->WriteToModel(*newFieldFiltersModelPtr, fieldFiltersIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (fieldFilters.IsNull()){
		model.SetData("fieldFilters", QVariant(), modelIndex);
	}
	if (groupFilters){
		::imtbase::CTreeItemModel* newGroupFiltersModelPtr = model.AddTreeModel("groupFilters", modelIndex);
		newGroupFiltersModelPtr->setIsArray(true);
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFilters->size(); ++groupFiltersIndex){
			if (!groupFilters->at(groupFiltersIndex)){
				newGroupFiltersModelPtr->InsertNewItem();
				newGroupFiltersModelPtr->SetData(QByteArray(), QVariant(), groupFiltersIndex);
				continue;
			}
			newGroupFiltersModelPtr->InsertNewItem();
			if (!(groupFilters->at(groupFiltersIndex)->WriteToModel(*newGroupFiltersModelPtr, groupFiltersIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (groupFilters.IsNull()){
		model.SetData("groupFilters", QVariant(), modelIndex);
	}
	if (!logicalOperation){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

		return false;
	}
	QString logicalOperationStringValue;
	switch (*logicalOperation){
	case LogicalOperation::And:
		logicalOperationStringValue = "And";
		break;
	case LogicalOperation::Or:
		logicalOperationStringValue = "Or";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("logicalOperation", logicalOperationStringValue, modelIndex);


	return true;
}


bool V1_0::imtbase::CGroupFilter::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* fieldFiltersModel = model.GetTreeItemModel("fieldFilters", modelIndex);
	if (model.ContainsKey("fieldFilters", modelIndex) && fieldFiltersModel == nullptr){
		fieldFilters.SetNull();
	}
	if (fieldFiltersModel != nullptr){
		int fieldFiltersCount = fieldFiltersModel->GetItemsCount();
		imtsdl::TElementList<V1_0::imtbase::CFieldFilter> fieldFiltersList;
		for (int fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersCount; ++fieldFiltersIndex){
			if (fieldFiltersModel->ContainsKey(QByteArray(), fieldFiltersIndex) && !fieldFiltersModel->GetData(QByteArray(), fieldFiltersIndex).isValid()){
				fieldFiltersList.AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldFilter t_fieldFilters;
			if (!t_fieldFilters.ReadFromModel(*fieldFiltersModel, fieldFiltersIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFiltersList << t_fieldFilters;
		}
		fieldFilters = fieldFiltersList;

	}

	::imtbase::CTreeItemModel* groupFiltersModel = model.GetTreeItemModel("groupFilters", modelIndex);
	if (model.ContainsKey("groupFilters", modelIndex) && groupFiltersModel == nullptr){
		groupFilters.SetNull();
	}
	if (groupFiltersModel != nullptr){
		int groupFiltersCount = groupFiltersModel->GetItemsCount();
		imtsdl::TElementList<V1_0::imtbase::CGroupFilter> groupFiltersList;
		for (int groupFiltersIndex = 0; groupFiltersIndex < groupFiltersCount; ++groupFiltersIndex){
			if (groupFiltersModel->ContainsKey(QByteArray(), groupFiltersIndex) && !groupFiltersModel->GetData(QByteArray(), groupFiltersIndex).isValid()){
				groupFiltersList.AppendNull();
				continue;
			}
			V1_0::imtbase::CGroupFilter t_groupFilters;
			if (!t_groupFilters.ReadFromModel(*groupFiltersModel, groupFiltersIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			groupFiltersList << t_groupFilters;
		}
		groupFilters = groupFiltersList;

	}

	QVariant logicalOperationData = model.GetData("logicalOperation", modelIndex);
	if (logicalOperationData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

		return false;
	}
	QString logicalOperationStringValue = logicalOperationData.toString();
	if(logicalOperationStringValue == "And"){
		logicalOperation = LogicalOperation::And;
	}
	else if(logicalOperationStringValue == "Or"){
		logicalOperation = LogicalOperation::Or;
	}
	else {
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

		return false;
	}

	return true;
}


bool V1_0::imtbase::CGroupFilter::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* fieldFiltersModel = model.GetTreeItemModel("fieldFilters", modelIndex);
	if (model.ContainsKey("fieldFilters", modelIndex) && fieldFiltersModel == nullptr){
		fieldFilters.SetNull();
	}
	if (fieldFiltersModel != nullptr){
		int fieldFiltersCount = fieldFiltersModel->GetItemsCount();
		imtsdl::TElementList<V1_0::imtbase::CFieldFilter> fieldFiltersList;
		for (int fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersCount; ++fieldFiltersIndex){
			if (fieldFiltersModel->ContainsKey(QByteArray(), fieldFiltersIndex) && !fieldFiltersModel->GetData(QByteArray(), fieldFiltersIndex).isValid()){
				fieldFiltersList.AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldFilter t_fieldFilters;
			if (!t_fieldFilters.OptReadFromModel(*fieldFiltersModel, fieldFiltersIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFiltersList << t_fieldFilters;
		}
		fieldFilters = fieldFiltersList;

	}

	::imtbase::CTreeItemModel* groupFiltersModel = model.GetTreeItemModel("groupFilters", modelIndex);
	if (model.ContainsKey("groupFilters", modelIndex) && groupFiltersModel == nullptr){
		groupFilters.SetNull();
	}
	if (groupFiltersModel != nullptr){
		int groupFiltersCount = groupFiltersModel->GetItemsCount();
		imtsdl::TElementList<V1_0::imtbase::CGroupFilter> groupFiltersList;
		for (int groupFiltersIndex = 0; groupFiltersIndex < groupFiltersCount; ++groupFiltersIndex){
			if (groupFiltersModel->ContainsKey(QByteArray(), groupFiltersIndex) && !groupFiltersModel->GetData(QByteArray(), groupFiltersIndex).isValid()){
				groupFiltersList.AppendNull();
				continue;
			}
			V1_0::imtbase::CGroupFilter t_groupFilters;
			if (!t_groupFilters.OptReadFromModel(*groupFiltersModel, groupFiltersIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			groupFiltersList << t_groupFilters;
		}
		groupFilters = groupFiltersList;

	}

	QVariant logicalOperationData = model.GetData("logicalOperation", modelIndex);
	if (!logicalOperationData.isNull()){
		QString logicalOperationStringValue = logicalOperationData.toString();
		if(logicalOperationStringValue == "And"){
			logicalOperation = LogicalOperation::And;
		}
		else if(logicalOperationStringValue == "Or"){
			logicalOperation = LogicalOperation::Or;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::imtbase::CGroupFilter::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (fieldFilters){
		QList<::imtgql::CGqlParamObject> fieldFiltersDataObjectList;
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFilters->size(); ++fieldFiltersIndex){
			if (!fieldFilters->at(fieldFiltersIndex)){
				fieldFiltersDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newFieldFiltersGqlObject;
			if (!fieldFilters->at(fieldFiltersIndex)->WriteToGraphQlObject(newFieldFiltersGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFiltersDataObjectList << newFieldFiltersGqlObject;
		}
		gqlObject.InsertParam("fieldFilters", fieldFiltersDataObjectList);
	}
	else if (fieldFilters.IsNull()){
		gqlObject.InsertParam("fieldFilters", QVariant());
	}

	if (groupFilters){
		QList<::imtgql::CGqlParamObject> groupFiltersDataObjectList;
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFilters->size(); ++groupFiltersIndex){
			if (!groupFilters->at(groupFiltersIndex)){
				groupFiltersDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newGroupFiltersGqlObject;
			if (!groupFilters->at(groupFiltersIndex)->WriteToGraphQlObject(newGroupFiltersGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			groupFiltersDataObjectList << newGroupFiltersGqlObject;
		}
		gqlObject.InsertParam("groupFilters", groupFiltersDataObjectList);
	}
	else if (groupFilters.IsNull()){
		gqlObject.InsertParam("groupFilters", QVariant());
	}

	if (!logicalOperation){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

		return false;
	}
	QString logicalOperationStringValue;
	switch (*logicalOperation){
	case LogicalOperation::And:
		logicalOperationStringValue = "And";
		break;
	case LogicalOperation::Or:
		logicalOperationStringValue = "Or";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("logicalOperation", QVariant(logicalOperationStringValue));

	gqlObject.InsertParam("__typename", QVariant("GroupFilter"));

	return true;
}


bool V1_0::imtbase::CGroupFilter::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("fieldFilters") && gqlObject["fieldFilters"].isNull()){
		fieldFilters.SetNull();
	}
	if (gqlObject.ContainsParam("fieldFilters") && !(gqlObject["fieldFilters"].isNull())){
		const qsizetype fieldFiltersElementsCount = gqlObject.GetObjectsCount("fieldFilters");
		fieldFilters = imtsdl::TElementList<V1_0::imtbase::CFieldFilter>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersElementsCount; ++fieldFiltersIndex){
			const ::imtgql::CGqlParamObject* fieldFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("fieldFilters", fieldFiltersIndex);
			if (fieldFiltersDataObjectPtr == nullptr || fieldFiltersDataObjectPtr->IsNull()){
				fieldFilters->AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldFilter tempFieldFilters;
			if (!tempFieldFilters.ReadFromGraphQlObject(*fieldFiltersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (gqlObject.ContainsParam("groupFilters") && gqlObject["groupFilters"].isNull()){
		groupFilters.SetNull();
	}
	if (gqlObject.ContainsParam("groupFilters") && !(gqlObject["groupFilters"].isNull())){
		const qsizetype groupFiltersElementsCount = gqlObject.GetObjectsCount("groupFilters");
		groupFilters = imtsdl::TElementList<V1_0::imtbase::CGroupFilter>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersElementsCount; ++groupFiltersIndex){
			const ::imtgql::CGqlParamObject* groupFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("groupFilters", groupFiltersIndex);
			if (groupFiltersDataObjectPtr == nullptr || groupFiltersDataObjectPtr->IsNull()){
				groupFilters->AppendNull();
				continue;
			}
			V1_0::imtbase::CGroupFilter tempGroupFilters;
			if (!tempGroupFilters.ReadFromGraphQlObject(*groupFiltersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			groupFilters->append(tempGroupFilters);
		}
	}

	if (!gqlObject.ContainsParam("logicalOperation") || (gqlObject["logicalOperation"].userType() != QMetaType::QString && gqlObject["logicalOperation"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

		return false;
	}
	const QString logicalOperationStringValue = gqlObject["logicalOperation"].toString();
	if(logicalOperationStringValue == "And"){
		logicalOperation = LogicalOperation::And;
	}
	else if(logicalOperationStringValue == "Or"){
		logicalOperation = LogicalOperation::Or;
	}
	else {
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

		return false;
	}

	return true;
}


bool V1_0::imtbase::CGroupFilter::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("fieldFilters") && gqlObject["fieldFilters"].isNull()){
		fieldFilters.SetNull();
	}
	if (gqlObject.ContainsParam("fieldFilters") && !(gqlObject["fieldFilters"].isNull())){
		const qsizetype fieldFiltersElementsCount = gqlObject.GetObjectsCount("fieldFilters");
		fieldFilters = imtsdl::TElementList<V1_0::imtbase::CFieldFilter>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersElementsCount; ++fieldFiltersIndex){
			const ::imtgql::CGqlParamObject* fieldFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("fieldFilters", fieldFiltersIndex);
			if (fieldFiltersDataObjectPtr == nullptr || fieldFiltersDataObjectPtr->IsNull()){
				fieldFilters->AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldFilter tempFieldFilters;
			if (!tempFieldFilters.OptReadFromGraphQlObject(*fieldFiltersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (gqlObject.ContainsParam("groupFilters") && gqlObject["groupFilters"].isNull()){
		groupFilters.SetNull();
	}
	if (gqlObject.ContainsParam("groupFilters") && !(gqlObject["groupFilters"].isNull())){
		const qsizetype groupFiltersElementsCount = gqlObject.GetObjectsCount("groupFilters");
		groupFilters = imtsdl::TElementList<V1_0::imtbase::CGroupFilter>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersElementsCount; ++groupFiltersIndex){
			const ::imtgql::CGqlParamObject* groupFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("groupFilters", groupFiltersIndex);
			if (groupFiltersDataObjectPtr == nullptr || groupFiltersDataObjectPtr->IsNull()){
				groupFilters->AppendNull();
				continue;
			}
			V1_0::imtbase::CGroupFilter tempGroupFilters;
			if (!tempGroupFilters.OptReadFromGraphQlObject(*groupFiltersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			groupFilters->append(tempGroupFilters);
		}
	}

	if (gqlObject.ContainsParam("logicalOperation") && (gqlObject["logicalOperation"].userType() == QMetaType::QString || gqlObject["logicalOperation"].userType() == QMetaType::QByteArray)){
		const QString logicalOperationStringValue = gqlObject["logicalOperation"].toString();
		if(logicalOperationStringValue == "And"){
			logicalOperation = LogicalOperation::And;
		}
		else if(logicalOperationStringValue == "Or"){
			logicalOperation = LogicalOperation::Or;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::imtbase::CGroupFilter::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (fieldFilters){
		QJsonArray newFieldFiltersArray;
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFilters->size(); ++fieldFiltersIndex){
			if (!fieldFilters->at(fieldFiltersIndex)){
				newFieldFiltersArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newFieldFiltersJsonObject;
			if (!fieldFilters->at(fieldFiltersIndex)->WriteToJsonObject(newFieldFiltersJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			newFieldFiltersArray << newFieldFiltersJsonObject;
		}
		jsonObject["fieldFilters"] = newFieldFiltersArray;
	}
	else if (fieldFilters.IsNull()){
		jsonObject["fieldFilters"] = QJsonValue(QJsonValue::Null);
	}

	if (groupFilters){
		QJsonArray newGroupFiltersArray;
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFilters->size(); ++groupFiltersIndex){
			if (!groupFilters->at(groupFiltersIndex)){
				newGroupFiltersArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newGroupFiltersJsonObject;
			if (!groupFilters->at(groupFiltersIndex)->WriteToJsonObject(newGroupFiltersJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			newGroupFiltersArray << newGroupFiltersJsonObject;
		}
		jsonObject["groupFilters"] = newGroupFiltersArray;
	}
	else if (groupFilters.IsNull()){
		jsonObject["groupFilters"] = QJsonValue(QJsonValue::Null);
	}

	if (!logicalOperation){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

		return false;
	}
	QString logicalOperationStringValue;
	switch (*logicalOperation){
	case LogicalOperation::And:
		logicalOperationStringValue = "And";
		break;
	case LogicalOperation::Or:
		logicalOperationStringValue = "Or";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["logicalOperation"] = QJsonValue::fromVariant(logicalOperationStringValue);

	jsonObject["__typename"] = "GroupFilter";

	return true;
}


bool V1_0::imtbase::CGroupFilter::ReadFromJsonObject(const QJsonObject& jsonObject)
{
if (jsonObject.contains("fieldFilters") && jsonObject["fieldFilters"].isNull()){
		fieldFilters.SetNull();
	}
		if (jsonObject.contains("fieldFilters") && jsonObject["fieldFilters"].isArray()){
		const QJsonArray fieldFiltersJsonArray = jsonObject["fieldFilters"].toArray();
		const qsizetype fieldFiltersArrayCount = fieldFiltersJsonArray.size();
		fieldFilters = imtsdl::TElementList<V1_0::imtbase::CFieldFilter>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersArrayCount; ++fieldFiltersIndex){
			if (fieldFiltersJsonArray[fieldFiltersIndex].isNull()){
				fieldFilters->AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldFilter tempFieldFilters;
			if (!tempFieldFilters.ReadFromJsonObject(fieldFiltersJsonArray[fieldFiltersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

if (jsonObject.contains("groupFilters") && jsonObject["groupFilters"].isNull()){
		groupFilters.SetNull();
	}
		if (jsonObject.contains("groupFilters") && jsonObject["groupFilters"].isArray()){
		const QJsonArray groupFiltersJsonArray = jsonObject["groupFilters"].toArray();
		const qsizetype groupFiltersArrayCount = groupFiltersJsonArray.size();
		groupFilters = imtsdl::TElementList<V1_0::imtbase::CGroupFilter>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersArrayCount; ++groupFiltersIndex){
			if (groupFiltersJsonArray[groupFiltersIndex].isNull()){
				groupFilters->AppendNull();
				continue;
			}
			V1_0::imtbase::CGroupFilter tempGroupFilters;
			if (!tempGroupFilters.ReadFromJsonObject(groupFiltersJsonArray[groupFiltersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			groupFilters->append(tempGroupFilters);
		}
	}

	if (!jsonObject.contains("logicalOperation") || ! jsonObject["logicalOperation"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

		return false;
	}
	const QString logicalOperationStringValue = jsonObject["logicalOperation"].toString();
	if(logicalOperationStringValue == "And"){
		logicalOperation = LogicalOperation::And;
	}
	else if(logicalOperationStringValue == "Or"){
		logicalOperation = LogicalOperation::Or;
	}
	else {
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

		return false;
	}

	return true;
}


bool V1_0::imtbase::CGroupFilter::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
if (jsonObject.contains("fieldFilters") && jsonObject["fieldFilters"].isNull()){
		fieldFilters.SetNull();
	}
		if (jsonObject.contains("fieldFilters") && jsonObject["fieldFilters"].isArray()){
		const QJsonArray fieldFiltersJsonArray = jsonObject["fieldFilters"].toArray();
		const qsizetype fieldFiltersArrayCount = fieldFiltersJsonArray.size();
		fieldFilters = imtsdl::TElementList<V1_0::imtbase::CFieldFilter>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersArrayCount; ++fieldFiltersIndex){
			if (fieldFiltersJsonArray[fieldFiltersIndex].isNull()){
				fieldFilters->AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldFilter tempFieldFilters;
			if (!tempFieldFilters.OptReadFromJsonObject(fieldFiltersJsonArray[fieldFiltersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

if (jsonObject.contains("groupFilters") && jsonObject["groupFilters"].isNull()){
		groupFilters.SetNull();
	}
		if (jsonObject.contains("groupFilters") && jsonObject["groupFilters"].isArray()){
		const QJsonArray groupFiltersJsonArray = jsonObject["groupFilters"].toArray();
		const qsizetype groupFiltersArrayCount = groupFiltersJsonArray.size();
		groupFilters = imtsdl::TElementList<V1_0::imtbase::CGroupFilter>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersArrayCount; ++groupFiltersIndex){
			if (groupFiltersJsonArray[groupFiltersIndex].isNull()){
				groupFilters->AppendNull();
				continue;
			}
			V1_0::imtbase::CGroupFilter tempGroupFilters;
			if (!tempGroupFilters.OptReadFromJsonObject(groupFiltersJsonArray[groupFiltersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			groupFilters->append(tempGroupFilters);
		}
	}

	if (jsonObject.contains("logicalOperation") && jsonObject["logicalOperation"].isString()){
		const QString logicalOperationStringValue = jsonObject["logicalOperation"].toString();
		if(logicalOperationStringValue == "And"){
			logicalOperation = LogicalOperation::And;
		}
		else if(logicalOperationStringValue == "Or"){
			logicalOperation = LogicalOperation::Or;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

			return false;
		}
	}

	return true;
}




QByteArray CComplexCollectionFilter::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CComplexCollectionFilter::operator==(const CComplexCollectionFilter& other) const
{
	return 
				sortingInfo == other.sortingInfo &&
				fieldsFilter == other.fieldsFilter &&
				timeFilter == other.timeFilter &&
				distinctFields == other.distinctFields;
}


bool V1_0::imtbase::CComplexCollectionFilter::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "ComplexCollectionFilter", modelIndex);

	if (sortingInfo){
		::imtbase::CTreeItemModel* newSortingInfoModelPtr = model.AddTreeModel("sortingInfo", modelIndex);
		newSortingInfoModelPtr->setIsArray(true);
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfo->size(); ++sortingInfoIndex){
			if (!sortingInfo->at(sortingInfoIndex)){
				newSortingInfoModelPtr->InsertNewItem();
				newSortingInfoModelPtr->SetData(QByteArray(), QVariant(), sortingInfoIndex);
				continue;
			}
			newSortingInfoModelPtr->InsertNewItem();
			if (!(sortingInfo->at(sortingInfoIndex)->WriteToModel(*newSortingInfoModelPtr, sortingInfoIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (sortingInfo.IsNull()){
		model.SetData("sortingInfo", QVariant(), modelIndex);
	}

	if (fieldsFilter){
		::imtbase::CTreeItemModel* fieldsFilterNewModelPtr = model.AddTreeModel("fieldsFilter", modelIndex);
		const bool isFieldsFilterAdded = fieldsFilter->WriteToModel(*fieldsFilterNewModelPtr, 0);
		if (!isFieldsFilterAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}

	}

	if (timeFilter){
		::imtbase::CTreeItemModel* timeFilterNewModelPtr = model.AddTreeModel("timeFilter", modelIndex);
		const bool isTimeFilterAdded = timeFilter->WriteToModel(*timeFilterNewModelPtr, 0);
		if (!isTimeFilterAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}

	}
	if (distinctFields){
		::imtbase::CTreeItemModel* newDistinctFieldsModelPtr = model.AddTreeModel("distinctFields", modelIndex);
		newDistinctFieldsModelPtr->setIsArray(true);
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFields->size(); ++distinctFieldsIndex){
			if (!distinctFields->at(distinctFieldsIndex)){
				newDistinctFieldsModelPtr->InsertNewItem();
				newDistinctFieldsModelPtr->SetData(QByteArray(), QVariant(), distinctFieldsIndex);
				continue;
			}
			newDistinctFieldsModelPtr->InsertNewItem();
			newDistinctFieldsModelPtr->SetData(QByteArray(), *distinctFields->at(distinctFieldsIndex), distinctFieldsIndex);
		}
	}
	else if (distinctFields.IsNull()){
		model.SetData("distinctFields", QVariant(), modelIndex);
	}


	return true;
}


bool V1_0::imtbase::CComplexCollectionFilter::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* sortingInfoModel = model.GetTreeItemModel("sortingInfo", modelIndex);
	if (model.ContainsKey("sortingInfo", modelIndex) && sortingInfoModel == nullptr){
		sortingInfo.SetNull();
	}
	if (sortingInfoModel != nullptr){
		int sortingInfoCount = sortingInfoModel->GetItemsCount();
		imtsdl::TElementList<V1_0::imtbase::CFieldSortingInfo> sortingInfoList;
		for (int sortingInfoIndex = 0; sortingInfoIndex < sortingInfoCount; ++sortingInfoIndex){
			if (sortingInfoModel->ContainsKey(QByteArray(), sortingInfoIndex) && !sortingInfoModel->GetData(QByteArray(), sortingInfoIndex).isValid()){
				sortingInfoList.AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldSortingInfo t_sortingInfo;
			if (!t_sortingInfo.ReadFromModel(*sortingInfoModel, sortingInfoIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfoList << t_sortingInfo;
		}
		sortingInfo = sortingInfoList;

	}

	::imtbase::CTreeItemModel* fieldsFilterDataModelPtr = model.GetTreeItemModel("fieldsFilter", modelIndex);
	if (fieldsFilterDataModelPtr != nullptr){
		fieldsFilter = V1_0::imtbase::CGroupFilter();
		const bool isFieldsFilterRead = fieldsFilter->ReadFromModel(*fieldsFilterDataModelPtr, modelIndex);
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* timeFilterDataModelPtr = model.GetTreeItemModel("timeFilter", modelIndex);
	if (timeFilterDataModelPtr != nullptr){
		timeFilter = V1_0::imtbase::CTimeFilter();
		const bool isTimeFilterRead = timeFilter->ReadFromModel(*timeFilterDataModelPtr, modelIndex);
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* distinctFieldsModel = model.GetTreeItemModel("distinctFields", modelIndex);
	if (model.ContainsKey("distinctFields", modelIndex) && distinctFieldsModel == nullptr){
		distinctFields.SetNull();
	}
	if (distinctFieldsModel != nullptr){
		int distinctFieldsCount = distinctFieldsModel->GetItemsCount();
		imtsdl::TElementList<QByteArray> distinctFieldsList;
		for (int distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsCount; ++distinctFieldsIndex){
			if (!distinctFieldsModel->GetData(QByteArray(), distinctFieldsIndex).isValid()){
				distinctFieldsList.AppendNull();
				continue;
			}
			QByteArray t_distinctFields = distinctFieldsModel->GetData(QByteArray(), distinctFieldsIndex).toByteArray();
			distinctFieldsList << t_distinctFields;
		}
		distinctFields = distinctFieldsList;

	}

	return true;
}


bool V1_0::imtbase::CComplexCollectionFilter::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* sortingInfoModel = model.GetTreeItemModel("sortingInfo", modelIndex);
	if (model.ContainsKey("sortingInfo", modelIndex) && sortingInfoModel == nullptr){
		sortingInfo.SetNull();
	}
	if (sortingInfoModel != nullptr){
		int sortingInfoCount = sortingInfoModel->GetItemsCount();
		imtsdl::TElementList<V1_0::imtbase::CFieldSortingInfo> sortingInfoList;
		for (int sortingInfoIndex = 0; sortingInfoIndex < sortingInfoCount; ++sortingInfoIndex){
			if (sortingInfoModel->ContainsKey(QByteArray(), sortingInfoIndex) && !sortingInfoModel->GetData(QByteArray(), sortingInfoIndex).isValid()){
				sortingInfoList.AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldSortingInfo t_sortingInfo;
			if (!t_sortingInfo.OptReadFromModel(*sortingInfoModel, sortingInfoIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfoList << t_sortingInfo;
		}
		sortingInfo = sortingInfoList;

	}

	::imtbase::CTreeItemModel* fieldsFilterDataModelPtr = model.GetTreeItemModel("fieldsFilter", modelIndex);
	if (fieldsFilterDataModelPtr != nullptr){
		fieldsFilter = V1_0::imtbase::CGroupFilter();
		const bool isFieldsFilterRead = fieldsFilter->ReadFromModel(*fieldsFilterDataModelPtr, modelIndex);
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* timeFilterDataModelPtr = model.GetTreeItemModel("timeFilter", modelIndex);
	if (timeFilterDataModelPtr != nullptr){
		timeFilter = V1_0::imtbase::CTimeFilter();
		const bool isTimeFilterRead = timeFilter->ReadFromModel(*timeFilterDataModelPtr, modelIndex);
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* distinctFieldsModel = model.GetTreeItemModel("distinctFields", modelIndex);
	if (model.ContainsKey("distinctFields", modelIndex) && distinctFieldsModel == nullptr){
		distinctFields.SetNull();
	}
	if (distinctFieldsModel != nullptr){
		int distinctFieldsCount = distinctFieldsModel->GetItemsCount();
		imtsdl::TElementList<QByteArray> distinctFieldsList;
		for (int distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsCount; ++distinctFieldsIndex){
			if (!distinctFieldsModel->GetData(QByteArray(), distinctFieldsIndex).isValid()){
				distinctFieldsList.AppendNull();
				continue;
			}
			QByteArray t_distinctFields = distinctFieldsModel->GetData(QByteArray(), distinctFieldsIndex).toByteArray();
			distinctFieldsList << t_distinctFields;
		}
		distinctFields = distinctFieldsList;

	}

	return true;
}


bool V1_0::imtbase::CComplexCollectionFilter::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (sortingInfo){
		QList<::imtgql::CGqlParamObject> sortingInfoDataObjectList;
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfo->size(); ++sortingInfoIndex){
			if (!sortingInfo->at(sortingInfoIndex)){
				sortingInfoDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newSortingInfoGqlObject;
			if (!sortingInfo->at(sortingInfoIndex)->WriteToGraphQlObject(newSortingInfoGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfoDataObjectList << newSortingInfoGqlObject;
		}
		gqlObject.InsertParam("sortingInfo", sortingInfoDataObjectList);
	}
	else if (sortingInfo.IsNull()){
		gqlObject.InsertParam("sortingInfo", QVariant());
	}

	if (fieldsFilter){
		::imtgql::CGqlParamObject fieldsFilterGqlObject;
		const bool isFieldsFilterAdded = fieldsFilter->WriteToGraphQlObject(fieldsFilterGqlObject);
		if (!isFieldsFilterAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("fieldsFilter", fieldsFilterGqlObject);
	}

	if (timeFilter){
		::imtgql::CGqlParamObject timeFilterGqlObject;
		const bool isTimeFilterAdded = timeFilter->WriteToGraphQlObject(timeFilterGqlObject);
		if (!isTimeFilterAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("timeFilter", timeFilterGqlObject);
	}

	if (distinctFields){
		QVariantList distinctFieldsDataObjectList;
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFields->size(); ++distinctFieldsIndex){
			if (!distinctFields->at(distinctFieldsIndex)){
				distinctFieldsDataObjectList << QVariant();
				continue;
			}
			distinctFieldsDataObjectList << *distinctFields->at(distinctFieldsIndex);
		}
		gqlObject.InsertParam("distinctFields", distinctFieldsDataObjectList);
	}
	else if (distinctFields.IsNull()){
		gqlObject.InsertParam("distinctFields", QVariant());
	}

	gqlObject.InsertParam("__typename", QVariant("ComplexCollectionFilter"));

	return true;
}


bool V1_0::imtbase::CComplexCollectionFilter::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("sortingInfo") && gqlObject["sortingInfo"].isNull()){
		sortingInfo.SetNull();
	}
	if (gqlObject.ContainsParam("sortingInfo") && !(gqlObject["sortingInfo"].isNull())){
		const qsizetype sortingInfoElementsCount = gqlObject.GetObjectsCount("sortingInfo");
		sortingInfo = imtsdl::TElementList<V1_0::imtbase::CFieldSortingInfo>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoElementsCount; ++sortingInfoIndex){
			const ::imtgql::CGqlParamObject* sortingInfoDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("sortingInfo", sortingInfoIndex);
			if (sortingInfoDataObjectPtr == nullptr || sortingInfoDataObjectPtr->IsNull()){
				sortingInfo->AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldSortingInfo tempSortingInfo;
			if (!tempSortingInfo.ReadFromGraphQlObject(*sortingInfoDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (gqlObject.ContainsParam("fieldsFilter") && (gqlObject.GetParamArgumentObjectPtr("fieldsFilter") != nullptr)){
		fieldsFilter = V1_0::imtbase::CGroupFilter();
		const bool isFieldsFilterRead = fieldsFilter->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("fieldsFilter"));
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("timeFilter") && (gqlObject.GetParamArgumentObjectPtr("timeFilter") != nullptr)){
		timeFilter = V1_0::imtbase::CTimeFilter();
		const bool isTimeFilterRead = timeFilter->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("timeFilter"));
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("distinctFields") && gqlObject["distinctFields"].isNull()){
		distinctFields.SetNull();
	}
	if (gqlObject.ContainsParam("distinctFields") && !(gqlObject["distinctFields"].isNull())){
		const QVariant distinctFieldsData = gqlObject["distinctFields"];
		const QVariantList distinctFieldsDataList = distinctFieldsData.toList();
		const qsizetype distinctFieldsElementsCount = distinctFieldsDataList.size();
		distinctFields = imtsdl::TElementList<QByteArray>();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsElementsCount; ++distinctFieldsIndex){
			if (!distinctFieldsDataList[distinctFieldsIndex].isValid() || distinctFieldsDataList[distinctFieldsIndex].isNull()){
				distinctFields->AppendNull();
				continue;
			}
			QByteArray tempDistinctFields = distinctFieldsDataList[distinctFieldsIndex].toByteArray();
			distinctFields->append(tempDistinctFields);
		}
	}

	return true;
}


bool V1_0::imtbase::CComplexCollectionFilter::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("sortingInfo") && gqlObject["sortingInfo"].isNull()){
		sortingInfo.SetNull();
	}
	if (gqlObject.ContainsParam("sortingInfo") && !(gqlObject["sortingInfo"].isNull())){
		const qsizetype sortingInfoElementsCount = gqlObject.GetObjectsCount("sortingInfo");
		sortingInfo = imtsdl::TElementList<V1_0::imtbase::CFieldSortingInfo>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoElementsCount; ++sortingInfoIndex){
			const ::imtgql::CGqlParamObject* sortingInfoDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("sortingInfo", sortingInfoIndex);
			if (sortingInfoDataObjectPtr == nullptr || sortingInfoDataObjectPtr->IsNull()){
				sortingInfo->AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldSortingInfo tempSortingInfo;
			if (!tempSortingInfo.OptReadFromGraphQlObject(*sortingInfoDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (gqlObject.ContainsParam("fieldsFilter") && (gqlObject.GetParamArgumentObjectPtr("fieldsFilter") != nullptr)){
		fieldsFilter = V1_0::imtbase::CGroupFilter();
		const bool isFieldsFilterRead = fieldsFilter->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("fieldsFilter"));
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("timeFilter") && (gqlObject.GetParamArgumentObjectPtr("timeFilter") != nullptr)){
		timeFilter = V1_0::imtbase::CTimeFilter();
		const bool isTimeFilterRead = timeFilter->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("timeFilter"));
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("distinctFields") && gqlObject["distinctFields"].isNull()){
		distinctFields.SetNull();
	}
	if (gqlObject.ContainsParam("distinctFields") && !(gqlObject["distinctFields"].isNull())){
		const QVariant distinctFieldsData = gqlObject["distinctFields"];
		const QVariantList distinctFieldsDataList = distinctFieldsData.toList();
		const qsizetype distinctFieldsElementsCount = distinctFieldsDataList.size();
		distinctFields = imtsdl::TElementList<QByteArray>();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsElementsCount; ++distinctFieldsIndex){
			if (!distinctFieldsDataList[distinctFieldsIndex].isValid() || distinctFieldsDataList[distinctFieldsIndex].isNull()){
				distinctFields->AppendNull();
				continue;
			}
			QByteArray tempDistinctFields = distinctFieldsDataList[distinctFieldsIndex].toByteArray();
			distinctFields->append(tempDistinctFields);
		}
	}

	return true;
}


bool V1_0::imtbase::CComplexCollectionFilter::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (sortingInfo){
		QJsonArray newSortingInfoArray;
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfo->size(); ++sortingInfoIndex){
			if (!sortingInfo->at(sortingInfoIndex)){
				newSortingInfoArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newSortingInfoJsonObject;
			if (!sortingInfo->at(sortingInfoIndex)->WriteToJsonObject(newSortingInfoJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			newSortingInfoArray << newSortingInfoJsonObject;
		}
		jsonObject["sortingInfo"] = newSortingInfoArray;
	}
	else if (sortingInfo.IsNull()){
		jsonObject["sortingInfo"] = QJsonValue(QJsonValue::Null);
	}

	if (fieldsFilter){
		QJsonObject fieldsFilterJsonObject;
		const bool isFieldsFilterAdded = fieldsFilter->WriteToJsonObject(fieldsFilterJsonObject);
		if (!isFieldsFilterAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["fieldsFilter"] = fieldsFilterJsonObject;
	}

	if (timeFilter){
		QJsonObject timeFilterJsonObject;
		const bool isTimeFilterAdded = timeFilter->WriteToJsonObject(timeFilterJsonObject);
		if (!isTimeFilterAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["timeFilter"] = timeFilterJsonObject;
	}

	if (distinctFields){
		QJsonArray newDistinctFieldsArray;
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFields->size(); ++distinctFieldsIndex){
			if (!distinctFields->at(distinctFieldsIndex)){
				newDistinctFieldsArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			newDistinctFieldsArray << QString(*distinctFields->at(distinctFieldsIndex));
		}
		jsonObject["distinctFields"] = newDistinctFieldsArray;
	}
	else if (distinctFields.IsNull()){
		jsonObject["distinctFields"] = QJsonValue(QJsonValue::Null);
	}

	jsonObject["__typename"] = "ComplexCollectionFilter";

	return true;
}


bool V1_0::imtbase::CComplexCollectionFilter::ReadFromJsonObject(const QJsonObject& jsonObject)
{
if (jsonObject.contains("sortingInfo") && jsonObject["sortingInfo"].isNull()){
		sortingInfo.SetNull();
	}
		if (jsonObject.contains("sortingInfo") && jsonObject["sortingInfo"].isArray()){
		const QJsonArray sortingInfoJsonArray = jsonObject["sortingInfo"].toArray();
		const qsizetype sortingInfoArrayCount = sortingInfoJsonArray.size();
		sortingInfo = imtsdl::TElementList<V1_0::imtbase::CFieldSortingInfo>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoArrayCount; ++sortingInfoIndex){
			if (sortingInfoJsonArray[sortingInfoIndex].isNull()){
				sortingInfo->AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldSortingInfo tempSortingInfo;
			if (!tempSortingInfo.ReadFromJsonObject(sortingInfoJsonArray[sortingInfoIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (jsonObject.contains("fieldsFilter") && jsonObject["fieldsFilter"].isObject()){
		fieldsFilter = V1_0::imtbase::CGroupFilter();
		const bool isFieldsFilterRead = fieldsFilter->ReadFromJsonObject(jsonObject["fieldsFilter"].toObject());
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("timeFilter") && jsonObject["timeFilter"].isObject()){
		timeFilter = V1_0::imtbase::CTimeFilter();
		const bool isTimeFilterRead = timeFilter->ReadFromJsonObject(jsonObject["timeFilter"].toObject());
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

if (jsonObject.contains("distinctFields") && jsonObject["distinctFields"].isNull()){
		distinctFields.SetNull();
	}
		if (jsonObject.contains("distinctFields") && jsonObject["distinctFields"].isArray()){
		const QJsonArray distinctFieldsJsonArray = jsonObject["distinctFields"].toArray();
		const qsizetype distinctFieldsArrayCount = distinctFieldsJsonArray.size();
		distinctFields = imtsdl::TElementList<QByteArray>();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsArrayCount; ++distinctFieldsIndex){
			if (distinctFieldsJsonArray[distinctFieldsIndex].isNull()){
				distinctFields->AppendNull();
				continue;
			}
			QByteArray tempDistinctFields = distinctFieldsJsonArray[distinctFieldsIndex].toString().toUtf8();
			distinctFields->append(tempDistinctFields);
		}
	}

	return true;
}


bool V1_0::imtbase::CComplexCollectionFilter::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
if (jsonObject.contains("sortingInfo") && jsonObject["sortingInfo"].isNull()){
		sortingInfo.SetNull();
	}
		if (jsonObject.contains("sortingInfo") && jsonObject["sortingInfo"].isArray()){
		const QJsonArray sortingInfoJsonArray = jsonObject["sortingInfo"].toArray();
		const qsizetype sortingInfoArrayCount = sortingInfoJsonArray.size();
		sortingInfo = imtsdl::TElementList<V1_0::imtbase::CFieldSortingInfo>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoArrayCount; ++sortingInfoIndex){
			if (sortingInfoJsonArray[sortingInfoIndex].isNull()){
				sortingInfo->AppendNull();
				continue;
			}
			V1_0::imtbase::CFieldSortingInfo tempSortingInfo;
			if (!tempSortingInfo.OptReadFromJsonObject(sortingInfoJsonArray[sortingInfoIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (jsonObject.contains("fieldsFilter") && jsonObject["fieldsFilter"].isObject()){
		fieldsFilter = V1_0::imtbase::CGroupFilter();
		const bool isFieldsFilterRead = fieldsFilter->OptReadFromJsonObject(jsonObject["fieldsFilter"].toObject());
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("timeFilter") && jsonObject["timeFilter"].isObject()){
		timeFilter = V1_0::imtbase::CTimeFilter();
		const bool isTimeFilterRead = timeFilter->OptReadFromJsonObject(jsonObject["timeFilter"].toObject());
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

if (jsonObject.contains("distinctFields") && jsonObject["distinctFields"].isNull()){
		distinctFields.SetNull();
	}
		if (jsonObject.contains("distinctFields") && jsonObject["distinctFields"].isArray()){
		const QJsonArray distinctFieldsJsonArray = jsonObject["distinctFields"].toArray();
		const qsizetype distinctFieldsArrayCount = distinctFieldsJsonArray.size();
		distinctFields = imtsdl::TElementList<QByteArray>();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsArrayCount; ++distinctFieldsIndex){
			if (distinctFieldsJsonArray[distinctFieldsIndex].isNull()){
				distinctFields->AppendNull();
				continue;
			}
			QByteArray tempDistinctFields = distinctFieldsJsonArray[distinctFieldsIndex].toString().toUtf8();
			distinctFields->append(tempDistinctFields);
		}
	}

	return true;
}




} // namespace sdl::V1_0::imtbase
