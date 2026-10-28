#include "ComplexUnion1Objects.h"


namespace sdl::V1_0::complextest
{


CPointObject::CPointObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPointObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPointObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPointObject::GetX()
{
	return CPoint::X ? CPoint::X.value() : QVariant();
}


void CPointObject::SetX(const QVariant& v)
{
	CPoint::X = v.value<double>();
	xChanged();
}


bool CPointObject::hasX()
{
	 return CPoint::X.HasValue();
}


QVariant CPointObject::GetY()
{
	return CPoint::Y ? CPoint::Y.value() : QVariant();
}


void CPointObject::SetY(const QVariant& v)
{
	CPoint::Y = v.value<double>();
	yChanged();
}


bool CPointObject::hasY()
{
	 return CPoint::Y.HasValue();
}


QString CPointObject::toJson() const
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


bool CPointObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPointObject::fromObject(const QJsonObject& jsonObject)
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


QString CPointObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CPointObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CPointObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "x")){
		return "X";
	}
	if (propertyName == (QStringLiteral("m_") + "y")){
		return "Y";
	}

	return propertyName;
}





bool CPointObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPointObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPointObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPointObjectList::append(sdl::V1_0::complextest::CPointObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CPointObjectList* sdl::V1_0::complextest::CPointObjectList::copyMe()
{
	sdl::V1_0::complextest::CPointObjectList* retVal = new sdl::V1_0::complextest::CPointObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CPointObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CPointObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CPointObjectList::addElement(sdl::V1_0::complextest::CPointObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CPointObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CPointObjectList::isEqualWithModel(sdl::V1_0::complextest::CPointObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CPointObjectList::insert(int index, sdl::V1_0::complextest::CPointObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CPointObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CPointObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CPointObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CPointObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_x"){
		return Version_1_0->at(index)->X ? QVariant::fromValue(Version_1_0->at(index)->X.value()) : QVariant();
	}
	if (nameId == "m_y"){
		return Version_1_0->at(index)->Y ? QVariant::fromValue(Version_1_0->at(index)->Y.value()) : QVariant();
	}

	return QVariant();
}
CGeometryObject::CGeometryObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CGeometryObject::geometryTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::radiusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::pointsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::requiredPointsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::optionalPointsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CGeometryObject::GetGeometryType()
{
	if (!GeometryType) {return {};}
	sdl::V1_0::complextest::GeometryType valueType = CGeometry::GeometryType.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::GeometryType>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CGeometryObject::SetGeometryType(const QVariant& v)
{
	CGeometry::GeometryType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::GeometryType>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CGeometry::GeometryType = (sdl::V1_0::complextest::GeometryType)key;
	}
	geometryTypeChanged();
}


bool CGeometryObject::hasGeometryType()
{
	 return CGeometry::GeometryType.HasValue();
}


QVariant CGeometryObject::GetRadius()
{
	return CGeometry::Radius ? CGeometry::Radius.value() : QVariant();
}


void CGeometryObject::SetRadius(const QVariant& v)
{
	CGeometry::Radius = v.value<double>();
	radiusChanged();
}


bool CGeometryObject::hasRadius()
{
	 return CGeometry::Radius.HasValue();
}


QVariant CGeometryObject::GetPoints()
{
	if (!Points) {return {};}
	if (!m_pointsQObjectPtr.isValid()){
		m_pointsQObjectPtr = CreateObject("Points");
		auto itemPtr = m_pointsQObjectPtr.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CGeometry::Points;
	}
	return m_pointsQObjectPtr;
}


void CGeometryObject::SetPoints(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){return;}
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CPointObjectList* itemPtr = v.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr){
			if (!itemPtr->Version_1_0){return;}
			CGeometry::Points = itemPtr->Version_1_0;
		}
	}
	else {
		CGeometry::Points.SetNull();
	}
	m_pointsQObjectPtr = v;

	pointsChanged();
}


bool CGeometryObject::hasPoints()
{
	 return CGeometry::Points.HasValue();
}


void CGeometryObject::emplacePoints()
{
	ResetPoints();
	CGeometry::Points.emplace();
}


void CGeometryObject::ResetPoints()
{
	auto* itemPtr = m_pointsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_pointsQObjectPtr = QVariant();
}


QVariant CGeometryObject::createPointsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CPointObject());
}


QVariant CGeometryObject::GetRequiredPoints()
{
	if (!RequiredPoints) {return {};}
	if (!m_requiredPointsQObjectPtr.isValid()){
		m_requiredPointsQObjectPtr = CreateObject("RequiredPoints");
		auto itemPtr = m_requiredPointsQObjectPtr.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CGeometry::RequiredPoints;
	}
	return m_requiredPointsQObjectPtr;
}


void CGeometryObject::SetRequiredPoints(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){return;}
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CPointObjectList* itemPtr = v.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr){
			if (!itemPtr->Version_1_0){return;}
			if (itemPtr->Version_1_0){
				for (const auto& tempValue: itemPtr->Version_1_0.value()){
					if (!tempValue){return;}
				}
			}
			CGeometry::RequiredPoints = itemPtr->Version_1_0;
		}
	}
	else {
		CGeometry::RequiredPoints.SetNull();
	}
	m_requiredPointsQObjectPtr = v;

	requiredPointsChanged();
}


bool CGeometryObject::hasRequiredPoints()
{
	 return CGeometry::RequiredPoints.HasValue();
}


void CGeometryObject::emplaceRequiredPoints()
{
	ResetRequiredPoints();
	CGeometry::RequiredPoints.emplace();
}


void CGeometryObject::ResetRequiredPoints()
{
	auto* itemPtr = m_requiredPointsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_requiredPointsQObjectPtr = QVariant();
}


QVariant CGeometryObject::createRequiredPointsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CPointObject());
}


QVariant CGeometryObject::GetOptionalPoints()
{
	if (!OptionalPoints) {return {};}
	if (!m_optionalPointsQObjectPtr.isValid()){
		m_optionalPointsQObjectPtr = CreateObject("OptionalPoints");
		auto itemPtr = m_optionalPointsQObjectPtr.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CGeometry::OptionalPoints;
	}
	return m_optionalPointsQObjectPtr;
}


void CGeometryObject::SetOptionalPoints(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::complextest::CPointObjectList* itemPtr = v.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr){
			CGeometry::OptionalPoints = itemPtr->Version_1_0;
		}
	}
	else {
		CGeometry::OptionalPoints.SetNull();
	}
	m_optionalPointsQObjectPtr = v;

	optionalPointsChanged();
}


bool CGeometryObject::hasOptionalPoints()
{
	 return CGeometry::OptionalPoints.HasValue();
}


void CGeometryObject::emplaceOptionalPoints()
{
	ResetOptionalPoints();
	CGeometry::OptionalPoints.emplace();
}


void CGeometryObject::ResetOptionalPoints()
{
	auto* itemPtr = m_optionalPointsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_optionalPointsQObjectPtr = QVariant();
}


QVariant CGeometryObject::createOptionalPointsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CPointObject());
}


QString CGeometryObject::toJson() const
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


bool CGeometryObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CGeometryObject::fromObject(const QJsonObject& jsonObject)
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


QString CGeometryObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CGeometryObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "Points"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CPointObjectList(this));
	}
	if (key == "RequiredPoints"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CPointObjectList(this));
	}
	if (key == "OptionalPoints"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CPointObjectList(this));
	}
	return QVariant();
}


QString CGeometryObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "geometryType")){
		return "GeometryType";
	}
	if (propertyName == (QStringLiteral("m_") + "radius")){
		return "Radius";
	}
	if (propertyName == (QStringLiteral("m_") + "points")){
		return "Points";
	}
	if (propertyName == (QStringLiteral("m_") + "requiredPoints")){
		return "RequiredPoints";
	}
	if (propertyName == (QStringLiteral("m_") + "optionalPoints")){
		return "OptionalPoints";
	}

	return propertyName;
}





bool CGeometryObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CGeometryObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CGeometryObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CGeometryObjectList::append(sdl::V1_0::complextest::CGeometryObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CGeometryObjectList* sdl::V1_0::complextest::CGeometryObjectList::copyMe()
{
	sdl::V1_0::complextest::CGeometryObjectList* retVal = new sdl::V1_0::complextest::CGeometryObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CGeometryObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CGeometryObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CGeometryObjectList::addElement(sdl::V1_0::complextest::CGeometryObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CGeometryObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CGeometryObjectList::isEqualWithModel(sdl::V1_0::complextest::CGeometryObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CGeometryObjectList::insert(int index, sdl::V1_0::complextest::CGeometryObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CGeometryObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CGeometryObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CGeometryObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CGeometryObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_geometryType"){
		return Version_1_0->at(index)->GeometryType ? QVariant::fromValue(Version_1_0->at(index)->GeometryType.value()) : QVariant();
	}
	if (nameId == "m_radius"){
		return Version_1_0->at(index)->Radius ? QVariant::fromValue(Version_1_0->at(index)->Radius.value()) : QVariant();
	}
	if (nameId == "m_points"){
		return itemPtr->GetPoints();
	}
	if (nameId == "m_requiredPoints"){
		return itemPtr->GetRequiredPoints();
	}
	if (nameId == "m_optionalPoints"){
		return itemPtr->GetOptionalPoints();
	}

	return QVariant();
}
} // namespace sdl::V1_0::complextest
