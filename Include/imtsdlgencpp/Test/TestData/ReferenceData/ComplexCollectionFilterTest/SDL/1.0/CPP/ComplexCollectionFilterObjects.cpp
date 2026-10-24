#include "ComplexCollectionFilterObjects.h"


namespace sdl::V1_0::imtbase
{


CTimeFilterObject::CTimeFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CTimeFilterObject::timeRangeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeFilterObject::timeUnitChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeFilterObject::interpretationModeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeFilterObject::unitMultiplierChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CTimeFilterObject::GetTimeRange()
{
	if (!timeRange) {return {};}
	
	if (!m_timeRangeQObjectPtr.isValid()){
		m_timeRangeQObjectPtr = CreateObject("timeRange");
		auto itemPtr = m_timeRangeQObjectPtr.value<sdl::V1_0::imtbase::CTimeRangeObject*>();
		if (itemPtr != nullptr) {
			itemPtr->Begin = timeRange->Begin;
			itemPtr->End = timeRange->End;
		}
	}
	return m_timeRangeQObjectPtr;
}


void CTimeFilterObject::SetTimeRange(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::imtbase::CTimeRangeObject* itemPtr = v.value<sdl::V1_0::imtbase::CTimeRangeObject*>();
		if (itemPtr != nullptr) CTimeFilter::timeRange = (*itemPtr);
	}
	else {
		CTimeFilter::timeRange.SetNull();
	}
	m_timeRangeQObjectPtr = v;

	timeRangeChanged();
}


bool CTimeFilterObject::hasTimeRange()
{
	 return CTimeFilter::timeRange.HasValue();
}


void CTimeFilterObject::emplaceTimeRange()
{
	ResetTimeRange();
	CTimeFilter::timeRange.emplace();
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
	return CTimeFilter::timeUnit ? CTimeFilter::timeUnit.value() : QVariant();
}


void CTimeFilterObject::SetTimeUnit(const QVariant& v)
{
	CTimeFilter::timeUnit = v.value<QString>();
	timeUnitChanged();
}


bool CTimeFilterObject::hasTimeUnit()
{
	 return CTimeFilter::timeUnit.HasValue();
}


QVariant CTimeFilterObject::GetInterpretationMode()
{
	return CTimeFilter::interpretationMode ? CTimeFilter::interpretationMode.value() : QVariant();
}


void CTimeFilterObject::SetInterpretationMode(const QVariant& v)
{
	CTimeFilter::interpretationMode = v.value<QString>();
	interpretationModeChanged();
}


bool CTimeFilterObject::hasInterpretationMode()
{
	 return CTimeFilter::interpretationMode.HasValue();
}


QVariant CTimeFilterObject::GetUnitMultiplier()
{
	return CTimeFilter::unitMultiplier ? CTimeFilter::unitMultiplier.value() : QVariant();
}


void CTimeFilterObject::SetUnitMultiplier(const QVariant& v)
{
	CTimeFilter::unitMultiplier = v.value<int>();
	unitMultiplierChanged();
}


bool CTimeFilterObject::hasUnitMultiplier()
{
	 return CTimeFilter::unitMultiplier.HasValue();
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
		return QVariant::fromValue(new sdl::V1_0::imtbase::CTimeRangeObject(this));
	}
	return QVariant();
}


QString CTimeFilterObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "timeRange")){
		return "timeRange";
	}
	if (propertyName == (QStringLiteral("m_") + "timeUnit")){
		return "timeUnit";
	}
	if (propertyName == (QStringLiteral("m_") + "interpretationMode")){
		return "interpretationMode";
	}
	if (propertyName == (QStringLiteral("m_") + "unitMultiplier")){
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


void CTimeFilterObjectList::append(sdl::V1_0::imtbase::CTimeFilterObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CTimeFilterObjectList* sdl::V1_0::imtbase::CTimeFilterObjectList::copyMe()
{
	sdl::V1_0::imtbase::CTimeFilterObjectList* retVal = new sdl::V1_0::imtbase::CTimeFilterObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CTimeFilterObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CTimeFilterObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CTimeFilterObjectList::addElement(sdl::V1_0::imtbase::CTimeFilterObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CTimeFilterObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CTimeFilterObjectList::isEqualWithModel(sdl::V1_0::imtbase::CTimeFilterObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CTimeFilterObjectList::insert(int index, sdl::V1_0::imtbase::CTimeFilterObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CTimeFilterObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CTimeFilterObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CTimeFilterObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CTimeFilterObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_timeRange"){
		return itemPtr->GetTimeRange();
	}
	if (nameId == "m_timeUnit"){
		return Version_1_0->at(index)->timeUnit ? QVariant::fromValue(Version_1_0->at(index)->timeUnit.value()) : QVariant();
	}
	if (nameId == "m_interpretationMode"){
		return Version_1_0->at(index)->interpretationMode ? QVariant::fromValue(Version_1_0->at(index)->interpretationMode.value()) : QVariant();
	}
	if (nameId == "m_unitMultiplier"){
		return Version_1_0->at(index)->unitMultiplier ? QVariant::fromValue(Version_1_0->at(index)->unitMultiplier.value()) : QVariant();
	}

	return QVariant();
}
CFieldSortingInfoObject::CFieldSortingInfoObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CFieldSortingInfoObject::fieldIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldSortingInfoObject::sortingOrderChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CFieldSortingInfoObject::GetFieldId()
{
	return CFieldSortingInfo::fieldId ? CFieldSortingInfo::fieldId.value() : QVariant();
}


void CFieldSortingInfoObject::SetFieldId(const QVariant& v)
{
	CFieldSortingInfo::fieldId = v.value<QString>();
	fieldIdChanged();
}


bool CFieldSortingInfoObject::hasFieldId()
{
	 return CFieldSortingInfo::fieldId.HasValue();
}


QVariant CFieldSortingInfoObject::GetSortingOrder()
{
	return CFieldSortingInfo::sortingOrder ? CFieldSortingInfo::sortingOrder.value() : QVariant();
}


void CFieldSortingInfoObject::SetSortingOrder(const QVariant& v)
{
	CFieldSortingInfo::sortingOrder = v.value<QString>();
	sortingOrderChanged();
}


bool CFieldSortingInfoObject::hasSortingOrder()
{
	 return CFieldSortingInfo::sortingOrder.HasValue();
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
	if (propertyName == (QStringLiteral("m_") + "fieldId")){
		return "fieldId";
	}
	if (propertyName == (QStringLiteral("m_") + "sortingOrder")){
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


void CFieldSortingInfoObjectList::append(sdl::V1_0::imtbase::CFieldSortingInfoObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CFieldSortingInfoObjectList* sdl::V1_0::imtbase::CFieldSortingInfoObjectList::copyMe()
{
	sdl::V1_0::imtbase::CFieldSortingInfoObjectList* retVal = new sdl::V1_0::imtbase::CFieldSortingInfoObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CFieldSortingInfoObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CFieldSortingInfoObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CFieldSortingInfoObjectList::addElement(sdl::V1_0::imtbase::CFieldSortingInfoObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CFieldSortingInfoObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CFieldSortingInfoObjectList::isEqualWithModel(sdl::V1_0::imtbase::CFieldSortingInfoObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CFieldSortingInfoObjectList::insert(int index, sdl::V1_0::imtbase::CFieldSortingInfoObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CFieldSortingInfoObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CFieldSortingInfoObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CFieldSortingInfoObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CFieldSortingInfoObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_fieldId"){
		return Version_1_0->at(index)->fieldId ? QVariant::fromValue(Version_1_0->at(index)->fieldId.value()) : QVariant();
	}
	if (nameId == "m_sortingOrder"){
		return Version_1_0->at(index)->sortingOrder ? QVariant::fromValue(Version_1_0->at(index)->sortingOrder.value()) : QVariant();
	}

	return QVariant();
}
CFieldFilterObject::CFieldFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CFieldFilterObject::fieldIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldFilterObject::filterValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldFilterObject::filterValueTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFieldFilterObject::filterOperationsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CFieldFilterObject::GetFieldId()
{
	return CFieldFilter::fieldId ? CFieldFilter::fieldId.value() : QVariant();
}


void CFieldFilterObject::SetFieldId(const QVariant& v)
{
	CFieldFilter::fieldId = v.value<QString>().toUtf8();
	fieldIdChanged();
}


bool CFieldFilterObject::hasFieldId()
{
	 return CFieldFilter::fieldId.HasValue();
}


QVariant CFieldFilterObject::GetFilterValue()
{
	return CFieldFilter::filterValue ? CFieldFilter::filterValue.value() : QVariant();
}


void CFieldFilterObject::SetFilterValue(const QVariant& v)
{
	CFieldFilter::filterValue = v.value<QString>();
	filterValueChanged();
}


bool CFieldFilterObject::hasFilterValue()
{
	 return CFieldFilter::filterValue.HasValue();
}


QVariant CFieldFilterObject::GetFilterValueType()
{
	if (!filterValueType) {return {};}
	sdl::V1_0::imtbase::ValueType valueType = CFieldFilter::filterValueType.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::imtbase::ValueType>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CFieldFilterObject::SetFilterValueType(const QVariant& v)
{
	CFieldFilter::filterValueType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::imtbase::ValueType>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CFieldFilter::filterValueType = (sdl::V1_0::imtbase::ValueType)key;
	}
	filterValueTypeChanged();
}


bool CFieldFilterObject::hasFilterValueType()
{
	 return CFieldFilter::filterValueType.HasValue();
}


QVariant CFieldFilterObject::GetFilterOperations()
{
	if (!filterOperations) {return {};}

	QVariantList tempFilterOperationsList;
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::imtbase::FilterOperation>();
	for (const auto& tempValue: filterOperations.value()){
		tempFilterOperationsList << (tempValue ? QVariant(QString::fromUtf8(metaEnum.valueToKey(int(*tempValue)))) : QVariant());
	}

	return tempFilterOperationsList;
}


void CFieldFilterObject::SetFilterOperations(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){
		return;
	}
	if (!CFieldFilter::filterOperations){
		CFieldFilter::filterOperations.emplace();
	}
	else{
		CFieldFilter::filterOperations->clear();
	}
	for (const QVariant& tempValue: v.toList()){
		if (!tempValue.isValid() || tempValue.isNull()){
			CFieldFilter::filterOperations->AppendNull();
			continue;
		}
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::imtbase::FilterOperation>();
		const int key = metaEnum.keyToValue(tempValue.toString().toUtf8());
		if (key < 0){continue;}
		istd::TNullableValue<sdl::V1_0::imtbase::FilterOperation> tempItem(static_cast<sdl::V1_0::imtbase::FilterOperation>(key));
		CFieldFilter::filterOperations->append(tempItem);
	}

	filterOperationsChanged();
}


bool CFieldFilterObject::hasFilterOperations()
{
	 return CFieldFilter::filterOperations.HasValue();
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
	if (propertyName == (QStringLiteral("m_") + "fieldId")){
		return "fieldId";
	}
	if (propertyName == (QStringLiteral("m_") + "filterValue")){
		return "filterValue";
	}
	if (propertyName == (QStringLiteral("m_") + "filterValueType")){
		return "filterValueType";
	}
	if (propertyName == (QStringLiteral("m_") + "filterOperations")){
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


void CFieldFilterObjectList::append(sdl::V1_0::imtbase::CFieldFilterObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CFieldFilterObjectList* sdl::V1_0::imtbase::CFieldFilterObjectList::copyMe()
{
	sdl::V1_0::imtbase::CFieldFilterObjectList* retVal = new sdl::V1_0::imtbase::CFieldFilterObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CFieldFilterObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CFieldFilterObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CFieldFilterObjectList::addElement(sdl::V1_0::imtbase::CFieldFilterObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CFieldFilterObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CFieldFilterObjectList::isEqualWithModel(sdl::V1_0::imtbase::CFieldFilterObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CFieldFilterObjectList::insert(int index, sdl::V1_0::imtbase::CFieldFilterObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CFieldFilterObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CFieldFilterObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CFieldFilterObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CFieldFilterObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_fieldId"){
		return Version_1_0->at(index)->fieldId ? QVariant::fromValue(Version_1_0->at(index)->fieldId.value()) : QVariant();
	}
	if (nameId == "m_filterValue"){
		return Version_1_0->at(index)->filterValue ? QVariant::fromValue(Version_1_0->at(index)->filterValue.value()) : QVariant();
	}
	if (nameId == "m_filterValueType"){
		return Version_1_0->at(index)->filterValueType ? QVariant::fromValue(Version_1_0->at(index)->filterValueType.value()) : QVariant();
	}
	if (nameId == "m_filterOperations"){
		return Version_1_0->at(index)->filterOperations ? QVariant::fromValue(Version_1_0->at(index)->filterOperations.value()) : QVariant();
	}

	return QVariant();
}
CGroupFilterObject::CGroupFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CGroupFilterObject::fieldFiltersChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGroupFilterObject::groupFiltersChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGroupFilterObject::logicalOperationChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CGroupFilterObject::GetFieldFilters()
{
	if (!fieldFilters) {return {};}
	if (!m_fieldFiltersQObjectPtr.isValid()){
		m_fieldFiltersQObjectPtr = CreateObject("fieldFilters");
		auto itemPtr = m_fieldFiltersQObjectPtr.value<sdl::V1_0::imtbase::CFieldFilterObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CGroupFilter::fieldFilters;
	}
	return m_fieldFiltersQObjectPtr;
}


void CGroupFilterObject::SetFieldFilters(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::imtbase::CFieldFilterObjectList* itemPtr = v.value<sdl::V1_0::imtbase::CFieldFilterObjectList*>();
		if (itemPtr != nullptr){
			CGroupFilter::fieldFilters = itemPtr->Version_1_0;
		}
	}
	else {
		CGroupFilter::fieldFilters.SetNull();
	}
	m_fieldFiltersQObjectPtr = v;

	fieldFiltersChanged();
}


bool CGroupFilterObject::hasFieldFilters()
{
	 return CGroupFilter::fieldFilters.HasValue();
}


void CGroupFilterObject::emplaceFieldFilters()
{
	ResetFieldFilters();
	CGroupFilter::fieldFilters.emplace();
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
	return QVariant::fromValue(new sdl::V1_0::imtbase::CFieldFilterObject());
}


QVariant CGroupFilterObject::GetGroupFilters()
{
	if (!groupFilters) {return {};}
	if (!m_groupFiltersQObjectPtr.isValid()){
		m_groupFiltersQObjectPtr = CreateObject("groupFilters");
		auto itemPtr = m_groupFiltersQObjectPtr.value<sdl::V1_0::imtbase::CGroupFilterObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CGroupFilter::groupFilters;
	}
	return m_groupFiltersQObjectPtr;
}


void CGroupFilterObject::SetGroupFilters(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::imtbase::CGroupFilterObjectList* itemPtr = v.value<sdl::V1_0::imtbase::CGroupFilterObjectList*>();
		if (itemPtr != nullptr){
			CGroupFilter::groupFilters = itemPtr->Version_1_0;
		}
	}
	else {
		CGroupFilter::groupFilters.SetNull();
	}
	m_groupFiltersQObjectPtr = v;

	groupFiltersChanged();
}


bool CGroupFilterObject::hasGroupFilters()
{
	 return CGroupFilter::groupFilters.HasValue();
}


void CGroupFilterObject::emplaceGroupFilters()
{
	ResetGroupFilters();
	CGroupFilter::groupFilters.emplace();
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
	return QVariant::fromValue(new sdl::V1_0::imtbase::CGroupFilterObject());
}


QVariant CGroupFilterObject::GetLogicalOperation()
{
	if (!logicalOperation) {return {};}
	sdl::V1_0::imtbase::LogicalOperation valueType = CGroupFilter::logicalOperation.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::imtbase::LogicalOperation>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CGroupFilterObject::SetLogicalOperation(const QVariant& v)
{
	CGroupFilter::logicalOperation.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::imtbase::LogicalOperation>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CGroupFilter::logicalOperation = (sdl::V1_0::imtbase::LogicalOperation)key;
	}
	logicalOperationChanged();
}


bool CGroupFilterObject::hasLogicalOperation()
{
	 return CGroupFilter::logicalOperation.HasValue();
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
		return QVariant::fromValue(new sdl::V1_0::imtbase::CFieldFilterObjectList(this));
	}
	if (key == "groupFilters"){
		return QVariant::fromValue(new sdl::V1_0::imtbase::CGroupFilterObjectList(this));
	}
	return QVariant();
}


QString CGroupFilterObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "fieldFilters")){
		return "fieldFilters";
	}
	if (propertyName == (QStringLiteral("m_") + "groupFilters")){
		return "groupFilters";
	}
	if (propertyName == (QStringLiteral("m_") + "logicalOperation")){
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


void CGroupFilterObjectList::append(sdl::V1_0::imtbase::CGroupFilterObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CGroupFilterObjectList* sdl::V1_0::imtbase::CGroupFilterObjectList::copyMe()
{
	sdl::V1_0::imtbase::CGroupFilterObjectList* retVal = new sdl::V1_0::imtbase::CGroupFilterObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CGroupFilterObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CGroupFilterObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CGroupFilterObjectList::addElement(sdl::V1_0::imtbase::CGroupFilterObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CGroupFilterObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CGroupFilterObjectList::isEqualWithModel(sdl::V1_0::imtbase::CGroupFilterObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CGroupFilterObjectList::insert(int index, sdl::V1_0::imtbase::CGroupFilterObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CGroupFilterObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CGroupFilterObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CGroupFilterObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CGroupFilterObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
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
		return Version_1_0->at(index)->logicalOperation ? QVariant::fromValue(Version_1_0->at(index)->logicalOperation.value()) : QVariant();
	}

	return QVariant();
}
CComplexCollectionFilterObject::CComplexCollectionFilterObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CComplexCollectionFilterObject::sortingInfoChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CComplexCollectionFilterObject::fieldsFilterChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CComplexCollectionFilterObject::timeFilterChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CComplexCollectionFilterObject::distinctFieldsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CComplexCollectionFilterObject::GetSortingInfo()
{
	if (!sortingInfo) {return {};}
	if (!m_sortingInfoQObjectPtr.isValid()){
		m_sortingInfoQObjectPtr = CreateObject("sortingInfo");
		auto itemPtr = m_sortingInfoQObjectPtr.value<sdl::V1_0::imtbase::CFieldSortingInfoObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CComplexCollectionFilter::sortingInfo;
	}
	return m_sortingInfoQObjectPtr;
}


void CComplexCollectionFilterObject::SetSortingInfo(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::imtbase::CFieldSortingInfoObjectList* itemPtr = v.value<sdl::V1_0::imtbase::CFieldSortingInfoObjectList*>();
		if (itemPtr != nullptr){
			CComplexCollectionFilter::sortingInfo = itemPtr->Version_1_0;
		}
	}
	else {
		CComplexCollectionFilter::sortingInfo.SetNull();
	}
	m_sortingInfoQObjectPtr = v;

	sortingInfoChanged();
}


bool CComplexCollectionFilterObject::hasSortingInfo()
{
	 return CComplexCollectionFilter::sortingInfo.HasValue();
}


void CComplexCollectionFilterObject::emplaceSortingInfo()
{
	ResetSortingInfo();
	CComplexCollectionFilter::sortingInfo.emplace();
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
	return QVariant::fromValue(new sdl::V1_0::imtbase::CFieldSortingInfoObject());
}


QVariant CComplexCollectionFilterObject::GetFieldsFilter()
{
	if (!fieldsFilter) {return {};}
	
	if (!m_fieldsFilterQObjectPtr.isValid()){
		m_fieldsFilterQObjectPtr = CreateObject("fieldsFilter");
		auto itemPtr = m_fieldsFilterQObjectPtr.value<sdl::V1_0::imtbase::CGroupFilterObject*>();
		if (itemPtr != nullptr) {
			itemPtr->fieldFilters = fieldsFilter->fieldFilters;
			itemPtr->groupFilters = fieldsFilter->groupFilters;
			itemPtr->logicalOperation = fieldsFilter->logicalOperation;
		}
	}
	return m_fieldsFilterQObjectPtr;
}


void CComplexCollectionFilterObject::SetFieldsFilter(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::imtbase::CGroupFilterObject* itemPtr = v.value<sdl::V1_0::imtbase::CGroupFilterObject*>();
		if (itemPtr != nullptr) CComplexCollectionFilter::fieldsFilter = (*itemPtr);
	}
	else {
		CComplexCollectionFilter::fieldsFilter.SetNull();
	}
	m_fieldsFilterQObjectPtr = v;

	fieldsFilterChanged();
}


bool CComplexCollectionFilterObject::hasFieldsFilter()
{
	 return CComplexCollectionFilter::fieldsFilter.HasValue();
}


void CComplexCollectionFilterObject::emplaceFieldsFilter()
{
	ResetFieldsFilter();
	CComplexCollectionFilter::fieldsFilter.emplace();
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
	if (!timeFilter) {return {};}
	
	if (!m_timeFilterQObjectPtr.isValid()){
		m_timeFilterQObjectPtr = CreateObject("timeFilter");
		auto itemPtr = m_timeFilterQObjectPtr.value<sdl::V1_0::imtbase::CTimeFilterObject*>();
		if (itemPtr != nullptr) {
			itemPtr->timeRange = timeFilter->timeRange;
			itemPtr->timeUnit = timeFilter->timeUnit;
			itemPtr->interpretationMode = timeFilter->interpretationMode;
			itemPtr->unitMultiplier = timeFilter->unitMultiplier;
		}
	}
	return m_timeFilterQObjectPtr;
}


void CComplexCollectionFilterObject::SetTimeFilter(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::imtbase::CTimeFilterObject* itemPtr = v.value<sdl::V1_0::imtbase::CTimeFilterObject*>();
		if (itemPtr != nullptr) CComplexCollectionFilter::timeFilter = (*itemPtr);
	}
	else {
		CComplexCollectionFilter::timeFilter.SetNull();
	}
	m_timeFilterQObjectPtr = v;

	timeFilterChanged();
}


bool CComplexCollectionFilterObject::hasTimeFilter()
{
	 return CComplexCollectionFilter::timeFilter.HasValue();
}


void CComplexCollectionFilterObject::emplaceTimeFilter()
{
	ResetTimeFilter();
	CComplexCollectionFilter::timeFilter.emplace();
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
	if (!distinctFields ){return {};}

	QVariantList tempDistinctFieldsList;
	for (const auto& tempValue: distinctFields.value()){
		tempDistinctFieldsList << (tempValue ? QVariant(QString::fromUtf8(*tempValue)) : QVariant());
	}

	return tempDistinctFieldsList;
}


void CComplexCollectionFilterObject::SetDistinctFields(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){
		CComplexCollectionFilter::distinctFields.SetNull();
		distinctFieldsChanged();
		return;
	}
	if (!CComplexCollectionFilter::distinctFields){
		CComplexCollectionFilter::distinctFields.emplace();
	}
	else{
		CComplexCollectionFilter::distinctFields->clear();
	}
	for (const QVariant& tempValue: v.toList()){
		if (!tempValue.isValid() || tempValue.isNull()){
			CComplexCollectionFilter::distinctFields->AppendNull();
			continue;
		}
		istd::TNullableValue<QByteArray> tempItem(tempValue.toString().toUtf8());
		CComplexCollectionFilter::distinctFields->append(tempItem);
	}

	distinctFieldsChanged();
}


bool CComplexCollectionFilterObject::hasDistinctFields()
{
	 return CComplexCollectionFilter::distinctFields.HasValue();
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
		return QVariant::fromValue(new sdl::V1_0::imtbase::CFieldSortingInfoObjectList(this));
	}
	if (key == "fieldsFilter"){
		return QVariant::fromValue(new sdl::V1_0::imtbase::CGroupFilterObject(this));
	}
	if (key == "timeFilter"){
		return QVariant::fromValue(new sdl::V1_0::imtbase::CTimeFilterObject(this));
	}
	return QVariant();
}


QString CComplexCollectionFilterObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "sortingInfo")){
		return "sortingInfo";
	}
	if (propertyName == (QStringLiteral("m_") + "fieldsFilter")){
		return "fieldsFilter";
	}
	if (propertyName == (QStringLiteral("m_") + "timeFilter")){
		return "timeFilter";
	}
	if (propertyName == (QStringLiteral("m_") + "distinctFields")){
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


void CComplexCollectionFilterObjectList::append(sdl::V1_0::imtbase::CComplexCollectionFilterObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CComplexCollectionFilterObjectList* sdl::V1_0::imtbase::CComplexCollectionFilterObjectList::copyMe()
{
	sdl::V1_0::imtbase::CComplexCollectionFilterObjectList* retVal = new sdl::V1_0::imtbase::CComplexCollectionFilterObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CComplexCollectionFilterObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CComplexCollectionFilterObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CComplexCollectionFilterObjectList::addElement(sdl::V1_0::imtbase::CComplexCollectionFilterObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CComplexCollectionFilterObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CComplexCollectionFilterObjectList::isEqualWithModel(sdl::V1_0::imtbase::CComplexCollectionFilterObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CComplexCollectionFilterObjectList::insert(int index, sdl::V1_0::imtbase::CComplexCollectionFilterObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CComplexCollectionFilterObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CComplexCollectionFilterObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CComplexCollectionFilterObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CComplexCollectionFilterObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
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
		return Version_1_0->at(index)->distinctFields ? QVariant::fromValue(Version_1_0->at(index)->distinctFields.value()) : QVariant();
	}

	return QVariant();
}
} // namespace sdl::V1_0::imtbase
