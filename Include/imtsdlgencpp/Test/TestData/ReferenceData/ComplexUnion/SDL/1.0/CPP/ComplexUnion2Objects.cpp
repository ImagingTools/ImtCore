#include "ComplexUnion2Objects.h"


namespace sdl::V1_0::complextest
{


CResultObject::CResultObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CResultObject::areaResultChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::expectedMinValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::expectedMaxValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::measuredValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::minMeasuredValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::maxMeasuredValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::meanMeasuredValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::measurementTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::measurementUnitChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::lengthChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::errorTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::geometryChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CResultObject::GetAreaResult()
{
	if (!AreaResult) {return {};}
	sdl::V1_0::complextest::StatusCode valueType = CResult::AreaResult.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CResultObject::SetAreaResult(const QVariant& v)
{
	CResult::AreaResult.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CResult::AreaResult = (sdl::V1_0::complextest::StatusCode)key;
	}
	areaResultChanged();
}


bool CResultObject::hasAreaResult()
{
	 return CResult::AreaResult.HasValue();
}


QVariant CResultObject::GetExpectedMinValue()
{
	return CResult::ExpectedMinValue ? CResult::ExpectedMinValue.value() : QVariant();
}


void CResultObject::SetExpectedMinValue(const QVariant& v)
{
	CResult::ExpectedMinValue = v.value<double>();
	expectedMinValueChanged();
}


bool CResultObject::hasExpectedMinValue()
{
	 return CResult::ExpectedMinValue.HasValue();
}


QVariant CResultObject::GetExpectedMaxValue()
{
	return CResult::ExpectedMaxValue ? CResult::ExpectedMaxValue.value() : QVariant();
}


void CResultObject::SetExpectedMaxValue(const QVariant& v)
{
	CResult::ExpectedMaxValue = v.value<double>();
	expectedMaxValueChanged();
}


bool CResultObject::hasExpectedMaxValue()
{
	 return CResult::ExpectedMaxValue.HasValue();
}


QVariant CResultObject::GetMeasuredValue()
{
	return CResult::MeasuredValue ? CResult::MeasuredValue.value() : QVariant();
}


void CResultObject::SetMeasuredValue(const QVariant& v)
{
	CResult::MeasuredValue = v.value<double>();
	measuredValueChanged();
}


bool CResultObject::hasMeasuredValue()
{
	 return CResult::MeasuredValue.HasValue();
}


QVariant CResultObject::GetMinMeasuredValue()
{
	return CResult::MinMeasuredValue ? CResult::MinMeasuredValue.value() : QVariant();
}


void CResultObject::SetMinMeasuredValue(const QVariant& v)
{
	CResult::MinMeasuredValue = v.value<double>();
	minMeasuredValueChanged();
}


bool CResultObject::hasMinMeasuredValue()
{
	 return CResult::MinMeasuredValue.HasValue();
}


QVariant CResultObject::GetMaxMeasuredValue()
{
	return CResult::MaxMeasuredValue ? CResult::MaxMeasuredValue.value() : QVariant();
}


void CResultObject::SetMaxMeasuredValue(const QVariant& v)
{
	CResult::MaxMeasuredValue = v.value<double>();
	maxMeasuredValueChanged();
}


bool CResultObject::hasMaxMeasuredValue()
{
	 return CResult::MaxMeasuredValue.HasValue();
}


QVariant CResultObject::GetMeanMeasuredValue()
{
	return CResult::MeanMeasuredValue ? CResult::MeanMeasuredValue.value() : QVariant();
}


void CResultObject::SetMeanMeasuredValue(const QVariant& v)
{
	CResult::MeanMeasuredValue = v.value<double>();
	meanMeasuredValueChanged();
}


bool CResultObject::hasMeanMeasuredValue()
{
	 return CResult::MeanMeasuredValue.HasValue();
}


QVariant CResultObject::GetMeasurementType()
{
	if (!MeasurementType) {return {};}
	sdl::V1_0::complextest::MeasurementType valueType = CResult::MeasurementType.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::MeasurementType>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CResultObject::SetMeasurementType(const QVariant& v)
{
	CResult::MeasurementType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::MeasurementType>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CResult::MeasurementType = (sdl::V1_0::complextest::MeasurementType)key;
	}
	measurementTypeChanged();
}


bool CResultObject::hasMeasurementType()
{
	 return CResult::MeasurementType.HasValue();
}


QVariant CResultObject::GetMeasurementUnit()
{
	if (!MeasurementUnit) {return {};}
	sdl::V1_0::complextest::MeasurementUnit valueType = CResult::MeasurementUnit.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::MeasurementUnit>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CResultObject::SetMeasurementUnit(const QVariant& v)
{
	CResult::MeasurementUnit.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::MeasurementUnit>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CResult::MeasurementUnit = (sdl::V1_0::complextest::MeasurementUnit)key;
	}
	measurementUnitChanged();
}


bool CResultObject::hasMeasurementUnit()
{
	 return CResult::MeasurementUnit.HasValue();
}


QVariant CResultObject::GetLength()
{
	return CResult::Length ? CResult::Length.value() : QVariant();
}


void CResultObject::SetLength(const QVariant& v)
{
	CResult::Length = v.value<double>();
	lengthChanged();
}


bool CResultObject::hasLength()
{
	 return CResult::Length.HasValue();
}


QVariant CResultObject::GetErrorType()
{
	if (!ErrorType) {return {};}
	sdl::V1_0::complextest::ErrorCode valueType = CResult::ErrorType.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::ErrorCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CResultObject::SetErrorType(const QVariant& v)
{
	CResult::ErrorType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::ErrorCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CResult::ErrorType = (sdl::V1_0::complextest::ErrorCode)key;
	}
	errorTypeChanged();
}


bool CResultObject::hasErrorType()
{
	 return CResult::ErrorType.HasValue();
}


QVariant CResultObject::GetGeometry()
{
	if (!Geometry) {return {};}
	
	if (!m_geometryQObjectPtr.isValid()){
		m_geometryQObjectPtr = CreateObject("Geometry");
		auto itemPtr = m_geometryQObjectPtr.value<sdl::V1_0::complextest::CGeometryObject*>();
		if (itemPtr != nullptr) {
			itemPtr->GeometryType = Geometry->GeometryType;
			itemPtr->Radius = Geometry->Radius;
			itemPtr->Points = Geometry->Points;
			itemPtr->RequiredPoints = Geometry->RequiredPoints;
			itemPtr->OptionalPoints = Geometry->OptionalPoints;
		}
	}
	return m_geometryQObjectPtr;
}


void CResultObject::SetGeometry(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CGeometryObject* itemPtr = v.value<sdl::V1_0::complextest::CGeometryObject*>();
		if (itemPtr != nullptr) CResult::Geometry = (*itemPtr);
	}
	else {
		CResult::Geometry.SetNull();
	}
	m_geometryQObjectPtr = v;

	geometryChanged();
}


bool CResultObject::hasGeometry()
{
	 return CResult::Geometry.HasValue();
}


void CResultObject::emplaceGeometry()
{
	ResetGeometry();
	CResult::Geometry.emplace();
}


void CResultObject::ResetGeometry()
{
	auto* itemPtr = m_geometryQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_geometryQObjectPtr = QVariant();
}


QString CResultObject::toJson() const
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


bool CResultObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CResultObject::fromObject(const QJsonObject& jsonObject)
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


QString CResultObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CResultObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "Geometry"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CGeometryObject(this));
	}
	return QVariant();
}


QString CResultObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "areaResult")){
		return "AreaResult";
	}
	if (propertyName == (QStringLiteral("m_") + "expectedMinValue")){
		return "ExpectedMinValue";
	}
	if (propertyName == (QStringLiteral("m_") + "expectedMaxValue")){
		return "ExpectedMaxValue";
	}
	if (propertyName == (QStringLiteral("m_") + "measuredValue")){
		return "MeasuredValue";
	}
	if (propertyName == (QStringLiteral("m_") + "minMeasuredValue")){
		return "MinMeasuredValue";
	}
	if (propertyName == (QStringLiteral("m_") + "maxMeasuredValue")){
		return "MaxMeasuredValue";
	}
	if (propertyName == (QStringLiteral("m_") + "meanMeasuredValue")){
		return "MeanMeasuredValue";
	}
	if (propertyName == (QStringLiteral("m_") + "measurementType")){
		return "MeasurementType";
	}
	if (propertyName == (QStringLiteral("m_") + "measurementUnit")){
		return "MeasurementUnit";
	}
	if (propertyName == (QStringLiteral("m_") + "length")){
		return "Length";
	}
	if (propertyName == (QStringLiteral("m_") + "errorType")){
		return "ErrorType";
	}
	if (propertyName == (QStringLiteral("m_") + "geometry")){
		return "Geometry";
	}

	return propertyName;
}





bool CResultObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CResultObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CResultObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CResultObjectList::append(sdl::V1_0::complextest::CResultObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CResultObjectList* sdl::V1_0::complextest::CResultObjectList::copyMe()
{
	sdl::V1_0::complextest::CResultObjectList* retVal = new sdl::V1_0::complextest::CResultObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CResultObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CResultObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CResultObjectList::addElement(sdl::V1_0::complextest::CResultObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CResultObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CResultObjectList::isEqualWithModel(sdl::V1_0::complextest::CResultObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CResultObjectList::insert(int index, sdl::V1_0::complextest::CResultObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CResultObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CResultObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CResultObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CResultObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_areaResult"){
		return Version_1_0->at(index)->AreaResult ? QVariant::fromValue(Version_1_0->at(index)->AreaResult.value()) : QVariant();
	}
	if (nameId == "m_expectedMinValue"){
		return Version_1_0->at(index)->ExpectedMinValue ? QVariant::fromValue(Version_1_0->at(index)->ExpectedMinValue.value()) : QVariant();
	}
	if (nameId == "m_expectedMaxValue"){
		return Version_1_0->at(index)->ExpectedMaxValue ? QVariant::fromValue(Version_1_0->at(index)->ExpectedMaxValue.value()) : QVariant();
	}
	if (nameId == "m_measuredValue"){
		return Version_1_0->at(index)->MeasuredValue ? QVariant::fromValue(Version_1_0->at(index)->MeasuredValue.value()) : QVariant();
	}
	if (nameId == "m_minMeasuredValue"){
		return Version_1_0->at(index)->MinMeasuredValue ? QVariant::fromValue(Version_1_0->at(index)->MinMeasuredValue.value()) : QVariant();
	}
	if (nameId == "m_maxMeasuredValue"){
		return Version_1_0->at(index)->MaxMeasuredValue ? QVariant::fromValue(Version_1_0->at(index)->MaxMeasuredValue.value()) : QVariant();
	}
	if (nameId == "m_meanMeasuredValue"){
		return Version_1_0->at(index)->MeanMeasuredValue ? QVariant::fromValue(Version_1_0->at(index)->MeanMeasuredValue.value()) : QVariant();
	}
	if (nameId == "m_measurementType"){
		return Version_1_0->at(index)->MeasurementType ? QVariant::fromValue(Version_1_0->at(index)->MeasurementType.value()) : QVariant();
	}
	if (nameId == "m_measurementUnit"){
		return Version_1_0->at(index)->MeasurementUnit ? QVariant::fromValue(Version_1_0->at(index)->MeasurementUnit.value()) : QVariant();
	}
	if (nameId == "m_length"){
		return Version_1_0->at(index)->Length ? QVariant::fromValue(Version_1_0->at(index)->Length.value()) : QVariant();
	}
	if (nameId == "m_errorType"){
		return Version_1_0->at(index)->ErrorType ? QVariant::fromValue(Version_1_0->at(index)->ErrorType.value()) : QVariant();
	}
	if (nameId == "m_geometry"){
		return itemPtr->GetGeometry();
	}

	return QVariant();
}
CAreaObject::CAreaObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CAreaObject::areaNameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::statusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::iconPositionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::errorTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::resultsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CAreaObject::GetAreaName()
{
	return CArea::AreaName ? CArea::AreaName.value() : QVariant();
}


void CAreaObject::SetAreaName(const QVariant& v)
{
	CArea::AreaName = v.value<QString>();
	areaNameChanged();
}


bool CAreaObject::hasAreaName()
{
	 return CArea::AreaName.HasValue();
}


QVariant CAreaObject::GetStatus()
{
	if (!Status) {return {};}
	sdl::V1_0::complextest::StatusCode valueType = CArea::Status.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CAreaObject::SetStatus(const QVariant& v)
{
	CArea::Status.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CArea::Status = (sdl::V1_0::complextest::StatusCode)key;
	}
	statusChanged();
}


bool CAreaObject::hasStatus()
{
	 return CArea::Status.HasValue();
}


QVariant CAreaObject::GetIconPosition()
{
	if (!IconPosition) {return {};}
	
	if (!m_iconPositionQObjectPtr.isValid()){
		m_iconPositionQObjectPtr = CreateObject("IconPosition");
		auto itemPtr = m_iconPositionQObjectPtr.value<sdl::V1_0::complextest::CPointObject*>();
		if (itemPtr != nullptr) {
			itemPtr->X = IconPosition->X;
			itemPtr->Y = IconPosition->Y;
		}
	}
	return m_iconPositionQObjectPtr;
}


void CAreaObject::SetIconPosition(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CPointObject* itemPtr = v.value<sdl::V1_0::complextest::CPointObject*>();
		if (itemPtr != nullptr) CArea::IconPosition = (*itemPtr);
	}
	else {
		CArea::IconPosition.SetNull();
	}
	m_iconPositionQObjectPtr = v;

	iconPositionChanged();
}


bool CAreaObject::hasIconPosition()
{
	 return CArea::IconPosition.HasValue();
}


void CAreaObject::emplaceIconPosition()
{
	ResetIconPosition();
	CArea::IconPosition.emplace();
}


void CAreaObject::ResetIconPosition()
{
	auto* itemPtr = m_iconPositionQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_iconPositionQObjectPtr = QVariant();
}


QVariant CAreaObject::GetErrorType()
{
	if (!ErrorType) {return {};}
	sdl::V1_0::complextest::ErrorCode valueType = CArea::ErrorType.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::ErrorCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CAreaObject::SetErrorType(const QVariant& v)
{
	CArea::ErrorType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::ErrorCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CArea::ErrorType = (sdl::V1_0::complextest::ErrorCode)key;
	}
	errorTypeChanged();
}


bool CAreaObject::hasErrorType()
{
	 return CArea::ErrorType.HasValue();
}


QVariant CAreaObject::GetResults()
{
	if (!Results) {return {};}
	if (!m_resultsQObjectPtr.isValid()){
		m_resultsQObjectPtr = CreateObject("Results");
		auto itemPtr = m_resultsQObjectPtr.value<sdl::V1_0::complextest::CResultObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CArea::Results;
	}
	return m_resultsQObjectPtr;
}


void CAreaObject::SetResults(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){return;}
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CResultObjectList* itemPtr = v.value<sdl::V1_0::complextest::CResultObjectList*>();
		if (itemPtr != nullptr){
			if (!itemPtr->Version_1_0){return;}
			CArea::Results = itemPtr->Version_1_0;
		}
	}
	else {
		CArea::Results.SetNull();
	}
	m_resultsQObjectPtr = v;

	resultsChanged();
}


bool CAreaObject::hasResults()
{
	 return CArea::Results.HasValue();
}


void CAreaObject::emplaceResults()
{
	ResetResults();
	CArea::Results.emplace();
}


void CAreaObject::ResetResults()
{
	auto* itemPtr = m_resultsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultsQObjectPtr = QVariant();
}


QVariant CAreaObject::createResultsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CResultObject());
}


QString CAreaObject::toJson() const
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


bool CAreaObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CAreaObject::fromObject(const QJsonObject& jsonObject)
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


QString CAreaObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CAreaObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "IconPosition"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CPointObject(this));
	}
	if (key == "Results"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CResultObjectList(this));
	}
	return QVariant();
}


QString CAreaObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "areaName")){
		return "AreaName";
	}
	if (propertyName == (QStringLiteral("m_") + "status")){
		return "Status";
	}
	if (propertyName == (QStringLiteral("m_") + "iconPosition")){
		return "IconPosition";
	}
	if (propertyName == (QStringLiteral("m_") + "errorType")){
		return "ErrorType";
	}
	if (propertyName == (QStringLiteral("m_") + "results")){
		return "Results";
	}

	return propertyName;
}





bool CAreaObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CAreaObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CAreaObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CAreaObjectList::append(sdl::V1_0::complextest::CAreaObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CAreaObjectList* sdl::V1_0::complextest::CAreaObjectList::copyMe()
{
	sdl::V1_0::complextest::CAreaObjectList* retVal = new sdl::V1_0::complextest::CAreaObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CAreaObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CAreaObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CAreaObjectList::addElement(sdl::V1_0::complextest::CAreaObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CAreaObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CAreaObjectList::isEqualWithModel(sdl::V1_0::complextest::CAreaObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CAreaObjectList::insert(int index, sdl::V1_0::complextest::CAreaObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CAreaObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CAreaObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CAreaObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CAreaObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_areaName"){
		return Version_1_0->at(index)->AreaName ? QVariant::fromValue(Version_1_0->at(index)->AreaName.value()) : QVariant();
	}
	if (nameId == "m_status"){
		return Version_1_0->at(index)->Status ? QVariant::fromValue(Version_1_0->at(index)->Status.value()) : QVariant();
	}
	if (nameId == "m_iconPosition"){
		return itemPtr->GetIconPosition();
	}
	if (nameId == "m_errorType"){
		return Version_1_0->at(index)->ErrorType ? QVariant::fromValue(Version_1_0->at(index)->ErrorType.value()) : QVariant();
	}
	if (nameId == "m_results"){
		return itemPtr->GetResults();
	}

	return QVariant();
}
CAnalyzerObject::CAnalyzerObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CAnalyzerObject::analyzerNameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAnalyzerObject::analyzerResultChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAnalyzerObject::areasChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CAnalyzerObject::GetAnalyzerName()
{
	return CAnalyzer::AnalyzerName ? CAnalyzer::AnalyzerName.value() : QVariant();
}


void CAnalyzerObject::SetAnalyzerName(const QVariant& v)
{
	CAnalyzer::AnalyzerName = v.value<QString>();
	analyzerNameChanged();
}


bool CAnalyzerObject::hasAnalyzerName()
{
	 return CAnalyzer::AnalyzerName.HasValue();
}


QVariant CAnalyzerObject::GetAnalyzerResult()
{
	if (!AnalyzerResult) {return {};}
	sdl::V1_0::complextest::StatusCode valueType = CAnalyzer::AnalyzerResult.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CAnalyzerObject::SetAnalyzerResult(const QVariant& v)
{
	CAnalyzer::AnalyzerResult.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CAnalyzer::AnalyzerResult = (sdl::V1_0::complextest::StatusCode)key;
	}
	analyzerResultChanged();
}


bool CAnalyzerObject::hasAnalyzerResult()
{
	 return CAnalyzer::AnalyzerResult.HasValue();
}


QVariant CAnalyzerObject::GetAreas()
{
	if (!Areas) {return {};}
	if (!m_areasQObjectPtr.isValid()){
		m_areasQObjectPtr = CreateObject("Areas");
		auto itemPtr = m_areasQObjectPtr.value<sdl::V1_0::complextest::CAreaObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CAnalyzer::Areas;
	}
	return m_areasQObjectPtr;
}


void CAnalyzerObject::SetAreas(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CAreaObjectList* itemPtr = v.value<sdl::V1_0::complextest::CAreaObjectList*>();
		if (itemPtr != nullptr){
			CAnalyzer::Areas = itemPtr->Version_1_0;
		}
	}
	else {
		CAnalyzer::Areas.SetNull();
	}
	m_areasQObjectPtr = v;

	areasChanged();
}


bool CAnalyzerObject::hasAreas()
{
	 return CAnalyzer::Areas.HasValue();
}


void CAnalyzerObject::emplaceAreas()
{
	ResetAreas();
	CAnalyzer::Areas.emplace();
}


void CAnalyzerObject::ResetAreas()
{
	auto* itemPtr = m_areasQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_areasQObjectPtr = QVariant();
}


QVariant CAnalyzerObject::createAreasArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CAreaObject());
}


QString CAnalyzerObject::toJson() const
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


bool CAnalyzerObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CAnalyzerObject::fromObject(const QJsonObject& jsonObject)
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


QString CAnalyzerObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CAnalyzerObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "Areas"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CAreaObjectList(this));
	}
	return QVariant();
}


QString CAnalyzerObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "analyzerName")){
		return "AnalyzerName";
	}
	if (propertyName == (QStringLiteral("m_") + "analyzerResult")){
		return "AnalyzerResult";
	}
	if (propertyName == (QStringLiteral("m_") + "areas")){
		return "Areas";
	}

	return propertyName;
}





bool CAnalyzerObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CAnalyzerObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CAnalyzerObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CAnalyzerObjectList::append(sdl::V1_0::complextest::CAnalyzerObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CAnalyzerObjectList* sdl::V1_0::complextest::CAnalyzerObjectList::copyMe()
{
	sdl::V1_0::complextest::CAnalyzerObjectList* retVal = new sdl::V1_0::complextest::CAnalyzerObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CAnalyzerObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CAnalyzerObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CAnalyzerObjectList::addElement(sdl::V1_0::complextest::CAnalyzerObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CAnalyzerObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CAnalyzerObjectList::isEqualWithModel(sdl::V1_0::complextest::CAnalyzerObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CAnalyzerObjectList::insert(int index, sdl::V1_0::complextest::CAnalyzerObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CAnalyzerObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CAnalyzerObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CAnalyzerObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CAnalyzerObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_analyzerName"){
		return Version_1_0->at(index)->AnalyzerName ? QVariant::fromValue(Version_1_0->at(index)->AnalyzerName.value()) : QVariant();
	}
	if (nameId == "m_analyzerResult"){
		return Version_1_0->at(index)->AnalyzerResult ? QVariant::fromValue(Version_1_0->at(index)->AnalyzerResult.value()) : QVariant();
	}
	if (nameId == "m_areas"){
		return itemPtr->GetAreas();
	}

	return QVariant();
}
CInspectionObject::CInspectionObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CInspectionObject::iDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::typeIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::statusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::analyzersChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CInspectionObject::GetID()
{
	return CInspection::ID ? CInspection::ID.value() : QVariant();
}


void CInspectionObject::SetID(const QVariant& v)
{
	CInspection::ID = v.value<QString>();
	iDChanged();
}


bool CInspectionObject::hasID()
{
	 return CInspection::ID.HasValue();
}


QVariant CInspectionObject::GetTypeID()
{
	return CInspection::TypeID ? CInspection::TypeID.value() : QVariant();
}


void CInspectionObject::SetTypeID(const QVariant& v)
{
	CInspection::TypeID = v.value<QString>();
	typeIDChanged();
}


bool CInspectionObject::hasTypeID()
{
	 return CInspection::TypeID.HasValue();
}


QVariant CInspectionObject::GetName()
{
	return CInspection::Name ? CInspection::Name.value() : QVariant();
}


void CInspectionObject::SetName(const QVariant& v)
{
	CInspection::Name = v.value<QString>();
	nameChanged();
}


bool CInspectionObject::hasName()
{
	 return CInspection::Name.HasValue();
}


QVariant CInspectionObject::GetStatus()
{
	if (!Status) {return {};}
	sdl::V1_0::complextest::StatusCode valueType = CInspection::Status.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CInspectionObject::SetStatus(const QVariant& v)
{
	CInspection::Status.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CInspection::Status = (sdl::V1_0::complextest::StatusCode)key;
	}
	statusChanged();
}


bool CInspectionObject::hasStatus()
{
	 return CInspection::Status.HasValue();
}


QVariant CInspectionObject::GetAnalyzers()
{
	if (!Analyzers) {return {};}
	if (!m_analyzersQObjectPtr.isValid()){
		m_analyzersQObjectPtr = CreateObject("Analyzers");
		auto itemPtr = m_analyzersQObjectPtr.value<sdl::V1_0::complextest::CAnalyzerObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CInspection::Analyzers;
	}
	return m_analyzersQObjectPtr;
}


void CInspectionObject::SetAnalyzers(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CAnalyzerObjectList* itemPtr = v.value<sdl::V1_0::complextest::CAnalyzerObjectList*>();
		if (itemPtr != nullptr){
			CInspection::Analyzers = itemPtr->Version_1_0;
		}
	}
	else {
		CInspection::Analyzers.SetNull();
	}
	m_analyzersQObjectPtr = v;

	analyzersChanged();
}


bool CInspectionObject::hasAnalyzers()
{
	 return CInspection::Analyzers.HasValue();
}


void CInspectionObject::emplaceAnalyzers()
{
	ResetAnalyzers();
	CInspection::Analyzers.emplace();
}


void CInspectionObject::ResetAnalyzers()
{
	auto* itemPtr = m_analyzersQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_analyzersQObjectPtr = QVariant();
}


QVariant CInspectionObject::createAnalyzersArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CAnalyzerObject());
}


QString CInspectionObject::toJson() const
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


bool CInspectionObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CInspectionObject::fromObject(const QJsonObject& jsonObject)
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


QString CInspectionObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CInspectionObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "Analyzers"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CAnalyzerObjectList(this));
	}
	return QVariant();
}


QString CInspectionObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "iD")){
		return "ID";
	}
	if (propertyName == (QStringLiteral("m_") + "typeID")){
		return "TypeID";
	}
	if (propertyName == (QStringLiteral("m_") + "name")){
		return "Name";
	}
	if (propertyName == (QStringLiteral("m_") + "status")){
		return "Status";
	}
	if (propertyName == (QStringLiteral("m_") + "analyzers")){
		return "Analyzers";
	}

	return propertyName;
}





bool CInspectionObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CInspectionObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CInspectionObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CInspectionObjectList::append(sdl::V1_0::complextest::CInspectionObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CInspectionObjectList* sdl::V1_0::complextest::CInspectionObjectList::copyMe()
{
	sdl::V1_0::complextest::CInspectionObjectList* retVal = new sdl::V1_0::complextest::CInspectionObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CInspectionObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CInspectionObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CInspectionObjectList::addElement(sdl::V1_0::complextest::CInspectionObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CInspectionObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CInspectionObjectList::isEqualWithModel(sdl::V1_0::complextest::CInspectionObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CInspectionObjectList::insert(int index, sdl::V1_0::complextest::CInspectionObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CInspectionObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CInspectionObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CInspectionObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CInspectionObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_iD"){
		return Version_1_0->at(index)->ID ? QVariant::fromValue(Version_1_0->at(index)->ID.value()) : QVariant();
	}
	if (nameId == "m_typeID"){
		return Version_1_0->at(index)->TypeID ? QVariant::fromValue(Version_1_0->at(index)->TypeID.value()) : QVariant();
	}
	if (nameId == "m_name"){
		return Version_1_0->at(index)->Name ? QVariant::fromValue(Version_1_0->at(index)->Name.value()) : QVariant();
	}
	if (nameId == "m_status"){
		return Version_1_0->at(index)->Status ? QVariant::fromValue(Version_1_0->at(index)->Status.value()) : QVariant();
	}
	if (nameId == "m_analyzers"){
		return itemPtr->GetAnalyzers();
	}

	return QVariant();
}
CProductOverviewObject::CProductOverviewObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CProductOverviewObject::productIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::statusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::partSerialNumberChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::timestampChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::productImageChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::inspectionsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CProductOverviewObject::GetProductID()
{
	return CProductOverview::ProductID ? CProductOverview::ProductID.value() : QVariant();
}


void CProductOverviewObject::SetProductID(const QVariant& v)
{
	CProductOverview::ProductID = v.value<int>();
	productIDChanged();
}


bool CProductOverviewObject::hasProductID()
{
	 return CProductOverview::ProductID.HasValue();
}


QVariant CProductOverviewObject::GetName()
{
	return CProductOverview::Name ? CProductOverview::Name.value() : QVariant();
}


void CProductOverviewObject::SetName(const QVariant& v)
{
	CProductOverview::Name = v.value<QString>();
	nameChanged();
}


bool CProductOverviewObject::hasName()
{
	 return CProductOverview::Name.HasValue();
}


QVariant CProductOverviewObject::GetStatus()
{
	if (!Status) {return {};}
	sdl::V1_0::complextest::StatusCode valueType = CProductOverview::Status.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CProductOverviewObject::SetStatus(const QVariant& v)
{
	CProductOverview::Status.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CProductOverview::Status = (sdl::V1_0::complextest::StatusCode)key;
	}
	statusChanged();
}


bool CProductOverviewObject::hasStatus()
{
	 return CProductOverview::Status.HasValue();
}


QVariant CProductOverviewObject::GetPartSerialNumber()
{
	return CProductOverview::PartSerialNumber ? CProductOverview::PartSerialNumber.value() : QVariant();
}


void CProductOverviewObject::SetPartSerialNumber(const QVariant& v)
{
	CProductOverview::PartSerialNumber = v.value<QString>();
	partSerialNumberChanged();
}


bool CProductOverviewObject::hasPartSerialNumber()
{
	 return CProductOverview::PartSerialNumber.HasValue();
}


QVariant CProductOverviewObject::GetTimestamp()
{
	return CProductOverview::Timestamp ? CProductOverview::Timestamp.value() : QVariant();
}


void CProductOverviewObject::SetTimestamp(const QVariant& v)
{
	CProductOverview::Timestamp = v.value<QString>();
	timestampChanged();
}


bool CProductOverviewObject::hasTimestamp()
{
	 return CProductOverview::Timestamp.HasValue();
}


QVariant CProductOverviewObject::GetProductImage()
{
	return CProductOverview::ProductImage ? CProductOverview::ProductImage.value() : QVariant();
}


void CProductOverviewObject::SetProductImage(const QVariant& v)
{
	CProductOverview::ProductImage = v.value<QString>();
	productImageChanged();
}


bool CProductOverviewObject::hasProductImage()
{
	 return CProductOverview::ProductImage.HasValue();
}


QVariant CProductOverviewObject::GetInspections()
{
	if (!Inspections) {return {};}
	if (!m_inspectionsQObjectPtr.isValid()){
		m_inspectionsQObjectPtr = CreateObject("Inspections");
		auto itemPtr = m_inspectionsQObjectPtr.value<sdl::V1_0::complextest::CInspectionObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CProductOverview::Inspections;
	}
	return m_inspectionsQObjectPtr;
}


void CProductOverviewObject::SetInspections(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CInspectionObjectList* itemPtr = v.value<sdl::V1_0::complextest::CInspectionObjectList*>();
		if (itemPtr != nullptr){
			CProductOverview::Inspections = itemPtr->Version_1_0;
		}
	}
	else {
		CProductOverview::Inspections.SetNull();
	}
	m_inspectionsQObjectPtr = v;

	inspectionsChanged();
}


bool CProductOverviewObject::hasInspections()
{
	 return CProductOverview::Inspections.HasValue();
}


void CProductOverviewObject::emplaceInspections()
{
	ResetInspections();
	CProductOverview::Inspections.emplace();
}


void CProductOverviewObject::ResetInspections()
{
	auto* itemPtr = m_inspectionsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_inspectionsQObjectPtr = QVariant();
}


QVariant CProductOverviewObject::createInspectionsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CInspectionObject());
}


QString CProductOverviewObject::toJson() const
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


bool CProductOverviewObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CProductOverviewObject::fromObject(const QJsonObject& jsonObject)
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


QString CProductOverviewObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CProductOverviewObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "Inspections"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CInspectionObjectList(this));
	}
	return QVariant();
}


QString CProductOverviewObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "productID")){
		return "ProductID";
	}
	if (propertyName == (QStringLiteral("m_") + "name")){
		return "Name";
	}
	if (propertyName == (QStringLiteral("m_") + "status")){
		return "Status";
	}
	if (propertyName == (QStringLiteral("m_") + "partSerialNumber")){
		return "PartSerialNumber";
	}
	if (propertyName == (QStringLiteral("m_") + "timestamp")){
		return "Timestamp";
	}
	if (propertyName == (QStringLiteral("m_") + "productImage")){
		return "ProductImage";
	}
	if (propertyName == (QStringLiteral("m_") + "inspections")){
		return "Inspections";
	}

	return propertyName;
}





bool CProductOverviewObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CProductOverviewObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CProductOverviewObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CProductOverviewObjectList::append(sdl::V1_0::complextest::CProductOverviewObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CProductOverviewObjectList* sdl::V1_0::complextest::CProductOverviewObjectList::copyMe()
{
	sdl::V1_0::complextest::CProductOverviewObjectList* retVal = new sdl::V1_0::complextest::CProductOverviewObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CProductOverviewObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CProductOverviewObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CProductOverviewObjectList::addElement(sdl::V1_0::complextest::CProductOverviewObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CProductOverviewObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CProductOverviewObjectList::isEqualWithModel(sdl::V1_0::complextest::CProductOverviewObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CProductOverviewObjectList::insert(int index, sdl::V1_0::complextest::CProductOverviewObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CProductOverviewObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CProductOverviewObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CProductOverviewObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CProductOverviewObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_productID"){
		return Version_1_0->at(index)->ProductID ? QVariant::fromValue(Version_1_0->at(index)->ProductID.value()) : QVariant();
	}
	if (nameId == "m_name"){
		return Version_1_0->at(index)->Name ? QVariant::fromValue(Version_1_0->at(index)->Name.value()) : QVariant();
	}
	if (nameId == "m_status"){
		return Version_1_0->at(index)->Status ? QVariant::fromValue(Version_1_0->at(index)->Status.value()) : QVariant();
	}
	if (nameId == "m_partSerialNumber"){
		return Version_1_0->at(index)->PartSerialNumber ? QVariant::fromValue(Version_1_0->at(index)->PartSerialNumber.value()) : QVariant();
	}
	if (nameId == "m_timestamp"){
		return Version_1_0->at(index)->Timestamp ? QVariant::fromValue(Version_1_0->at(index)->Timestamp.value()) : QVariant();
	}
	if (nameId == "m_productImage"){
		return Version_1_0->at(index)->ProductImage ? QVariant::fromValue(Version_1_0->at(index)->ProductImage.value()) : QVariant();
	}
	if (nameId == "m_inspections"){
		return itemPtr->GetInspections();
	}

	return QVariant();
}
CLocalizedTextObject::CLocalizedTextObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CLocalizedTextObject::textChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CLocalizedTextObject::localeChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CLocalizedTextObject::GetText()
{
	return CLocalizedText::text ? CLocalizedText::text.value() : QVariant();
}


void CLocalizedTextObject::SetText(const QVariant& v)
{
	CLocalizedText::text = v.value<QString>();
	textChanged();
}


bool CLocalizedTextObject::hasText()
{
	 return CLocalizedText::text.HasValue();
}


QVariant CLocalizedTextObject::GetLocale()
{
	return CLocalizedText::locale ? CLocalizedText::locale.value() : QVariant();
}


void CLocalizedTextObject::SetLocale(const QVariant& v)
{
	CLocalizedText::locale = v.value<QString>();
	localeChanged();
}


bool CLocalizedTextObject::hasLocale()
{
	 return CLocalizedText::locale.HasValue();
}


QString CLocalizedTextObject::toJson() const
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


bool CLocalizedTextObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CLocalizedTextObject::fromObject(const QJsonObject& jsonObject)
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


QString CLocalizedTextObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CLocalizedTextObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CLocalizedTextObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "text")){
		return "text";
	}
	if (propertyName == (QStringLiteral("m_") + "locale")){
		return "locale";
	}

	return propertyName;
}





bool CLocalizedTextObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CLocalizedTextObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CLocalizedTextObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CLocalizedTextObjectList::append(sdl::V1_0::complextest::CLocalizedTextObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CLocalizedTextObjectList* sdl::V1_0::complextest::CLocalizedTextObjectList::copyMe()
{
	sdl::V1_0::complextest::CLocalizedTextObjectList* retVal = new sdl::V1_0::complextest::CLocalizedTextObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CLocalizedTextObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CLocalizedTextObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CLocalizedTextObjectList::addElement(sdl::V1_0::complextest::CLocalizedTextObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CLocalizedTextObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CLocalizedTextObjectList::isEqualWithModel(sdl::V1_0::complextest::CLocalizedTextObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CLocalizedTextObjectList::insert(int index, sdl::V1_0::complextest::CLocalizedTextObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CLocalizedTextObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CLocalizedTextObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CLocalizedTextObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CLocalizedTextObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_text"){
		return Version_1_0->at(index)->text ? QVariant::fromValue(Version_1_0->at(index)->text.value()) : QVariant();
	}
	if (nameId == "m_locale"){
		return Version_1_0->at(index)->locale ? QVariant::fromValue(Version_1_0->at(index)->locale.value()) : QVariant();
	}

	return QVariant();
}
CCoordinatesObject::CCoordinatesObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCoordinatesObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCoordinatesObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCoordinatesObject::zChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCoordinatesObject::GetX()
{
	return CCoordinates::x ? CCoordinates::x.value() : QVariant();
}


void CCoordinatesObject::SetX(const QVariant& v)
{
	CCoordinates::x = v.value<double>();
	xChanged();
}


bool CCoordinatesObject::hasX()
{
	 return CCoordinates::x.HasValue();
}


QVariant CCoordinatesObject::GetY()
{
	return CCoordinates::y ? CCoordinates::y.value() : QVariant();
}


void CCoordinatesObject::SetY(const QVariant& v)
{
	CCoordinates::y = v.value<double>();
	yChanged();
}


bool CCoordinatesObject::hasY()
{
	 return CCoordinates::y.HasValue();
}


QVariant CCoordinatesObject::GetZ()
{
	return CCoordinates::z ? CCoordinates::z.value() : QVariant();
}


void CCoordinatesObject::SetZ(const QVariant& v)
{
	CCoordinates::z = v.value<double>();
	zChanged();
}


bool CCoordinatesObject::hasZ()
{
	 return CCoordinates::z.HasValue();
}


QString CCoordinatesObject::toJson() const
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


bool CCoordinatesObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCoordinatesObject::fromObject(const QJsonObject& jsonObject)
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


QString CCoordinatesObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CCoordinatesObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CCoordinatesObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "x")){
		return "x";
	}
	if (propertyName == (QStringLiteral("m_") + "y")){
		return "y";
	}
	if (propertyName == (QStringLiteral("m_") + "z")){
		return "z";
	}

	return propertyName;
}





bool CCoordinatesObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCoordinatesObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCoordinatesObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCoordinatesObjectList::append(sdl::V1_0::complextest::CCoordinatesObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CCoordinatesObjectList* sdl::V1_0::complextest::CCoordinatesObjectList::copyMe()
{
	sdl::V1_0::complextest::CCoordinatesObjectList* retVal = new sdl::V1_0::complextest::CCoordinatesObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CCoordinatesObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CCoordinatesObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CCoordinatesObjectList::addElement(sdl::V1_0::complextest::CCoordinatesObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CCoordinatesObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CCoordinatesObjectList::isEqualWithModel(sdl::V1_0::complextest::CCoordinatesObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CCoordinatesObjectList::insert(int index, sdl::V1_0::complextest::CCoordinatesObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CCoordinatesObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CCoordinatesObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CCoordinatesObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CCoordinatesObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_x"){
		return Version_1_0->at(index)->x ? QVariant::fromValue(Version_1_0->at(index)->x.value()) : QVariant();
	}
	if (nameId == "m_y"){
		return Version_1_0->at(index)->y ? QVariant::fromValue(Version_1_0->at(index)->y.value()) : QVariant();
	}
	if (nameId == "m_z"){
		return Version_1_0->at(index)->z ? QVariant::fromValue(Version_1_0->at(index)->z.value()) : QVariant();
	}

	return QVariant();
}
CExtendedMetaDataObject::CExtendedMetaDataObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CExtendedMetaDataObject::keyChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CExtendedMetaDataObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CExtendedMetaDataObject::GetKey()
{
	return CExtendedMetaData::key ? CExtendedMetaData::key.value() : QVariant();
}


void CExtendedMetaDataObject::SetKey(const QVariant& v)
{
	CExtendedMetaData::key = v.value<QString>();
	keyChanged();
}


bool CExtendedMetaDataObject::hasKey()
{
	 return CExtendedMetaData::key.HasValue();
}


QVariant CExtendedMetaDataObject::GetValue()
{
	if (m_valueQObjectPtr.isValid()){
		if (const CCoordinates* coordinatesVal = std::get_if<CCoordinates>((CExtendedMetaData::value).GetPtr())){
			CCoordinatesObject *newObjectPtr = new CCoordinatesObject(this);
						newObjectPtr->CCoordinates::x = coordinatesVal->CCoordinates::x;
			newObjectPtr->CCoordinates::y = coordinatesVal->CCoordinates::y;
			newObjectPtr->CCoordinates::z = coordinatesVal->CCoordinates::z;

			m_valueQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const double* floatVal = std::get_if<double>((CExtendedMetaData::value).GetPtr())){
			m_valueQObjectPtr = QVariant::fromValue(floatVal);
		}
		if (const bool* booleanVal = std::get_if<bool>((CExtendedMetaData::value).GetPtr())){
			m_valueQObjectPtr = QVariant::fromValue(booleanVal);
		}
		if (const QString* stringVal = std::get_if<QString>((CExtendedMetaData::value).GetPtr())){
			m_valueQObjectPtr = QVariant::fromValue(stringVal);
		}
	}
	return m_valueQObjectPtr;
}


void CExtendedMetaDataObject::SetValue(const QVariant& v)
{
	if (v.isValid()){
		if (const CCoordinatesObject* coordinatesVal = v.value<const CCoordinatesObject*>()){
			CExtendedMetaData::value = *coordinatesVal;
		}
		if (const double* floatVal = v.value<const double*>()){
			CExtendedMetaData::value = *floatVal;
		}
		if (const bool* booleanVal = v.value<const bool*>()){
			CExtendedMetaData::value = *booleanVal;
		}
		if (const QString* stringVal = v.value<const QString*>()){
			CExtendedMetaData::value = *stringVal;
		}
	}
	else {
		CExtendedMetaData::value.SetNull();
	}
	m_valueQObjectPtr = v;

	valueChanged();
}


bool CExtendedMetaDataObject::hasValue()
{
	 return CExtendedMetaData::value.HasValue();
}


void CExtendedMetaDataObject::emplaceValue()
{
	ResetValue();
	CExtendedMetaData::value.emplace();
}


void CExtendedMetaDataObject::ResetValue()
{
	auto* itemPtr = m_valueQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_valueQObjectPtr = QVariant();
}


QString CExtendedMetaDataObject::toJson() const
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


bool CExtendedMetaDataObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CExtendedMetaDataObject::fromObject(const QJsonObject& jsonObject)
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


QString CExtendedMetaDataObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CExtendedMetaDataObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CExtendedMetaDataObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "key")){
		return "key";
	}
	if (propertyName == (QStringLiteral("m_") + "value")){
		return "value";
	}

	return propertyName;
}





bool CExtendedMetaDataObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CExtendedMetaDataObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CExtendedMetaDataObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CExtendedMetaDataObjectList::append(sdl::V1_0::complextest::CExtendedMetaDataObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CExtendedMetaDataObjectList* sdl::V1_0::complextest::CExtendedMetaDataObjectList::copyMe()
{
	sdl::V1_0::complextest::CExtendedMetaDataObjectList* retVal = new sdl::V1_0::complextest::CExtendedMetaDataObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CExtendedMetaDataObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CExtendedMetaDataObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CExtendedMetaDataObjectList::addElement(sdl::V1_0::complextest::CExtendedMetaDataObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CExtendedMetaDataObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CExtendedMetaDataObjectList::isEqualWithModel(sdl::V1_0::complextest::CExtendedMetaDataObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CExtendedMetaDataObjectList::insert(int index, sdl::V1_0::complextest::CExtendedMetaDataObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CExtendedMetaDataObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CExtendedMetaDataObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CExtendedMetaDataObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CExtendedMetaDataObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_key"){
		return Version_1_0->at(index)->key ? QVariant::fromValue(Version_1_0->at(index)->key.value()) : QVariant();
	}
	if (nameId == "m_value"){
		return itemPtr->GetValue();
	}

	return QVariant();
}
CResultMetaDataObject::CResultMetaDataObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CResultMetaDataObject::resultIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::creationTimeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::partIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::productIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::classificationChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::descriptionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::extendedMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CResultMetaDataObject::GetResultId()
{
	return CResultMetaData::resultId ? CResultMetaData::resultId.value() : QVariant();
}


void CResultMetaDataObject::SetResultId(const QVariant& v)
{
	CResultMetaData::resultId = v.value<QString>().toUtf8();
	resultIdChanged();
}


bool CResultMetaDataObject::hasResultId()
{
	 return CResultMetaData::resultId.HasValue();
}


QVariant CResultMetaDataObject::GetCreationTime()
{
	return CResultMetaData::creationTime ? CResultMetaData::creationTime.value() : QVariant();
}


void CResultMetaDataObject::SetCreationTime(const QVariant& v)
{
	CResultMetaData::creationTime = v.value<QString>();
	creationTimeChanged();
}


bool CResultMetaDataObject::hasCreationTime()
{
	 return CResultMetaData::creationTime.HasValue();
}


QVariant CResultMetaDataObject::GetPartId()
{
	return CResultMetaData::partId ? CResultMetaData::partId.value() : QVariant();
}


void CResultMetaDataObject::SetPartId(const QVariant& v)
{
	CResultMetaData::partId = v.value<QString>();
	partIdChanged();
}


bool CResultMetaDataObject::hasPartId()
{
	 return CResultMetaData::partId.HasValue();
}


QVariant CResultMetaDataObject::GetProductId()
{
	return CResultMetaData::productId ? CResultMetaData::productId.value() : QVariant();
}


void CResultMetaDataObject::SetProductId(const QVariant& v)
{
	CResultMetaData::productId = v.value<QString>();
	productIdChanged();
}


bool CResultMetaDataObject::hasProductId()
{
	 return CResultMetaData::productId.HasValue();
}


QVariant CResultMetaDataObject::GetName()
{
	return CResultMetaData::name ? CResultMetaData::name.value() : QVariant();
}


void CResultMetaDataObject::SetName(const QVariant& v)
{
	CResultMetaData::name = v.value<QString>();
	nameChanged();
}


bool CResultMetaDataObject::hasName()
{
	 return CResultMetaData::name.HasValue();
}


QVariant CResultMetaDataObject::GetClassification()
{
	return CResultMetaData::classification ? CResultMetaData::classification.value() : QVariant();
}


void CResultMetaDataObject::SetClassification(const QVariant& v)
{
	CResultMetaData::classification = v.value<int>();
	classificationChanged();
}


bool CResultMetaDataObject::hasClassification()
{
	 return CResultMetaData::classification.HasValue();
}


QVariant CResultMetaDataObject::GetDescription()
{
	if (!description) {return {};}
	
	if (!m_descriptionQObjectPtr.isValid()){
		m_descriptionQObjectPtr = CreateObject("description");
		auto itemPtr = m_descriptionQObjectPtr.value<sdl::V1_0::complextest::CLocalizedTextObject*>();
		if (itemPtr != nullptr) {
			itemPtr->text = description->text;
			itemPtr->locale = description->locale;
		}
	}
	return m_descriptionQObjectPtr;
}


void CResultMetaDataObject::SetDescription(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CLocalizedTextObject* itemPtr = v.value<sdl::V1_0::complextest::CLocalizedTextObject*>();
		if (itemPtr != nullptr) CResultMetaData::description = (*itemPtr);
	}
	else {
		CResultMetaData::description.SetNull();
	}
	m_descriptionQObjectPtr = v;

	descriptionChanged();
}


bool CResultMetaDataObject::hasDescription()
{
	 return CResultMetaData::description.HasValue();
}


void CResultMetaDataObject::emplaceDescription()
{
	ResetDescription();
	CResultMetaData::description.emplace();
}


void CResultMetaDataObject::ResetDescription()
{
	auto* itemPtr = m_descriptionQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_descriptionQObjectPtr = QVariant();
}


QVariant CResultMetaDataObject::GetExtendedMetaData()
{
	if (!extendedMetaData) {return {};}
	if (!m_extendedMetaDataQObjectPtr.isValid()){
		m_extendedMetaDataQObjectPtr = CreateObject("extendedMetaData");
		auto itemPtr = m_extendedMetaDataQObjectPtr.value<sdl::V1_0::complextest::CExtendedMetaDataObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CResultMetaData::extendedMetaData;
	}
	return m_extendedMetaDataQObjectPtr;
}


void CResultMetaDataObject::SetExtendedMetaData(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CExtendedMetaDataObjectList* itemPtr = v.value<sdl::V1_0::complextest::CExtendedMetaDataObjectList*>();
		if (itemPtr != nullptr){
			CResultMetaData::extendedMetaData = itemPtr->Version_1_0;
		}
	}
	else {
		CResultMetaData::extendedMetaData.SetNull();
	}
	m_extendedMetaDataQObjectPtr = v;

	extendedMetaDataChanged();
}


bool CResultMetaDataObject::hasExtendedMetaData()
{
	 return CResultMetaData::extendedMetaData.HasValue();
}


void CResultMetaDataObject::emplaceExtendedMetaData()
{
	ResetExtendedMetaData();
	CResultMetaData::extendedMetaData.emplace();
}


void CResultMetaDataObject::ResetExtendedMetaData()
{
	auto* itemPtr = m_extendedMetaDataQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_extendedMetaDataQObjectPtr = QVariant();
}


QVariant CResultMetaDataObject::createExtendedMetaDataArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CExtendedMetaDataObject());
}


QString CResultMetaDataObject::toJson() const
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


bool CResultMetaDataObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CResultMetaDataObject::fromObject(const QJsonObject& jsonObject)
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


QString CResultMetaDataObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CResultMetaDataObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "description"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CLocalizedTextObject(this));
	}
	if (key == "extendedMetaData"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CExtendedMetaDataObjectList(this));
	}
	return QVariant();
}


QString CResultMetaDataObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "resultId")){
		return "resultId";
	}
	if (propertyName == (QStringLiteral("m_") + "creationTime")){
		return "creationTime";
	}
	if (propertyName == (QStringLiteral("m_") + "partId")){
		return "partId";
	}
	if (propertyName == (QStringLiteral("m_") + "productId")){
		return "productId";
	}
	if (propertyName == (QStringLiteral("m_") + "name")){
		return "name";
	}
	if (propertyName == (QStringLiteral("m_") + "classification")){
		return "classification";
	}
	if (propertyName == (QStringLiteral("m_") + "description")){
		return "description";
	}
	if (propertyName == (QStringLiteral("m_") + "extendedMetaData")){
		return "extendedMetaData";
	}

	return propertyName;
}





bool CResultMetaDataObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CResultMetaDataObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CResultMetaDataObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CResultMetaDataObjectList::append(sdl::V1_0::complextest::CResultMetaDataObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CResultMetaDataObjectList* sdl::V1_0::complextest::CResultMetaDataObjectList::copyMe()
{
	sdl::V1_0::complextest::CResultMetaDataObjectList* retVal = new sdl::V1_0::complextest::CResultMetaDataObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CResultMetaDataObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CResultMetaDataObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CResultMetaDataObjectList::addElement(sdl::V1_0::complextest::CResultMetaDataObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CResultMetaDataObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CResultMetaDataObjectList::isEqualWithModel(sdl::V1_0::complextest::CResultMetaDataObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CResultMetaDataObjectList::insert(int index, sdl::V1_0::complextest::CResultMetaDataObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CResultMetaDataObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CResultMetaDataObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CResultMetaDataObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_resultId"){
		return Version_1_0->at(index)->resultId ? QVariant::fromValue(Version_1_0->at(index)->resultId.value()) : QVariant();
	}
	if (nameId == "m_creationTime"){
		return Version_1_0->at(index)->creationTime ? QVariant::fromValue(Version_1_0->at(index)->creationTime.value()) : QVariant();
	}
	if (nameId == "m_partId"){
		return Version_1_0->at(index)->partId ? QVariant::fromValue(Version_1_0->at(index)->partId.value()) : QVariant();
	}
	if (nameId == "m_productId"){
		return Version_1_0->at(index)->productId ? QVariant::fromValue(Version_1_0->at(index)->productId.value()) : QVariant();
	}
	if (nameId == "m_name"){
		return Version_1_0->at(index)->name ? QVariant::fromValue(Version_1_0->at(index)->name.value()) : QVariant();
	}
	if (nameId == "m_classification"){
		return Version_1_0->at(index)->classification ? QVariant::fromValue(Version_1_0->at(index)->classification.value()) : QVariant();
	}
	if (nameId == "m_description"){
		return itemPtr->GetDescription();
	}
	if (nameId == "m_extendedMetaData"){
		return itemPtr->GetExtendedMetaData();
	}

	return QVariant();
}
COverallResultValuesObject::COverallResultValuesObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &COverallResultValuesObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::valueIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::measuredValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::lowLimitChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::highLimitChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant COverallResultValuesObject::GetName()
{
	return COverallResultValues::name ? COverallResultValues::name.value() : QVariant();
}


void COverallResultValuesObject::SetName(const QVariant& v)
{
	COverallResultValues::name = v.value<QString>();
	nameChanged();
}


bool COverallResultValuesObject::hasName()
{
	 return COverallResultValues::name.HasValue();
}


QVariant COverallResultValuesObject::GetValueId()
{
	return COverallResultValues::valueId ? COverallResultValues::valueId.value() : QVariant();
}


void COverallResultValuesObject::SetValueId(const QVariant& v)
{
	COverallResultValues::valueId = v.value<QString>();
	valueIdChanged();
}


bool COverallResultValuesObject::hasValueId()
{
	 return COverallResultValues::valueId.HasValue();
}


QVariant COverallResultValuesObject::GetMeasuredValue()
{
	return COverallResultValues::measuredValue ? COverallResultValues::measuredValue.value() : QVariant();
}


void COverallResultValuesObject::SetMeasuredValue(const QVariant& v)
{
	COverallResultValues::measuredValue = v.value<double>();
	measuredValueChanged();
}


bool COverallResultValuesObject::hasMeasuredValue()
{
	 return COverallResultValues::measuredValue.HasValue();
}


QVariant COverallResultValuesObject::GetLowLimit()
{
	return COverallResultValues::lowLimit ? COverallResultValues::lowLimit.value() : QVariant();
}


void COverallResultValuesObject::SetLowLimit(const QVariant& v)
{
	COverallResultValues::lowLimit = v.value<double>();
	lowLimitChanged();
}


bool COverallResultValuesObject::hasLowLimit()
{
	 return COverallResultValues::lowLimit.HasValue();
}


QVariant COverallResultValuesObject::GetHighLimit()
{
	return COverallResultValues::highLimit ? COverallResultValues::highLimit.value() : QVariant();
}


void COverallResultValuesObject::SetHighLimit(const QVariant& v)
{
	COverallResultValues::highLimit = v.value<double>();
	highLimitChanged();
}


bool COverallResultValuesObject::hasHighLimit()
{
	 return COverallResultValues::highLimit.HasValue();
}


QString COverallResultValuesObject::toJson() const
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


bool COverallResultValuesObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool COverallResultValuesObject::fromObject(const QJsonObject& jsonObject)
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


QString COverallResultValuesObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant COverallResultValuesObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString COverallResultValuesObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "name")){
		return "name";
	}
	if (propertyName == (QStringLiteral("m_") + "valueId")){
		return "valueId";
	}
	if (propertyName == (QStringLiteral("m_") + "measuredValue")){
		return "measuredValue";
	}
	if (propertyName == (QStringLiteral("m_") + "lowLimit")){
		return "lowLimit";
	}
	if (propertyName == (QStringLiteral("m_") + "highLimit")){
		return "highLimit";
	}

	return propertyName;
}





bool COverallResultValuesObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int COverallResultValuesObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap COverallResultValuesObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void COverallResultValuesObjectList::append(sdl::V1_0::complextest::COverallResultValuesObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::COverallResultValuesObjectList* sdl::V1_0::complextest::COverallResultValuesObjectList::copyMe()
{
	sdl::V1_0::complextest::COverallResultValuesObjectList* retVal = new sdl::V1_0::complextest::COverallResultValuesObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::COverallResultValuesObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::COverallResultValuesObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::COverallResultValuesObjectList::addElement(sdl::V1_0::complextest::COverallResultValuesObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::COverallResultValuesObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::COverallResultValuesObjectList::isEqualWithModel(sdl::V1_0::complextest::COverallResultValuesObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::COverallResultValuesObjectList::insert(int index, sdl::V1_0::complextest::COverallResultValuesObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::COverallResultValuesObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::COverallResultValuesObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::COverallResultValuesObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::COverallResultValuesObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_name"){
		return Version_1_0->at(index)->name ? QVariant::fromValue(Version_1_0->at(index)->name.value()) : QVariant();
	}
	if (nameId == "m_valueId"){
		return Version_1_0->at(index)->valueId ? QVariant::fromValue(Version_1_0->at(index)->valueId.value()) : QVariant();
	}
	if (nameId == "m_measuredValue"){
		return Version_1_0->at(index)->measuredValue ? QVariant::fromValue(Version_1_0->at(index)->measuredValue.value()) : QVariant();
	}
	if (nameId == "m_lowLimit"){
		return Version_1_0->at(index)->lowLimit ? QVariant::fromValue(Version_1_0->at(index)->lowLimit.value()) : QVariant();
	}
	if (nameId == "m_highLimit"){
		return Version_1_0->at(index)->highLimit ? QVariant::fromValue(Version_1_0->at(index)->highLimit.value()) : QVariant();
	}

	return QVariant();
}
CUnionTestingTypeObject::CUnionTestingTypeObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CUnionTestingTypeObject::simpleUnionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUnionTestingTypeObject::complexUnionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUnionTestingTypeObject::mixedUnionChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CUnionTestingTypeObject::GetSimpleUnion()
{
	if (m_simpleUnionQObjectPtr.isValid()){
		if (const double* floatVal = std::get_if<double>((CUnionTestingType::simpleUnion).GetPtr())){
			m_simpleUnionQObjectPtr = QVariant::fromValue(floatVal);
		}
		if (const bool* booleanVal = std::get_if<bool>((CUnionTestingType::simpleUnion).GetPtr())){
			m_simpleUnionQObjectPtr = QVariant::fromValue(booleanVal);
		}
		if (const QString* stringVal = std::get_if<QString>((CUnionTestingType::simpleUnion).GetPtr())){
			m_simpleUnionQObjectPtr = QVariant::fromValue(stringVal);
		}
	}
	return m_simpleUnionQObjectPtr;
}


void CUnionTestingTypeObject::SetSimpleUnion(const QVariant& v)
{
	if (v.isValid()){
		if (const double* floatVal = v.value<const double*>()){
			CUnionTestingType::simpleUnion = *floatVal;
		}
		if (const bool* booleanVal = v.value<const bool*>()){
			CUnionTestingType::simpleUnion = *booleanVal;
		}
		if (const QString* stringVal = v.value<const QString*>()){
			CUnionTestingType::simpleUnion = *stringVal;
		}
	}
	else {
		CUnionTestingType::simpleUnion.SetNull();
	}
	m_simpleUnionQObjectPtr = v;

	simpleUnionChanged();
}


bool CUnionTestingTypeObject::hasSimpleUnion()
{
	 return CUnionTestingType::simpleUnion.HasValue();
}


void CUnionTestingTypeObject::emplaceSimpleUnion()
{
	ResetSimpleUnion();
	CUnionTestingType::simpleUnion.emplace();
}


void CUnionTestingTypeObject::ResetSimpleUnion()
{
	auto* itemPtr = m_simpleUnionQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_simpleUnionQObjectPtr = QVariant();
}


QVariant CUnionTestingTypeObject::GetComplexUnion()
{
	if (m_complexUnionQObjectPtr.isValid()){
		if (const CCDMResultVarString* cDMResultVarStringVal = std::get_if<CCDMResultVarString>((CUnionTestingType::complexUnion).GetPtr())){
			CCDMResultVarStringObject *newObjectPtr = new CCDMResultVarStringObject(this);
						newObjectPtr->CCDMResultVarString::dataModelTitle = cDMResultVarStringVal->CCDMResultVarString::dataModelTitle;
			newObjectPtr->CCDMResultVarString::dataModelVersion = cDMResultVarStringVal->CCDMResultVarString::dataModelVersion;
			newObjectPtr->CCDMResultVarString::resultMetaData = cDMResultVarStringVal->CCDMResultVarString::resultMetaData;
			newObjectPtr->CCDMResultVarString::resultContent = cDMResultVarStringVal->CCDMResultVarString::resultContent;
			newObjectPtr->CCDMResultVarString::overallResultValues = cDMResultVarStringVal->CCDMResultVarString::overallResultValues;

			m_complexUnionQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<CCDMResultVarRecursive>((CUnionTestingType::complexUnion).GetPtr())){
			CCDMResultVarRecursiveObject *newObjectPtr = new CCDMResultVarRecursiveObject(this);
						newObjectPtr->CCDMResultVarRecursive::dataModelTitle = cDMResultVarRecursiveVal->CCDMResultVarRecursive::dataModelTitle;
			newObjectPtr->CCDMResultVarRecursive::dataModelVersion = cDMResultVarRecursiveVal->CCDMResultVarRecursive::dataModelVersion;
			newObjectPtr->CCDMResultVarRecursive::resultMetaData = cDMResultVarRecursiveVal->CCDMResultVarRecursive::resultMetaData;
			newObjectPtr->CCDMResultVarRecursive::resultContent = cDMResultVarRecursiveVal->CCDMResultVarRecursive::resultContent;
			newObjectPtr->CCDMResultVarRecursive::overallResultValues = cDMResultVarRecursiveVal->CCDMResultVarRecursive::overallResultValues;

			m_complexUnionQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
	}
	return m_complexUnionQObjectPtr;
}


void CUnionTestingTypeObject::SetComplexUnion(const QVariant& v)
{
	if (v.isValid()){
		if (const CCDMResultVarStringObject* cDMResultVarStringVal = v.value<const CCDMResultVarStringObject*>()){
			CUnionTestingType::complexUnion = *cDMResultVarStringVal;
		}
		if (const CCDMResultVarRecursiveObject* cDMResultVarRecursiveVal = v.value<const CCDMResultVarRecursiveObject*>()){
			CUnionTestingType::complexUnion = *cDMResultVarRecursiveVal;
		}
	}
	else {
		CUnionTestingType::complexUnion.SetNull();
	}
	m_complexUnionQObjectPtr = v;

	complexUnionChanged();
}


bool CUnionTestingTypeObject::hasComplexUnion()
{
	 return CUnionTestingType::complexUnion.HasValue();
}


void CUnionTestingTypeObject::emplaceComplexUnion()
{
	ResetComplexUnion();
	CUnionTestingType::complexUnion.emplace();
}


void CUnionTestingTypeObject::ResetComplexUnion()
{
	auto* itemPtr = m_complexUnionQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_complexUnionQObjectPtr = QVariant();
}


QVariant CUnionTestingTypeObject::GetMixedUnion()
{
	if (m_mixedUnionQObjectPtr.isValid()){
		if (const CCoordinates* coordinatesVal = std::get_if<CCoordinates>((CUnionTestingType::mixedUnion).GetPtr())){
			CCoordinatesObject *newObjectPtr = new CCoordinatesObject(this);
						newObjectPtr->CCoordinates::x = coordinatesVal->CCoordinates::x;
			newObjectPtr->CCoordinates::y = coordinatesVal->CCoordinates::y;
			newObjectPtr->CCoordinates::z = coordinatesVal->CCoordinates::z;

			m_mixedUnionQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const double* floatVal = std::get_if<double>((CUnionTestingType::mixedUnion).GetPtr())){
			m_mixedUnionQObjectPtr = QVariant::fromValue(floatVal);
		}
		if (const bool* booleanVal = std::get_if<bool>((CUnionTestingType::mixedUnion).GetPtr())){
			m_mixedUnionQObjectPtr = QVariant::fromValue(booleanVal);
		}
		if (const QString* stringVal = std::get_if<QString>((CUnionTestingType::mixedUnion).GetPtr())){
			m_mixedUnionQObjectPtr = QVariant::fromValue(stringVal);
		}
	}
	return m_mixedUnionQObjectPtr;
}


void CUnionTestingTypeObject::SetMixedUnion(const QVariant& v)
{
	if (v.isValid()){
		if (const CCoordinatesObject* coordinatesVal = v.value<const CCoordinatesObject*>()){
			CUnionTestingType::mixedUnion = *coordinatesVal;
		}
		if (const double* floatVal = v.value<const double*>()){
			CUnionTestingType::mixedUnion = *floatVal;
		}
		if (const bool* booleanVal = v.value<const bool*>()){
			CUnionTestingType::mixedUnion = *booleanVal;
		}
		if (const QString* stringVal = v.value<const QString*>()){
			CUnionTestingType::mixedUnion = *stringVal;
		}
	}
	else {
		CUnionTestingType::mixedUnion.SetNull();
	}
	m_mixedUnionQObjectPtr = v;

	mixedUnionChanged();
}


bool CUnionTestingTypeObject::hasMixedUnion()
{
	 return CUnionTestingType::mixedUnion.HasValue();
}


void CUnionTestingTypeObject::emplaceMixedUnion()
{
	ResetMixedUnion();
	CUnionTestingType::mixedUnion.emplace();
}


void CUnionTestingTypeObject::ResetMixedUnion()
{
	auto* itemPtr = m_mixedUnionQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_mixedUnionQObjectPtr = QVariant();
}


QString CUnionTestingTypeObject::toJson() const
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


bool CUnionTestingTypeObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CUnionTestingTypeObject::fromObject(const QJsonObject& jsonObject)
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


QString CUnionTestingTypeObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CUnionTestingTypeObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CUnionTestingTypeObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "simpleUnion")){
		return "simpleUnion";
	}
	if (propertyName == (QStringLiteral("m_") + "complexUnion")){
		return "complexUnion";
	}
	if (propertyName == (QStringLiteral("m_") + "mixedUnion")){
		return "mixedUnion";
	}

	return propertyName;
}





bool CUnionTestingTypeObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CUnionTestingTypeObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CUnionTestingTypeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CUnionTestingTypeObjectList::append(sdl::V1_0::complextest::CUnionTestingTypeObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CUnionTestingTypeObjectList* sdl::V1_0::complextest::CUnionTestingTypeObjectList::copyMe()
{
	sdl::V1_0::complextest::CUnionTestingTypeObjectList* retVal = new sdl::V1_0::complextest::CUnionTestingTypeObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CUnionTestingTypeObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CUnionTestingTypeObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CUnionTestingTypeObjectList::addElement(sdl::V1_0::complextest::CUnionTestingTypeObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CUnionTestingTypeObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CUnionTestingTypeObjectList::isEqualWithModel(sdl::V1_0::complextest::CUnionTestingTypeObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CUnionTestingTypeObjectList::insert(int index, sdl::V1_0::complextest::CUnionTestingTypeObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CUnionTestingTypeObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CUnionTestingTypeObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CUnionTestingTypeObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CUnionTestingTypeObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_simpleUnion"){
		return itemPtr->GetSimpleUnion();
	}
	if (nameId == "m_complexUnion"){
		return itemPtr->GetComplexUnion();
	}
	if (nameId == "m_mixedUnion"){
		return itemPtr->GetMixedUnion();
	}

	return QVariant();
}
CCDMResultVarRecursiveObject::CCDMResultVarRecursiveObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCDMResultVarRecursiveObject::dataModelTitleChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::dataModelVersionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::resultMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::resultContentChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::overallResultValuesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCDMResultVarRecursiveObject::GetDataModelTitle()
{
	return CCDMResultVarRecursive::dataModelTitle ? CCDMResultVarRecursive::dataModelTitle.value() : QVariant();
}


void CCDMResultVarRecursiveObject::SetDataModelTitle(const QVariant& v)
{
	CCDMResultVarRecursive::dataModelTitle = v.value<QString>();
	dataModelTitleChanged();
}


bool CCDMResultVarRecursiveObject::hasDataModelTitle()
{
	 return CCDMResultVarRecursive::dataModelTitle.HasValue();
}


QVariant CCDMResultVarRecursiveObject::GetDataModelVersion()
{
	return CCDMResultVarRecursive::dataModelVersion ? CCDMResultVarRecursive::dataModelVersion.value() : QVariant();
}


void CCDMResultVarRecursiveObject::SetDataModelVersion(const QVariant& v)
{
	CCDMResultVarRecursive::dataModelVersion = v.value<QString>();
	dataModelVersionChanged();
}


bool CCDMResultVarRecursiveObject::hasDataModelVersion()
{
	 return CCDMResultVarRecursive::dataModelVersion.HasValue();
}


QVariant CCDMResultVarRecursiveObject::GetResultMetaData()
{
	if (!resultMetaData) {return {};}
	
	if (!m_resultMetaDataQObjectPtr.isValid()){
		m_resultMetaDataQObjectPtr = CreateObject("resultMetaData");
		auto itemPtr = m_resultMetaDataQObjectPtr.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr) {
			itemPtr->resultId = resultMetaData->resultId;
			itemPtr->creationTime = resultMetaData->creationTime;
			itemPtr->partId = resultMetaData->partId;
			itemPtr->productId = resultMetaData->productId;
			itemPtr->name = resultMetaData->name;
			itemPtr->classification = resultMetaData->classification;
			itemPtr->description = resultMetaData->description;
			itemPtr->extendedMetaData = resultMetaData->extendedMetaData;
		}
	}
	return m_resultMetaDataQObjectPtr;
}


void CCDMResultVarRecursiveObject::SetResultMetaData(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CResultMetaDataObject* itemPtr = v.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr) CCDMResultVarRecursive::resultMetaData = (*itemPtr);
	}
	else {
		CCDMResultVarRecursive::resultMetaData.SetNull();
	}
	m_resultMetaDataQObjectPtr = v;

	resultMetaDataChanged();
}


bool CCDMResultVarRecursiveObject::hasResultMetaData()
{
	 return CCDMResultVarRecursive::resultMetaData.HasValue();
}


void CCDMResultVarRecursiveObject::emplaceResultMetaData()
{
	ResetResultMetaData();
	CCDMResultVarRecursive::resultMetaData.emplace();
}


void CCDMResultVarRecursiveObject::ResetResultMetaData()
{
	auto* itemPtr = m_resultMetaDataQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultMetaDataQObjectPtr = QVariant();
}


QVariant CCDMResultVarRecursiveObject::GetResultContent()
{
	if (!resultContent) {return {};}
	if (!m_resultContentQObjectPtr.isValid()){
		m_resultContentQObjectPtr = CreateObject("resultContent");
		auto itemPtr = m_resultContentQObjectPtr.value<sdl::V1_0::complextest::CCDMResultUnionTypeObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CCDMResultVarRecursive::resultContent;
	}
	return m_resultContentQObjectPtr;
}


void CCDMResultVarRecursiveObject::SetResultContent(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* itemPtr = v.value<sdl::V1_0::complextest::CCDMResultUnionTypeObjectList*>();
		if (itemPtr != nullptr){
			CCDMResultVarRecursive::resultContent = itemPtr->Version_1_0;
		}
	}
	else {
		CCDMResultVarRecursive::resultContent.SetNull();
	}
	m_resultContentQObjectPtr = v;

	resultContentChanged();
}


bool CCDMResultVarRecursiveObject::hasResultContent()
{
	 return CCDMResultVarRecursive::resultContent.HasValue();
}


void CCDMResultVarRecursiveObject::emplaceResultContent()
{
	ResetResultContent();
	CCDMResultVarRecursive::resultContent.emplace();
}


void CCDMResultVarRecursiveObject::ResetResultContent()
{
	auto* itemPtr = m_resultContentQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultContentQObjectPtr = QVariant();
}


QVariant CCDMResultVarRecursiveObject::GetOverallResultValues()
{
	if (!overallResultValues) {return {};}
	if (!m_overallResultValuesQObjectPtr.isValid()){
		m_overallResultValuesQObjectPtr = CreateObject("overallResultValues");
		auto itemPtr = m_overallResultValuesQObjectPtr.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CCDMResultVarRecursive::overallResultValues;
	}
	return m_overallResultValuesQObjectPtr;
}


void CCDMResultVarRecursiveObject::SetOverallResultValues(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::COverallResultValuesObjectList* itemPtr = v.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr){
			CCDMResultVarRecursive::overallResultValues = itemPtr->Version_1_0;
		}
	}
	else {
		CCDMResultVarRecursive::overallResultValues.SetNull();
	}
	m_overallResultValuesQObjectPtr = v;

	overallResultValuesChanged();
}


bool CCDMResultVarRecursiveObject::hasOverallResultValues()
{
	 return CCDMResultVarRecursive::overallResultValues.HasValue();
}


void CCDMResultVarRecursiveObject::emplaceOverallResultValues()
{
	ResetOverallResultValues();
	CCDMResultVarRecursive::overallResultValues.emplace();
}


void CCDMResultVarRecursiveObject::ResetOverallResultValues()
{
	auto* itemPtr = m_overallResultValuesQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_overallResultValuesQObjectPtr = QVariant();
}


QVariant CCDMResultVarRecursiveObject::createOverallResultValuesArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObject());
}


QString CCDMResultVarRecursiveObject::toJson() const
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


bool CCDMResultVarRecursiveObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCDMResultVarRecursiveObject::fromObject(const QJsonObject& jsonObject)
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


QString CCDMResultVarRecursiveObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CCDMResultVarRecursiveObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "resultMetaData"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CResultMetaDataObject(this));
	}
	if (key == "resultContent"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CCDMResultUnionTypeObjectList(this));
	}
	if (key == "overallResultValues"){
		return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObjectList(this));
	}
	return QVariant();
}


QString CCDMResultVarRecursiveObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "dataModelTitle")){
		return "dataModelTitle";
	}
	if (propertyName == (QStringLiteral("m_") + "dataModelVersion")){
		return "dataModelVersion";
	}
	if (propertyName == (QStringLiteral("m_") + "resultMetaData")){
		return "resultMetaData";
	}
	if (propertyName == (QStringLiteral("m_") + "resultContent")){
		return "resultContent";
	}
	if (propertyName == (QStringLiteral("m_") + "overallResultValues")){
		return "overallResultValues";
	}

	return propertyName;
}





bool CCDMResultVarRecursiveObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCDMResultVarRecursiveObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCDMResultVarRecursiveObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCDMResultVarRecursiveObjectList::append(sdl::V1_0::complextest::CCDMResultVarRecursiveObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList* sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::copyMe()
{
	sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList* retVal = new sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::addElement(sdl::V1_0::complextest::CCDMResultVarRecursiveObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::isEqualWithModel(sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::insert(int index, sdl::V1_0::complextest::CCDMResultVarRecursiveObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CCDMResultVarRecursiveObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_dataModelTitle"){
		return Version_1_0->at(index)->dataModelTitle ? QVariant::fromValue(Version_1_0->at(index)->dataModelTitle.value()) : QVariant();
	}
	if (nameId == "m_dataModelVersion"){
		return Version_1_0->at(index)->dataModelVersion ? QVariant::fromValue(Version_1_0->at(index)->dataModelVersion.value()) : QVariant();
	}
	if (nameId == "m_resultMetaData"){
		return itemPtr->GetResultMetaData();
	}
	if (nameId == "m_resultContent"){
		return itemPtr->GetResultContent();
	}
	if (nameId == "m_overallResultValues"){
		return itemPtr->GetOverallResultValues();
	}

	return QVariant();
}
CCDMResultVarStringObject::CCDMResultVarStringObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCDMResultVarStringObject::dataModelTitleChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::dataModelVersionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::resultMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::resultContentChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::overallResultValuesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCDMResultVarStringObject::GetDataModelTitle()
{
	return CCDMResultVarString::dataModelTitle ? CCDMResultVarString::dataModelTitle.value() : QVariant();
}


void CCDMResultVarStringObject::SetDataModelTitle(const QVariant& v)
{
	CCDMResultVarString::dataModelTitle = v.value<QString>();
	dataModelTitleChanged();
}


bool CCDMResultVarStringObject::hasDataModelTitle()
{
	 return CCDMResultVarString::dataModelTitle.HasValue();
}


QVariant CCDMResultVarStringObject::GetDataModelVersion()
{
	return CCDMResultVarString::dataModelVersion ? CCDMResultVarString::dataModelVersion.value() : QVariant();
}


void CCDMResultVarStringObject::SetDataModelVersion(const QVariant& v)
{
	CCDMResultVarString::dataModelVersion = v.value<QString>();
	dataModelVersionChanged();
}


bool CCDMResultVarStringObject::hasDataModelVersion()
{
	 return CCDMResultVarString::dataModelVersion.HasValue();
}


QVariant CCDMResultVarStringObject::GetResultMetaData()
{
	if (!resultMetaData) {return {};}
	
	if (!m_resultMetaDataQObjectPtr.isValid()){
		m_resultMetaDataQObjectPtr = CreateObject("resultMetaData");
		auto itemPtr = m_resultMetaDataQObjectPtr.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr) {
			itemPtr->resultId = resultMetaData->resultId;
			itemPtr->creationTime = resultMetaData->creationTime;
			itemPtr->partId = resultMetaData->partId;
			itemPtr->productId = resultMetaData->productId;
			itemPtr->name = resultMetaData->name;
			itemPtr->classification = resultMetaData->classification;
			itemPtr->description = resultMetaData->description;
			itemPtr->extendedMetaData = resultMetaData->extendedMetaData;
		}
	}
	return m_resultMetaDataQObjectPtr;
}


void CCDMResultVarStringObject::SetResultMetaData(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CResultMetaDataObject* itemPtr = v.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr) CCDMResultVarString::resultMetaData = (*itemPtr);
	}
	else {
		CCDMResultVarString::resultMetaData.SetNull();
	}
	m_resultMetaDataQObjectPtr = v;

	resultMetaDataChanged();
}


bool CCDMResultVarStringObject::hasResultMetaData()
{
	 return CCDMResultVarString::resultMetaData.HasValue();
}


void CCDMResultVarStringObject::emplaceResultMetaData()
{
	ResetResultMetaData();
	CCDMResultVarString::resultMetaData.emplace();
}


void CCDMResultVarStringObject::ResetResultMetaData()
{
	auto* itemPtr = m_resultMetaDataQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultMetaDataQObjectPtr = QVariant();
}


QVariant CCDMResultVarStringObject::GetResultContent()
{
	if (!resultContent ){return {};}

	QVariantList tempResultContentList;
	for (const auto& tempValue: resultContent.value()){
		tempResultContentList << (tempValue ? QVariant::fromValue(*tempValue) : QVariant());
	}

	return tempResultContentList;
}


void CCDMResultVarStringObject::SetResultContent(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){
		CCDMResultVarString::resultContent.SetNull();
		resultContentChanged();
		return;
	}
	if (!CCDMResultVarString::resultContent){
		CCDMResultVarString::resultContent.emplace();
	}
	else{
		CCDMResultVarString::resultContent->clear();
	}
	for (const QVariant& tempValue: v.toList()){
		if (!tempValue.isValid() || tempValue.isNull()){
			CCDMResultVarString::resultContent->AppendNull();
			continue;
		}
		istd::TNullableValue<QString> tempItem(tempValue.value<QString>());
		CCDMResultVarString::resultContent->append(tempItem);
	}

	resultContentChanged();
}


bool CCDMResultVarStringObject::hasResultContent()
{
	 return CCDMResultVarString::resultContent.HasValue();
}


QVariant CCDMResultVarStringObject::GetOverallResultValues()
{
	if (!overallResultValues) {return {};}
	if (!m_overallResultValuesQObjectPtr.isValid()){
		m_overallResultValuesQObjectPtr = CreateObject("overallResultValues");
		auto itemPtr = m_overallResultValuesQObjectPtr.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CCDMResultVarString::overallResultValues;
	}
	return m_overallResultValuesQObjectPtr;
}


void CCDMResultVarStringObject::SetOverallResultValues(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::COverallResultValuesObjectList* itemPtr = v.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr){
			CCDMResultVarString::overallResultValues = itemPtr->Version_1_0;
		}
	}
	else {
		CCDMResultVarString::overallResultValues.SetNull();
	}
	m_overallResultValuesQObjectPtr = v;

	overallResultValuesChanged();
}


bool CCDMResultVarStringObject::hasOverallResultValues()
{
	 return CCDMResultVarString::overallResultValues.HasValue();
}


void CCDMResultVarStringObject::emplaceOverallResultValues()
{
	ResetOverallResultValues();
	CCDMResultVarString::overallResultValues.emplace();
}


void CCDMResultVarStringObject::ResetOverallResultValues()
{
	auto* itemPtr = m_overallResultValuesQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_overallResultValuesQObjectPtr = QVariant();
}


QVariant CCDMResultVarStringObject::createOverallResultValuesArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObject());
}


QString CCDMResultVarStringObject::toJson() const
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


bool CCDMResultVarStringObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCDMResultVarStringObject::fromObject(const QJsonObject& jsonObject)
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


QString CCDMResultVarStringObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CCDMResultVarStringObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "resultMetaData"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CResultMetaDataObject(this));
	}
	if (key == "overallResultValues"){
		return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObjectList(this));
	}
	return QVariant();
}


QString CCDMResultVarStringObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "dataModelTitle")){
		return "dataModelTitle";
	}
	if (propertyName == (QStringLiteral("m_") + "dataModelVersion")){
		return "dataModelVersion";
	}
	if (propertyName == (QStringLiteral("m_") + "resultMetaData")){
		return "resultMetaData";
	}
	if (propertyName == (QStringLiteral("m_") + "resultContent")){
		return "resultContent";
	}
	if (propertyName == (QStringLiteral("m_") + "overallResultValues")){
		return "overallResultValues";
	}

	return propertyName;
}





bool CCDMResultVarStringObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCDMResultVarStringObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCDMResultVarStringObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCDMResultVarStringObjectList::append(sdl::V1_0::complextest::CCDMResultVarStringObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CCDMResultVarStringObjectList* sdl::V1_0::complextest::CCDMResultVarStringObjectList::copyMe()
{
	sdl::V1_0::complextest::CCDMResultVarStringObjectList* retVal = new sdl::V1_0::complextest::CCDMResultVarStringObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CCDMResultVarStringObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CCDMResultVarStringObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CCDMResultVarStringObjectList::addElement(sdl::V1_0::complextest::CCDMResultVarStringObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CCDMResultVarStringObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CCDMResultVarStringObjectList::isEqualWithModel(sdl::V1_0::complextest::CCDMResultVarStringObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CCDMResultVarStringObjectList::insert(int index, sdl::V1_0::complextest::CCDMResultVarStringObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CCDMResultVarStringObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CCDMResultVarStringObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CCDMResultVarStringObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CCDMResultVarStringObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_dataModelTitle"){
		return Version_1_0->at(index)->dataModelTitle ? QVariant::fromValue(Version_1_0->at(index)->dataModelTitle.value()) : QVariant();
	}
	if (nameId == "m_dataModelVersion"){
		return Version_1_0->at(index)->dataModelVersion ? QVariant::fromValue(Version_1_0->at(index)->dataModelVersion.value()) : QVariant();
	}
	if (nameId == "m_resultMetaData"){
		return itemPtr->GetResultMetaData();
	}
	if (nameId == "m_resultContent"){
		return Version_1_0->at(index)->resultContent ? QVariant::fromValue(Version_1_0->at(index)->resultContent.value()) : QVariant();
	}
	if (nameId == "m_overallResultValues"){
		return itemPtr->GetOverallResultValues();
	}

	return QVariant();
}
CCDMResultObject::CCDMResultObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCDMResultObject::dataModelTitleChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::dataModelVersionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::resultMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::resultContentChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::overallResultValuesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCDMResultObject::GetDataModelTitle()
{
	return CCDMResult::dataModelTitle ? CCDMResult::dataModelTitle.value() : QVariant();
}


void CCDMResultObject::SetDataModelTitle(const QVariant& v)
{
	CCDMResult::dataModelTitle = v.value<QString>();
	dataModelTitleChanged();
}


bool CCDMResultObject::hasDataModelTitle()
{
	 return CCDMResult::dataModelTitle.HasValue();
}


QVariant CCDMResultObject::GetDataModelVersion()
{
	return CCDMResult::dataModelVersion ? CCDMResult::dataModelVersion.value() : QVariant();
}


void CCDMResultObject::SetDataModelVersion(const QVariant& v)
{
	CCDMResult::dataModelVersion = v.value<QString>();
	dataModelVersionChanged();
}


bool CCDMResultObject::hasDataModelVersion()
{
	 return CCDMResult::dataModelVersion.HasValue();
}


QVariant CCDMResultObject::GetResultMetaData()
{
	if (!resultMetaData) {return {};}
	
	if (!m_resultMetaDataQObjectPtr.isValid()){
		m_resultMetaDataQObjectPtr = CreateObject("resultMetaData");
		auto itemPtr = m_resultMetaDataQObjectPtr.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr) {
			itemPtr->resultId = resultMetaData->resultId;
			itemPtr->creationTime = resultMetaData->creationTime;
			itemPtr->partId = resultMetaData->partId;
			itemPtr->productId = resultMetaData->productId;
			itemPtr->name = resultMetaData->name;
			itemPtr->classification = resultMetaData->classification;
			itemPtr->description = resultMetaData->description;
			itemPtr->extendedMetaData = resultMetaData->extendedMetaData;
		}
	}
	return m_resultMetaDataQObjectPtr;
}


void CCDMResultObject::SetResultMetaData(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CResultMetaDataObject* itemPtr = v.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr) CCDMResult::resultMetaData = (*itemPtr);
	}
	else {
		CCDMResult::resultMetaData.SetNull();
	}
	m_resultMetaDataQObjectPtr = v;

	resultMetaDataChanged();
}


bool CCDMResultObject::hasResultMetaData()
{
	 return CCDMResult::resultMetaData.HasValue();
}


void CCDMResultObject::emplaceResultMetaData()
{
	ResetResultMetaData();
	CCDMResult::resultMetaData.emplace();
}


void CCDMResultObject::ResetResultMetaData()
{
	auto* itemPtr = m_resultMetaDataQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultMetaDataQObjectPtr = QVariant();
}


QVariant CCDMResultObject::GetResultContent()
{
	if (!resultContent) {return {};}
	if (!m_resultContentQObjectPtr.isValid()){
		m_resultContentQObjectPtr = CreateObject("resultContent");
		auto itemPtr = m_resultContentQObjectPtr.value<sdl::V1_0::complextest::CCDMResultUnionTypeObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CCDMResult::resultContent;
	}
	return m_resultContentQObjectPtr;
}


void CCDMResultObject::SetResultContent(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* itemPtr = v.value<sdl::V1_0::complextest::CCDMResultUnionTypeObjectList*>();
		if (itemPtr != nullptr){
			CCDMResult::resultContent = itemPtr->Version_1_0;
		}
	}
	else {
		CCDMResult::resultContent.SetNull();
	}
	m_resultContentQObjectPtr = v;

	resultContentChanged();
}


bool CCDMResultObject::hasResultContent()
{
	 return CCDMResult::resultContent.HasValue();
}


void CCDMResultObject::emplaceResultContent()
{
	ResetResultContent();
	CCDMResult::resultContent.emplace();
}


void CCDMResultObject::ResetResultContent()
{
	auto* itemPtr = m_resultContentQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultContentQObjectPtr = QVariant();
}


QVariant CCDMResultObject::GetOverallResultValues()
{
	if (!overallResultValues) {return {};}
	if (!m_overallResultValuesQObjectPtr.isValid()){
		m_overallResultValuesQObjectPtr = CreateObject("overallResultValues");
		auto itemPtr = m_overallResultValuesQObjectPtr.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CCDMResult::overallResultValues;
	}
	return m_overallResultValuesQObjectPtr;
}


void CCDMResultObject::SetOverallResultValues(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::COverallResultValuesObjectList* itemPtr = v.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr){
			CCDMResult::overallResultValues = itemPtr->Version_1_0;
		}
	}
	else {
		CCDMResult::overallResultValues.SetNull();
	}
	m_overallResultValuesQObjectPtr = v;

	overallResultValuesChanged();
}


bool CCDMResultObject::hasOverallResultValues()
{
	 return CCDMResult::overallResultValues.HasValue();
}


void CCDMResultObject::emplaceOverallResultValues()
{
	ResetOverallResultValues();
	CCDMResult::overallResultValues.emplace();
}


void CCDMResultObject::ResetOverallResultValues()
{
	auto* itemPtr = m_overallResultValuesQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_overallResultValuesQObjectPtr = QVariant();
}


QVariant CCDMResultObject::createOverallResultValuesArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObject());
}


QString CCDMResultObject::toJson() const
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


bool CCDMResultObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCDMResultObject::fromObject(const QJsonObject& jsonObject)
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


QString CCDMResultObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CCDMResultObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "resultMetaData"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CResultMetaDataObject(this));
	}
	if (key == "resultContent"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CCDMResultUnionTypeObjectList(this));
	}
	if (key == "overallResultValues"){
		return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObjectList(this));
	}
	return QVariant();
}


QString CCDMResultObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "dataModelTitle")){
		return "dataModelTitle";
	}
	if (propertyName == (QStringLiteral("m_") + "dataModelVersion")){
		return "dataModelVersion";
	}
	if (propertyName == (QStringLiteral("m_") + "resultMetaData")){
		return "resultMetaData";
	}
	if (propertyName == (QStringLiteral("m_") + "resultContent")){
		return "resultContent";
	}
	if (propertyName == (QStringLiteral("m_") + "overallResultValues")){
		return "overallResultValues";
	}

	return propertyName;
}





bool CCDMResultObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCDMResultObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCDMResultObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCDMResultObjectList::append(sdl::V1_0::complextest::CCDMResultObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CCDMResultObjectList* sdl::V1_0::complextest::CCDMResultObjectList::copyMe()
{
	sdl::V1_0::complextest::CCDMResultObjectList* retVal = new sdl::V1_0::complextest::CCDMResultObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CCDMResultObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CCDMResultObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CCDMResultObjectList::addElement(sdl::V1_0::complextest::CCDMResultObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CCDMResultObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CCDMResultObjectList::isEqualWithModel(sdl::V1_0::complextest::CCDMResultObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CCDMResultObjectList::insert(int index, sdl::V1_0::complextest::CCDMResultObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CCDMResultObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CCDMResultObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CCDMResultObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CCDMResultObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_dataModelTitle"){
		return Version_1_0->at(index)->dataModelTitle ? QVariant::fromValue(Version_1_0->at(index)->dataModelTitle.value()) : QVariant();
	}
	if (nameId == "m_dataModelVersion"){
		return Version_1_0->at(index)->dataModelVersion ? QVariant::fromValue(Version_1_0->at(index)->dataModelVersion.value()) : QVariant();
	}
	if (nameId == "m_resultMetaData"){
		return itemPtr->GetResultMetaData();
	}
	if (nameId == "m_resultContent"){
		return itemPtr->GetResultContent();
	}
	if (nameId == "m_overallResultValues"){
		return itemPtr->GetOverallResultValues();
	}

	return QVariant();
}
CPointsInCoordinateFormatObject::CPointsInCoordinateFormatObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPointsInCoordinateFormatObject::pointAsCoordinatesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPointsInCoordinateFormatObject::GetPointAsCoordinates()
{
	return CPointsInCoordinateFormat::pointAsCoordinates ? CPointsInCoordinateFormat::pointAsCoordinates.value() : QVariant();
}


void CPointsInCoordinateFormatObject::SetPointAsCoordinates(const QVariant& v)
{
	CPointsInCoordinateFormat::pointAsCoordinates = v.value<bool>();
	pointAsCoordinatesChanged();
}


bool CPointsInCoordinateFormatObject::hasPointAsCoordinates()
{
	 return CPointsInCoordinateFormat::pointAsCoordinates.HasValue();
}


QString CPointsInCoordinateFormatObject::toJson() const
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


bool CPointsInCoordinateFormatObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPointsInCoordinateFormatObject::fromObject(const QJsonObject& jsonObject)
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


QString CPointsInCoordinateFormatObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CPointsInCoordinateFormatObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CPointsInCoordinateFormatObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "pointAsCoordinates")){
		return "pointAsCoordinates";
	}

	return propertyName;
}





bool CPointsInCoordinateFormatObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPointsInCoordinateFormatObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPointsInCoordinateFormatObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPointsInCoordinateFormatObjectList::append(sdl::V1_0::complextest::CPointsInCoordinateFormatObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList* sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::copyMe()
{
	sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList* retVal = new sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::addElement(sdl::V1_0::complextest::CPointsInCoordinateFormatObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::isEqualWithModel(sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::insert(int index, sdl::V1_0::complextest::CPointsInCoordinateFormatObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CPointsInCoordinateFormatObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_pointAsCoordinates"){
		return Version_1_0->at(index)->pointAsCoordinates ? QVariant::fromValue(Version_1_0->at(index)->pointAsCoordinates.value()) : QVariant();
	}

	return QVariant();
}
CCollectionViewParamsTestObject::CCollectionViewParamsTestObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCollectionViewParamsTestObject::countChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCollectionViewParamsTestObject::offsetChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCollectionViewParamsTestObject::filterModelChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCollectionViewParamsTestObject::GetCount()
{
	return CCollectionViewParamsTest::count ? CCollectionViewParamsTest::count.value() : QVariant();
}


void CCollectionViewParamsTestObject::SetCount(const QVariant& v)
{
	CCollectionViewParamsTest::count = v.value<int>();
	countChanged();
}


bool CCollectionViewParamsTestObject::hasCount()
{
	 return CCollectionViewParamsTest::count.HasValue();
}


QVariant CCollectionViewParamsTestObject::GetOffset()
{
	return CCollectionViewParamsTest::offset ? CCollectionViewParamsTest::offset.value() : QVariant();
}


void CCollectionViewParamsTestObject::SetOffset(const QVariant& v)
{
	CCollectionViewParamsTest::offset = v.value<int>();
	offsetChanged();
}


bool CCollectionViewParamsTestObject::hasOffset()
{
	 return CCollectionViewParamsTest::offset.HasValue();
}


QVariant CCollectionViewParamsTestObject::GetFilterModel()
{
	if (!filterModel) {return {};}
	
	if (!m_filterModelQObjectPtr.isValid()){
		m_filterModelQObjectPtr = CreateObject("filterModel");
		auto itemPtr = m_filterModelQObjectPtr.value<sdl::V1_0::imtbase::CComplexCollectionFilterObject*>();
		if (itemPtr != nullptr) {
			itemPtr->sortingInfo = filterModel->sortingInfo;
			itemPtr->fieldsFilter = filterModel->fieldsFilter;
			itemPtr->timeFilter = filterModel->timeFilter;
			itemPtr->distinctFields = filterModel->distinctFields;
		}
	}
	return m_filterModelQObjectPtr;
}


void CCollectionViewParamsTestObject::SetFilterModel(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::imtbase::CComplexCollectionFilterObject* itemPtr = v.value<sdl::V1_0::imtbase::CComplexCollectionFilterObject*>();
		if (itemPtr != nullptr) CCollectionViewParamsTest::filterModel = (*itemPtr);
	}
	else {
		CCollectionViewParamsTest::filterModel.SetNull();
	}
	m_filterModelQObjectPtr = v;

	filterModelChanged();
}


bool CCollectionViewParamsTestObject::hasFilterModel()
{
	 return CCollectionViewParamsTest::filterModel.HasValue();
}


void CCollectionViewParamsTestObject::emplaceFilterModel()
{
	ResetFilterModel();
	CCollectionViewParamsTest::filterModel.emplace();
}


void CCollectionViewParamsTestObject::ResetFilterModel()
{
	auto* itemPtr = m_filterModelQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_filterModelQObjectPtr = QVariant();
}


QString CCollectionViewParamsTestObject::toJson() const
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


bool CCollectionViewParamsTestObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCollectionViewParamsTestObject::fromObject(const QJsonObject& jsonObject)
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


QString CCollectionViewParamsTestObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CCollectionViewParamsTestObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "filterModel"){
		return QVariant::fromValue(new sdl::V1_0::imtbase::CComplexCollectionFilterObject(this));
	}
	return QVariant();
}


QString CCollectionViewParamsTestObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "count")){
		return "count";
	}
	if (propertyName == (QStringLiteral("m_") + "offset")){
		return "offset";
	}
	if (propertyName == (QStringLiteral("m_") + "filterModel")){
		return "filterModel";
	}

	return propertyName;
}





bool CCollectionViewParamsTestObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCollectionViewParamsTestObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCollectionViewParamsTestObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCollectionViewParamsTestObjectList::append(sdl::V1_0::complextest::CCollectionViewParamsTestObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CCollectionViewParamsTestObjectList* sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::copyMe()
{
	sdl::V1_0::complextest::CCollectionViewParamsTestObjectList* retVal = new sdl::V1_0::complextest::CCollectionViewParamsTestObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::addElement(sdl::V1_0::complextest::CCollectionViewParamsTestObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::isEqualWithModel(sdl::V1_0::complextest::CCollectionViewParamsTestObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::insert(int index, sdl::V1_0::complextest::CCollectionViewParamsTestObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CCollectionViewParamsTestObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_count"){
		return Version_1_0->at(index)->count ? QVariant::fromValue(Version_1_0->at(index)->count.value()) : QVariant();
	}
	if (nameId == "m_offset"){
		return Version_1_0->at(index)->offset ? QVariant::fromValue(Version_1_0->at(index)->offset.value()) : QVariant();
	}
	if (nameId == "m_filterModel"){
		return itemPtr->GetFilterModel();
	}

	return QVariant();
}



QVariant CExtendedMetaDataUnionTypeObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const CCoordinates* coordinatesVal = std::get_if<CCoordinates>(this->Version_1_0->at(index).GetPtr())){
			CCoordinatesObject *newObjectPtr = new CCoordinatesObject();
			static_cast<CCoordinates&>(*newObjectPtr) = *coordinatesVal;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const double* floatVal = std::get_if<double>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(floatVal);
		}

		if (const bool* booleanVal = std::get_if<bool>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(booleanVal);
		}

		if (const QString* stringVal = std::get_if<QString>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(stringVal);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CExtendedMetaDataUnionTypeObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CExtendedMetaDataUnionTypeObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CExtendedMetaDataUnionTypeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CExtendedMetaDataUnionTypeObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CCoordinatesObject*>()){
		CCoordinatesObject* coordinatesVal = item.value<CCoordinatesObject*>();
		CCoordinates* newItemPtr = dynamic_cast<CCoordinates*>(coordinatesVal);
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionType>(*newItemPtr));
	}

	if (item.canConvert<double>()){
		double floatVal = item.value<double>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionType>(floatVal));
	}

	if (item.canConvert<bool>()){
		bool booleanVal = item.value<bool>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionType>(booleanVal));
	}

	if (item.canConvert<QString>()){
		QString stringVal = item.value<QString>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionType>(stringVal));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::complextest::CExtendedMetaDataUnionTypeObjectList* CExtendedMetaDataUnionTypeObjectList::copyMe()
{
	sdl::V1_0::complextest::CExtendedMetaDataUnionTypeObjectList* objectListPtr = new sdl::V1_0::complextest::CExtendedMetaDataUnionTypeObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<CCoordinatesObject*>()){
			CCoordinatesObject* coordinatesVal = item.value<CCoordinatesObject*>();
			objectListPtr->addElement(QVariant::fromValue(coordinatesVal->copyMe()));
		}

		if (item.canConvert<double>()){
			double floatVal = item.value<double>();
			objectListPtr->addElement(floatVal);
		}

		if (item.canConvert<bool>()){
			bool booleanVal = item.value<bool>();
			objectListPtr->addElement(booleanVal);
		}

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			objectListPtr->addElement(stringVal);
		}
	}
	return objectListPtr;
}


QString CExtendedMetaDataUnionTypeObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCoordinatesObject*>()){
			CCoordinatesObject* coordinatesVal = item.value<CCoordinatesObject*>();
			retVal += coordinatesVal->toJson();
		}

		if (item.canConvert<double>()){
			double floatVal = item.value<double>();
			retVal += QString::number(floatVal);
		}

		if (item.canConvert<bool>()){
			bool booleanVal = item.value<bool>();
			retVal += QString(booleanVal ? "true" : "false");
		}

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			retVal += QString(stringVal);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CExtendedMetaDataUnionTypeObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCoordinatesObject*>()){
			CCoordinatesObject* coordinatesVal = item.value<CCoordinatesObject*>();
			retVal += coordinatesVal->toGraphQL();
		}

		if (item.canConvert<double>()){
			double floatVal = item.value<double>();
			retVal += QString::number(floatVal);
		}

		if (item.canConvert<bool>()){
			bool booleanVal = item.value<bool>();
			retVal += QString(booleanVal ? "true" : "false");
		}

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			retVal += QString(stringVal);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CExtendedMetaDataUnionTypeObjectList::addElement(QVariant item)
{
	append(item);
}

void CExtendedMetaDataUnionTypeObjectList::removeElement(int index)
{
	remove(index);
}

bool CExtendedMetaDataUnionTypeObjectList::isEqualWithModel(sdl::V1_0::complextest::CExtendedMetaDataUnionTypeObjectList* otherModelPtr)
{
	if (otherModelPtr == nullptr){
		return false;
	}

	if (this == otherModelPtr){
		return false;
	}

	if (this->rowCount() != otherModelPtr->rowCount()){
		return false;
	}

	for (int i = 0; i < this->rowCount(); i++){
		QVariant selfItem = this->getData("item", i);
		QVariant otherItem = otherModelPtr->getData("item", i);
		if (selfItem != otherItem){
			return false;
		}

	}

	return true;
}


void CExtendedMetaDataUnionTypeObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > Version_1_0->size()) return;
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CCoordinatesObject*>()){
		CCoordinatesObject* coordinatesVal = item.value<CCoordinatesObject*>();
		CCoordinates* newItemPtr = dynamic_cast<CCoordinates*>(coordinatesVal);
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionType>(*newItemPtr));
	}

	if (item.canConvert<double>()){
		double floatVal = item.value<double>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionType>(floatVal));
	}

	if (item.canConvert<bool>()){
		bool booleanVal = item.value<bool>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionType>(booleanVal));
	}

	if (item.canConvert<QString>()){
		QString stringVal = item.value<QString>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionType>(stringVal));
	}

	ClearCache();
	endInsertRows();
}


void CExtendedMetaDataUnionTypeObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CExtendedMetaDataUnionTypeObjectList::clear()
{
	BaseClass::clear();
}


QVariant CExtendedMetaDataUnionTypeObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}



QVariant CExtendedMetaDataUnionSimpleTypeObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const double* floatVal = std::get_if<double>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(floatVal);
		}

		if (const bool* booleanVal = std::get_if<bool>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(booleanVal);
		}

		if (const QString* stringVal = std::get_if<QString>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(stringVal);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CExtendedMetaDataUnionSimpleTypeObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CExtendedMetaDataUnionSimpleTypeObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CExtendedMetaDataUnionSimpleTypeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CExtendedMetaDataUnionSimpleTypeObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<double>()){
		double floatVal = item.value<double>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionSimpleType>(floatVal));
	}

	if (item.canConvert<bool>()){
		bool booleanVal = item.value<bool>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionSimpleType>(booleanVal));
	}

	if (item.canConvert<QString>()){
		QString stringVal = item.value<QString>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionSimpleType>(stringVal));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::complextest::CExtendedMetaDataUnionSimpleTypeObjectList* CExtendedMetaDataUnionSimpleTypeObjectList::copyMe()
{
	sdl::V1_0::complextest::CExtendedMetaDataUnionSimpleTypeObjectList* objectListPtr = new sdl::V1_0::complextest::CExtendedMetaDataUnionSimpleTypeObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<double>()){
			double floatVal = item.value<double>();
			objectListPtr->addElement(floatVal);
		}

		if (item.canConvert<bool>()){
			bool booleanVal = item.value<bool>();
			objectListPtr->addElement(booleanVal);
		}

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			objectListPtr->addElement(stringVal);
		}
	}
	return objectListPtr;
}


QString CExtendedMetaDataUnionSimpleTypeObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<double>()){
			double floatVal = item.value<double>();
			retVal += QString::number(floatVal);
		}

		if (item.canConvert<bool>()){
			bool booleanVal = item.value<bool>();
			retVal += QString(booleanVal ? "true" : "false");
		}

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			retVal += QString(stringVal);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CExtendedMetaDataUnionSimpleTypeObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<double>()){
			double floatVal = item.value<double>();
			retVal += QString::number(floatVal);
		}

		if (item.canConvert<bool>()){
			bool booleanVal = item.value<bool>();
			retVal += QString(booleanVal ? "true" : "false");
		}

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			retVal += QString(stringVal);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CExtendedMetaDataUnionSimpleTypeObjectList::addElement(QVariant item)
{
	append(item);
}

void CExtendedMetaDataUnionSimpleTypeObjectList::removeElement(int index)
{
	remove(index);
}

bool CExtendedMetaDataUnionSimpleTypeObjectList::isEqualWithModel(sdl::V1_0::complextest::CExtendedMetaDataUnionSimpleTypeObjectList* otherModelPtr)
{
	if (otherModelPtr == nullptr){
		return false;
	}

	if (this == otherModelPtr){
		return false;
	}

	if (this->rowCount() != otherModelPtr->rowCount()){
		return false;
	}

	for (int i = 0; i < this->rowCount(); i++){
		QVariant selfItem = this->getData("item", i);
		QVariant otherItem = otherModelPtr->getData("item", i);
		if (selfItem != otherItem){
			return false;
		}

	}

	return true;
}


void CExtendedMetaDataUnionSimpleTypeObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > Version_1_0->size()) return;
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<double>()){
		double floatVal = item.value<double>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionSimpleType>(floatVal));
	}

	if (item.canConvert<bool>()){
		bool booleanVal = item.value<bool>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionSimpleType>(booleanVal));
	}

	if (item.canConvert<QString>()){
		QString stringVal = item.value<QString>();
		Version_1_0->append(istd::TNullableValue<ExtendedMetaDataUnionSimpleType>(stringVal));
	}

	ClearCache();
	endInsertRows();
}


void CExtendedMetaDataUnionSimpleTypeObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CExtendedMetaDataUnionSimpleTypeObjectList::clear()
{
	BaseClass::clear();
}


QVariant CExtendedMetaDataUnionSimpleTypeObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}



QVariant CCDMResultUnionTypeObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const CCDMResultVarString* cDMResultVarStringVal = std::get_if<CCDMResultVarString>(this->Version_1_0->at(index).GetPtr())){
			CCDMResultVarStringObject *newObjectPtr = new CCDMResultVarStringObject();
			static_cast<CCDMResultVarString&>(*newObjectPtr) = *cDMResultVarStringVal;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<CCDMResultVarRecursive>(this->Version_1_0->at(index).GetPtr())){
			CCDMResultVarRecursiveObject *newObjectPtr = new CCDMResultVarRecursiveObject();
			static_cast<CCDMResultVarRecursive&>(*newObjectPtr) = *cDMResultVarRecursiveVal;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CCDMResultUnionTypeObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CCDMResultUnionTypeObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CCDMResultUnionTypeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCDMResultUnionTypeObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CCDMResultVarStringObject*>()){
		CCDMResultVarStringObject* cDMResultVarStringVal = item.value<CCDMResultVarStringObject*>();
		CCDMResultVarString* newItemPtr = dynamic_cast<CCDMResultVarString*>(cDMResultVarStringVal);
		Version_1_0->append(istd::TNullableValue<CDMResultUnionType>(*newItemPtr));
	}

	if (item.canConvert<CCDMResultVarRecursiveObject*>()){
		CCDMResultVarRecursiveObject* cDMResultVarRecursiveVal = item.value<CCDMResultVarRecursiveObject*>();
		CCDMResultVarRecursive* newItemPtr = dynamic_cast<CCDMResultVarRecursive*>(cDMResultVarRecursiveVal);
		Version_1_0->append(istd::TNullableValue<CDMResultUnionType>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* CCDMResultUnionTypeObjectList::copyMe()
{
	sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* objectListPtr = new sdl::V1_0::complextest::CCDMResultUnionTypeObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<CCDMResultVarStringObject*>()){
			CCDMResultVarStringObject* cDMResultVarStringVal = item.value<CCDMResultVarStringObject*>();
			objectListPtr->addElement(QVariant::fromValue(cDMResultVarStringVal->copyMe()));
		}

		if (item.canConvert<CCDMResultVarRecursiveObject*>()){
			CCDMResultVarRecursiveObject* cDMResultVarRecursiveVal = item.value<CCDMResultVarRecursiveObject*>();
			objectListPtr->addElement(QVariant::fromValue(cDMResultVarRecursiveVal->copyMe()));
		}
	}
	return objectListPtr;
}


QString CCDMResultUnionTypeObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCDMResultVarStringObject*>()){
			CCDMResultVarStringObject* cDMResultVarStringVal = item.value<CCDMResultVarStringObject*>();
			retVal += cDMResultVarStringVal->toJson();
		}

		if (item.canConvert<CCDMResultVarRecursiveObject*>()){
			CCDMResultVarRecursiveObject* cDMResultVarRecursiveVal = item.value<CCDMResultVarRecursiveObject*>();
			retVal += cDMResultVarRecursiveVal->toJson();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CCDMResultUnionTypeObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCDMResultVarStringObject*>()){
			CCDMResultVarStringObject* cDMResultVarStringVal = item.value<CCDMResultVarStringObject*>();
			retVal += cDMResultVarStringVal->toGraphQL();
		}

		if (item.canConvert<CCDMResultVarRecursiveObject*>()){
			CCDMResultVarRecursiveObject* cDMResultVarRecursiveVal = item.value<CCDMResultVarRecursiveObject*>();
			retVal += cDMResultVarRecursiveVal->toGraphQL();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CCDMResultUnionTypeObjectList::addElement(QVariant item)
{
	append(item);
}

void CCDMResultUnionTypeObjectList::removeElement(int index)
{
	remove(index);
}

bool CCDMResultUnionTypeObjectList::isEqualWithModel(sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* otherModelPtr)
{
	if (otherModelPtr == nullptr){
		return false;
	}

	if (this == otherModelPtr){
		return false;
	}

	if (this->rowCount() != otherModelPtr->rowCount()){
		return false;
	}

	for (int i = 0; i < this->rowCount(); i++){
		QVariant selfItem = this->getData("item", i);
		QVariant otherItem = otherModelPtr->getData("item", i);
		if (selfItem != otherItem){
			return false;
		}

	}

	return true;
}


void CCDMResultUnionTypeObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > Version_1_0->size()) return;
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CCDMResultVarStringObject*>()){
		CCDMResultVarStringObject* cDMResultVarStringVal = item.value<CCDMResultVarStringObject*>();
		CCDMResultVarString* newItemPtr = dynamic_cast<CCDMResultVarString*>(cDMResultVarStringVal);
		Version_1_0->append(istd::TNullableValue<CDMResultUnionType>(*newItemPtr));
	}

	if (item.canConvert<CCDMResultVarRecursiveObject*>()){
		CCDMResultVarRecursiveObject* cDMResultVarRecursiveVal = item.value<CCDMResultVarRecursiveObject*>();
		CCDMResultVarRecursive* newItemPtr = dynamic_cast<CCDMResultVarRecursive*>(cDMResultVarRecursiveVal);
		Version_1_0->append(istd::TNullableValue<CDMResultUnionType>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


void CCDMResultUnionTypeObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CCDMResultUnionTypeObjectList::clear()
{
	BaseClass::clear();
}


QVariant CCDMResultUnionTypeObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}
} // namespace sdl::V1_0::complextest
