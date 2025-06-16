#include "ComplexCollectionFilter.h"


#include "ComplexCollectionFilter.h"


#include "ComplexCollectionFilter.h"


/// \file CTimeFilter.cpp

namespace sdl::imtbase::ComplexCollectionFilter
{


QByteArray CTimeFilter::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CTimeFilter::V1_0::operator==(const V1_0& other) const
{
	return 
				timeRange == other.timeRange &&
				timeUnit == other.timeUnit &&
				interpretationMode == other.interpretationMode &&
				unitMultiplier == other.unitMultiplier;
}


bool CTimeFilter::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{

	if (timeRange){
		::imtbase::CTreeItemModel* timeRangeNewModelPtr = model.AddTreeModel("timeRange", modelIndex);
		const bool isTimeRangeAdded = timeRange->WriteToModel(*timeRangeNewModelPtr, 0);
		if (!isTimeRangeAdded){
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


bool CTimeFilter::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* timeRangeDataModelPtr = model.GetTreeItemModel("timeRange", modelIndex);
	if (timeRangeDataModelPtr != nullptr){
		timeRange = ImtBaseTypes::CTimeRange::V1_0();
		const bool isTimeRangeReaded = timeRange->ReadFromModel(*timeRangeDataModelPtr, modelIndex);
		if (!isTimeRangeReaded){
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


bool CTimeFilter::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* timeRangeDataModelPtr = model.GetTreeItemModel("timeRange", modelIndex);
	if (timeRangeDataModelPtr != nullptr){
		timeRange = ImtBaseTypes::CTimeRange::V1_0();
		const bool isTimeRangeReaded = timeRange->ReadFromModel(*timeRangeDataModelPtr, modelIndex);
		if (!isTimeRangeReaded){
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


bool CTimeFilter::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& request) const
{
	if (timeRange){
		::imtgql::CGqlParamObject timeRangeDataObject;
		if (!timeRange->WriteToGraphQlObject(timeRangeDataObject)){
			return false;
		}
		request.InsertParam("timeRange", timeRangeDataObject);
	}

	if (timeUnit){
		request.InsertParam("timeUnit", QVariant(*timeUnit));
	}

	if (interpretationMode){
		request.InsertParam("interpretationMode", QVariant(*interpretationMode));
	}

	if (unitMultiplier){
		request.InsertParam("unitMultiplier", QVariant(*unitMultiplier));
	}

	return true;
}

bool CTimeFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& request)
{
	const ::imtgql::CGqlParamObject* timeRangeDataObjectPtr = request.GetParamArgumentObjectPtr("timeRange");
	if (timeRangeDataObjectPtr != nullptr){
		timeRange = ImtBaseTypes::CTimeRange::V1_0();
		const bool isTimeRangeRead = timeRange->ReadFromGraphQlObject(*timeRangeDataObjectPtr);
		if (!isTimeRangeRead){
			return false;
		}

	}

	QVariant timeUnitData = request.GetParamArgumentValue("timeUnit");
	if (!timeUnitData.isNull()){
		timeUnit = timeUnitData.toString();
	}

	QVariant interpretationModeData = request.GetParamArgumentValue("interpretationMode");
	if (!interpretationModeData.isNull()){
		interpretationMode = interpretationModeData.toString();
	}

	QVariant unitMultiplierData = request.GetParamArgumentValue("unitMultiplier");
	if (!unitMultiplierData.isNull()){
		unitMultiplier = unitMultiplierData.toInt();
	}

	return true;
}


bool CTimeFilter::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& request)
{
	const ::imtgql::CGqlParamObject* timeRangeDataObjectPtr = request.GetParamArgumentObjectPtr("timeRange");
	if (timeRangeDataObjectPtr != nullptr){
		timeRange = ImtBaseTypes::CTimeRange::V1_0();
		const bool isTimeRangeRead = timeRange->OptReadFromGraphQlObject(*timeRangeDataObjectPtr);
		if (!isTimeRangeRead){
			return false;
		}

	}

	QVariant timeUnitData = request.GetParamArgumentValue("timeUnit");
	if (!timeUnitData.isNull()){
		timeUnit = timeUnitData.toString();
	}

	QVariant interpretationModeData = request.GetParamArgumentValue("interpretationMode");
	if (!interpretationModeData.isNull()){
		interpretationMode = interpretationModeData.toString();
	}

	QVariant unitMultiplierData = request.GetParamArgumentValue("unitMultiplier");
	if (!unitMultiplierData.isNull()){
		unitMultiplier = unitMultiplierData.toInt();
	}

	return true;
}


bool CTimeFilter::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (timeRange){
		QJsonObject timeRangejsonObject;
		const bool isTimeRangeAdded = timeRange->WriteToJsonObject(timeRangejsonObject);
		if (!isTimeRangeAdded){
			return false;
		}
		jsonObject["timeRange"] = timeRangejsonObject;
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


	return true;
}


bool CTimeFilter::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("timeRange") && jsonObject["timeRange"].isObject()){
		timeRange = ImtBaseTypes::CTimeRange::V1_0();
		const bool isTimeRangeReaded = timeRange->ReadFromJsonObject(jsonObject["timeRange"].toObject());
		if (!isTimeRangeReaded){
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


bool CTimeFilter::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("timeRange") && jsonObject["timeRange"].isObject()){
		timeRange = ImtBaseTypes::CTimeRange::V1_0();
		const bool isTimeRangeReaded = timeRange->OptReadFromJsonObject(jsonObject["timeRange"].toObject());
		if (!isTimeRangeReaded){
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


// serialize methods

bool CTimeFilter::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CTimeFilter::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CTimeFilter::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CTimeFilter::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CTimeFilter::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CTimeFilter::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CTimeFilter::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CTimeFilter::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CTimeFilter::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


} // namespace sdl::imtbase::ComplexCollectionFilter




/// \file CFieldSortingInfo.cpp

namespace sdl::imtbase::ComplexCollectionFilter
{


QByteArray CFieldSortingInfo::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CFieldSortingInfo::V1_0::operator==(const V1_0& other) const
{
	return 
				fieldId == other.fieldId &&
				sortingOrder == other.sortingOrder;
}


bool CFieldSortingInfo::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!fieldId){
		return false;
	}
	model.SetData("fieldId", *fieldId, modelIndex);

	if (!sortingOrder){
		return false;
	}
	model.SetData("sortingOrder", *sortingOrder, modelIndex);


	return true;
}


bool CFieldSortingInfo::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant fieldIdData = model.GetData("fieldId", modelIndex);
	if (fieldIdData.isNull()){
		return false;
	}
	fieldId = fieldIdData.toString();

	QVariant sortingOrderData = model.GetData("sortingOrder", modelIndex);
	if (sortingOrderData.isNull()){
		return false;
	}
	sortingOrder = sortingOrderData.toString();

	return true;
}


bool CFieldSortingInfo::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CFieldSortingInfo::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& request) const
{
	if (!fieldId){
		return false;
	}
	request.InsertParam("fieldId", QVariant(*fieldId));

	if (!sortingOrder){
		return false;
	}
	request.InsertParam("sortingOrder", QVariant(*sortingOrder));

	return true;
}

bool CFieldSortingInfo::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& request)
{
	QVariant fieldIdData = request.GetParamArgumentValue("fieldId");
	if (fieldIdData.isNull()){
		return false;
	}
	fieldId = fieldIdData.toString();

	QVariant sortingOrderData = request.GetParamArgumentValue("sortingOrder");
	if (sortingOrderData.isNull()){
		return false;
	}
	sortingOrder = sortingOrderData.toString();

	return true;
}


bool CFieldSortingInfo::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& request)
{
	QVariant fieldIdData = request.GetParamArgumentValue("fieldId");
	if (!fieldIdData.isNull()){
		fieldId = fieldIdData.toString();
	}

	QVariant sortingOrderData = request.GetParamArgumentValue("sortingOrder");
	if (!sortingOrderData.isNull()){
		sortingOrder = sortingOrderData.toString();
	}

	return true;
}


bool CFieldSortingInfo::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!fieldId){
		return false;
	}
	jsonObject["fieldId"] = QJsonValue::fromVariant(*fieldId);

	if (!sortingOrder){
		return false;
	}
	jsonObject["sortingOrder"] = QJsonValue::fromVariant(*sortingOrder);

	return true;
}


bool CFieldSortingInfo::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("fieldId") || ! jsonObject["fieldId"].isString()){
		return false;
	}
	fieldId = jsonObject["fieldId"].toString();

	if (!jsonObject.contains("sortingOrder") || ! jsonObject["sortingOrder"].isString()){
		return false;
	}
	sortingOrder = jsonObject["sortingOrder"].toString();

	return true;
}


bool CFieldSortingInfo::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("fieldId") && jsonObject["fieldId"].isString()){
		fieldId = jsonObject["fieldId"].toString();
	}

	if (jsonObject.contains("sortingOrder") && jsonObject["sortingOrder"].isString()){
		sortingOrder = jsonObject["sortingOrder"].toString();
	}

	return true;
}


// serialize methods

bool CFieldSortingInfo::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldSortingInfo::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldSortingInfo::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldSortingInfo::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldSortingInfo::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldSortingInfo::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldSortingInfo::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldSortingInfo::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldSortingInfo::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


} // namespace sdl::imtbase::ComplexCollectionFilter




/// \file CFieldFilter.cpp

namespace sdl::imtbase::ComplexCollectionFilter
{


QByteArray CFieldFilter::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CFieldFilter::V1_0::operator==(const V1_0& other) const
{
	return 
				fieldId == other.fieldId &&
				filterValue == other.filterValue &&
				filterValueType == other.filterValueType &&
				filterOperations == other.filterOperations;
}


bool CFieldFilter::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!fieldId){
		return false;
	}
	model.SetData("fieldId", *fieldId, modelIndex);

	if (!filterValue){
		return false;
	}
	model.SetData("filterValue", *filterValue, modelIndex);

	if (!filterValueType){
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

	::imtbase::CTreeItemModel* newFilterOperationsModelPtr = model.AddTreeModel("filterOperations", modelIndex);
	newFilterOperationsModelPtr->setIsArray(true);
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperations->size(); ++filterOperationsIndex){
		QString filterOperationsStringValue;
		switch (filterOperations->at(filterOperationsIndex)){
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


bool CFieldFilter::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant fieldIdData = model.GetData("fieldId", modelIndex);
	if (fieldIdData.isNull()){
		return false;
	}
	fieldId = fieldIdData.toByteArray();

	QVariant filterValueData = model.GetData("filterValue", modelIndex);
	if (filterValueData.isNull()){
		return false;
	}
	filterValue = filterValueData.toString();

	QVariant filterValueTypeData = model.GetData("filterValueType", modelIndex);
	if (filterValueTypeData.isNull()){
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
		return false;
	}

	::imtbase::CTreeItemModel* filterOperationsModel = model.GetTreeItemModel("filterOperations", modelIndex);
	if (filterOperationsModel == nullptr){
		return false;
	}
	int filterOperationsCount = filterOperationsModel->GetItemsCount();
	QList<FilterOperation> filterOperationsList;
	for (int filterOperationsIndex = 0; filterOperationsIndex < filterOperationsCount; ++filterOperationsIndex){
		FilterOperation filterOperationsData;
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
			return false;
		}

		filterOperationsList << filterOperationsData;
	}
	filterOperations = filterOperationsList;

	return true;
}


bool CFieldFilter::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
			return false;
		}
	}

	::imtbase::CTreeItemModel* filterOperationsModel = model.GetTreeItemModel("filterOperations", modelIndex);
	if (filterOperationsModel != nullptr){
		int filterOperationsCount = filterOperationsModel->GetItemsCount();
		QList<FilterOperation> filterOperationsList;
		for (int filterOperationsIndex = 0; filterOperationsIndex < filterOperationsCount; ++filterOperationsIndex){
			FilterOperation filterOperationsData;
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
				return false;
			}

			filterOperationsList << filterOperationsData;
		}
		filterOperations = filterOperationsList;

	}

	return true;
}


bool CFieldFilter::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& request) const
{
	if (!fieldId){
		return false;
	}
	request.InsertParam("fieldId", QVariant(*fieldId));

	if (!filterValue){
		return false;
	}
	request.InsertParam("filterValue", QVariant(*filterValue));

	if (!filterValueType){
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
	request.InsertParam("filterValueType", QVariant(filterValueTypeStringValue));

	if (!filterOperations){
		return false;
	}
	QVariantList filterOperationsTempList;
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperations->size(); ++filterOperationsIndex){
		QString filterOperationsStringValue;
		switch (filterOperations->at(filterOperationsIndex)){
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
		filterOperationsTempList << filterOperationsStringValue;
	}
	request.InsertParam("filterOperations", QVariant(filterOperationsTempList));

	return true;
}

bool CFieldFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& request)
{
	QVariant fieldIdData = request.GetParamArgumentValue("fieldId");
	if (fieldIdData.isNull()){
		return false;
	}
	fieldId = fieldIdData.toByteArray();

	QVariant filterValueData = request.GetParamArgumentValue("filterValue");
	if (filterValueData.isNull()){
		return false;
	}
	filterValue = filterValueData.toString();

	QVariant filterValueTypeData = request.GetParamArgumentValue("filterValueType");
	if (filterValueTypeData.isNull()){
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
		return false;
	}

	QVariant filterOperationsData = request.GetParamArgumentValue("filterOperations");
	if (filterOperationsData.isNull()){
		return false;
	}
	QList<FilterOperation> filterOperationsList;
	QVariantList filterOperationsDataList = filterOperationsData.toList();
	qsizetype filterOperationsCount = filterOperationsDataList.size();
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex != filterOperationsCount ; ++filterOperationsIndex){
		FilterOperation filterOperationsData;
		QString filterOperationsStringValue = filterOperationsDataList[filterOperationsIndex].toString();
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
			return false;
		}
		filterOperationsList << filterOperationsData;
	}
	filterOperations = filterOperationsList;


	return true;
}


bool CFieldFilter::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& request)
{
	QVariant fieldIdData = request.GetParamArgumentValue("fieldId");
	if (!fieldIdData.isNull()){
		fieldId = fieldIdData.toByteArray();
	}

	QVariant filterValueData = request.GetParamArgumentValue("filterValue");
	if (!filterValueData.isNull()){
		filterValue = filterValueData.toString();
	}

	QVariant filterValueTypeData = request.GetParamArgumentValue("filterValueType");
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
			return false;
		}
	}

	QVariant filterOperationsData = request.GetParamArgumentValue("filterOperations");
	if (!filterOperationsData.isNull()){
		QList<FilterOperation> filterOperationsList;
		QVariantList filterOperationsDataList = filterOperationsData.toList();
		qsizetype filterOperationsCount = filterOperationsDataList.size();
		for (qsizetype filterOperationsIndex = 0; filterOperationsIndex != filterOperationsCount ; ++filterOperationsIndex){
			FilterOperation filterOperationsData;
			QString filterOperationsStringValue = filterOperationsDataList[filterOperationsIndex].toString();
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
				return false;
			}
			filterOperationsList << filterOperationsData;
		}
		filterOperations = filterOperationsList;

	}

	return true;
}


bool CFieldFilter::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!fieldId){
		return false;
	}
	jsonObject["fieldId"] = QJsonValue::fromVariant(*fieldId);

	if (!filterValue){
		return false;
	}
	jsonObject["filterValue"] = QJsonValue::fromVariant(*filterValue);

	if (!filterValueType){
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

	QJsonArray newFilterOperationsArray;
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperations->size(); ++filterOperationsIndex){
		QString filterOperationsStringValue;
		switch (filterOperations->at(filterOperationsIndex)){
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

	return true;
}


bool CFieldFilter::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("fieldId") || ! jsonObject["fieldId"].isString()){
		return false;
	}
	fieldId = jsonObject["fieldId"].toString().toUtf8();

	if (!jsonObject.contains("filterValue") || ! jsonObject["filterValue"].isString()){
		return false;
	}
	filterValue = jsonObject["filterValue"].toString();

	if (!jsonObject.contains("filterValueType") || ! jsonObject["filterValueType"].isString()){
		return false;
	}
	QString filterValueTypeStringValue = jsonObject["filterValueType"].toString();
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
		return false;
	}

	if (!jsonObject.contains("filterOperations") || ! jsonObject["filterOperations"].isArray()){
		return false;
	}
	const QJsonArray filterOperationsjsonArray = jsonObject["filterOperations"].toArray();
	const qsizetype filterOperationsArrayCount = filterOperationsjsonArray.size();
	filterOperations = QList<imtbase::ComplexCollectionFilter::FilterOperation>();
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperationsArrayCount; ++filterOperationsIndex){
		QString tempFilterOperations = filterOperationsjsonArray[filterOperationsIndex].toString();
		imtbase::ComplexCollectionFilter::FilterOperation filterOperationsDataValue;
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
			return false;
		}
		filterOperations->append(filterOperationsDataValue);
	}

	return true;
}


bool CFieldFilter::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("fieldId") && jsonObject["fieldId"].isString()){
		fieldId = jsonObject["fieldId"].toString().toUtf8();
	}

	if (jsonObject.contains("filterValue") && jsonObject["filterValue"].isString()){
		filterValue = jsonObject["filterValue"].toString();
	}

	if (jsonObject.contains("filterValueType") && jsonObject["filterValueType"].isString()){
		QString filterValueTypeStringValue = jsonObject["filterValueType"].toString();
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
			return false;
		}
	}

	if (jsonObject.contains("filterOperations") && jsonObject["filterOperations"].isArray()){
		const QJsonArray filterOperationsjsonArray = jsonObject["filterOperations"].toArray();
		const qsizetype filterOperationsArrayCount = filterOperationsjsonArray.size();
		filterOperations = QList<imtbase::ComplexCollectionFilter::FilterOperation>();
		for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperationsArrayCount; ++filterOperationsIndex){
			QString tempFilterOperations = filterOperationsjsonArray[filterOperationsIndex].toString();
			imtbase::ComplexCollectionFilter::FilterOperation filterOperationsDataValue;
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
				return false;
			}
			filterOperations->append(filterOperationsDataValue);
		}
	}

	return true;
}


// serialize methods

bool CFieldFilter::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldFilter::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldFilter::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldFilter::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldFilter::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldFilter::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldFilter::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldFilter::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFieldFilter::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


} // namespace sdl::imtbase::ComplexCollectionFilter




/// \file CGroupFilter.cpp

namespace sdl::imtbase::ComplexCollectionFilter
{


QByteArray CGroupFilter::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CGroupFilter::V1_0::operator==(const V1_0& other) const
{
	return 
				fieldFilters == other.fieldFilters &&
				groupFilters == other.groupFilters &&
				logicalOperation == other.logicalOperation;
}


bool CGroupFilter::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (fieldFilters){
		::imtbase::CTreeItemModel* newFieldFiltersModelPtr = model.AddTreeModel("fieldFilters", modelIndex);
		newFieldFiltersModelPtr->setIsArray(true);
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFilters->size(); ++fieldFiltersIndex){
			newFieldFiltersModelPtr->InsertNewItem();
			if (!(fieldFilters->at(fieldFiltersIndex).WriteToModel(*newFieldFiltersModelPtr, fieldFiltersIndex))){
				return false;
			}
		}
	}
	if (groupFilters){
		::imtbase::CTreeItemModel* newGroupFiltersModelPtr = model.AddTreeModel("groupFilters", modelIndex);
		newGroupFiltersModelPtr->setIsArray(true);
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFilters->size(); ++groupFiltersIndex){
			newGroupFiltersModelPtr->InsertNewItem();
			if (!(groupFilters->at(groupFiltersIndex).WriteToModel(*newGroupFiltersModelPtr, groupFiltersIndex))){
				return false;
			}
		}
	}
	if (!logicalOperation){
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


bool CGroupFilter::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* fieldFiltersModel = model.GetTreeItemModel("fieldFilters", modelIndex);
	if (fieldFiltersModel != nullptr){
		int fieldFiltersCount = fieldFiltersModel->GetItemsCount();
		QList<CFieldFilter::V1_0> fieldFiltersList;
		for (int fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersCount; ++fieldFiltersIndex){
			CFieldFilter::V1_0 fieldFilters;
			if (!fieldFilters.ReadFromModel(*fieldFiltersModel, fieldFiltersIndex)){
				return false;
			}
			fieldFiltersList << fieldFilters;
		}
		fieldFilters = fieldFiltersList;

	}

	::imtbase::CTreeItemModel* groupFiltersModel = model.GetTreeItemModel("groupFilters", modelIndex);
	if (groupFiltersModel != nullptr){
		int groupFiltersCount = groupFiltersModel->GetItemsCount();
		QList<CGroupFilter::V1_0> groupFiltersList;
		for (int groupFiltersIndex = 0; groupFiltersIndex < groupFiltersCount; ++groupFiltersIndex){
			CGroupFilter::V1_0 groupFilters;
			if (!groupFilters.ReadFromModel(*groupFiltersModel, groupFiltersIndex)){
				return false;
			}
			groupFiltersList << groupFilters;
		}
		groupFilters = groupFiltersList;

	}

	QVariant logicalOperationData = model.GetData("logicalOperation", modelIndex);
	if (logicalOperationData.isNull()){
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
		return false;
	}

	return true;
}


bool CGroupFilter::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* fieldFiltersModel = model.GetTreeItemModel("fieldFilters", modelIndex);
	if (fieldFiltersModel != nullptr){
		int fieldFiltersCount = fieldFiltersModel->GetItemsCount();
		QList<CFieldFilter::V1_0> fieldFiltersList;
		for (int fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersCount; ++fieldFiltersIndex){
			CFieldFilter::V1_0 fieldFilters;
			if (!fieldFilters.OptReadFromModel(*fieldFiltersModel, fieldFiltersIndex)){
				return false;
			}
			fieldFiltersList << fieldFilters;
		}
		fieldFilters = fieldFiltersList;

	}

	::imtbase::CTreeItemModel* groupFiltersModel = model.GetTreeItemModel("groupFilters", modelIndex);
	if (groupFiltersModel != nullptr){
		int groupFiltersCount = groupFiltersModel->GetItemsCount();
		QList<CGroupFilter::V1_0> groupFiltersList;
		for (int groupFiltersIndex = 0; groupFiltersIndex < groupFiltersCount; ++groupFiltersIndex){
			CGroupFilter::V1_0 groupFilters;
			if (!groupFilters.OptReadFromModel(*groupFiltersModel, groupFiltersIndex)){
				return false;
			}
			groupFiltersList << groupFilters;
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
			return false;
		}
	}

	return true;
}


bool CGroupFilter::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& request) const
{
	if (fieldFilters){
		QList<::imtgql::CGqlParamObject> fieldFiltersDataObjectList;
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFilters->size(); ++fieldFiltersIndex){
			::imtgql::CGqlParamObject fieldFiltersDataObject;
			if (!fieldFilters->at(fieldFiltersIndex).WriteToGraphQlObject(fieldFiltersDataObject)){
				return false;
			}
			fieldFiltersDataObjectList << fieldFiltersDataObject;
		}
		request.InsertParam("fieldFilters", fieldFiltersDataObjectList);
	}

	if (groupFilters){
		QList<::imtgql::CGqlParamObject> groupFiltersDataObjectList;
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFilters->size(); ++groupFiltersIndex){
			::imtgql::CGqlParamObject groupFiltersDataObject;
			if (!groupFilters->at(groupFiltersIndex).WriteToGraphQlObject(groupFiltersDataObject)){
				return false;
			}
			groupFiltersDataObjectList << groupFiltersDataObject;
		}
		request.InsertParam("groupFilters", groupFiltersDataObjectList);
	}

	if (!logicalOperation){
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
	request.InsertParam("logicalOperation", QVariant(logicalOperationStringValue));

	return true;
}

bool CGroupFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& request)
{
	int fieldFiltersCount = request.GetObjectsCount("fieldFilters");
	if (fieldFiltersCount > 0){
		QList<CFieldFilter::V1_0> fieldFiltersList;
		for (int fieldFiltersIndex = 0; fieldFiltersIndex != fieldFiltersCount ; ++fieldFiltersIndex){
			const ::imtgql::CGqlParamObject* fieldFiltersDataObjectPtr = request.GetParamArgumentObjectPtr("fieldFilters",fieldFiltersIndex);
			if (fieldFiltersDataObjectPtr == nullptr){
				return false;
			}
			CFieldFilter::V1_0 fieldFilters;
			if (!fieldFilters.ReadFromGraphQlObject(*fieldFiltersDataObjectPtr)){
				return false;
			}
			fieldFiltersList << fieldFilters;
		}
		fieldFilters = fieldFiltersList;

	}

	int groupFiltersCount = request.GetObjectsCount("groupFilters");
	if (groupFiltersCount > 0){
		QList<CGroupFilter::V1_0> groupFiltersList;
		for (int groupFiltersIndex = 0; groupFiltersIndex != groupFiltersCount ; ++groupFiltersIndex){
			const ::imtgql::CGqlParamObject* groupFiltersDataObjectPtr = request.GetParamArgumentObjectPtr("groupFilters",groupFiltersIndex);
			if (groupFiltersDataObjectPtr == nullptr){
				return false;
			}
			CGroupFilter::V1_0 groupFilters;
			if (!groupFilters.ReadFromGraphQlObject(*groupFiltersDataObjectPtr)){
				return false;
			}
			groupFiltersList << groupFilters;
		}
		groupFilters = groupFiltersList;

	}

	QVariant logicalOperationData = request.GetParamArgumentValue("logicalOperation");
	if (logicalOperationData.isNull()){
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
		return false;
	}

	return true;
}


bool CGroupFilter::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& request)
{
	int fieldFiltersCount = request.GetObjectsCount("fieldFilters");
	if (fieldFiltersCount > 0){
		QList<CFieldFilter::V1_0> fieldFiltersList;
		for (int fieldFiltersIndex = 0; fieldFiltersIndex != fieldFiltersCount ; ++fieldFiltersIndex){
			const ::imtgql::CGqlParamObject* fieldFiltersDataObjectPtr = request.GetParamArgumentObjectPtr("fieldFilters",fieldFiltersIndex);
			if (fieldFiltersDataObjectPtr == nullptr){
				return false;
			}
			CFieldFilter::V1_0 fieldFilters;
			if (!fieldFilters.OptReadFromGraphQlObject(*fieldFiltersDataObjectPtr)){
				return false;
			}
			fieldFiltersList << fieldFilters;
		}
		fieldFilters = fieldFiltersList;

	}

	int groupFiltersCount = request.GetObjectsCount("groupFilters");
	if (groupFiltersCount > 0){
		QList<CGroupFilter::V1_0> groupFiltersList;
		for (int groupFiltersIndex = 0; groupFiltersIndex != groupFiltersCount ; ++groupFiltersIndex){
			const ::imtgql::CGqlParamObject* groupFiltersDataObjectPtr = request.GetParamArgumentObjectPtr("groupFilters",groupFiltersIndex);
			if (groupFiltersDataObjectPtr == nullptr){
				return false;
			}
			CGroupFilter::V1_0 groupFilters;
			if (!groupFilters.OptReadFromGraphQlObject(*groupFiltersDataObjectPtr)){
				return false;
			}
			groupFiltersList << groupFilters;
		}
		groupFilters = groupFiltersList;

	}

	QVariant logicalOperationData = request.GetParamArgumentValue("logicalOperation");
	if (!logicalOperationData.isNull()){
QString logicalOperationStringValue = logicalOperationData.toString();
		if(logicalOperationStringValue == "And"){
			logicalOperation = LogicalOperation::And;
		}
		else if(logicalOperationStringValue == "Or"){
			logicalOperation = LogicalOperation::Or;
		}
		else {
			return false;
		}
	}

	return true;
}


bool CGroupFilter::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (fieldFilters){
		QJsonArray newFieldFiltersArray;
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFilters->size(); ++fieldFiltersIndex){
			QJsonObject newFieldFiltersJsonObject;
			if (!fieldFilters->at(fieldFiltersIndex).WriteToJsonObject(newFieldFiltersJsonObject)){
				return false;
			}
			newFieldFiltersArray << newFieldFiltersJsonObject;
		}
		jsonObject["fieldFilters"] = newFieldFiltersArray;
	}

	if (groupFilters){
		QJsonArray newGroupFiltersArray;
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFilters->size(); ++groupFiltersIndex){
			QJsonObject newGroupFiltersJsonObject;
			if (!groupFilters->at(groupFiltersIndex).WriteToJsonObject(newGroupFiltersJsonObject)){
				return false;
			}
			newGroupFiltersArray << newGroupFiltersJsonObject;
		}
		jsonObject["groupFilters"] = newGroupFiltersArray;
	}

	if (!logicalOperation){
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

	return true;
}


bool CGroupFilter::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("fieldFilters") && jsonObject["fieldFilters"].isArray()){
		const QJsonArray fieldFiltersjsonArray = jsonObject["fieldFilters"].toArray();
		const qsizetype fieldFiltersArrayCount = fieldFiltersjsonArray.size();
		fieldFilters = QList<CFieldFilter::V1_0>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersArrayCount; ++fieldFiltersIndex){
			CFieldFilter::V1_0 tempFieldFilters;
			if (!tempFieldFilters.ReadFromJsonObject(fieldFiltersjsonArray[fieldFiltersIndex].toObject())){
				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (jsonObject.contains("groupFilters") && jsonObject["groupFilters"].isArray()){
		const QJsonArray groupFiltersjsonArray = jsonObject["groupFilters"].toArray();
		const qsizetype groupFiltersArrayCount = groupFiltersjsonArray.size();
		groupFilters = QList<CGroupFilter::V1_0>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersArrayCount; ++groupFiltersIndex){
			CGroupFilter::V1_0 tempGroupFilters;
			if (!tempGroupFilters.ReadFromJsonObject(groupFiltersjsonArray[groupFiltersIndex].toObject())){
				return false;
			}
			groupFilters->append(tempGroupFilters);
		}
	}

	if (!jsonObject.contains("logicalOperation") || ! jsonObject["logicalOperation"].isString()){
		return false;
	}
	QString logicalOperationStringValue = jsonObject["logicalOperation"].toString();
	if(logicalOperationStringValue == "And"){
		logicalOperation = LogicalOperation::And;
	}
	else if(logicalOperationStringValue == "Or"){
		logicalOperation = LogicalOperation::Or;
	}
	else {
		return false;
	}

	return true;
}


bool CGroupFilter::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("fieldFilters") && jsonObject["fieldFilters"].isArray()){
		const QJsonArray fieldFiltersjsonArray = jsonObject["fieldFilters"].toArray();
		const qsizetype fieldFiltersArrayCount = fieldFiltersjsonArray.size();
		fieldFilters = QList<CFieldFilter::V1_0>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersArrayCount; ++fieldFiltersIndex){
			CFieldFilter::V1_0 tempFieldFilters;
			if (!tempFieldFilters.OptReadFromJsonObject(fieldFiltersjsonArray[fieldFiltersIndex].toObject())){
				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (jsonObject.contains("groupFilters") && jsonObject["groupFilters"].isArray()){
		const QJsonArray groupFiltersjsonArray = jsonObject["groupFilters"].toArray();
		const qsizetype groupFiltersArrayCount = groupFiltersjsonArray.size();
		groupFilters = QList<CGroupFilter::V1_0>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersArrayCount; ++groupFiltersIndex){
			CGroupFilter::V1_0 tempGroupFilters;
			if (!tempGroupFilters.OptReadFromJsonObject(groupFiltersjsonArray[groupFiltersIndex].toObject())){
				return false;
			}
			groupFilters->append(tempGroupFilters);
		}
	}

	if (jsonObject.contains("logicalOperation") && jsonObject["logicalOperation"].isString()){
		QString logicalOperationStringValue = jsonObject["logicalOperation"].toString();
		if(logicalOperationStringValue == "And"){
			logicalOperation = LogicalOperation::And;
		}
		else if(logicalOperationStringValue == "Or"){
			logicalOperation = LogicalOperation::Or;
		}
		else {
			return false;
		}
	}

	return true;
}


// serialize methods

bool CGroupFilter::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CGroupFilter::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CGroupFilter::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CGroupFilter::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CGroupFilter::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CGroupFilter::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CGroupFilter::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CGroupFilter::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CGroupFilter::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


} // namespace sdl::imtbase::ComplexCollectionFilter




/// \file CComplexCollectionFilter.cpp

namespace sdl::imtbase::ComplexCollectionFilter
{


QByteArray CComplexCollectionFilter::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CComplexCollectionFilter::V1_0::operator==(const V1_0& other) const
{
	return 
				sortingInfo == other.sortingInfo &&
				fieldsFilter == other.fieldsFilter &&
				timeFilter == other.timeFilter &&
				distinctFields == other.distinctFields;
}


bool CComplexCollectionFilter::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (sortingInfo){
		::imtbase::CTreeItemModel* newSortingInfoModelPtr = model.AddTreeModel("sortingInfo", modelIndex);
		newSortingInfoModelPtr->setIsArray(true);
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfo->size(); ++sortingInfoIndex){
			newSortingInfoModelPtr->InsertNewItem();
			if (!(sortingInfo->at(sortingInfoIndex).WriteToModel(*newSortingInfoModelPtr, sortingInfoIndex))){
				return false;
			}
		}
	}

	if (fieldsFilter){
		::imtbase::CTreeItemModel* fieldsFilterNewModelPtr = model.AddTreeModel("fieldsFilter", modelIndex);
		const bool isFieldsFilterAdded = fieldsFilter->WriteToModel(*fieldsFilterNewModelPtr, 0);
		if (!isFieldsFilterAdded){
			return false;
		}

	}

	if (timeFilter){
		::imtbase::CTreeItemModel* timeFilterNewModelPtr = model.AddTreeModel("timeFilter", modelIndex);
		const bool isTimeFilterAdded = timeFilter->WriteToModel(*timeFilterNewModelPtr, 0);
		if (!isTimeFilterAdded){
			return false;
		}

	}
	if (distinctFields){
		::imtbase::CTreeItemModel* newDistinctFieldsModelPtr = model.AddTreeModel("distinctFields", modelIndex);
		newDistinctFieldsModelPtr->setIsArray(true);
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFields->size(); ++distinctFieldsIndex){
			newDistinctFieldsModelPtr->InsertNewItem();
			newDistinctFieldsModelPtr->SetData(QByteArray(), distinctFields->at(distinctFieldsIndex), distinctFieldsIndex);
		}
	}


	return true;
}


bool CComplexCollectionFilter::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* sortingInfoModel = model.GetTreeItemModel("sortingInfo", modelIndex);
	if (sortingInfoModel != nullptr){
		int sortingInfoCount = sortingInfoModel->GetItemsCount();
		QList<CFieldSortingInfo::V1_0> sortingInfoList;
		for (int sortingInfoIndex = 0; sortingInfoIndex < sortingInfoCount; ++sortingInfoIndex){
			CFieldSortingInfo::V1_0 sortingInfo;
			if (!sortingInfo.ReadFromModel(*sortingInfoModel, sortingInfoIndex)){
				return false;
			}
			sortingInfoList << sortingInfo;
		}
		sortingInfo = sortingInfoList;

	}

	::imtbase::CTreeItemModel* fieldsFilterDataModelPtr = model.GetTreeItemModel("fieldsFilter", modelIndex);
	if (fieldsFilterDataModelPtr != nullptr){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterReaded = fieldsFilter->ReadFromModel(*fieldsFilterDataModelPtr, modelIndex);
		if (!isFieldsFilterReaded){
			return false;
		}
	}

	::imtbase::CTreeItemModel* timeFilterDataModelPtr = model.GetTreeItemModel("timeFilter", modelIndex);
	if (timeFilterDataModelPtr != nullptr){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterReaded = timeFilter->ReadFromModel(*timeFilterDataModelPtr, modelIndex);
		if (!isTimeFilterReaded){
			return false;
		}
	}

	::imtbase::CTreeItemModel* distinctFieldsModel = model.GetTreeItemModel("distinctFields", modelIndex);
	if (distinctFieldsModel != nullptr){
		int distinctFieldsCount = distinctFieldsModel->GetItemsCount();
		QList<QByteArray> distinctFieldsList;
		for (int distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsCount; ++distinctFieldsIndex){
			QByteArray distinctFields = distinctFieldsModel->GetData(QByteArray(), distinctFieldsIndex).toByteArray();
			distinctFieldsList << distinctFields;
		}
		distinctFields = distinctFieldsList;

	}

	return true;
}


bool CComplexCollectionFilter::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* sortingInfoModel = model.GetTreeItemModel("sortingInfo", modelIndex);
	if (sortingInfoModel != nullptr){
		int sortingInfoCount = sortingInfoModel->GetItemsCount();
		QList<CFieldSortingInfo::V1_0> sortingInfoList;
		for (int sortingInfoIndex = 0; sortingInfoIndex < sortingInfoCount; ++sortingInfoIndex){
			CFieldSortingInfo::V1_0 sortingInfo;
			if (!sortingInfo.OptReadFromModel(*sortingInfoModel, sortingInfoIndex)){
				return false;
			}
			sortingInfoList << sortingInfo;
		}
		sortingInfo = sortingInfoList;

	}

	::imtbase::CTreeItemModel* fieldsFilterDataModelPtr = model.GetTreeItemModel("fieldsFilter", modelIndex);
	if (fieldsFilterDataModelPtr != nullptr){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterReaded = fieldsFilter->ReadFromModel(*fieldsFilterDataModelPtr, modelIndex);
		if (!isFieldsFilterReaded){
			return false;
		}
	}

	::imtbase::CTreeItemModel* timeFilterDataModelPtr = model.GetTreeItemModel("timeFilter", modelIndex);
	if (timeFilterDataModelPtr != nullptr){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterReaded = timeFilter->ReadFromModel(*timeFilterDataModelPtr, modelIndex);
		if (!isTimeFilterReaded){
			return false;
		}
	}

	::imtbase::CTreeItemModel* distinctFieldsModel = model.GetTreeItemModel("distinctFields", modelIndex);
	if (distinctFieldsModel != nullptr){
		int distinctFieldsCount = distinctFieldsModel->GetItemsCount();
		QList<QByteArray> distinctFieldsList;
		for (int distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsCount; ++distinctFieldsIndex){
			QByteArray distinctFields = distinctFieldsModel->GetData(QByteArray(), distinctFieldsIndex).toByteArray();
			distinctFieldsList << distinctFields;
		}
		distinctFields = distinctFieldsList;

	}

	return true;
}


bool CComplexCollectionFilter::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& request) const
{
	if (sortingInfo){
		QList<::imtgql::CGqlParamObject> sortingInfoDataObjectList;
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfo->size(); ++sortingInfoIndex){
			::imtgql::CGqlParamObject sortingInfoDataObject;
			if (!sortingInfo->at(sortingInfoIndex).WriteToGraphQlObject(sortingInfoDataObject)){
				return false;
			}
			sortingInfoDataObjectList << sortingInfoDataObject;
		}
		request.InsertParam("sortingInfo", sortingInfoDataObjectList);
	}

	if (fieldsFilter){
		::imtgql::CGqlParamObject fieldsFilterDataObject;
		if (!fieldsFilter->WriteToGraphQlObject(fieldsFilterDataObject)){
			return false;
		}
		request.InsertParam("fieldsFilter", fieldsFilterDataObject);
	}

	if (timeFilter){
		::imtgql::CGqlParamObject timeFilterDataObject;
		if (!timeFilter->WriteToGraphQlObject(timeFilterDataObject)){
			return false;
		}
		request.InsertParam("timeFilter", timeFilterDataObject);
	}

	if (distinctFields){
		QVariantList distinctFieldsTempList;
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFields->size(); ++distinctFieldsIndex){
			distinctFieldsTempList << distinctFields->at(distinctFieldsIndex);
		}
		request.InsertParam("distinctFields", QVariant(distinctFieldsTempList));
	}

	return true;
}

bool CComplexCollectionFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& request)
{
	int sortingInfoCount = request.GetObjectsCount("sortingInfo");
	if (sortingInfoCount > 0){
		QList<CFieldSortingInfo::V1_0> sortingInfoList;
		for (int sortingInfoIndex = 0; sortingInfoIndex != sortingInfoCount ; ++sortingInfoIndex){
			const ::imtgql::CGqlParamObject* sortingInfoDataObjectPtr = request.GetParamArgumentObjectPtr("sortingInfo",sortingInfoIndex);
			if (sortingInfoDataObjectPtr == nullptr){
				return false;
			}
			CFieldSortingInfo::V1_0 sortingInfo;
			if (!sortingInfo.ReadFromGraphQlObject(*sortingInfoDataObjectPtr)){
				return false;
			}
			sortingInfoList << sortingInfo;
		}
		sortingInfo = sortingInfoList;

	}

	const ::imtgql::CGqlParamObject* fieldsFilterDataObjectPtr = request.GetParamArgumentObjectPtr("fieldsFilter");
	if (fieldsFilterDataObjectPtr != nullptr){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->ReadFromGraphQlObject(*fieldsFilterDataObjectPtr);
		if (!isFieldsFilterRead){
			return false;
		}

	}

	const ::imtgql::CGqlParamObject* timeFilterDataObjectPtr = request.GetParamArgumentObjectPtr("timeFilter");
	if (timeFilterDataObjectPtr != nullptr){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->ReadFromGraphQlObject(*timeFilterDataObjectPtr);
		if (!isTimeFilterRead){
			return false;
		}

	}

	QVariant distinctFieldsData = request.GetParamArgumentValue("distinctFields");
	if (!distinctFieldsData.isNull()){
		QList<QByteArray> distinctFieldsList;
		QVariantList distinctFieldsDataList = distinctFieldsData.toList();
		qsizetype distinctFieldsCount = distinctFieldsDataList.size();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex != distinctFieldsCount ; ++distinctFieldsIndex){
			QByteArray distinctFields = distinctFieldsDataList[distinctFieldsIndex].toByteArray();
			distinctFieldsList << distinctFields;
		}
		distinctFields = distinctFieldsList;

	}

	return true;
}


bool CComplexCollectionFilter::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& request)
{
	int sortingInfoCount = request.GetObjectsCount("sortingInfo");
	if (sortingInfoCount > 0){
		QList<CFieldSortingInfo::V1_0> sortingInfoList;
		for (int sortingInfoIndex = 0; sortingInfoIndex != sortingInfoCount ; ++sortingInfoIndex){
			const ::imtgql::CGqlParamObject* sortingInfoDataObjectPtr = request.GetParamArgumentObjectPtr("sortingInfo",sortingInfoIndex);
			if (sortingInfoDataObjectPtr == nullptr){
				return false;
			}
			CFieldSortingInfo::V1_0 sortingInfo;
			if (!sortingInfo.OptReadFromGraphQlObject(*sortingInfoDataObjectPtr)){
				return false;
			}
			sortingInfoList << sortingInfo;
		}
		sortingInfo = sortingInfoList;

	}

	const ::imtgql::CGqlParamObject* fieldsFilterDataObjectPtr = request.GetParamArgumentObjectPtr("fieldsFilter");
	if (fieldsFilterDataObjectPtr != nullptr){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->OptReadFromGraphQlObject(*fieldsFilterDataObjectPtr);
		if (!isFieldsFilterRead){
			return false;
		}

	}

	const ::imtgql::CGqlParamObject* timeFilterDataObjectPtr = request.GetParamArgumentObjectPtr("timeFilter");
	if (timeFilterDataObjectPtr != nullptr){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->OptReadFromGraphQlObject(*timeFilterDataObjectPtr);
		if (!isTimeFilterRead){
			return false;
		}

	}

	QVariant distinctFieldsData = request.GetParamArgumentValue("distinctFields");
	if (!distinctFieldsData.isNull()){
		QList<QByteArray> distinctFieldsList;
		QVariantList distinctFieldsDataList = distinctFieldsData.toList();
		qsizetype distinctFieldsCount = distinctFieldsDataList.size();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex != distinctFieldsCount ; ++distinctFieldsIndex){
			QByteArray distinctFields = distinctFieldsDataList[distinctFieldsIndex].toByteArray();
			distinctFieldsList << distinctFields;
		}
		distinctFields = distinctFieldsList;

	}

	return true;
}


bool CComplexCollectionFilter::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (sortingInfo){
		QJsonArray newSortingInfoArray;
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfo->size(); ++sortingInfoIndex){
			QJsonObject newSortingInfoJsonObject;
			if (!sortingInfo->at(sortingInfoIndex).WriteToJsonObject(newSortingInfoJsonObject)){
				return false;
			}
			newSortingInfoArray << newSortingInfoJsonObject;
		}
		jsonObject["sortingInfo"] = newSortingInfoArray;
	}

	if (fieldsFilter){
		QJsonObject fieldsFilterjsonObject;
		const bool isFieldsFilterAdded = fieldsFilter->WriteToJsonObject(fieldsFilterjsonObject);
		if (!isFieldsFilterAdded){
			return false;
		}
		jsonObject["fieldsFilter"] = fieldsFilterjsonObject;
	}

	if (timeFilter){
		QJsonObject timeFilterjsonObject;
		const bool isTimeFilterAdded = timeFilter->WriteToJsonObject(timeFilterjsonObject);
		if (!isTimeFilterAdded){
			return false;
		}
		jsonObject["timeFilter"] = timeFilterjsonObject;
	}

	if (distinctFields){
		QJsonArray newDistinctFieldsArray;
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFields->size(); ++distinctFieldsIndex){
			newDistinctFieldsArray << QString(distinctFields->at(distinctFieldsIndex));
		}
		jsonObject["distinctFields"] = newDistinctFieldsArray;
	}

	return true;
}


bool CComplexCollectionFilter::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("sortingInfo") && jsonObject["sortingInfo"].isArray()){
		const QJsonArray sortingInfojsonArray = jsonObject["sortingInfo"].toArray();
		const qsizetype sortingInfoArrayCount = sortingInfojsonArray.size();
		sortingInfo = QList<CFieldSortingInfo::V1_0>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoArrayCount; ++sortingInfoIndex){
			CFieldSortingInfo::V1_0 tempSortingInfo;
			if (!tempSortingInfo.ReadFromJsonObject(sortingInfojsonArray[sortingInfoIndex].toObject())){
				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (jsonObject.contains("fieldsFilter") && jsonObject["fieldsFilter"].isObject()){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterReaded = fieldsFilter->ReadFromJsonObject(jsonObject["fieldsFilter"].toObject());
		if (!isFieldsFilterReaded){
			return false;
		}
	}

	if (jsonObject.contains("timeFilter") && jsonObject["timeFilter"].isObject()){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterReaded = timeFilter->ReadFromJsonObject(jsonObject["timeFilter"].toObject());
		if (!isTimeFilterReaded){
			return false;
		}
	}

	if (jsonObject.contains("distinctFields") && jsonObject["distinctFields"].isArray()){
		const QJsonArray distinctFieldsjsonArray = jsonObject["distinctFields"].toArray();
		const qsizetype distinctFieldsArrayCount = distinctFieldsjsonArray.size();
		distinctFields = QList<QByteArray>();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsArrayCount; ++distinctFieldsIndex){
			QByteArray tempDistinctFields = distinctFieldsjsonArray[distinctFieldsIndex].toString().toUtf8();
			distinctFields->append(tempDistinctFields);
		}
	}

	return true;
}


bool CComplexCollectionFilter::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("sortingInfo") && jsonObject["sortingInfo"].isArray()){
		const QJsonArray sortingInfojsonArray = jsonObject["sortingInfo"].toArray();
		const qsizetype sortingInfoArrayCount = sortingInfojsonArray.size();
		sortingInfo = QList<CFieldSortingInfo::V1_0>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoArrayCount; ++sortingInfoIndex){
			CFieldSortingInfo::V1_0 tempSortingInfo;
			if (!tempSortingInfo.OptReadFromJsonObject(sortingInfojsonArray[sortingInfoIndex].toObject())){
				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (jsonObject.contains("fieldsFilter") && jsonObject["fieldsFilter"].isObject()){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterReaded = fieldsFilter->OptReadFromJsonObject(jsonObject["fieldsFilter"].toObject());
		if (!isFieldsFilterReaded){
			return false;
		}
	}

	if (jsonObject.contains("timeFilter") && jsonObject["timeFilter"].isObject()){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterReaded = timeFilter->OptReadFromJsonObject(jsonObject["timeFilter"].toObject());
		if (!isTimeFilterReaded){
			return false;
		}
	}

	if (jsonObject.contains("distinctFields") && jsonObject["distinctFields"].isArray()){
		const QJsonArray distinctFieldsjsonArray = jsonObject["distinctFields"].toArray();
		const qsizetype distinctFieldsArrayCount = distinctFieldsjsonArray.size();
		distinctFields = QList<QByteArray>();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsArrayCount; ++distinctFieldsIndex){
			QByteArray tempDistinctFields = distinctFieldsjsonArray[distinctFieldsIndex].toString().toUtf8();
			distinctFields->append(tempDistinctFields);
		}
	}

	return true;
}


// serialize methods

bool CComplexCollectionFilter::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CComplexCollectionFilter::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CComplexCollectionFilter::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CComplexCollectionFilter::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CComplexCollectionFilter::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CComplexCollectionFilter::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CComplexCollectionFilter::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CComplexCollectionFilter::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CComplexCollectionFilter::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


} // namespace sdl::imtbase::ComplexCollectionFilter




