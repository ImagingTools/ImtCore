#include "SubstrateSpecificationsObjects.h"


namespace sdl::V1_0::substrate
{


CInputIdObject::CInputIdObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CInputIdObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CInputIdObject::GetId()
{
	return CInputId::id ? CInputId::id.value() : QVariant();
}


void CInputIdObject::SetId(const QVariant& v)
{
	CInputId::id = v.value<QString>().toUtf8();
	idChanged();
}


bool CInputIdObject::hasId()
{
	 return CInputId::id.HasValue();
}


QString CInputIdObject::toJson() const
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


bool CInputIdObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CInputIdObject::fromObject(const QJsonObject& jsonObject)
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


QString CInputIdObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CInputIdObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CInputIdObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "id")){
		return "id";
	}

	return propertyName;
}





bool CInputIdObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CInputIdObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CInputIdObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CInputIdObjectList::append(sdl::V1_0::substrate::CInputIdObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::substrate::CInputIdObjectList* sdl::V1_0::substrate::CInputIdObjectList::copyMe()
{
	sdl::V1_0::substrate::CInputIdObjectList* retVal = new sdl::V1_0::substrate::CInputIdObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::substrate::CInputIdObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::substrate::CInputIdObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::substrate::CInputIdObjectList::addElement(sdl::V1_0::substrate::CInputIdObject* item)
{
	append(item);
}


void sdl::V1_0::substrate::CInputIdObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::substrate::CInputIdObjectList::isEqualWithModel(sdl::V1_0::substrate::CInputIdObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::substrate::CInputIdObjectList::insert(int index, sdl::V1_0::substrate::CInputIdObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::substrate::CInputIdObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::substrate::CInputIdObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::substrate::CInputIdObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::substrate::CInputIdObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_id"){
		return Version_1_0->at(index)->id ? QVariant::fromValue(Version_1_0->at(index)->id.value()) : QVariant();
	}

	return QVariant();
}
CCollectionViewParamsTestObject::CCollectionViewParamsTestObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCollectionViewParamsTestObject::countChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCollectionViewParamsTestObject::offsetChanged, this, &CItemModelBase::OnInternalModelChanged);
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
	Q_UNUSED(key);	return QVariant();
}


QString CCollectionViewParamsTestObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "count")){
		return "count";
	}
	if (propertyName == (QStringLiteral("m_") + "offset")){
		return "offset";
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


void CCollectionViewParamsTestObjectList::append(sdl::V1_0::substrate::CCollectionViewParamsTestObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::substrate::CCollectionViewParamsTestObjectList* sdl::V1_0::substrate::CCollectionViewParamsTestObjectList::copyMe()
{
	sdl::V1_0::substrate::CCollectionViewParamsTestObjectList* retVal = new sdl::V1_0::substrate::CCollectionViewParamsTestObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::substrate::CCollectionViewParamsTestObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::substrate::CCollectionViewParamsTestObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::substrate::CCollectionViewParamsTestObjectList::addElement(sdl::V1_0::substrate::CCollectionViewParamsTestObject* item)
{
	append(item);
}


void sdl::V1_0::substrate::CCollectionViewParamsTestObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::substrate::CCollectionViewParamsTestObjectList::isEqualWithModel(sdl::V1_0::substrate::CCollectionViewParamsTestObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::substrate::CCollectionViewParamsTestObjectList::insert(int index, sdl::V1_0::substrate::CCollectionViewParamsTestObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::substrate::CCollectionViewParamsTestObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::substrate::CCollectionViewParamsTestObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::substrate::CCollectionViewParamsTestObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::substrate::CCollectionViewParamsTestObject*>();
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

	return QVariant();
}
CSubstrateSpecificationDocumentTypeIdsObject::CSubstrateSpecificationDocumentTypeIdsObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationDocumentTypeIdsObject::paperIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationDocumentTypeIdsObject::cardboardIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationDocumentTypeIdsObject::filmIDChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationDocumentTypeIdsObject::GetPaperID()
{
	return CSubstrateSpecificationDocumentTypeIds::PaperID ? CSubstrateSpecificationDocumentTypeIds::PaperID.value() : QVariant();
}


void CSubstrateSpecificationDocumentTypeIdsObject::SetPaperID(const QVariant& v)
{
	CSubstrateSpecificationDocumentTypeIds::PaperID = v.value<QString>().toUtf8();
	paperIDChanged();
}


bool CSubstrateSpecificationDocumentTypeIdsObject::hasPaperID()
{
	 return CSubstrateSpecificationDocumentTypeIds::PaperID.HasValue();
}


QVariant CSubstrateSpecificationDocumentTypeIdsObject::GetCardboardID()
{
	return CSubstrateSpecificationDocumentTypeIds::CardboardID ? CSubstrateSpecificationDocumentTypeIds::CardboardID.value() : QVariant();
}


void CSubstrateSpecificationDocumentTypeIdsObject::SetCardboardID(const QVariant& v)
{
	CSubstrateSpecificationDocumentTypeIds::CardboardID = v.value<QString>().toUtf8();
	cardboardIDChanged();
}


bool CSubstrateSpecificationDocumentTypeIdsObject::hasCardboardID()
{
	 return CSubstrateSpecificationDocumentTypeIds::CardboardID.HasValue();
}


QVariant CSubstrateSpecificationDocumentTypeIdsObject::GetFilmID()
{
	return CSubstrateSpecificationDocumentTypeIds::FilmID ? CSubstrateSpecificationDocumentTypeIds::FilmID.value() : QVariant();
}


void CSubstrateSpecificationDocumentTypeIdsObject::SetFilmID(const QVariant& v)
{
	CSubstrateSpecificationDocumentTypeIds::FilmID = v.value<QString>().toUtf8();
	filmIDChanged();
}


bool CSubstrateSpecificationDocumentTypeIdsObject::hasFilmID()
{
	 return CSubstrateSpecificationDocumentTypeIds::FilmID.HasValue();
}


QString CSubstrateSpecificationDocumentTypeIdsObject::toJson() const
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


bool CSubstrateSpecificationDocumentTypeIdsObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationDocumentTypeIdsObject::fromObject(const QJsonObject& jsonObject)
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


QString CSubstrateSpecificationDocumentTypeIdsObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationDocumentTypeIdsObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSubstrateSpecificationDocumentTypeIdsObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "paperID")){
		return "PaperID";
	}
	if (propertyName == (QStringLiteral("m_") + "cardboardID")){
		return "CardboardID";
	}
	if (propertyName == (QStringLiteral("m_") + "filmID")){
		return "FilmID";
	}

	return propertyName;
}





bool CSubstrateSpecificationDocumentTypeIdsObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationDocumentTypeIdsObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationDocumentTypeIdsObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationDocumentTypeIdsObjectList::append(sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList* sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList::copyMe()
{
	sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList* retVal = new sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList::addElement(sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObject* item)
{
	append(item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList::isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList::insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_paperID"){
		return Version_1_0->at(index)->PaperID ? QVariant::fromValue(Version_1_0->at(index)->PaperID.value()) : QVariant();
	}
	if (nameId == "m_cardboardID"){
		return Version_1_0->at(index)->CardboardID ? QVariant::fromValue(Version_1_0->at(index)->CardboardID.value()) : QVariant();
	}
	if (nameId == "m_filmID"){
		return Version_1_0->at(index)->FilmID ? QVariant::fromValue(Version_1_0->at(index)->FilmID.value()) : QVariant();
	}

	return QVariant();
}
CSubstrateSpecificationListInputObject::CSubstrateSpecificationListInputObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationListInputObject::viewParamsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationListInputObject::GetViewParams()
{
	if (!viewParams) {return {};}
	
	if (!m_viewParamsQObjectPtr.isValid()){
		m_viewParamsQObjectPtr = CreateObject("viewParams");
		auto itemPtr = m_viewParamsQObjectPtr.value<sdl::V1_0::substrate::CCollectionViewParamsTestObject*>();
		if (itemPtr != nullptr) {
			itemPtr->count = viewParams->count;
			itemPtr->offset = viewParams->offset;
		}
	}
	return m_viewParamsQObjectPtr;
}


void CSubstrateSpecificationListInputObject::SetViewParams(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::substrate::CCollectionViewParamsTestObject* itemPtr = v.value<sdl::V1_0::substrate::CCollectionViewParamsTestObject*>();
		if (itemPtr != nullptr) CSubstrateSpecificationListInput::viewParams = (*itemPtr);
	}
	else {
		CSubstrateSpecificationListInput::viewParams.SetNull();
	}
	m_viewParamsQObjectPtr = v;

	viewParamsChanged();
}


bool CSubstrateSpecificationListInputObject::hasViewParams()
{
	 return CSubstrateSpecificationListInput::viewParams.HasValue();
}


void CSubstrateSpecificationListInputObject::emplaceViewParams()
{
	ResetViewParams();
	CSubstrateSpecificationListInput::viewParams.emplace();
}


void CSubstrateSpecificationListInputObject::ResetViewParams()
{
	auto* itemPtr = m_viewParamsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_viewParamsQObjectPtr = QVariant();
}


QString CSubstrateSpecificationListInputObject::toJson() const
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


bool CSubstrateSpecificationListInputObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationListInputObject::fromObject(const QJsonObject& jsonObject)
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


QString CSubstrateSpecificationListInputObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationListInputObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "viewParams"){
		return QVariant::fromValue(new sdl::V1_0::substrate::CCollectionViewParamsTestObject(this));
	}
	return QVariant();
}


QString CSubstrateSpecificationListInputObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "viewParams")){
		return "viewParams";
	}

	return propertyName;
}





bool CSubstrateSpecificationListInputObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationListInputObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationListInputObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationListInputObjectList::append(sdl::V1_0::substrate::CSubstrateSpecificationListInputObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList* sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList::copyMe()
{
	sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList* retVal = new sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList::addElement(sdl::V1_0::substrate::CSubstrateSpecificationListInputObject* item)
{
	append(item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList::isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList::insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationListInputObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::substrate::CSubstrateSpecificationListInputObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_viewParams"){
		return itemPtr->GetViewParams();
	}

	return QVariant();
}
CSubstrateSpecificationListItemObject::CSubstrateSpecificationListItemObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationListItemObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::typeIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::materialIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::materialNameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::addedChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::timeStampChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::colorPreviewChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationListItemObject::GetId()
{
	return CSubstrateSpecificationListItem::id ? CSubstrateSpecificationListItem::id.value() : QVariant();
}


void CSubstrateSpecificationListItemObject::SetId(const QVariant& v)
{
	CSubstrateSpecificationListItem::id = v.value<QString>().toUtf8();
	idChanged();
}


bool CSubstrateSpecificationListItemObject::hasId()
{
	 return CSubstrateSpecificationListItem::id.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetTypeId()
{
	return CSubstrateSpecificationListItem::typeId ? CSubstrateSpecificationListItem::typeId.value() : QVariant();
}


void CSubstrateSpecificationListItemObject::SetTypeId(const QVariant& v)
{
	CSubstrateSpecificationListItem::typeId = v.value<QString>().toUtf8();
	typeIdChanged();
}


bool CSubstrateSpecificationListItemObject::hasTypeId()
{
	 return CSubstrateSpecificationListItem::typeId.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetName()
{
	return CSubstrateSpecificationListItem::name ? CSubstrateSpecificationListItem::name.value() : QVariant();
}


void CSubstrateSpecificationListItemObject::SetName(const QVariant& v)
{
	CSubstrateSpecificationListItem::name = v.value<QString>();
	nameChanged();
}


bool CSubstrateSpecificationListItemObject::hasName()
{
	 return CSubstrateSpecificationListItem::name.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetMaterialId()
{
	return CSubstrateSpecificationListItem::materialId ? CSubstrateSpecificationListItem::materialId.value() : QVariant();
}


void CSubstrateSpecificationListItemObject::SetMaterialId(const QVariant& v)
{
	CSubstrateSpecificationListItem::materialId = v.value<QString>().toUtf8();
	materialIdChanged();
}


bool CSubstrateSpecificationListItemObject::hasMaterialId()
{
	 return CSubstrateSpecificationListItem::materialId.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetMaterialName()
{
	return CSubstrateSpecificationListItem::materialName ? CSubstrateSpecificationListItem::materialName.value() : QVariant();
}


void CSubstrateSpecificationListItemObject::SetMaterialName(const QVariant& v)
{
	CSubstrateSpecificationListItem::materialName = v.value<QString>();
	materialNameChanged();
}


bool CSubstrateSpecificationListItemObject::hasMaterialName()
{
	 return CSubstrateSpecificationListItem::materialName.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetAdded()
{
	return CSubstrateSpecificationListItem::added ? CSubstrateSpecificationListItem::added.value() : QVariant();
}


void CSubstrateSpecificationListItemObject::SetAdded(const QVariant& v)
{
	CSubstrateSpecificationListItem::added = v.value<QString>();
	addedChanged();
}


bool CSubstrateSpecificationListItemObject::hasAdded()
{
	 return CSubstrateSpecificationListItem::added.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetTimeStamp()
{
	return CSubstrateSpecificationListItem::timeStamp ? CSubstrateSpecificationListItem::timeStamp.value() : QVariant();
}


void CSubstrateSpecificationListItemObject::SetTimeStamp(const QVariant& v)
{
	CSubstrateSpecificationListItem::timeStamp = v.value<QString>();
	timeStampChanged();
}


bool CSubstrateSpecificationListItemObject::hasTimeStamp()
{
	 return CSubstrateSpecificationListItem::timeStamp.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetColorPreview()
{
	return CSubstrateSpecificationListItem::colorPreview ? CSubstrateSpecificationListItem::colorPreview.value() : QVariant();
}


void CSubstrateSpecificationListItemObject::SetColorPreview(const QVariant& v)
{
	CSubstrateSpecificationListItem::colorPreview = v.value<QString>();
	colorPreviewChanged();
}


bool CSubstrateSpecificationListItemObject::hasColorPreview()
{
	 return CSubstrateSpecificationListItem::colorPreview.HasValue();
}


QString CSubstrateSpecificationListItemObject::toJson() const
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


bool CSubstrateSpecificationListItemObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationListItemObject::fromObject(const QJsonObject& jsonObject)
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


QString CSubstrateSpecificationListItemObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationListItemObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSubstrateSpecificationListItemObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "id")){
		return "id";
	}
	if (propertyName == (QStringLiteral("m_") + "typeId")){
		return "typeId";
	}
	if (propertyName == (QStringLiteral("m_") + "name")){
		return "name";
	}
	if (propertyName == (QStringLiteral("m_") + "materialId")){
		return "materialId";
	}
	if (propertyName == (QStringLiteral("m_") + "materialName")){
		return "materialName";
	}
	if (propertyName == (QStringLiteral("m_") + "added")){
		return "added";
	}
	if (propertyName == (QStringLiteral("m_") + "timeStamp")){
		return "timeStamp";
	}
	if (propertyName == (QStringLiteral("m_") + "colorPreview")){
		return "colorPreview";
	}

	return propertyName;
}





bool CSubstrateSpecificationListItemObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationListItemObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationListItemObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationListItemObjectList::append(sdl::V1_0::substrate::CSubstrateSpecificationListItemObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList* sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList::copyMe()
{
	sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList* retVal = new sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList::addElement(sdl::V1_0::substrate::CSubstrateSpecificationListItemObject* item)
{
	append(item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList::isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList::insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationListItemObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::substrate::CSubstrateSpecificationListItemObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_id"){
		return Version_1_0->at(index)->id ? QVariant::fromValue(Version_1_0->at(index)->id.value()) : QVariant();
	}
	if (nameId == "m_typeId"){
		return Version_1_0->at(index)->typeId ? QVariant::fromValue(Version_1_0->at(index)->typeId.value()) : QVariant();
	}
	if (nameId == "m_name"){
		return Version_1_0->at(index)->name ? QVariant::fromValue(Version_1_0->at(index)->name.value()) : QVariant();
	}
	if (nameId == "m_materialId"){
		return Version_1_0->at(index)->materialId ? QVariant::fromValue(Version_1_0->at(index)->materialId.value()) : QVariant();
	}
	if (nameId == "m_materialName"){
		return Version_1_0->at(index)->materialName ? QVariant::fromValue(Version_1_0->at(index)->materialName.value()) : QVariant();
	}
	if (nameId == "m_added"){
		return Version_1_0->at(index)->added ? QVariant::fromValue(Version_1_0->at(index)->added.value()) : QVariant();
	}
	if (nameId == "m_timeStamp"){
		return Version_1_0->at(index)->timeStamp ? QVariant::fromValue(Version_1_0->at(index)->timeStamp.value()) : QVariant();
	}
	if (nameId == "m_colorPreview"){
		return Version_1_0->at(index)->colorPreview ? QVariant::fromValue(Version_1_0->at(index)->colorPreview.value()) : QVariant();
	}

	return QVariant();
}
CSubstrateSpecificationListResponseObject::CSubstrateSpecificationListResponseObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationListResponseObject::itemsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationListResponseObject::GetItems()
{
	if (!items) {return {};}
	if (!m_itemsQObjectPtr.isValid()){
		m_itemsQObjectPtr = CreateObject("items");
		auto itemPtr = m_itemsQObjectPtr.value<sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CSubstrateSpecificationListResponse::items;
	}
	return m_itemsQObjectPtr;
}


void CSubstrateSpecificationListResponseObject::SetItems(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList* itemPtr = v.value<sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList*>();
		if (itemPtr != nullptr){
			CSubstrateSpecificationListResponse::items = itemPtr->Version_1_0;
		}
	}
	else {
		CSubstrateSpecificationListResponse::items.SetNull();
	}
	m_itemsQObjectPtr = v;

	itemsChanged();
}


bool CSubstrateSpecificationListResponseObject::hasItems()
{
	 return CSubstrateSpecificationListResponse::items.HasValue();
}


void CSubstrateSpecificationListResponseObject::emplaceItems()
{
	ResetItems();
	CSubstrateSpecificationListResponse::items.emplace();
}


void CSubstrateSpecificationListResponseObject::ResetItems()
{
	auto* itemPtr = m_itemsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_itemsQObjectPtr = QVariant();
}


QVariant CSubstrateSpecificationListResponseObject::createItemsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::substrate::CSubstrateSpecificationListItemObject());
}


QString CSubstrateSpecificationListResponseObject::toJson() const
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


bool CSubstrateSpecificationListResponseObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationListResponseObject::fromObject(const QJsonObject& jsonObject)
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


QString CSubstrateSpecificationListResponseObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationListResponseObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "items"){
		return QVariant::fromValue(new sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList(this));
	}
	return QVariant();
}


QString CSubstrateSpecificationListResponseObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "items")){
		return "items";
	}

	return propertyName;
}





bool CSubstrateSpecificationListResponseObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationListResponseObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationListResponseObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationListResponseObjectList::append(sdl::V1_0::substrate::CSubstrateSpecificationListResponseObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList* sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList::copyMe()
{
	sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList* retVal = new sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList::addElement(sdl::V1_0::substrate::CSubstrateSpecificationListResponseObject* item)
{
	append(item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList::isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList::insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationListResponseObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::substrate::CSubstrateSpecificationListResponseObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_items"){
		return itemPtr->GetItems();
	}

	return QVariant();
}
CCardboardSubstrateSpecificationObject::CCardboardSubstrateSpecificationObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCardboardSubstrateSpecificationObject::colorChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCardboardSubstrateSpecificationObject::grammageChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCardboardSubstrateSpecificationObject::GetColor()
{
	return CCardboardSubstrateSpecification::color ? CCardboardSubstrateSpecification::color.value() : QVariant();
}


void CCardboardSubstrateSpecificationObject::SetColor(const QVariant& v)
{
	CCardboardSubstrateSpecification::color = v.value<QString>();
	colorChanged();
}


bool CCardboardSubstrateSpecificationObject::hasColor()
{
	 return CCardboardSubstrateSpecification::color.HasValue();
}


QVariant CCardboardSubstrateSpecificationObject::GetGrammage()
{
	return CCardboardSubstrateSpecification::grammage ? CCardboardSubstrateSpecification::grammage.value() : QVariant();
}


void CCardboardSubstrateSpecificationObject::SetGrammage(const QVariant& v)
{
	CCardboardSubstrateSpecification::grammage = v.value<double>();
	grammageChanged();
}


bool CCardboardSubstrateSpecificationObject::hasGrammage()
{
	 return CCardboardSubstrateSpecification::grammage.HasValue();
}


QString CCardboardSubstrateSpecificationObject::toJson() const
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


bool CCardboardSubstrateSpecificationObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCardboardSubstrateSpecificationObject::fromObject(const QJsonObject& jsonObject)
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


QString CCardboardSubstrateSpecificationObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CCardboardSubstrateSpecificationObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CCardboardSubstrateSpecificationObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "color")){
		return "color";
	}
	if (propertyName == (QStringLiteral("m_") + "grammage")){
		return "grammage";
	}

	return propertyName;
}





bool CCardboardSubstrateSpecificationObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCardboardSubstrateSpecificationObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCardboardSubstrateSpecificationObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCardboardSubstrateSpecificationObjectList::append(sdl::V1_0::substrate::CCardboardSubstrateSpecificationObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList* sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList::copyMe()
{
	sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList* retVal = new sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList::addElement(sdl::V1_0::substrate::CCardboardSubstrateSpecificationObject* item)
{
	append(item);
}


void sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList::isEqualWithModel(sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList::insert(int index, sdl::V1_0::substrate::CCardboardSubstrateSpecificationObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::substrate::CCardboardSubstrateSpecificationObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_color"){
		return Version_1_0->at(index)->color ? QVariant::fromValue(Version_1_0->at(index)->color.value()) : QVariant();
	}
	if (nameId == "m_grammage"){
		return Version_1_0->at(index)->grammage ? QVariant::fromValue(Version_1_0->at(index)->grammage.value()) : QVariant();
	}

	return QVariant();
}
CPaperSubstrateSpecificationObject::CPaperSubstrateSpecificationObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPaperSubstrateSpecificationObject::grammageChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPaperSubstrateSpecificationObject::colorChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPaperSubstrateSpecificationObject::coatTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPaperSubstrateSpecificationObject::GetGrammage()
{
	return CPaperSubstrateSpecification::grammage ? CPaperSubstrateSpecification::grammage.value() : QVariant();
}


void CPaperSubstrateSpecificationObject::SetGrammage(const QVariant& v)
{
	CPaperSubstrateSpecification::grammage = v.value<double>();
	grammageChanged();
}


bool CPaperSubstrateSpecificationObject::hasGrammage()
{
	 return CPaperSubstrateSpecification::grammage.HasValue();
}


QVariant CPaperSubstrateSpecificationObject::GetColor()
{
	return CPaperSubstrateSpecification::color ? CPaperSubstrateSpecification::color.value() : QVariant();
}


void CPaperSubstrateSpecificationObject::SetColor(const QVariant& v)
{
	CPaperSubstrateSpecification::color = v.value<QString>();
	colorChanged();
}


bool CPaperSubstrateSpecificationObject::hasColor()
{
	 return CPaperSubstrateSpecification::color.HasValue();
}


QVariant CPaperSubstrateSpecificationObject::GetCoatType()
{
	return CPaperSubstrateSpecification::coatType ? CPaperSubstrateSpecification::coatType.value() : QVariant();
}


void CPaperSubstrateSpecificationObject::SetCoatType(const QVariant& v)
{
	CPaperSubstrateSpecification::coatType = v.value<QString>();
	coatTypeChanged();
}


bool CPaperSubstrateSpecificationObject::hasCoatType()
{
	 return CPaperSubstrateSpecification::coatType.HasValue();
}


QString CPaperSubstrateSpecificationObject::toJson() const
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


bool CPaperSubstrateSpecificationObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPaperSubstrateSpecificationObject::fromObject(const QJsonObject& jsonObject)
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


QString CPaperSubstrateSpecificationObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CPaperSubstrateSpecificationObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CPaperSubstrateSpecificationObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "grammage")){
		return "grammage";
	}
	if (propertyName == (QStringLiteral("m_") + "color")){
		return "color";
	}
	if (propertyName == (QStringLiteral("m_") + "coatType")){
		return "coatType";
	}

	return propertyName;
}





bool CPaperSubstrateSpecificationObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPaperSubstrateSpecificationObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPaperSubstrateSpecificationObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPaperSubstrateSpecificationObjectList::append(sdl::V1_0::substrate::CPaperSubstrateSpecificationObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList* sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList::copyMe()
{
	sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList* retVal = new sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList::addElement(sdl::V1_0::substrate::CPaperSubstrateSpecificationObject* item)
{
	append(item);
}


void sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList::isEqualWithModel(sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList::insert(int index, sdl::V1_0::substrate::CPaperSubstrateSpecificationObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::substrate::CPaperSubstrateSpecificationObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_grammage"){
		return Version_1_0->at(index)->grammage ? QVariant::fromValue(Version_1_0->at(index)->grammage.value()) : QVariant();
	}
	if (nameId == "m_color"){
		return Version_1_0->at(index)->color ? QVariant::fromValue(Version_1_0->at(index)->color.value()) : QVariant();
	}
	if (nameId == "m_coatType"){
		return Version_1_0->at(index)->coatType ? QVariant::fromValue(Version_1_0->at(index)->coatType.value()) : QVariant();
	}

	return QVariant();
}
CFilmSubstrateSpecificationObject::CFilmSubstrateSpecificationObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CFilmSubstrateSpecificationObject::transparencyChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFilmSubstrateSpecificationObject::colorChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFilmSubstrateSpecificationObject::materialChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CFilmSubstrateSpecificationObject::GetTransparency()
{
	return CFilmSubstrateSpecification::transparency ? CFilmSubstrateSpecification::transparency.value() : QVariant();
}


void CFilmSubstrateSpecificationObject::SetTransparency(const QVariant& v)
{
	CFilmSubstrateSpecification::transparency = v.value<double>();
	transparencyChanged();
}


bool CFilmSubstrateSpecificationObject::hasTransparency()
{
	 return CFilmSubstrateSpecification::transparency.HasValue();
}


QVariant CFilmSubstrateSpecificationObject::GetColor()
{
	return CFilmSubstrateSpecification::color ? CFilmSubstrateSpecification::color.value() : QVariant();
}


void CFilmSubstrateSpecificationObject::SetColor(const QVariant& v)
{
	CFilmSubstrateSpecification::color = v.value<QString>();
	colorChanged();
}


bool CFilmSubstrateSpecificationObject::hasColor()
{
	 return CFilmSubstrateSpecification::color.HasValue();
}


QVariant CFilmSubstrateSpecificationObject::GetMaterial()
{
	return CFilmSubstrateSpecification::material ? CFilmSubstrateSpecification::material.value() : QVariant();
}


void CFilmSubstrateSpecificationObject::SetMaterial(const QVariant& v)
{
	CFilmSubstrateSpecification::material = v.value<QString>();
	materialChanged();
}


bool CFilmSubstrateSpecificationObject::hasMaterial()
{
	 return CFilmSubstrateSpecification::material.HasValue();
}


QString CFilmSubstrateSpecificationObject::toJson() const
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


bool CFilmSubstrateSpecificationObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CFilmSubstrateSpecificationObject::fromObject(const QJsonObject& jsonObject)
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


QString CFilmSubstrateSpecificationObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CFilmSubstrateSpecificationObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CFilmSubstrateSpecificationObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "transparency")){
		return "transparency";
	}
	if (propertyName == (QStringLiteral("m_") + "color")){
		return "color";
	}
	if (propertyName == (QStringLiteral("m_") + "material")){
		return "material";
	}

	return propertyName;
}





bool CFilmSubstrateSpecificationObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CFilmSubstrateSpecificationObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CFilmSubstrateSpecificationObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CFilmSubstrateSpecificationObjectList::append(sdl::V1_0::substrate::CFilmSubstrateSpecificationObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList* sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList::copyMe()
{
	sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList* retVal = new sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList::addElement(sdl::V1_0::substrate::CFilmSubstrateSpecificationObject* item)
{
	append(item);
}


void sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList::isEqualWithModel(sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList::insert(int index, sdl::V1_0::substrate::CFilmSubstrateSpecificationObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::substrate::CFilmSubstrateSpecificationObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_transparency"){
		return Version_1_0->at(index)->transparency ? QVariant::fromValue(Version_1_0->at(index)->transparency.value()) : QVariant();
	}
	if (nameId == "m_color"){
		return Version_1_0->at(index)->color ? QVariant::fromValue(Version_1_0->at(index)->color.value()) : QVariant();
	}
	if (nameId == "m_material"){
		return Version_1_0->at(index)->material ? QVariant::fromValue(Version_1_0->at(index)->material.value()) : QVariant();
	}

	return QVariant();
}
CSubstrateSpecificationInputObject::CSubstrateSpecificationInputObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationInputObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::descriptionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::itemChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::typeIdChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationInputObject::GetId()
{
	return CSubstrateSpecificationInput::id ? CSubstrateSpecificationInput::id.value() : QVariant();
}


void CSubstrateSpecificationInputObject::SetId(const QVariant& v)
{
	CSubstrateSpecificationInput::id = v.value<QString>().toUtf8();
	idChanged();
}


bool CSubstrateSpecificationInputObject::hasId()
{
	 return CSubstrateSpecificationInput::id.HasValue();
}


QVariant CSubstrateSpecificationInputObject::GetName()
{
	return CSubstrateSpecificationInput::name ? CSubstrateSpecificationInput::name.value() : QVariant();
}


void CSubstrateSpecificationInputObject::SetName(const QVariant& v)
{
	CSubstrateSpecificationInput::name = v.value<QString>();
	nameChanged();
}


bool CSubstrateSpecificationInputObject::hasName()
{
	 return CSubstrateSpecificationInput::name.HasValue();
}


QVariant CSubstrateSpecificationInputObject::GetDescription()
{
	return CSubstrateSpecificationInput::description ? CSubstrateSpecificationInput::description.value() : QVariant();
}


void CSubstrateSpecificationInputObject::SetDescription(const QVariant& v)
{
	CSubstrateSpecificationInput::description = v.value<QString>();
	descriptionChanged();
}


bool CSubstrateSpecificationInputObject::hasDescription()
{
	 return CSubstrateSpecificationInput::description.HasValue();
}


QVariant CSubstrateSpecificationInputObject::GetItem()
{
	if (m_itemQObjectPtr.isValid()){
		if (const CCardboardSubstrateSpecification* cardboardSubstrateSpecificationVal = std::get_if<CCardboardSubstrateSpecification>((CSubstrateSpecificationInput::item).GetPtr())){
			CCardboardSubstrateSpecificationObject *newObjectPtr = new CCardboardSubstrateSpecificationObject(this);
						newObjectPtr->CCardboardSubstrateSpecification::color = cardboardSubstrateSpecificationVal->CCardboardSubstrateSpecification::color;
			newObjectPtr->CCardboardSubstrateSpecification::grammage = cardboardSubstrateSpecificationVal->CCardboardSubstrateSpecification::grammage;

			m_itemQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const CPaperSubstrateSpecification* paperSubstrateSpecificationVal = std::get_if<CPaperSubstrateSpecification>((CSubstrateSpecificationInput::item).GetPtr())){
			CPaperSubstrateSpecificationObject *newObjectPtr = new CPaperSubstrateSpecificationObject(this);
						newObjectPtr->CPaperSubstrateSpecification::grammage = paperSubstrateSpecificationVal->CPaperSubstrateSpecification::grammage;
			newObjectPtr->CPaperSubstrateSpecification::color = paperSubstrateSpecificationVal->CPaperSubstrateSpecification::color;
			newObjectPtr->CPaperSubstrateSpecification::coatType = paperSubstrateSpecificationVal->CPaperSubstrateSpecification::coatType;

			m_itemQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const CFilmSubstrateSpecification* filmSubstrateSpecificationVal = std::get_if<CFilmSubstrateSpecification>((CSubstrateSpecificationInput::item).GetPtr())){
			CFilmSubstrateSpecificationObject *newObjectPtr = new CFilmSubstrateSpecificationObject(this);
						newObjectPtr->CFilmSubstrateSpecification::transparency = filmSubstrateSpecificationVal->CFilmSubstrateSpecification::transparency;
			newObjectPtr->CFilmSubstrateSpecification::color = filmSubstrateSpecificationVal->CFilmSubstrateSpecification::color;
			newObjectPtr->CFilmSubstrateSpecification::material = filmSubstrateSpecificationVal->CFilmSubstrateSpecification::material;

			m_itemQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
	}
	return m_itemQObjectPtr;
}


void CSubstrateSpecificationInputObject::SetItem(const QVariant& v)
{
	if (v.isValid()){
		if (const CCardboardSubstrateSpecificationObject* cardboardSubstrateSpecificationVal = v.value<const CCardboardSubstrateSpecificationObject*>()){
			CSubstrateSpecificationInput::item = *cardboardSubstrateSpecificationVal;
		}
		if (const CPaperSubstrateSpecificationObject* paperSubstrateSpecificationVal = v.value<const CPaperSubstrateSpecificationObject*>()){
			CSubstrateSpecificationInput::item = *paperSubstrateSpecificationVal;
		}
		if (const CFilmSubstrateSpecificationObject* filmSubstrateSpecificationVal = v.value<const CFilmSubstrateSpecificationObject*>()){
			CSubstrateSpecificationInput::item = *filmSubstrateSpecificationVal;
		}
	}
	else {
		CSubstrateSpecificationInput::item.SetNull();
	}
	m_itemQObjectPtr = v;

	itemChanged();
}


bool CSubstrateSpecificationInputObject::hasItem()
{
	 return CSubstrateSpecificationInput::item.HasValue();
}


void CSubstrateSpecificationInputObject::emplaceItem()
{
	ResetItem();
	CSubstrateSpecificationInput::item.emplace();
}


void CSubstrateSpecificationInputObject::ResetItem()
{
	auto* itemPtr = m_itemQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_itemQObjectPtr = QVariant();
}


QVariant CSubstrateSpecificationInputObject::GetTypeId()
{
	return CSubstrateSpecificationInput::typeId ? CSubstrateSpecificationInput::typeId.value() : QVariant();
}


void CSubstrateSpecificationInputObject::SetTypeId(const QVariant& v)
{
	CSubstrateSpecificationInput::typeId = v.value<QString>().toUtf8();
	typeIdChanged();
}


bool CSubstrateSpecificationInputObject::hasTypeId()
{
	 return CSubstrateSpecificationInput::typeId.HasValue();
}


QString CSubstrateSpecificationInputObject::toJson() const
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


bool CSubstrateSpecificationInputObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationInputObject::fromObject(const QJsonObject& jsonObject)
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


QString CSubstrateSpecificationInputObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationInputObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSubstrateSpecificationInputObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "id")){
		return "id";
	}
	if (propertyName == (QStringLiteral("m_") + "name")){
		return "name";
	}
	if (propertyName == (QStringLiteral("m_") + "description")){
		return "description";
	}
	if (propertyName == (QStringLiteral("m_") + "item")){
		return "item";
	}
	if (propertyName == (QStringLiteral("m_") + "typeId")){
		return "typeId";
	}

	return propertyName;
}





bool CSubstrateSpecificationInputObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationInputObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationInputObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationInputObjectList::append(sdl::V1_0::substrate::CSubstrateSpecificationInputObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList* sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList::copyMe()
{
	sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList* retVal = new sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList::addElement(sdl::V1_0::substrate::CSubstrateSpecificationInputObject* item)
{
	append(item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList::isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList::insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationInputObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::substrate::CSubstrateSpecificationInputObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_id"){
		return Version_1_0->at(index)->id ? QVariant::fromValue(Version_1_0->at(index)->id.value()) : QVariant();
	}
	if (nameId == "m_name"){
		return Version_1_0->at(index)->name ? QVariant::fromValue(Version_1_0->at(index)->name.value()) : QVariant();
	}
	if (nameId == "m_description"){
		return Version_1_0->at(index)->description ? QVariant::fromValue(Version_1_0->at(index)->description.value()) : QVariant();
	}
	if (nameId == "m_item"){
		return itemPtr->GetItem();
	}
	if (nameId == "m_typeId"){
		return Version_1_0->at(index)->typeId ? QVariant::fromValue(Version_1_0->at(index)->typeId.value()) : QVariant();
	}

	return QVariant();
}
CSubstrateSpecificationDocumentMetaInfoObject::CSubstrateSpecificationDocumentMetaInfoObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationDocumentMetaInfoObject::materialIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationDocumentMetaInfoObject::labChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationDocumentMetaInfoObject::GetMaterialId()
{
	return CSubstrateSpecificationDocumentMetaInfo::materialId ? CSubstrateSpecificationDocumentMetaInfo::materialId.value() : QVariant();
}


void CSubstrateSpecificationDocumentMetaInfoObject::SetMaterialId(const QVariant& v)
{
	CSubstrateSpecificationDocumentMetaInfo::materialId = v.value<QString>().toUtf8();
	materialIdChanged();
}


bool CSubstrateSpecificationDocumentMetaInfoObject::hasMaterialId()
{
	 return CSubstrateSpecificationDocumentMetaInfo::materialId.HasValue();
}


QVariant CSubstrateSpecificationDocumentMetaInfoObject::GetLab()
{
	return CSubstrateSpecificationDocumentMetaInfo::lab ? CSubstrateSpecificationDocumentMetaInfo::lab.value() : QVariant();
}


void CSubstrateSpecificationDocumentMetaInfoObject::SetLab(const QVariant& v)
{
	CSubstrateSpecificationDocumentMetaInfo::lab = v.value<QString>();
	labChanged();
}


bool CSubstrateSpecificationDocumentMetaInfoObject::hasLab()
{
	 return CSubstrateSpecificationDocumentMetaInfo::lab.HasValue();
}


QString CSubstrateSpecificationDocumentMetaInfoObject::toJson() const
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


bool CSubstrateSpecificationDocumentMetaInfoObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationDocumentMetaInfoObject::fromObject(const QJsonObject& jsonObject)
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


QString CSubstrateSpecificationDocumentMetaInfoObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationDocumentMetaInfoObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSubstrateSpecificationDocumentMetaInfoObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "materialId")){
		return "materialId";
	}
	if (propertyName == (QStringLiteral("m_") + "lab")){
		return "lab";
	}

	return propertyName;
}





bool CSubstrateSpecificationDocumentMetaInfoObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationDocumentMetaInfoObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationDocumentMetaInfoObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationDocumentMetaInfoObjectList::append(sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList* sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList::copyMe()
{
	sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList* retVal = new sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList::addElement(sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObject* item)
{
	append(item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList::isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList::insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_materialId"){
		return Version_1_0->at(index)->materialId ? QVariant::fromValue(Version_1_0->at(index)->materialId.value()) : QVariant();
	}
	if (nameId == "m_lab"){
		return Version_1_0->at(index)->lab ? QVariant::fromValue(Version_1_0->at(index)->lab.value()) : QVariant();
	}

	return QVariant();
}



QVariant CSubstrateSpecificationDocumentObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const CCardboardSubstrateSpecification* cardboardSubstrateSpecificationVal = std::get_if<CCardboardSubstrateSpecification>(this->Version_1_0->at(index).GetPtr())){
			CCardboardSubstrateSpecificationObject *newObjectPtr = new CCardboardSubstrateSpecificationObject();
			static_cast<CCardboardSubstrateSpecification&>(*newObjectPtr) = *cardboardSubstrateSpecificationVal;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const CPaperSubstrateSpecification* paperSubstrateSpecificationVal = std::get_if<CPaperSubstrateSpecification>(this->Version_1_0->at(index).GetPtr())){
			CPaperSubstrateSpecificationObject *newObjectPtr = new CPaperSubstrateSpecificationObject();
			static_cast<CPaperSubstrateSpecification&>(*newObjectPtr) = *paperSubstrateSpecificationVal;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const CFilmSubstrateSpecification* filmSubstrateSpecificationVal = std::get_if<CFilmSubstrateSpecification>(this->Version_1_0->at(index).GetPtr())){
			CFilmSubstrateSpecificationObject *newObjectPtr = new CFilmSubstrateSpecificationObject();
			static_cast<CFilmSubstrateSpecification&>(*newObjectPtr) = *filmSubstrateSpecificationVal;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CSubstrateSpecificationDocumentObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CSubstrateSpecificationDocumentObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationDocumentObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationDocumentObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
		CCardboardSubstrateSpecificationObject* cardboardSubstrateSpecificationVal = item.value<CCardboardSubstrateSpecificationObject*>();
		CCardboardSubstrateSpecification* newItemPtr = dynamic_cast<CCardboardSubstrateSpecification*>(cardboardSubstrateSpecificationVal);
		Version_1_0->append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
		CPaperSubstrateSpecificationObject* paperSubstrateSpecificationVal = item.value<CPaperSubstrateSpecificationObject*>();
		CPaperSubstrateSpecification* newItemPtr = dynamic_cast<CPaperSubstrateSpecification*>(paperSubstrateSpecificationVal);
		Version_1_0->append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
		CFilmSubstrateSpecificationObject* filmSubstrateSpecificationVal = item.value<CFilmSubstrateSpecificationObject*>();
		CFilmSubstrateSpecification* newItemPtr = dynamic_cast<CFilmSubstrateSpecification*>(filmSubstrateSpecificationVal);
		Version_1_0->append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::substrate::CSubstrateSpecificationDocumentObjectList* CSubstrateSpecificationDocumentObjectList::copyMe()
{
	sdl::V1_0::substrate::CSubstrateSpecificationDocumentObjectList* objectListPtr = new sdl::V1_0::substrate::CSubstrateSpecificationDocumentObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
			CCardboardSubstrateSpecificationObject* cardboardSubstrateSpecificationVal = item.value<CCardboardSubstrateSpecificationObject*>();
			objectListPtr->addElement(QVariant::fromValue(cardboardSubstrateSpecificationVal->copyMe()));
		}

		if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
			CPaperSubstrateSpecificationObject* paperSubstrateSpecificationVal = item.value<CPaperSubstrateSpecificationObject*>();
			objectListPtr->addElement(QVariant::fromValue(paperSubstrateSpecificationVal->copyMe()));
		}

		if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
			CFilmSubstrateSpecificationObject* filmSubstrateSpecificationVal = item.value<CFilmSubstrateSpecificationObject*>();
			objectListPtr->addElement(QVariant::fromValue(filmSubstrateSpecificationVal->copyMe()));
		}
	}
	return objectListPtr;
}


QString CSubstrateSpecificationDocumentObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
			CCardboardSubstrateSpecificationObject* cardboardSubstrateSpecificationVal = item.value<CCardboardSubstrateSpecificationObject*>();
			retVal += cardboardSubstrateSpecificationVal->toJson();
		}

		if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
			CPaperSubstrateSpecificationObject* paperSubstrateSpecificationVal = item.value<CPaperSubstrateSpecificationObject*>();
			retVal += paperSubstrateSpecificationVal->toJson();
		}

		if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
			CFilmSubstrateSpecificationObject* filmSubstrateSpecificationVal = item.value<CFilmSubstrateSpecificationObject*>();
			retVal += filmSubstrateSpecificationVal->toJson();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CSubstrateSpecificationDocumentObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
			CCardboardSubstrateSpecificationObject* cardboardSubstrateSpecificationVal = item.value<CCardboardSubstrateSpecificationObject*>();
			retVal += cardboardSubstrateSpecificationVal->toGraphQL();
		}

		if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
			CPaperSubstrateSpecificationObject* paperSubstrateSpecificationVal = item.value<CPaperSubstrateSpecificationObject*>();
			retVal += paperSubstrateSpecificationVal->toGraphQL();
		}

		if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
			CFilmSubstrateSpecificationObject* filmSubstrateSpecificationVal = item.value<CFilmSubstrateSpecificationObject*>();
			retVal += filmSubstrateSpecificationVal->toGraphQL();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CSubstrateSpecificationDocumentObjectList::addElement(QVariant item)
{
	append(item);
}

void CSubstrateSpecificationDocumentObjectList::removeElement(int index)
{
	remove(index);
}

bool CSubstrateSpecificationDocumentObjectList::isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationDocumentObjectList* otherModelPtr)
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


void CSubstrateSpecificationDocumentObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > Version_1_0->size()) return;
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
		CCardboardSubstrateSpecificationObject* cardboardSubstrateSpecificationVal = item.value<CCardboardSubstrateSpecificationObject*>();
		CCardboardSubstrateSpecification* newItemPtr = dynamic_cast<CCardboardSubstrateSpecification*>(cardboardSubstrateSpecificationVal);
		Version_1_0->append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
		CPaperSubstrateSpecificationObject* paperSubstrateSpecificationVal = item.value<CPaperSubstrateSpecificationObject*>();
		CPaperSubstrateSpecification* newItemPtr = dynamic_cast<CPaperSubstrateSpecification*>(paperSubstrateSpecificationVal);
		Version_1_0->append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
		CFilmSubstrateSpecificationObject* filmSubstrateSpecificationVal = item.value<CFilmSubstrateSpecificationObject*>();
		CFilmSubstrateSpecification* newItemPtr = dynamic_cast<CFilmSubstrateSpecification*>(filmSubstrateSpecificationVal);
		Version_1_0->append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


void CSubstrateSpecificationDocumentObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CSubstrateSpecificationDocumentObjectList::clear()
{
	BaseClass::clear();
}


QVariant CSubstrateSpecificationDocumentObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}
} // namespace sdl::V1_0::substrate
