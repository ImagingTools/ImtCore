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


bool CTimeFilter::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (timeRange){
		::imtgql::CGqlParamObject timeRangeGqlObject;
		const bool isTimeRangeAdded = timeRange->WriteToGraphQlObject(timeRangeGqlObject);
		if (!isTimeRangeAdded){
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

	return true;
}


bool CTimeFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("timeRange") && (gqlObject.GetParamArgumentObjectPtr("timeRange") != nullptr)){
		timeRange = ImtBaseTypes::CTimeRange::V1_0();
		const bool isTimeRangeRead = timeRange->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("timeRange"));
		if (!isTimeRangeRead){
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

	return true;
}


bool CTimeFilter::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("timeRange") && jsonObject["timeRange"].isObject()){
		timeRange = ImtBaseTypes::CTimeRange::V1_0();
		const bool isTimeRangeRead = timeRange->ReadFromJsonObject(jsonObject["timeRange"].toObject());
		if (!isTimeRangeRead){
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


CTimeFilterObject::CTimeFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_timeRangeQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CTimeFilterObject::timeRangeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeFilterObject::timeUnitChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeFilterObject::interpretationModeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeFilterObject::unitMultiplierChanged, this, &CItemModelBase::OnInternalModelChanged);
}


sdl::imtbase::ImtBaseTypes::CTimeRangeObject* CTimeFilterObject::GetTimeRange()
{
	if (Version_1_0->timeRange.has_value()){
		if (!m_timeRangeQObjectPtr){
			m_timeRangeQObjectPtr = dynamic_cast<sdl::imtbase::ImtBaseTypes::CTimeRangeObject*>(CreateObject("timeRange"));
			m_timeRangeQObjectPtr->Version_1_0 = Version_1_0->timeRange;
		}
		return m_timeRangeQObjectPtr;
	}

	return nullptr;
}


void CTimeFilterObject::SetTimeRange(sdl::imtbase::ImtBaseTypes::CTimeRangeObject* v)
{
	if (v){
		Version_1_0->timeRange = v->Version_1_0;
		m_timeRangeQObjectPtr = v;
	}
	else {
		Version_1_0->timeRange = nullptr;
	}

	timeRangeChanged();
}


bool CTimeFilterObject::hasTimeRange()
{
	 return Version_1_0->timeRange.HasValue();
}


void CTimeFilterObject::createTimeRange()
{	Version_1_0->timeRange.emplace();

}


QString CTimeFilterObject::GetTimeUnit()
{
	if (Version_1_0->timeUnit.has_value()){
		return Version_1_0->timeUnit.value();
	}

	return QString();
}


void CTimeFilterObject::SetTimeUnit(QString v)
{
	Version_1_0->timeUnit = v;
	timeUnitChanged();
}


bool CTimeFilterObject::hasTimeUnit()
{
	 return Version_1_0->timeUnit.HasValue();
}


QString CTimeFilterObject::GetInterpretationMode()
{
	if (Version_1_0->interpretationMode.has_value()){
		return Version_1_0->interpretationMode.value();
	}

	return QString();
}


void CTimeFilterObject::SetInterpretationMode(QString v)
{
	Version_1_0->interpretationMode = v;
	interpretationModeChanged();
}


bool CTimeFilterObject::hasInterpretationMode()
{
	 return Version_1_0->interpretationMode.HasValue();
}


int CTimeFilterObject::GetUnitMultiplier()
{
	if (Version_1_0->unitMultiplier.has_value()){
		return Version_1_0->unitMultiplier.value();
	}

	return 0;
}


void CTimeFilterObject::SetUnitMultiplier(int v)
{
	Version_1_0->unitMultiplier = v;
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


QObject* CTimeFilterObject::CreateObject(const QString& key)
{
	if (key == "timeRange"){
		return new sdl::imtbase::ImtBaseTypes::CTimeRangeObject(this);
	}
	return nullptr;
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


bool CFieldSortingInfo::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!fieldId){
		return false;
	}
	gqlObject.InsertParam("fieldId", QVariant(*fieldId));

	if (!sortingOrder){
		return false;
	}
	gqlObject.InsertParam("sortingOrder", QVariant(*sortingOrder));

	return true;
}


bool CFieldSortingInfo::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("fieldId") || (gqlObject["fieldId"].userType() != QMetaType::QString && gqlObject["fieldId"].userType() != QMetaType::QByteArray)){
		return false;
	}
	fieldId = gqlObject["fieldId"].toString();

	if (!gqlObject.ContainsParam("sortingOrder") || (gqlObject["sortingOrder"].userType() != QMetaType::QString && gqlObject["sortingOrder"].userType() != QMetaType::QByteArray)){
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


CFieldSortingInfoObject::CFieldSortingInfoObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CFieldSortingInfoObject::fieldIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldSortingInfoObject::sortingOrderChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CFieldSortingInfoObject::GetFieldId()
{
	if (Version_1_0->fieldId.has_value()){
		return Version_1_0->fieldId.value();
	}

	return QString();
}


void CFieldSortingInfoObject::SetFieldId(QString v)
{
	Version_1_0->fieldId = v;
	fieldIdChanged();
}


bool CFieldSortingInfoObject::hasFieldId()
{
	 return Version_1_0->fieldId.HasValue();
}


QString CFieldSortingInfoObject::GetSortingOrder()
{
	if (Version_1_0->sortingOrder.has_value()){
		return Version_1_0->sortingOrder.value();
	}

	return QString();
}


void CFieldSortingInfoObject::SetSortingOrder(QString v)
{
	Version_1_0->sortingOrder = v;
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


QObject* CFieldSortingInfoObject::CreateObject(const QString& key)
{
	return nullptr;
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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ValueType", filterValueTypeStringValue);)

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
		return false;
	}
	gqlObject.InsertParam("fieldId", QVariant(*fieldId));

	if (!filterValue){
		return false;
	}
	gqlObject.InsertParam("filterValue", QVariant(*filterValue));

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
	gqlObject.InsertParam("filterValueType", QVariant(filterValueTypeStringValue));

	QVariantList filterOperationsDataObjectList;
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
		filterOperationsDataObjectList << filterOperationsStringValue;
	}
	gqlObject.InsertParam("filterOperations", filterOperationsDataObjectList);

	return true;
}


bool CFieldFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("fieldId") || (gqlObject["fieldId"].userType() != QMetaType::QString && gqlObject["fieldId"].userType() != QMetaType::QByteArray)){
		return false;
	}
	fieldId = gqlObject["fieldId"].toByteArray();

	if (!gqlObject.ContainsParam("filterValue") || (gqlObject["filterValue"].userType() != QMetaType::QString && gqlObject["filterValue"].userType() != QMetaType::QByteArray)){
		return false;
	}
	filterValue = gqlObject["filterValue"].toString();

	if (!gqlObject.ContainsParam("filterValueType") || (gqlObject["filterValueType"].userType() != QMetaType::QString && gqlObject["filterValueType"].userType() != QMetaType::QByteArray)){
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
		return false;
	}
	const QVariant filterOperationsData = gqlObject["filterOperations"];
	const QVariantList filterOperationsDataList = filterOperationsData.toList();
	const qsizetype filterOperationsElementsCount = filterOperationsDataList.size();
	filterOperations = QList<imtbase::ComplexCollectionFilter::FilterOperation>();
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

	if (gqlObject.ContainsParam("filterOperations") && (!gqlObject["filterOperations"].isNull())){
		const QVariant filterOperationsData = gqlObject["filterOperations"];
		const QVariantList filterOperationsDataList = filterOperationsData.toList();
		const qsizetype filterOperationsElementsCount = filterOperationsDataList.size();
		filterOperations = QList<imtbase::ComplexCollectionFilter::FilterOperation>();
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
		return false;
	}
	const QJsonArray filterOperationsJsonArray = jsonObject["filterOperations"].toArray();
	const qsizetype filterOperationsArrayCount = filterOperationsJsonArray.size();
	filterOperations = QList<imtbase::ComplexCollectionFilter::FilterOperation>();
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
		filterOperations = QList<imtbase::ComplexCollectionFilter::FilterOperation>();
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


CFieldFilterObject::CFieldFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CFieldFilterObject::fieldIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldFilterObject::filterValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldFilterObject::filterValueTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldFilterObject::filterOperationsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CFieldFilterObject::GetFieldId()
{
	if (Version_1_0->fieldId.has_value()){
		return Version_1_0->fieldId.value();
	}

	return QString();
}


void CFieldFilterObject::SetFieldId(QString v)
{
	Version_1_0->fieldId = v.toUtf8();
	fieldIdChanged();
}


bool CFieldFilterObject::hasFieldId()
{
	 return Version_1_0->fieldId.HasValue();
}


QString CFieldFilterObject::GetFilterValue()
{
	if (Version_1_0->filterValue.has_value()){
		return Version_1_0->filterValue.value();
	}

	return QString();
}


void CFieldFilterObject::SetFilterValue(QString v)
{
	Version_1_0->filterValue = v;
	filterValueChanged();
}


bool CFieldFilterObject::hasFilterValue()
{
	 return Version_1_0->filterValue.HasValue();
}


QString CFieldFilterObject::GetFilterValueType()
{
	if (Version_1_0->filterValueType.has_value()){
		sdl::imtbase::ComplexCollectionFilter::ValueType valueType = Version_1_0->filterValueType.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::imtbase::ComplexCollectionFilter::ValueType>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CFieldFilterObject::SetFilterValueType(QString v)
{
	Version_1_0->filterValueType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::imtbase::ComplexCollectionFilter::ValueType>();
	int key = metaEnum.keyToValue(v.toUtf8());
	if (key > -1){
		Version_1_0->filterValueType = (sdl::imtbase::ComplexCollectionFilter::ValueType)key;
	}
	filterValueTypeChanged();
}


bool CFieldFilterObject::hasFilterValueType()
{
	 return Version_1_0->filterValueType.HasValue();
}


QList<QString> CFieldFilterObject::GetFilterOperations()
{
	if (Version_1_0->filterOperations.has_value()){
		
	}

	return QStringList();
}


void CFieldFilterObject::SetFilterOperations(QList<QString> v)
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


QObject* CFieldFilterObject::CreateObject(const QString& key)
{
	return nullptr;
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
			if (!fieldFilters->at(fieldFiltersIndex).WriteToGraphQlObject(newFieldFiltersGqlObject)){
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
			if (!groupFilters->at(groupFiltersIndex).WriteToGraphQlObject(newGroupFiltersGqlObject)){
				return false;
			}
			groupFiltersDataObjectList << newGroupFiltersGqlObject;
		}
		gqlObject.InsertParam("groupFilters", groupFiltersDataObjectList);
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
	gqlObject.InsertParam("logicalOperation", QVariant(logicalOperationStringValue));

	return true;
}


bool CGroupFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("fieldFilters") && (gqlObject.GetObjectsCount("fieldFilters") > 0)){
		const qsizetype fieldFiltersElementsCount = gqlObject.GetObjectsCount("fieldFilters");
		fieldFilters = QList<CFieldFilter::V1_0>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersElementsCount; ++fieldFiltersIndex){
			const ::imtgql::CGqlParamObject* fieldFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("fieldFilters", fieldFiltersIndex);
			if (fieldFiltersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << fieldFiltersDataObjectPtr;
				return false;
			}
			CFieldFilter::V1_0 tempFieldFilters;
			if (!tempFieldFilters.ReadFromGraphQlObject(*fieldFiltersDataObjectPtr)){
				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (gqlObject.ContainsParam("groupFilters") && (gqlObject.GetObjectsCount("groupFilters") > 0)){
		const qsizetype groupFiltersElementsCount = gqlObject.GetObjectsCount("groupFilters");
		groupFilters = QList<CGroupFilter::V1_0>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersElementsCount; ++groupFiltersIndex){
			const ::imtgql::CGqlParamObject* groupFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("groupFilters", groupFiltersIndex);
			if (groupFiltersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << groupFiltersDataObjectPtr;
				return false;
			}
			CGroupFilter::V1_0 tempGroupFilters;
			if (!tempGroupFilters.ReadFromGraphQlObject(*groupFiltersDataObjectPtr)){
				return false;
			}
			groupFilters->append(tempGroupFilters);
		}
	}

	if (!gqlObject.ContainsParam("logicalOperation") || (gqlObject["logicalOperation"].userType() != QMetaType::QString && gqlObject["logicalOperation"].userType() != QMetaType::QByteArray)){
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
	if (gqlObject.ContainsParam("fieldFilters") && (gqlObject.GetObjectsCount("fieldFilters") > 0)){
		const qsizetype fieldFiltersElementsCount = gqlObject.GetObjectsCount("fieldFilters");
		fieldFilters = QList<CFieldFilter::V1_0>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersElementsCount; ++fieldFiltersIndex){
			const ::imtgql::CGqlParamObject* fieldFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("fieldFilters", fieldFiltersIndex);
			if (fieldFiltersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << fieldFiltersDataObjectPtr;
				return false;
			}
			CFieldFilter::V1_0 tempFieldFilters;
			if (!tempFieldFilters.OptReadFromGraphQlObject(*fieldFiltersDataObjectPtr)){
				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (gqlObject.ContainsParam("groupFilters") && (gqlObject.GetObjectsCount("groupFilters") > 0)){
		const qsizetype groupFiltersElementsCount = gqlObject.GetObjectsCount("groupFilters");
		groupFilters = QList<CGroupFilter::V1_0>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersElementsCount; ++groupFiltersIndex){
			const ::imtgql::CGqlParamObject* groupFiltersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("groupFilters", groupFiltersIndex);
			if (groupFiltersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << groupFiltersDataObjectPtr;
				return false;
			}
			CGroupFilter::V1_0 tempGroupFilters;
			if (!tempGroupFilters.OptReadFromGraphQlObject(*groupFiltersDataObjectPtr)){
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
		const QJsonArray fieldFiltersJsonArray = jsonObject["fieldFilters"].toArray();
		const qsizetype fieldFiltersArrayCount = fieldFiltersJsonArray.size();
		fieldFilters = QList<CFieldFilter::V1_0>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersArrayCount; ++fieldFiltersIndex){
			CFieldFilter::V1_0 tempFieldFilters;
			if (!tempFieldFilters.ReadFromJsonObject(fieldFiltersJsonArray[fieldFiltersIndex].toObject())){
				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (jsonObject.contains("groupFilters") && jsonObject["groupFilters"].isArray()){
		const QJsonArray groupFiltersJsonArray = jsonObject["groupFilters"].toArray();
		const qsizetype groupFiltersArrayCount = groupFiltersJsonArray.size();
		groupFilters = QList<CGroupFilter::V1_0>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersArrayCount; ++groupFiltersIndex){
			CGroupFilter::V1_0 tempGroupFilters;
			if (!tempGroupFilters.ReadFromJsonObject(groupFiltersJsonArray[groupFiltersIndex].toObject())){
				return false;
			}
			groupFilters->append(tempGroupFilters);
		}
	}

	if (!jsonObject.contains("logicalOperation") || ! jsonObject["logicalOperation"].isString()){
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
		fieldFilters = QList<CFieldFilter::V1_0>();
		for (qsizetype fieldFiltersIndex = 0; fieldFiltersIndex < fieldFiltersArrayCount; ++fieldFiltersIndex){
			CFieldFilter::V1_0 tempFieldFilters;
			if (!tempFieldFilters.OptReadFromJsonObject(fieldFiltersJsonArray[fieldFiltersIndex].toObject())){
				return false;
			}
			fieldFilters->append(tempFieldFilters);
		}
	}

	if (jsonObject.contains("groupFilters") && jsonObject["groupFilters"].isArray()){
		const QJsonArray groupFiltersJsonArray = jsonObject["groupFilters"].toArray();
		const qsizetype groupFiltersArrayCount = groupFiltersJsonArray.size();
		groupFilters = QList<CGroupFilter::V1_0>();
		for (qsizetype groupFiltersIndex = 0; groupFiltersIndex < groupFiltersArrayCount; ++groupFiltersIndex){
			CGroupFilter::V1_0 tempGroupFilters;
			if (!tempGroupFilters.OptReadFromJsonObject(groupFiltersJsonArray[groupFiltersIndex].toObject())){
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


CGroupFilterObject::CGroupFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_fieldFiltersQObjectPtr(nullptr)
			, m_groupFiltersQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CGroupFilterObject::fieldFiltersChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGroupFilterObject::groupFiltersChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGroupFilterObject::logicalOperationChanged, this, &CItemModelBase::OnInternalModelChanged);
}


sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* CGroupFilterObject::GetFieldFilters()
{
	if (Version_1_0->fieldFilters.has_value()){
		if (!m_fieldFiltersQObjectPtr){
			m_fieldFiltersQObjectPtr = dynamic_cast<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList*>(CreateObject("fieldFilters"));
			m_fieldFiltersQObjectPtr->Version_1_0 = Version_1_0->fieldFilters;
		}
		return m_fieldFiltersQObjectPtr;
	}

	return nullptr;
}


void CGroupFilterObject::SetFieldFilters(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* v)
{
	if (v){
		Version_1_0->fieldFilters = v->Version_1_0;
		m_fieldFiltersQObjectPtr = v;
	}
	else {
		Version_1_0->fieldFilters = nullptr;
	}

	fieldFiltersChanged();
}


bool CGroupFilterObject::hasFieldFilters()
{
	 return Version_1_0->fieldFilters.HasValue();
}


void CGroupFilterObject::createFieldFilters()
{	Version_1_0->fieldFilters.emplace();

}


sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* CGroupFilterObject::GetGroupFilters()
{
	if (Version_1_0->groupFilters.has_value()){
		if (!m_groupFiltersQObjectPtr){
			m_groupFiltersQObjectPtr = dynamic_cast<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList*>(CreateObject("groupFilters"));
			m_groupFiltersQObjectPtr->Version_1_0 = Version_1_0->groupFilters;
		}
		return m_groupFiltersQObjectPtr;
	}

	return nullptr;
}


void CGroupFilterObject::SetGroupFilters(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* v)
{
	if (v){
		Version_1_0->groupFilters = v->Version_1_0;
		m_groupFiltersQObjectPtr = v;
	}
	else {
		Version_1_0->groupFilters = nullptr;
	}

	groupFiltersChanged();
}


bool CGroupFilterObject::hasGroupFilters()
{
	 return Version_1_0->groupFilters.HasValue();
}


void CGroupFilterObject::createGroupFilters()
{	Version_1_0->groupFilters.emplace();

}


QString CGroupFilterObject::GetLogicalOperation()
{
	if (Version_1_0->logicalOperation.has_value()){
		sdl::imtbase::ComplexCollectionFilter::LogicalOperation valueType = Version_1_0->logicalOperation.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::imtbase::ComplexCollectionFilter::LogicalOperation>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CGroupFilterObject::SetLogicalOperation(QString v)
{
	Version_1_0->logicalOperation.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::imtbase::ComplexCollectionFilter::LogicalOperation>();
	int key = metaEnum.keyToValue(v.toUtf8());
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


QObject* CGroupFilterObject::CreateObject(const QString& key)
{
	if (key == "fieldFilters"){
		return new sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList(this);
	}
	if (key == "groupFilters"){
		return new sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList(this);
	}
	return nullptr;
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


bool CComplexCollectionFilter::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (sortingInfo){
		QList<::imtgql::CGqlParamObject> sortingInfoDataObjectList;
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfo->size(); ++sortingInfoIndex){
			::imtgql::CGqlParamObject newSortingInfoGqlObject;
			if (!sortingInfo->at(sortingInfoIndex).WriteToGraphQlObject(newSortingInfoGqlObject)){
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
			return false;
		}
		gqlObject.InsertParam("fieldsFilter", fieldsFilterGqlObject);
	}

	if (timeFilter){
		::imtgql::CGqlParamObject timeFilterGqlObject;
		const bool isTimeFilterAdded = timeFilter->WriteToGraphQlObject(timeFilterGqlObject);
		if (!isTimeFilterAdded){
			return false;
		}
		gqlObject.InsertParam("timeFilter", timeFilterGqlObject);
	}

	if (distinctFields){
		QVariantList distinctFieldsDataObjectList;
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFields->size(); ++distinctFieldsIndex){
			distinctFieldsDataObjectList << distinctFields->at(distinctFieldsIndex);
		}
		gqlObject.InsertParam("distinctFields", distinctFieldsDataObjectList);
	}

	return true;
}


bool CComplexCollectionFilter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("sortingInfo") && (gqlObject.GetObjectsCount("sortingInfo") > 0)){
		const qsizetype sortingInfoElementsCount = gqlObject.GetObjectsCount("sortingInfo");
		sortingInfo = QList<CFieldSortingInfo::V1_0>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoElementsCount; ++sortingInfoIndex){
			const ::imtgql::CGqlParamObject* sortingInfoDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("sortingInfo", sortingInfoIndex);
			if (sortingInfoDataObjectPtr == nullptr){
				qDebug() << "invalid type" << sortingInfoDataObjectPtr;
				return false;
			}
			CFieldSortingInfo::V1_0 tempSortingInfo;
			if (!tempSortingInfo.ReadFromGraphQlObject(*sortingInfoDataObjectPtr)){
				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (gqlObject.ContainsParam("fieldsFilter") && (gqlObject.GetParamArgumentObjectPtr("fieldsFilter") != nullptr)){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("fieldsFilter"));
		if (!isFieldsFilterRead){
			return false;
		}
	}

	if (gqlObject.ContainsParam("timeFilter") && (gqlObject.GetParamArgumentObjectPtr("timeFilter") != nullptr)){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("timeFilter"));
		if (!isTimeFilterRead){
			return false;
		}
	}

	if (gqlObject.ContainsParam("distinctFields") && (!gqlObject["distinctFields"].isNull())){
		const QVariant distinctFieldsData = gqlObject["distinctFields"];
		const QVariantList distinctFieldsDataList = distinctFieldsData.toList();
		const qsizetype distinctFieldsElementsCount = distinctFieldsDataList.size();
		distinctFields = QList<QByteArray>();
		for (qsizetype distinctFieldsIndex = 0; distinctFieldsIndex < distinctFieldsElementsCount; ++distinctFieldsIndex){
			QByteArray tempDistinctFields = distinctFieldsDataList[distinctFieldsIndex].toByteArray();
			distinctFields->append(tempDistinctFields);
		}
	}

	return true;
}


bool CComplexCollectionFilter::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("sortingInfo") && (gqlObject.GetObjectsCount("sortingInfo") > 0)){
		const qsizetype sortingInfoElementsCount = gqlObject.GetObjectsCount("sortingInfo");
		sortingInfo = QList<CFieldSortingInfo::V1_0>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoElementsCount; ++sortingInfoIndex){
			const ::imtgql::CGqlParamObject* sortingInfoDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("sortingInfo", sortingInfoIndex);
			if (sortingInfoDataObjectPtr == nullptr){
				qDebug() << "invalid type" << sortingInfoDataObjectPtr;
				return false;
			}
			CFieldSortingInfo::V1_0 tempSortingInfo;
			if (!tempSortingInfo.OptReadFromGraphQlObject(*sortingInfoDataObjectPtr)){
				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (gqlObject.ContainsParam("fieldsFilter") && (gqlObject.GetParamArgumentObjectPtr("fieldsFilter") != nullptr)){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("fieldsFilter"));
		if (!isFieldsFilterRead){
			return false;
		}
	}

	if (gqlObject.ContainsParam("timeFilter") && (gqlObject.GetParamArgumentObjectPtr("timeFilter") != nullptr)){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("timeFilter"));
		if (!isTimeFilterRead){
			return false;
		}
	}

	if (gqlObject.ContainsParam("distinctFields") && (!gqlObject["distinctFields"].isNull())){
		const QVariant distinctFieldsData = gqlObject["distinctFields"];
		const QVariantList distinctFieldsDataList = distinctFieldsData.toList();
		const qsizetype distinctFieldsElementsCount = distinctFieldsDataList.size();
		distinctFields = QList<QByteArray>();
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
			if (!sortingInfo->at(sortingInfoIndex).WriteToJsonObject(newSortingInfoJsonObject)){
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
			return false;
		}
		jsonObject["fieldsFilter"] = fieldsFilterJsonObject;
	}

	if (timeFilter){
		QJsonObject timeFilterJsonObject;
		const bool isTimeFilterAdded = timeFilter->WriteToJsonObject(timeFilterJsonObject);
		if (!isTimeFilterAdded){
			return false;
		}
		jsonObject["timeFilter"] = timeFilterJsonObject;
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
		const QJsonArray sortingInfoJsonArray = jsonObject["sortingInfo"].toArray();
		const qsizetype sortingInfoArrayCount = sortingInfoJsonArray.size();
		sortingInfo = QList<CFieldSortingInfo::V1_0>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoArrayCount; ++sortingInfoIndex){
			CFieldSortingInfo::V1_0 tempSortingInfo;
			if (!tempSortingInfo.ReadFromJsonObject(sortingInfoJsonArray[sortingInfoIndex].toObject())){
				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (jsonObject.contains("fieldsFilter") && jsonObject["fieldsFilter"].isObject()){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->ReadFromJsonObject(jsonObject["fieldsFilter"].toObject());
		if (!isFieldsFilterRead){
			return false;
		}
	}

	if (jsonObject.contains("timeFilter") && jsonObject["timeFilter"].isObject()){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->ReadFromJsonObject(jsonObject["timeFilter"].toObject());
		if (!isTimeFilterRead){
			return false;
		}
	}

	if (jsonObject.contains("distinctFields") && jsonObject["distinctFields"].isArray()){
		const QJsonArray distinctFieldsJsonArray = jsonObject["distinctFields"].toArray();
		const qsizetype distinctFieldsArrayCount = distinctFieldsJsonArray.size();
		distinctFields = QList<QByteArray>();
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
		sortingInfo = QList<CFieldSortingInfo::V1_0>();
		for (qsizetype sortingInfoIndex = 0; sortingInfoIndex < sortingInfoArrayCount; ++sortingInfoIndex){
			CFieldSortingInfo::V1_0 tempSortingInfo;
			if (!tempSortingInfo.OptReadFromJsonObject(sortingInfoJsonArray[sortingInfoIndex].toObject())){
				return false;
			}
			sortingInfo->append(tempSortingInfo);
		}
	}

	if (jsonObject.contains("fieldsFilter") && jsonObject["fieldsFilter"].isObject()){
		fieldsFilter = CGroupFilter::V1_0();
		const bool isFieldsFilterRead = fieldsFilter->OptReadFromJsonObject(jsonObject["fieldsFilter"].toObject());
		if (!isFieldsFilterRead){
			return false;
		}
	}

	if (jsonObject.contains("timeFilter") && jsonObject["timeFilter"].isObject()){
		timeFilter = CTimeFilter::V1_0();
		const bool isTimeFilterRead = timeFilter->OptReadFromJsonObject(jsonObject["timeFilter"].toObject());
		if (!isTimeFilterRead){
			return false;
		}
	}

	if (jsonObject.contains("distinctFields") && jsonObject["distinctFields"].isArray()){
		const QJsonArray distinctFieldsJsonArray = jsonObject["distinctFields"].toArray();
		const qsizetype distinctFieldsArrayCount = distinctFieldsJsonArray.size();
		distinctFields = QList<QByteArray>();
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


CComplexCollectionFilterObject::CComplexCollectionFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_sortingInfoQObjectPtr(nullptr)
			, m_fieldsFilterQObjectPtr(nullptr)
			, m_timeFilterQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CComplexCollectionFilterObject::sortingInfoChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CComplexCollectionFilterObject::fieldsFilterChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CComplexCollectionFilterObject::timeFilterChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CComplexCollectionFilterObject::distinctFieldsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* CComplexCollectionFilterObject::GetSortingInfo()
{
	if (Version_1_0->sortingInfo.has_value()){
		if (!m_sortingInfoQObjectPtr){
			m_sortingInfoQObjectPtr = dynamic_cast<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList*>(CreateObject("sortingInfo"));
			m_sortingInfoQObjectPtr->Version_1_0 = Version_1_0->sortingInfo;
		}
		return m_sortingInfoQObjectPtr;
	}

	return nullptr;
}


void CComplexCollectionFilterObject::SetSortingInfo(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* v)
{
	if (v){
		Version_1_0->sortingInfo = v->Version_1_0;
		m_sortingInfoQObjectPtr = v;
	}
	else {
		Version_1_0->sortingInfo = nullptr;
	}

	sortingInfoChanged();
}


bool CComplexCollectionFilterObject::hasSortingInfo()
{
	 return Version_1_0->sortingInfo.HasValue();
}


void CComplexCollectionFilterObject::createSortingInfo()
{	Version_1_0->sortingInfo.emplace();

}


sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* CComplexCollectionFilterObject::GetFieldsFilter()
{
	if (Version_1_0->fieldsFilter.has_value()){
		if (!m_fieldsFilterQObjectPtr){
			m_fieldsFilterQObjectPtr = dynamic_cast<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject*>(CreateObject("fieldsFilter"));
			m_fieldsFilterQObjectPtr->Version_1_0 = Version_1_0->fieldsFilter;
		}
		return m_fieldsFilterQObjectPtr;
	}

	return nullptr;
}


void CComplexCollectionFilterObject::SetFieldsFilter(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* v)
{
	if (v){
		Version_1_0->fieldsFilter = v->Version_1_0;
		m_fieldsFilterQObjectPtr = v;
	}
	else {
		Version_1_0->fieldsFilter = nullptr;
	}

	fieldsFilterChanged();
}


bool CComplexCollectionFilterObject::hasFieldsFilter()
{
	 return Version_1_0->fieldsFilter.HasValue();
}


void CComplexCollectionFilterObject::createFieldsFilter()
{	Version_1_0->fieldsFilter.emplace();

}


sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* CComplexCollectionFilterObject::GetTimeFilter()
{
	if (Version_1_0->timeFilter.has_value()){
		if (!m_timeFilterQObjectPtr){
			m_timeFilterQObjectPtr = dynamic_cast<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject*>(CreateObject("timeFilter"));
			m_timeFilterQObjectPtr->Version_1_0 = Version_1_0->timeFilter;
		}
		return m_timeFilterQObjectPtr;
	}

	return nullptr;
}


void CComplexCollectionFilterObject::SetTimeFilter(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* v)
{
	if (v){
		Version_1_0->timeFilter = v->Version_1_0;
		m_timeFilterQObjectPtr = v;
	}
	else {
		Version_1_0->timeFilter = nullptr;
	}

	timeFilterChanged();
}


bool CComplexCollectionFilterObject::hasTimeFilter()
{
	 return Version_1_0->timeFilter.HasValue();
}


void CComplexCollectionFilterObject::createTimeFilter()
{	Version_1_0->timeFilter.emplace();

}


QList<QString> CComplexCollectionFilterObject::GetDistinctFields()
{
	if (Version_1_0->distinctFields.has_value()){
		QStringList tempDistinctFieldsList;
		for (const auto& tempValue: Version_1_0->distinctFields.value()){
			tempDistinctFieldsList << tempValue;
		}
		return tempDistinctFieldsList;
	}

	return QStringList();
}


void CComplexCollectionFilterObject::SetDistinctFields(QList<QString> v)
{
	Version_1_0->distinctFields = QList<QByteArray>(); 
	for (const auto& tempValue: v){
		Version_1_0->distinctFields->append(tempValue.toUtf8());
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


QObject* CComplexCollectionFilterObject::CreateObject(const QString& key)
{
	if (key == "sortingInfo"){
		return new sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList(this);
	}
	if (key == "fieldsFilter"){
		return new sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject(this);
	}
	if (key == "timeFilter"){
		return new sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject(this);
	}
	return nullptr;
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


} // namespace sdl::imtbase::ComplexCollectionFilter




