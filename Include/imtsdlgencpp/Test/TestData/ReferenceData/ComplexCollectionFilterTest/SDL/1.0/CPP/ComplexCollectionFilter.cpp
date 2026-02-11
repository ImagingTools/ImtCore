// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ComplexCollectionFilter.h"


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
	model.SetData("__typename", "TimeFilter", modelIndex);


	if (timeRange){
		::imtbase::CTreeItemModel* timeRangeNewModelPtr = model.AddTreeModel("timeRange", modelIndex);
		const bool isTimeRangeAdded = timeRange->WriteToModel(*timeRangeNewModelPtr, 0);
		if (!isTimeRangeAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

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
		const bool isTimeRangeRead = timeRange->ReadFromModel(*timeRangeDataModelPtr, modelIndex);
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

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
		const bool isTimeRangeRead = timeRange->ReadFromModel(*timeRangeDataModelPtr, modelIndex);
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

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


bool CTimeFilter::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (timeRange){
		::imtgql::CGqlParamObject timeRangeGqlObject;
		const bool isTimeRangeAdded = timeRange->WriteToGraphQlObject(timeRangeGqlObject);
		if (!isTimeRangeAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

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


bool CTimeFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("timeRange") && (gqlObject.GetParamArgumentObjectPtr("timeRange") != nullptr)){
		timeRange = ImtBaseTypes::CTimeRange::V1_0();
		const bool isTimeRangeRead = timeRange->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("timeRange"));
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

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


bool CTimeFilter::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("timeRange") && (gqlObject.GetParamArgumentObjectPtr("timeRange") != nullptr)){
		timeRange = ImtBaseTypes::CTimeRange::V1_0();
		const bool isTimeRangeRead = timeRange->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("timeRange"));
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

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


bool CTimeFilter::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (timeRange){
		QJsonObject timeRangeJsonObject;
		const bool isTimeRangeAdded = timeRange->WriteToJsonObject(timeRangeJsonObject);
		if (!isTimeRangeAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

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


bool CTimeFilter::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("timeRange") && jsonObject["timeRange"].isObject()){
		timeRange = ImtBaseTypes::CTimeRange::V1_0();
		const bool isTimeRangeRead = timeRange->ReadFromJsonObject(jsonObject["timeRange"].toObject());
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

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
		const bool isTimeRangeRead = timeRange->OptReadFromJsonObject(jsonObject["timeRange"].toObject());
		if (!isTimeRangeRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeRange").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
	model.SetData("__typename", "FieldSortingInfo", modelIndex);

	if (!fieldId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("fieldId", *fieldId, modelIndex);

	if (!sortingOrder){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("sortingOrder", *sortingOrder, modelIndex);


	return true;
}


bool CFieldSortingInfo::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant fieldIdData = model.GetData("fieldId", modelIndex);
	if (fieldIdData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = fieldIdData.toString();

	QVariant sortingOrderData = model.GetData("sortingOrder", modelIndex);
	if (sortingOrderData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

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


bool CFieldSortingInfo::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!fieldId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("fieldId", QVariant(*fieldId));

	if (!sortingOrder){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("sortingOrder", QVariant(*sortingOrder));

	gqlObject.InsertParam("__typename", QVariant("FieldSortingInfo"));

	return true;
}


bool CFieldSortingInfo::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("fieldId") || (gqlObject["fieldId"].userType() != QMetaType::QString && gqlObject["fieldId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = gqlObject["fieldId"].toString();

	if (!gqlObject.ContainsParam("sortingOrder") || (gqlObject["sortingOrder"].userType() != QMetaType::QString && gqlObject["sortingOrder"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

		return false;
	}
	sortingOrder = gqlObject["sortingOrder"].toString();

	return true;
}


bool CFieldSortingInfo::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("fieldId") && (gqlObject["fieldId"].userType() == QMetaType::QString || gqlObject["fieldId"].userType() == QMetaType::QByteArray)){
		fieldId = gqlObject["fieldId"].toString();
	}

	if (gqlObject.ContainsParam("sortingOrder") && (gqlObject["sortingOrder"].userType() == QMetaType::QString || gqlObject["sortingOrder"].userType() == QMetaType::QByteArray)){
		sortingOrder = gqlObject["sortingOrder"].toString();
	}

	return true;
}


bool CFieldSortingInfo::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!fieldId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["fieldId"] = QJsonValue::fromVariant(*fieldId);

	if (!sortingOrder){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["sortingOrder"] = QJsonValue::fromVariant(*sortingOrder);

	jsonObject["__typename"] = "FieldSortingInfo";

	return true;
}


bool CFieldSortingInfo::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("fieldId") || ! jsonObject["fieldId"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = jsonObject["fieldId"].toString();

	if (!jsonObject.contains("sortingOrder") || ! jsonObject["sortingOrder"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "sortingOrder").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
	model.SetData("__typename", "FieldFilter", modelIndex);

	if (!fieldId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("fieldId", *fieldId, modelIndex);

	if (!filterValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("filterValue", *filterValue, modelIndex);

	if (!filterValueType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

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


bool CFieldFilter::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant fieldIdData = model.GetData("fieldId", modelIndex);
	if (fieldIdData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = fieldIdData.toByteArray();

	QVariant filterValueData = model.GetData("filterValue", modelIndex);
	if (filterValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	filterValue = filterValueData.toString();

	QVariant filterValueTypeData = model.GetData("filterValueType", modelIndex);
	if (filterValueTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* filterOperationsModel = model.GetTreeItemModel("filterOperations", modelIndex);
	if (filterOperationsModel == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required").arg(__FILE__, QString::number(__LINE__), "filterOperations").toLocal8Bit().constData();)

		return false;
	}
	int filterOperationsCount = filterOperationsModel->GetItemsCount();
	imtsdl::TElementList<FilterOperation> filterOperationsList;
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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", filterOperationsStringValue);)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* filterOperationsModel = model.GetTreeItemModel("filterOperations", modelIndex);
	if (filterOperationsModel != nullptr){
		int filterOperationsCount = filterOperationsModel->GetItemsCount();
		imtsdl::TElementList<FilterOperation> filterOperationsList;
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
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", filterOperationsStringValue);)

				return false;
			}

			filterOperationsList << filterOperationsData;
		}
		filterOperations = filterOperationsList;

	}

	return true;
}


bool CFieldFilter::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!fieldId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("fieldId", QVariant(*fieldId));

	if (!filterValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("filterValue", QVariant(*filterValue));

	if (!filterValueType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

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

	QVariantList filterOperationsDataObjectList;
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperations->size(); ++filterOperationsIndex){
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


bool CFieldFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("fieldId") || (gqlObject["fieldId"].userType() != QMetaType::QString && gqlObject["fieldId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = gqlObject["fieldId"].toByteArray();

	if (!gqlObject.ContainsParam("filterValue") || (gqlObject["filterValue"].userType() != QMetaType::QString && gqlObject["filterValue"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	filterValue = gqlObject["filterValue"].toString();

	if (!gqlObject.ContainsParam("filterValueType") || (gqlObject["filterValueType"].userType() != QMetaType::QString && gqlObject["filterValueType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("filterOperations") || (gqlObject["filterOperations"].isNull())){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterOperations").toLocal8Bit().constData();)

		return false;
	}
	const QVariant filterOperationsData = gqlObject["filterOperations"];
	const QVariantList filterOperationsDataList = filterOperationsData.toList();
	const qsizetype filterOperationsElementsCount = filterOperationsDataList.size();
	filterOperations = imtsdl::TElementList<imtbase::ComplexCollectionFilter::FilterOperation>();
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperationsElementsCount; ++filterOperationsIndex){
		const QString tempFilterOperations = filterOperationsDataList[filterOperationsIndex].toString();
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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", tempFilterOperations);)

			return false;
		}
		filterOperations->append(filterOperationsDataValue);
	}

	return true;
}


bool CFieldFilter::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("filterOperations")){
		filterOperations.emplace();
	}
	if (gqlObject.ContainsParam("filterOperations") && !(gqlObject["filterOperations"].isNull())){
		const QVariant filterOperationsData = gqlObject["filterOperations"];
		const QVariantList filterOperationsDataList = filterOperationsData.toList();
		const qsizetype filterOperationsElementsCount = filterOperationsDataList.size();
		filterOperations = imtsdl::TElementList<imtbase::ComplexCollectionFilter::FilterOperation>();
		for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperationsElementsCount; ++filterOperationsIndex){
			const QString tempFilterOperations = filterOperationsDataList[filterOperationsIndex].toString();
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
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", tempFilterOperations);)

				return false;
			}
			filterOperations->append(filterOperationsDataValue);
		}
	}

	return true;
}


bool CFieldFilter::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!fieldId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["fieldId"] = QJsonValue::fromVariant(*fieldId);

	if (!filterValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["filterValue"] = QJsonValue::fromVariant(*filterValue);

	if (!filterValueType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

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


bool CFieldFilter::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("fieldId") || ! jsonObject["fieldId"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "fieldId").toLocal8Bit().constData();)

		return false;
	}
	fieldId = jsonObject["fieldId"].toString().toUtf8();

	if (!jsonObject.contains("filterValue") || ! jsonObject["filterValue"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValue").toLocal8Bit().constData();)

		return false;
	}
	filterValue = jsonObject["filterValue"].toString();

	if (!jsonObject.contains("filterValueType") || ! jsonObject["filterValueType"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterValueType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

		return false;
	}

	if (!jsonObject.contains("filterOperations") || ! jsonObject["filterOperations"].isArray()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "filterOperations").toLocal8Bit().constData();)

		return false;
	}
	const QJsonArray filterOperationsJsonArray = jsonObject["filterOperations"].toArray();
	const qsizetype filterOperationsArrayCount = filterOperationsJsonArray.size();
	filterOperations = imtsdl::TElementList<imtbase::ComplexCollectionFilter::FilterOperation>();
	for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperationsArrayCount; ++filterOperationsIndex){
		const QString tempFilterOperations = filterOperationsJsonArray[filterOperationsIndex].toString();
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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", tempFilterOperations);)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("filterOperations") && jsonObject["filterOperations"].isArray()){
		const QJsonArray filterOperationsJsonArray = jsonObject["filterOperations"].toArray();
		const qsizetype filterOperationsArrayCount = filterOperationsJsonArray.size();
		filterOperations = imtsdl::TElementList<imtbase::ComplexCollectionFilter::FilterOperation>();
		for (qsizetype filterOperationsIndex = 0; filterOperationsIndex < filterOperationsArrayCount; ++filterOperationsIndex){
			const QString tempFilterOperations = filterOperationsJsonArray[filterOperationsIndex].toString();
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
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "FilterOperation", tempFilterOperations);)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
	model.SetData("__typename", "GroupFilter", modelIndex);

	if (fieldFilters){
		::imtbase::CTreeItemModel* newFieldFiltersModelPtr = model.AddTreeModel("fieldFilters", modelIndex);
		newFieldFiltersModelPtr->setIsArray(true);
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFilters->size(); ++fieldFiltersIndex){
			newFieldFiltersModelPtr->InsertNewItem();
			if (!(fieldFilters->at(fieldFiltersIndex)->WriteToModel(*newFieldFiltersModelPtr, fieldFiltersIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	if (groupFilters){
		::imtbase::CTreeItemModel* newGroupFiltersModelPtr = model.AddTreeModel("groupFilters", modelIndex);
		newGroupFiltersModelPtr->setIsArray(true);
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFilters->size(); ++groupFiltersIndex){
			newGroupFiltersModelPtr->InsertNewItem();
			if (!(groupFilters->at(groupFiltersIndex)->WriteToModel(*newGroupFiltersModelPtr, groupFiltersIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	if (!logicalOperation){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

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
		imtsdl::TElementList<CFieldFilter::V1_0> fieldFiltersList;
		for (int fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersCount; ++fieldFiltersIndex){
			CFieldFilter::V1_0 t_fieldFilters;
			if (!t_fieldFilters.ReadFromModel(*fieldFiltersModel, fieldFiltersIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFiltersList << t_fieldFilters;
		}
		fieldFilters = fieldFiltersList;

	}

	::imtbase::CTreeItemModel* groupFiltersModel = model.GetTreeItemModel("groupFilters", modelIndex);
	if (groupFiltersModel != nullptr){
		int groupFiltersCount = groupFiltersModel->GetItemsCount();
		imtsdl::TElementList<CGroupFilter::V1_0> groupFiltersList;
		for (int groupFiltersIndex = 0; groupFiltersIndex < groupFiltersCount; ++groupFiltersIndex){
			CGroupFilter::V1_0 t_groupFilters;
			if (!t_groupFilters.ReadFromModel(*groupFiltersModel, groupFiltersIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			groupFiltersList << t_groupFilters;
		}
		groupFilters = groupFiltersList;

	}

	QVariant logicalOperationData = model.GetData("logicalOperation", modelIndex);
	if (logicalOperationData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

		return false;
	}

	return true;
}


bool CGroupFilter::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* fieldFiltersModel = model.GetTreeItemModel("fieldFilters", modelIndex);
	if (fieldFiltersModel != nullptr){
		int fieldFiltersCount = fieldFiltersModel->GetItemsCount();
		imtsdl::TElementList<CFieldFilter::V1_0> fieldFiltersList;
		for (int fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersCount; ++fieldFiltersIndex){
			CFieldFilter::V1_0 t_fieldFilters;
			if (!t_fieldFilters.OptReadFromModel(*fieldFiltersModel, fieldFiltersIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFiltersList << t_fieldFilters;
		}
		fieldFilters = fieldFiltersList;

	}

	::imtbase::CTreeItemModel* groupFiltersModel = model.GetTreeItemModel("groupFilters", modelIndex);
	if (groupFiltersModel != nullptr){
		int groupFiltersCount = groupFiltersModel->GetItemsCount();
		imtsdl::TElementList<CGroupFilter::V1_0> groupFiltersList;
		for (int groupFiltersIndex = 0; groupFiltersIndex < groupFiltersCount; ++groupFiltersIndex){
			CGroupFilter::V1_0 t_groupFilters;
			if (!t_groupFilters.OptReadFromModel(*groupFiltersModel, groupFiltersIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

			return false;
		}
	}

	return true;
}


bool CGroupFilter::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (fieldFilters){
		QList<::imtgql::CGqlParamObject> fieldFiltersDataObjectList;
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFilters->size(); ++fieldFiltersIndex){
			::imtgql::CGqlParamObject newFieldFiltersGqlObject;
			if (!fieldFilters->at(fieldFiltersIndex)->WriteToGraphQlObject(newFieldFiltersGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFiltersDataObjectList << newFieldFiltersGqlObject;
		}
		gqlObject.InsertParam("fieldFilters", fieldFiltersDataObjectList);
	}

	if (groupFilters){
		QList<::imtgql::CGqlParamObject> groupFiltersDataObjectList;
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFilters->size(); ++groupFiltersIndex){
			::imtgql::CGqlParamObject newGroupFiltersGqlObject;
			if (!groupFilters->at(groupFiltersIndex)->WriteToGraphQlObject(newGroupFiltersGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			groupFiltersDataObjectList << newGroupFiltersGqlObject;
		}
		gqlObject.InsertParam("groupFilters", groupFiltersDataObjectList);
	}

	if (!logicalOperation){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

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


bool CGroupFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("fieldFilters")){
		fieldFilters.emplace();
	}
	if (gqlObject.ContainsParam("fieldFilters") && (gqlObject.GetObjectsCount("fieldFilters") > 0)){
		const qsizetype fieldFiltersElementsCount = gqlObject.GetObjectsCount("fieldFilters");
		fieldFilters = imtsdl::TElementList<CFieldFilter::V1_0>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersElementsCount; ++fieldFiltersIndex){
			const ::imtgql::CGqlParamObject* fieldFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("fieldFilters", fieldFiltersIndex);
			if (fieldFiltersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << fieldFiltersDataObjectPtr;
				return false;
			}
			CFieldFilter::V1_0 tempFieldFilters;
			if (!tempFieldFilters.ReadFromGraphQlObject(*fieldFiltersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (gqlObject.ContainsParam("groupFilters")){
		groupFilters.emplace();
	}
	if (gqlObject.ContainsParam("groupFilters") && (gqlObject.GetObjectsCount("groupFilters") > 0)){
		const qsizetype groupFiltersElementsCount = gqlObject.GetObjectsCount("groupFilters");
		groupFilters = imtsdl::TElementList<CGroupFilter::V1_0>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersElementsCount; ++groupFiltersIndex){
			const ::imtgql::CGqlParamObject* groupFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("groupFilters", groupFiltersIndex);
			if (groupFiltersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << groupFiltersDataObjectPtr;
				return false;
			}
			CGroupFilter::V1_0 tempGroupFilters;
			if (!tempGroupFilters.ReadFromGraphQlObject(*groupFiltersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			groupFilters->append(tempGroupFilters);
		}
	}

	if (!gqlObject.ContainsParam("logicalOperation") || (gqlObject["logicalOperation"].userType() != QMetaType::QString && gqlObject["logicalOperation"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

		return false;
	}

	return true;
}


bool CGroupFilter::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("fieldFilters")){
		fieldFilters.emplace();
	}
	if (gqlObject.ContainsParam("fieldFilters") && (gqlObject.GetObjectsCount("fieldFilters") > 0)){
		const qsizetype fieldFiltersElementsCount = gqlObject.GetObjectsCount("fieldFilters");
		fieldFilters = imtsdl::TElementList<CFieldFilter::V1_0>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersElementsCount; ++fieldFiltersIndex){
			const ::imtgql::CGqlParamObject* fieldFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("fieldFilters", fieldFiltersIndex);
			if (fieldFiltersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << fieldFiltersDataObjectPtr;
				return false;
			}
			CFieldFilter::V1_0 tempFieldFilters;
			if (!tempFieldFilters.OptReadFromGraphQlObject(*fieldFiltersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (gqlObject.ContainsParam("groupFilters")){
		groupFilters.emplace();
	}
	if (gqlObject.ContainsParam("groupFilters") && (gqlObject.GetObjectsCount("groupFilters") > 0)){
		const qsizetype groupFiltersElementsCount = gqlObject.GetObjectsCount("groupFilters");
		groupFilters = imtsdl::TElementList<CGroupFilter::V1_0>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersElementsCount; ++groupFiltersIndex){
			const ::imtgql::CGqlParamObject* groupFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("groupFilters", groupFiltersIndex);
			if (groupFiltersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << groupFiltersDataObjectPtr;
				return false;
			}
			CGroupFilter::V1_0 tempGroupFilters;
			if (!tempGroupFilters.OptReadFromGraphQlObject(*groupFiltersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

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
			if (!fieldFilters->at(fieldFiltersIndex)->WriteToJsonObject(newFieldFiltersJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

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
			if (!groupFilters->at(groupFiltersIndex)->WriteToJsonObject(newGroupFiltersJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			newGroupFiltersArray << newGroupFiltersJsonObject;
		}
		jsonObject["groupFilters"] = newGroupFiltersArray;
	}

	if (!logicalOperation){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

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


bool CGroupFilter::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("fieldFilters") && jsonObject["fieldFilters"].isArray()){
		const QJsonArray fieldFiltersJsonArray = jsonObject["fieldFilters"].toArray();
		const qsizetype fieldFiltersArrayCount = fieldFiltersJsonArray.size();
		fieldFilters = imtsdl::TElementList<CFieldFilter::V1_0>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersArrayCount; ++fieldFiltersIndex){
			CFieldFilter::V1_0 tempFieldFilters;
			if (!tempFieldFilters.ReadFromJsonObject(fieldFiltersJsonArray[fieldFiltersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (jsonObject.contains("groupFilters") && jsonObject["groupFilters"].isArray()){
		const QJsonArray groupFiltersJsonArray = jsonObject["groupFilters"].toArray();
		const qsizetype groupFiltersArrayCount = groupFiltersJsonArray.size();
		groupFilters = imtsdl::TElementList<CGroupFilter::V1_0>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersArrayCount; ++groupFiltersIndex){
			CGroupFilter::V1_0 tempGroupFilters;
			if (!tempGroupFilters.ReadFromJsonObject(groupFiltersJsonArray[groupFiltersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

				return false;
			}
			groupFilters->append(tempGroupFilters);
		}
	}

	if (!jsonObject.contains("logicalOperation") || ! jsonObject["logicalOperation"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "logicalOperation").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

		return false;
	}

	return true;
}


bool CGroupFilter::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("fieldFilters") && jsonObject["fieldFilters"].isArray()){
		const QJsonArray fieldFiltersJsonArray = jsonObject["fieldFilters"].toArray();
		const qsizetype fieldFiltersArrayCount = fieldFiltersJsonArray.size();
		fieldFilters = imtsdl::TElementList<CFieldFilter::V1_0>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersArrayCount; ++fieldFiltersIndex){
			CFieldFilter::V1_0 tempFieldFilters;
			if (!tempFieldFilters.OptReadFromJsonObject(fieldFiltersJsonArray[fieldFiltersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldFilters").toLocal8Bit().constData();)

				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (jsonObject.contains("groupFilters") && jsonObject["groupFilters"].isArray()){
		const QJsonArray groupFiltersJsonArray = jsonObject["groupFilters"].toArray();
		const qsizetype groupFiltersArrayCount = groupFiltersJsonArray.size();
		groupFilters = imtsdl::TElementList<CGroupFilter::V1_0>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersArrayCount; ++groupFiltersIndex){
			CGroupFilter::V1_0 tempGroupFilters;
			if (!tempGroupFilters.OptReadFromJsonObject(groupFiltersJsonArray[groupFiltersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "groupFilters").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LogicalOperation", logicalOperationStringValue);)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
	model.SetData("__typename", "ComplexCollectionFilter", modelIndex);

	if (sortingInfo){
		::imtbase::CTreeItemModel* newSortingInfoModelPtr = model.AddTreeModel("sortingInfo", modelIndex);
		newSortingInfoModelPtr->setIsArray(true);
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfo->size(); ++sortingInfoIndex){
			newSortingInfoModelPtr->InsertNewItem();
			if (!(sortingInfo->at(sortingInfoIndex)->WriteToModel(*newSortingInfoModelPtr, sortingInfoIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	if (fieldsFilter){
		::imtbase::CTreeItemModel* fieldsFilterNewModelPtr = model.AddTreeModel("fieldsFilter", modelIndex);
		const bool isFieldsFilterAdded = fieldsFilter->WriteToModel(*fieldsFilterNewModelPtr, 0);
		if (!isFieldsFilterAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}

	}

	if (timeFilter){
		::imtbase::CTreeItemModel* timeFilterNewModelPtr = model.AddTreeModel("timeFilter", modelIndex);
		const bool isTimeFilterAdded = timeFilter->WriteToModel(*timeFilterNewModelPtr, 0);
		if (!isTimeFilterAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}

	}
	if (distinctFields){
		::imtbase::CTreeItemModel* newDistinctFieldsModelPtr = model.AddTreeModel("distinctFields", modelIndex);
		newDistinctFieldsModelPtr->setIsArray(true);
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFields->size(); ++distinctFieldsIndex){
			newDistinctFieldsModelPtr->InsertNewItem();
			newDistinctFieldsModelPtr->SetData(QByteArray(), *distinctFields->at(distinctFieldsIndex), distinctFieldsIndex);
		}
	}


	return true;
}


bool CComplexCollectionFilter::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* sortingInfoModel = model.GetTreeItemModel("sortingInfo", modelIndex);
	if (sortingInfoModel != nullptr){
		int sortingInfoCount = sortingInfoModel->GetItemsCount();
		imtsdl::TElementList<CFieldSortingInfo::V1_0> sortingInfoList;
		for (int sortingInfoIndex = 0; sortingInfoIndex < sortingInfoCount; ++sortingInfoIndex){
			CFieldSortingInfo::V1_0 t_sortingInfo;
			if (!t_sortingInfo.ReadFromModel(*sortingInfoModel, sortingInfoIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfoList << t_sortingInfo;
		}
		sortingInfo = sortingInfoList;

	}

	::imtbase::CTreeItemModel* fieldsFilterDataModelPtr = model.GetTreeItemModel("fieldsFilter", modelIndex);
	if (fieldsFilterDataModelPtr != nullptr){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->ReadFromModel(*fieldsFilterDataModelPtr, modelIndex);
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* timeFilterDataModelPtr = model.GetTreeItemModel("timeFilter", modelIndex);
	if (timeFilterDataModelPtr != nullptr){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->ReadFromModel(*timeFilterDataModelPtr, modelIndex);
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* distinctFieldsModel = model.GetTreeItemModel("distinctFields", modelIndex);
	if (distinctFieldsModel != nullptr){
		int distinctFieldsCount = distinctFieldsModel->GetItemsCount();
		imtsdl::TElementList<QByteArray> distinctFieldsList;
		for (int distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsCount; ++distinctFieldsIndex){
			QByteArray t_distinctFields = distinctFieldsModel->GetData(QByteArray(), distinctFieldsIndex).toByteArray();
			distinctFieldsList << t_distinctFields;
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
		imtsdl::TElementList<CFieldSortingInfo::V1_0> sortingInfoList;
		for (int sortingInfoIndex = 0; sortingInfoIndex < sortingInfoCount; ++sortingInfoIndex){
			CFieldSortingInfo::V1_0 t_sortingInfo;
			if (!t_sortingInfo.OptReadFromModel(*sortingInfoModel, sortingInfoIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfoList << t_sortingInfo;
		}
		sortingInfo = sortingInfoList;

	}

	::imtbase::CTreeItemModel* fieldsFilterDataModelPtr = model.GetTreeItemModel("fieldsFilter", modelIndex);
	if (fieldsFilterDataModelPtr != nullptr){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->ReadFromModel(*fieldsFilterDataModelPtr, modelIndex);
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* timeFilterDataModelPtr = model.GetTreeItemModel("timeFilter", modelIndex);
	if (timeFilterDataModelPtr != nullptr){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->ReadFromModel(*timeFilterDataModelPtr, modelIndex);
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* distinctFieldsModel = model.GetTreeItemModel("distinctFields", modelIndex);
	if (distinctFieldsModel != nullptr){
		int distinctFieldsCount = distinctFieldsModel->GetItemsCount();
		imtsdl::TElementList<QByteArray> distinctFieldsList;
		for (int distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsCount; ++distinctFieldsIndex){
			QByteArray t_distinctFields = distinctFieldsModel->GetData(QByteArray(), distinctFieldsIndex).toByteArray();
			distinctFieldsList << t_distinctFields;
		}
		distinctFields = distinctFieldsList;

	}

	return true;
}


bool CComplexCollectionFilter::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (sortingInfo){
		QList<::imtgql::CGqlParamObject> sortingInfoDataObjectList;
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfo->size(); ++sortingInfoIndex){
			::imtgql::CGqlParamObject newSortingInfoGqlObject;
			if (!sortingInfo->at(sortingInfoIndex)->WriteToGraphQlObject(newSortingInfoGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfoDataObjectList << newSortingInfoGqlObject;
		}
		gqlObject.InsertParam("sortingInfo", sortingInfoDataObjectList);
	}

	if (fieldsFilter){
		::imtgql::CGqlParamObject fieldsFilterGqlObject;
		const bool isFieldsFilterAdded = fieldsFilter->WriteToGraphQlObject(fieldsFilterGqlObject);
		if (!isFieldsFilterAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("fieldsFilter", fieldsFilterGqlObject);
	}

	if (timeFilter){
		::imtgql::CGqlParamObject timeFilterGqlObject;
		const bool isTimeFilterAdded = timeFilter->WriteToGraphQlObject(timeFilterGqlObject);
		if (!isTimeFilterAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("timeFilter", timeFilterGqlObject);
	}

	if (distinctFields){
		QVariantList distinctFieldsDataObjectList;
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFields->size(); ++distinctFieldsIndex){
			distinctFieldsDataObjectList << *distinctFields->at(distinctFieldsIndex);
		}
		gqlObject.InsertParam("distinctFields", distinctFieldsDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("ComplexCollectionFilter"));

	return true;
}


bool CComplexCollectionFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("sortingInfo")){
		sortingInfo.emplace();
	}
	if (gqlObject.ContainsParam("sortingInfo") && (gqlObject.GetObjectsCount("sortingInfo") > 0)){
		const qsizetype sortingInfoElementsCount = gqlObject.GetObjectsCount("sortingInfo");
		sortingInfo = imtsdl::TElementList<CFieldSortingInfo::V1_0>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoElementsCount; ++sortingInfoIndex){
			const ::imtgql::CGqlParamObject* sortingInfoDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("sortingInfo", sortingInfoIndex);
			if (sortingInfoDataObjectPtr == nullptr){
				qDebug() << "invalid type" << sortingInfoDataObjectPtr;
				return false;
			}
			CFieldSortingInfo::V1_0 tempSortingInfo;
			if (!tempSortingInfo.ReadFromGraphQlObject(*sortingInfoDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (gqlObject.ContainsParam("fieldsFilter") && (gqlObject.GetParamArgumentObjectPtr("fieldsFilter") != nullptr)){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("fieldsFilter"));
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("timeFilter") && (gqlObject.GetParamArgumentObjectPtr("timeFilter") != nullptr)){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("timeFilter"));
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("distinctFields")){
		distinctFields.emplace();
	}
	if (gqlObject.ContainsParam("distinctFields") && !(gqlObject["distinctFields"].isNull())){
		const QVariant distinctFieldsData = gqlObject["distinctFields"];
		const QVariantList distinctFieldsDataList = distinctFieldsData.toList();
		const qsizetype distinctFieldsElementsCount = distinctFieldsDataList.size();
		distinctFields = imtsdl::TElementList<QByteArray>();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsElementsCount; ++distinctFieldsIndex){
			QByteArray tempDistinctFields = distinctFieldsDataList[distinctFieldsIndex].toByteArray();
			distinctFields->append(tempDistinctFields);
		}
	}

	return true;
}


bool CComplexCollectionFilter::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("sortingInfo")){
		sortingInfo.emplace();
	}
	if (gqlObject.ContainsParam("sortingInfo") && (gqlObject.GetObjectsCount("sortingInfo") > 0)){
		const qsizetype sortingInfoElementsCount = gqlObject.GetObjectsCount("sortingInfo");
		sortingInfo = imtsdl::TElementList<CFieldSortingInfo::V1_0>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoElementsCount; ++sortingInfoIndex){
			const ::imtgql::CGqlParamObject* sortingInfoDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("sortingInfo", sortingInfoIndex);
			if (sortingInfoDataObjectPtr == nullptr){
				qDebug() << "invalid type" << sortingInfoDataObjectPtr;
				return false;
			}
			CFieldSortingInfo::V1_0 tempSortingInfo;
			if (!tempSortingInfo.OptReadFromGraphQlObject(*sortingInfoDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (gqlObject.ContainsParam("fieldsFilter") && (gqlObject.GetParamArgumentObjectPtr("fieldsFilter") != nullptr)){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("fieldsFilter"));
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("timeFilter") && (gqlObject.GetParamArgumentObjectPtr("timeFilter") != nullptr)){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("timeFilter"));
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("distinctFields")){
		distinctFields.emplace();
	}
	if (gqlObject.ContainsParam("distinctFields") && !(gqlObject["distinctFields"].isNull())){
		const QVariant distinctFieldsData = gqlObject["distinctFields"];
		const QVariantList distinctFieldsDataList = distinctFieldsData.toList();
		const qsizetype distinctFieldsElementsCount = distinctFieldsDataList.size();
		distinctFields = imtsdl::TElementList<QByteArray>();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsElementsCount; ++distinctFieldsIndex){
			QByteArray tempDistinctFields = distinctFieldsDataList[distinctFieldsIndex].toByteArray();
			distinctFields->append(tempDistinctFields);
		}
	}

	return true;
}


bool CComplexCollectionFilter::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (sortingInfo){
		QJsonArray newSortingInfoArray;
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfo->size(); ++sortingInfoIndex){
			QJsonObject newSortingInfoJsonObject;
			if (!sortingInfo->at(sortingInfoIndex)->WriteToJsonObject(newSortingInfoJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			newSortingInfoArray << newSortingInfoJsonObject;
		}
		jsonObject["sortingInfo"] = newSortingInfoArray;
	}

	if (fieldsFilter){
		QJsonObject fieldsFilterJsonObject;
		const bool isFieldsFilterAdded = fieldsFilter->WriteToJsonObject(fieldsFilterJsonObject);
		if (!isFieldsFilterAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["fieldsFilter"] = fieldsFilterJsonObject;
	}

	if (timeFilter){
		QJsonObject timeFilterJsonObject;
		const bool isTimeFilterAdded = timeFilter->WriteToJsonObject(timeFilterJsonObject);
		if (!isTimeFilterAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["timeFilter"] = timeFilterJsonObject;
	}

	if (distinctFields){
		QJsonArray newDistinctFieldsArray;
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFields->size(); ++distinctFieldsIndex){
			newDistinctFieldsArray << QString(*distinctFields->at(distinctFieldsIndex));
		}
		jsonObject["distinctFields"] = newDistinctFieldsArray;
	}

	jsonObject["__typename"] = "ComplexCollectionFilter";

	return true;
}


bool CComplexCollectionFilter::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("sortingInfo") && jsonObject["sortingInfo"].isArray()){
		const QJsonArray sortingInfoJsonArray = jsonObject["sortingInfo"].toArray();
		const qsizetype sortingInfoArrayCount = sortingInfoJsonArray.size();
		sortingInfo = imtsdl::TElementList<CFieldSortingInfo::V1_0>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoArrayCount; ++sortingInfoIndex){
			CFieldSortingInfo::V1_0 tempSortingInfo;
			if (!tempSortingInfo.ReadFromJsonObject(sortingInfoJsonArray[sortingInfoIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (jsonObject.contains("fieldsFilter") && jsonObject["fieldsFilter"].isObject()){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->ReadFromJsonObject(jsonObject["fieldsFilter"].toObject());
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("timeFilter") && jsonObject["timeFilter"].isObject()){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->ReadFromJsonObject(jsonObject["timeFilter"].toObject());
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("distinctFields") && jsonObject["distinctFields"].isArray()){
		const QJsonArray distinctFieldsJsonArray = jsonObject["distinctFields"].toArray();
		const qsizetype distinctFieldsArrayCount = distinctFieldsJsonArray.size();
		distinctFields = imtsdl::TElementList<QByteArray>();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsArrayCount; ++distinctFieldsIndex){
			QByteArray tempDistinctFields = distinctFieldsJsonArray[distinctFieldsIndex].toString().toUtf8();
			distinctFields->append(tempDistinctFields);
		}
	}

	return true;
}


bool CComplexCollectionFilter::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("sortingInfo") && jsonObject["sortingInfo"].isArray()){
		const QJsonArray sortingInfoJsonArray = jsonObject["sortingInfo"].toArray();
		const qsizetype sortingInfoArrayCount = sortingInfoJsonArray.size();
		sortingInfo = imtsdl::TElementList<CFieldSortingInfo::V1_0>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoArrayCount; ++sortingInfoIndex){
			CFieldSortingInfo::V1_0 tempSortingInfo;
			if (!tempSortingInfo.OptReadFromJsonObject(sortingInfoJsonArray[sortingInfoIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "sortingInfo").toLocal8Bit().constData();)

				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (jsonObject.contains("fieldsFilter") && jsonObject["fieldsFilter"].isObject()){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->OptReadFromJsonObject(jsonObject["fieldsFilter"].toObject());
		if (!isFieldsFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "fieldsFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("timeFilter") && jsonObject["timeFilter"].isObject()){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->OptReadFromJsonObject(jsonObject["timeFilter"].toObject());
		if (!isTimeFilterRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "timeFilter").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("distinctFields") && jsonObject["distinctFields"].isArray()){
		const QJsonArray distinctFieldsJsonArray = jsonObject["distinctFields"].toArray();
		const qsizetype distinctFieldsArrayCount = distinctFieldsJsonArray.size();
		distinctFields = imtsdl::TElementList<QByteArray>();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsArrayCount; ++distinctFieldsIndex){
			QByteArray tempDistinctFields = distinctFieldsJsonArray[distinctFieldsIndex].toString().toUtf8();
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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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




CTimeFilterObject::CTimeFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CTimeFilterObject::timeRangeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeFilterObject::timeUnitChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeFilterObject::interpretationModeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeFilterObject::unitMultiplierChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CTimeFilterObject::GetTimeRange()
{
	if (Version_1_0->timeRange.has_value()){
		if (!m_timeRangeQObjectPtr.isValid()){
			m_timeRangeQObjectPtr = CreateObject("timeRange");
			auto itemPtr = m_timeRangeQObjectPtr.value<sdl::imtbase::ImtBaseTypes::CTimeRangeObject*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->timeRange;
		}
		return m_timeRangeQObjectPtr;
	}

	return QVariant();
}


void CTimeFilterObject::SetTimeRange(const QVariant& v)
{
	if (v.isValid()){
		sdl::imtbase::ImtBaseTypes::CTimeRangeObject* itemPtr = v.value<sdl::imtbase::ImtBaseTypes::CTimeRangeObject*>();
		if (itemPtr != nullptr)  Version_1_0->timeRange = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->timeRange = nullptr;
	}
	m_timeRangeQObjectPtr = v;

	timeRangeChanged();
}


bool CTimeFilterObject::hasTimeRange()
{
	 return Version_1_0->timeRange.HasValue();
}


void CTimeFilterObject::emplaceTimeRange()
{
	Version_1_0->timeRange.emplace();

}


void CTimeFilterObject::ResetTimeRange()
{
	auto* itemPtr = m_timeRangeQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_timeRangeQObjectPtr = QVariant();
}


QVariant CTimeFilterObject::GetTimeUnit()
{
	if (Version_1_0->timeUnit.has_value()){
		return Version_1_0->timeUnit.value();
	}

	return QVariant();
}


void CTimeFilterObject::SetTimeUnit(const QVariant& v)
{
	Version_1_0->timeUnit = v.value<QString>();
	timeUnitChanged();
}


bool CTimeFilterObject::hasTimeUnit()
{
	 return Version_1_0->timeUnit.HasValue();
}


QVariant CTimeFilterObject::GetInterpretationMode()
{
	if (Version_1_0->interpretationMode.has_value()){
		return Version_1_0->interpretationMode.value();
	}

	return QVariant();
}


void CTimeFilterObject::SetInterpretationMode(const QVariant& v)
{
	Version_1_0->interpretationMode = v.value<QString>();
	interpretationModeChanged();
}


bool CTimeFilterObject::hasInterpretationMode()
{
	 return Version_1_0->interpretationMode.HasValue();
}


QVariant CTimeFilterObject::GetUnitMultiplier()
{
	if (Version_1_0->unitMultiplier.has_value()){
		return Version_1_0->unitMultiplier.value();
	}

	return QVariant();
}


void CTimeFilterObject::SetUnitMultiplier(const QVariant& v)
{
	Version_1_0->unitMultiplier = v.value<int>();
	unitMultiplierChanged();
}


bool CTimeFilterObject::hasUnitMultiplier()
{
	 return Version_1_0->unitMultiplier.HasValue();
}


QString CTimeFilterObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CTimeFilterObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CTimeFilterObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CTimeFilterObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CTimeFilterObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "timeRange"){
		return QVariant::fromValue(new sdl::imtbase::ImtBaseTypes::CTimeRangeObject(this));
	}
	return QVariant();
}


QString CTimeFilterObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "timeRange")){
		return "timeRange";
	}
	if (propertyName == (QString("m_") + "timeUnit")){
		return "timeUnit";
	}
	if (propertyName == (QString("m_") + "interpretationMode")){
		return "interpretationMode";
	}
	if (propertyName == (QString("m_") + "unitMultiplier")){
		return "unitMultiplier";
	}

	return propertyName;
}





bool CTimeFilterObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CTimeFilterObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CTimeFilterObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CTimeFilterObjectList::append(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList* sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList::copyMe()
{
	sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList* retVal = new sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList::addElement(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* item)
{
	append(item);
}


void sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList::isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList::insert(int index, sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_timeRange"){
			return itemPtr->GetTimeRange();
		}
		if (nameId == "m_timeUnit"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->timeUnit.value());
		}
		if (nameId == "m_interpretationMode"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->interpretationMode.value());
		}
		if (nameId == "m_unitMultiplier"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->unitMultiplier.value());
		}
	return QVariant();
}
CFieldSortingInfoObject::CFieldSortingInfoObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CFieldSortingInfoObject::fieldIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldSortingInfoObject::sortingOrderChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CFieldSortingInfoObject::GetFieldId()
{
	if (Version_1_0->fieldId.has_value()){
		return Version_1_0->fieldId.value();
	}

	return QVariant();
}


void CFieldSortingInfoObject::SetFieldId(const QVariant& v)
{
	Version_1_0->fieldId = v.value<QString>();
	fieldIdChanged();
}


bool CFieldSortingInfoObject::hasFieldId()
{
	 return Version_1_0->fieldId.HasValue();
}


QVariant CFieldSortingInfoObject::GetSortingOrder()
{
	if (Version_1_0->sortingOrder.has_value()){
		return Version_1_0->sortingOrder.value();
	}

	return QVariant();
}


void CFieldSortingInfoObject::SetSortingOrder(const QVariant& v)
{
	Version_1_0->sortingOrder = v.value<QString>();
	sortingOrderChanged();
}


bool CFieldSortingInfoObject::hasSortingOrder()
{
	 return Version_1_0->sortingOrder.HasValue();
}


QString CFieldSortingInfoObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CFieldSortingInfoObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CFieldSortingInfoObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CFieldSortingInfoObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CFieldSortingInfoObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CFieldSortingInfoObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "fieldId")){
		return "fieldId";
	}
	if (propertyName == (QString("m_") + "sortingOrder")){
		return "sortingOrder";
	}

	return propertyName;
}





bool CFieldSortingInfoObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CFieldSortingInfoObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CFieldSortingInfoObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CFieldSortingInfoObjectList::append(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList::copyMe()
{
	sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* retVal = new sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList::addElement(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* item)
{
	append(item);
}


void sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList::isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList::insert(int index, sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_fieldId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->fieldId.value());
		}
		if (nameId == "m_sortingOrder"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->sortingOrder.value());
		}
	return QVariant();
}
CFieldFilterObject::CFieldFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CFieldFilterObject::fieldIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldFilterObject::filterValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldFilterObject::filterValueTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldFilterObject::filterOperationsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CFieldFilterObject::GetFieldId()
{
	if (Version_1_0->fieldId.has_value()){
		return Version_1_0->fieldId.value();
	}

	return QVariant();
}


void CFieldFilterObject::SetFieldId(const QVariant& v)
{
	Version_1_0->fieldId = v.value<QString>().toUtf8();
	fieldIdChanged();
}


bool CFieldFilterObject::hasFieldId()
{
	 return Version_1_0->fieldId.HasValue();
}


QVariant CFieldFilterObject::GetFilterValue()
{
	if (Version_1_0->filterValue.has_value()){
		return Version_1_0->filterValue.value();
	}

	return QVariant();
}


void CFieldFilterObject::SetFilterValue(const QVariant& v)
{
	Version_1_0->filterValue = v.value<QString>();
	filterValueChanged();
}


bool CFieldFilterObject::hasFilterValue()
{
	 return Version_1_0->filterValue.HasValue();
}


QVariant CFieldFilterObject::GetFilterValueType()
{
	if (Version_1_0->filterValueType.has_value()){
		sdl::imtbase::ComplexCollectionFilter::ValueType valueType = Version_1_0->filterValueType.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::imtbase::ComplexCollectionFilter::ValueType>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QVariant();
}


void CFieldFilterObject::SetFilterValueType(const QVariant& v)
{
	Version_1_0->filterValueType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::imtbase::ComplexCollectionFilter::ValueType>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		Version_1_0->filterValueType = (sdl::imtbase::ComplexCollectionFilter::ValueType)key;
	}
	filterValueTypeChanged();
}


bool CFieldFilterObject::hasFilterValueType()
{
	 return Version_1_0->filterValueType.HasValue();
}


QVariant CFieldFilterObject::GetFilterOperations()
{
	if (Version_1_0->filterOperations.has_value()){
		
	}

	return QVariant();
}


void CFieldFilterObject::SetFilterOperations(const QVariant& v)
{
	
	filterOperationsChanged();
}


bool CFieldFilterObject::hasFilterOperations()
{
	 return Version_1_0->filterOperations.HasValue();
}


QString CFieldFilterObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CFieldFilterObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CFieldFilterObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CFieldFilterObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CFieldFilterObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CFieldFilterObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "fieldId")){
		return "fieldId";
	}
	if (propertyName == (QString("m_") + "filterValue")){
		return "filterValue";
	}
	if (propertyName == (QString("m_") + "filterValueType")){
		return "filterValueType";
	}
	if (propertyName == (QString("m_") + "filterOperations")){
		return "filterOperations";
	}

	return propertyName;
}





bool CFieldFilterObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CFieldFilterObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CFieldFilterObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CFieldFilterObjectList::append(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList::copyMe()
{
	sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* retVal = new sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList::addElement(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* item)
{
	append(item);
}


void sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList::isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList::insert(int index, sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_fieldId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->fieldId.value());
		}
		if (nameId == "m_filterValue"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->filterValue.value());
		}
		if (nameId == "m_filterValueType"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->filterValueType.value());
		}
		if (nameId == "m_filterOperations"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->filterOperations.value());
		}
	return QVariant();
}
CGroupFilterObject::CGroupFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CGroupFilterObject::fieldFiltersChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGroupFilterObject::groupFiltersChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGroupFilterObject::logicalOperationChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CGroupFilterObject::GetFieldFilters()
{
	if (Version_1_0->fieldFilters.has_value()){
		if (!m_fieldFiltersQObjectPtr.isValid()){
			m_fieldFiltersQObjectPtr = CreateObject("fieldFilters");
			auto itemPtr = m_fieldFiltersQObjectPtr.value<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->fieldFilters;
		}
		return m_fieldFiltersQObjectPtr;
	}

	return QVariant();
}


void CGroupFilterObject::SetFieldFilters(const QVariant& v)
{
	if (v.isValid()){
		sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* itemPtr = v.value<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList*>();
		if (itemPtr != nullptr)  Version_1_0->fieldFilters = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->fieldFilters = nullptr;
	}
	m_fieldFiltersQObjectPtr = v;

	fieldFiltersChanged();
}


bool CGroupFilterObject::hasFieldFilters()
{
	 return Version_1_0->fieldFilters.HasValue();
}


void CGroupFilterObject::emplaceFieldFilters()
{
	Version_1_0->fieldFilters.emplace();

}


void CGroupFilterObject::ResetFieldFilters()
{
	auto* itemPtr = m_fieldFiltersQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_fieldFiltersQObjectPtr = QVariant();
}


QVariant CGroupFilterObject::createFieldFiltersArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject());
}


QVariant CGroupFilterObject::GetGroupFilters()
{
	if (Version_1_0->groupFilters.has_value()){
		if (!m_groupFiltersQObjectPtr.isValid()){
			m_groupFiltersQObjectPtr = CreateObject("groupFilters");
			auto itemPtr = m_groupFiltersQObjectPtr.value<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->groupFilters;
		}
		return m_groupFiltersQObjectPtr;
	}

	return QVariant();
}


void CGroupFilterObject::SetGroupFilters(const QVariant& v)
{
	if (v.isValid()){
		sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* itemPtr = v.value<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList*>();
		if (itemPtr != nullptr)  Version_1_0->groupFilters = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->groupFilters = nullptr;
	}
	m_groupFiltersQObjectPtr = v;

	groupFiltersChanged();
}


bool CGroupFilterObject::hasGroupFilters()
{
	 return Version_1_0->groupFilters.HasValue();
}


void CGroupFilterObject::emplaceGroupFilters()
{
	Version_1_0->groupFilters.emplace();

}


void CGroupFilterObject::ResetGroupFilters()
{
	auto* itemPtr = m_groupFiltersQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_groupFiltersQObjectPtr = QVariant();
}


QVariant CGroupFilterObject::createGroupFiltersArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject());
}


QVariant CGroupFilterObject::GetLogicalOperation()
{
	if (Version_1_0->logicalOperation.has_value()){
		sdl::imtbase::ComplexCollectionFilter::LogicalOperation valueType = Version_1_0->logicalOperation.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::imtbase::ComplexCollectionFilter::LogicalOperation>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QVariant();
}


void CGroupFilterObject::SetLogicalOperation(const QVariant& v)
{
	Version_1_0->logicalOperation.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::imtbase::ComplexCollectionFilter::LogicalOperation>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		Version_1_0->logicalOperation = (sdl::imtbase::ComplexCollectionFilter::LogicalOperation)key;
	}
	logicalOperationChanged();
}


bool CGroupFilterObject::hasLogicalOperation()
{
	 return Version_1_0->logicalOperation.HasValue();
}


QString CGroupFilterObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CGroupFilterObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CGroupFilterObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CGroupFilterObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CGroupFilterObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "fieldFilters"){
		return QVariant::fromValue(new sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList(this));
	}
	if (key == "groupFilters"){
		return QVariant::fromValue(new sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList(this));
	}
	return QVariant();
}


QString CGroupFilterObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "fieldFilters")){
		return "fieldFilters";
	}
	if (propertyName == (QString("m_") + "groupFilters")){
		return "groupFilters";
	}
	if (propertyName == (QString("m_") + "logicalOperation")){
		return "logicalOperation";
	}

	return propertyName;
}





bool CGroupFilterObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CGroupFilterObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CGroupFilterObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CGroupFilterObjectList::append(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList::copyMe()
{
	sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* retVal = new sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList::addElement(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* item)
{
	append(item);
}


void sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList::isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList::insert(int index, sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_fieldFilters"){
			return itemPtr->GetFieldFilters();
		}
		if (nameId == "m_groupFilters"){
			return itemPtr->GetGroupFilters();
		}
		if (nameId == "m_logicalOperation"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->logicalOperation.value());
		}
	return QVariant();
}
CComplexCollectionFilterObject::CComplexCollectionFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CComplexCollectionFilterObject::sortingInfoChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CComplexCollectionFilterObject::fieldsFilterChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CComplexCollectionFilterObject::timeFilterChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CComplexCollectionFilterObject::distinctFieldsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CComplexCollectionFilterObject::GetSortingInfo()
{
	if (Version_1_0->sortingInfo.has_value()){
		if (!m_sortingInfoQObjectPtr.isValid()){
			m_sortingInfoQObjectPtr = CreateObject("sortingInfo");
			auto itemPtr = m_sortingInfoQObjectPtr.value<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->sortingInfo;
		}
		return m_sortingInfoQObjectPtr;
	}

	return QVariant();
}


void CComplexCollectionFilterObject::SetSortingInfo(const QVariant& v)
{
	if (v.isValid()){
		sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* itemPtr = v.value<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList*>();
		if (itemPtr != nullptr)  Version_1_0->sortingInfo = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->sortingInfo = nullptr;
	}
	m_sortingInfoQObjectPtr = v;

	sortingInfoChanged();
}


bool CComplexCollectionFilterObject::hasSortingInfo()
{
	 return Version_1_0->sortingInfo.HasValue();
}


void CComplexCollectionFilterObject::emplaceSortingInfo()
{
	Version_1_0->sortingInfo.emplace();

}


void CComplexCollectionFilterObject::ResetSortingInfo()
{
	auto* itemPtr = m_sortingInfoQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_sortingInfoQObjectPtr = QVariant();
}


QVariant CComplexCollectionFilterObject::createSortingInfoArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject());
}


QVariant CComplexCollectionFilterObject::GetFieldsFilter()
{
	if (Version_1_0->fieldsFilter.has_value()){
		if (!m_fieldsFilterQObjectPtr.isValid()){
			m_fieldsFilterQObjectPtr = CreateObject("fieldsFilter");
			auto itemPtr = m_fieldsFilterQObjectPtr.value<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->fieldsFilter;
		}
		return m_fieldsFilterQObjectPtr;
	}

	return QVariant();
}


void CComplexCollectionFilterObject::SetFieldsFilter(const QVariant& v)
{
	if (v.isValid()){
		sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* itemPtr = v.value<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject*>();
		if (itemPtr != nullptr)  Version_1_0->fieldsFilter = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->fieldsFilter = nullptr;
	}
	m_fieldsFilterQObjectPtr = v;

	fieldsFilterChanged();
}


bool CComplexCollectionFilterObject::hasFieldsFilter()
{
	 return Version_1_0->fieldsFilter.HasValue();
}


void CComplexCollectionFilterObject::emplaceFieldsFilter()
{
	Version_1_0->fieldsFilter.emplace();

}


void CComplexCollectionFilterObject::ResetFieldsFilter()
{
	auto* itemPtr = m_fieldsFilterQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_fieldsFilterQObjectPtr = QVariant();
}


QVariant CComplexCollectionFilterObject::GetTimeFilter()
{
	if (Version_1_0->timeFilter.has_value()){
		if (!m_timeFilterQObjectPtr.isValid()){
			m_timeFilterQObjectPtr = CreateObject("timeFilter");
			auto itemPtr = m_timeFilterQObjectPtr.value<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->timeFilter;
		}
		return m_timeFilterQObjectPtr;
	}

	return QVariant();
}


void CComplexCollectionFilterObject::SetTimeFilter(const QVariant& v)
{
	if (v.isValid()){
		sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* itemPtr = v.value<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject*>();
		if (itemPtr != nullptr)  Version_1_0->timeFilter = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->timeFilter = nullptr;
	}
	m_timeFilterQObjectPtr = v;

	timeFilterChanged();
}


bool CComplexCollectionFilterObject::hasTimeFilter()
{
	 return Version_1_0->timeFilter.HasValue();
}


void CComplexCollectionFilterObject::emplaceTimeFilter()
{
	Version_1_0->timeFilter.emplace();

}


void CComplexCollectionFilterObject::ResetTimeFilter()
{
	auto* itemPtr = m_timeFilterQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_timeFilterQObjectPtr = QVariant();
}


QVariant CComplexCollectionFilterObject::GetDistinctFields()
{
	if (Version_1_0->distinctFields.has_value()){
		QList<QString> tempDistinctFieldsList;
		for (const auto& tempValue: Version_1_0->distinctFields.value()){
			tempDistinctFieldsList << *tempValue;
		}
		return QVariant::fromValue(tempDistinctFieldsList);
	}

	return QVariant();
}


void CComplexCollectionFilterObject::SetDistinctFields(const QVariant& v)
{
	Version_1_0->distinctFields->clear(); 
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QByteArray> value(tempValue.toUtf8());
		Version_1_0->distinctFields->append(value);
	}

	distinctFieldsChanged();
}


bool CComplexCollectionFilterObject::hasDistinctFields()
{
	 return Version_1_0->distinctFields.HasValue();
}


QString CComplexCollectionFilterObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CComplexCollectionFilterObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CComplexCollectionFilterObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CComplexCollectionFilterObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CComplexCollectionFilterObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "sortingInfo"){
		return QVariant::fromValue(new sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList(this));
	}
	if (key == "fieldsFilter"){
		return QVariant::fromValue(new sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject(this));
	}
	if (key == "timeFilter"){
		return QVariant::fromValue(new sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject(this));
	}
	return QVariant();
}


QString CComplexCollectionFilterObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "sortingInfo")){
		return "sortingInfo";
	}
	if (propertyName == (QString("m_") + "fieldsFilter")){
		return "fieldsFilter";
	}
	if (propertyName == (QString("m_") + "timeFilter")){
		return "timeFilter";
	}
	if (propertyName == (QString("m_") + "distinctFields")){
		return "distinctFields";
	}

	return propertyName;
}





bool CComplexCollectionFilterObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CComplexCollectionFilterObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CComplexCollectionFilterObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CComplexCollectionFilterObjectList::append(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList* sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList::copyMe()
{
	sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList* retVal = new sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList::addElement(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* item)
{
	append(item);
}


void sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList::isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList::insert(int index, sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_sortingInfo"){
			return itemPtr->GetSortingInfo();
		}
		if (nameId == "m_fieldsFilter"){
			return itemPtr->GetFieldsFilter();
		}
		if (nameId == "m_timeFilter"){
			return itemPtr->GetTimeFilter();
		}
		if (nameId == "m_distinctFields"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->distinctFields.value());
		}
	return QVariant();
}
} // namespace sdl::imtbase::ComplexCollectionFilter
