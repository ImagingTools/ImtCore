#include "UnionTestObjects.h"


namespace sdl::V1_0::modsdl
{


CCoordsObject::CCoordsObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCoordsObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCoordsObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCoordsObject::GetX()
{
	return CCoords::X ? CCoords::X.value() : QVariant();
}


void CCoordsObject::SetX(const QVariant& v)
{
	CCoords::X = v.value<double>();
	xChanged();
}


bool CCoordsObject::hasX()
{
	 return CCoords::X.HasValue();
}


QVariant CCoordsObject::GetY()
{
	return CCoords::Y ? CCoords::Y.value() : QVariant();
}


void CCoordsObject::SetY(const QVariant& v)
{
	CCoords::Y = v.value<double>();
	yChanged();
}


bool CCoordsObject::hasY()
{
	 return CCoords::Y.HasValue();
}


QString CCoordsObject::toJson() const
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


bool CCoordsObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCoordsObject::fromObject(const QJsonObject& jsonObject)
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


QString CCoordsObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CCoordsObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CCoordsObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "x")){
		return "X";
	}
	if (propertyName == (QStringLiteral("m_") + "y")){
		return "Y";
	}

	return propertyName;
}





bool CCoordsObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCoordsObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCoordsObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCoordsObjectList::append(sdl::V1_0::modsdl::CCoordsObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::modsdl::CCoordsObjectList* sdl::V1_0::modsdl::CCoordsObjectList::copyMe()
{
	sdl::V1_0::modsdl::CCoordsObjectList* retVal = new sdl::V1_0::modsdl::CCoordsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::modsdl::CCoordsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::modsdl::CCoordsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::modsdl::CCoordsObjectList::addElement(sdl::V1_0::modsdl::CCoordsObject* item)
{
	append(item);
}


void sdl::V1_0::modsdl::CCoordsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::modsdl::CCoordsObjectList::isEqualWithModel(sdl::V1_0::modsdl::CCoordsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::modsdl::CCoordsObjectList::insert(int index, sdl::V1_0::modsdl::CCoordsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::modsdl::CCoordsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::modsdl::CCoordsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::modsdl::CCoordsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::modsdl::CCoordsObject*>();
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
CPrinterSpecificationBaseObject::CPrinterSpecificationBaseObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPrinterSpecificationBaseObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPrinterSpecificationBaseObject::GetName()
{
	return CPrinterSpecificationBase::name ? CPrinterSpecificationBase::name.value() : QVariant();
}


void CPrinterSpecificationBaseObject::SetName(const QVariant& v)
{
	CPrinterSpecificationBase::name = v.value<QString>();
	nameChanged();
}


bool CPrinterSpecificationBaseObject::hasName()
{
	 return CPrinterSpecificationBase::name.HasValue();
}


QString CPrinterSpecificationBaseObject::toJson() const
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


bool CPrinterSpecificationBaseObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPrinterSpecificationBaseObject::fromObject(const QJsonObject& jsonObject)
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


QString CPrinterSpecificationBaseObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CPrinterSpecificationBaseObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CPrinterSpecificationBaseObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "name")){
		return "name";
	}

	return propertyName;
}





bool CPrinterSpecificationBaseObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPrinterSpecificationBaseObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPrinterSpecificationBaseObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPrinterSpecificationBaseObjectList::append(sdl::V1_0::modsdl::CPrinterSpecificationBaseObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList* sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::copyMe()
{
	sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList* retVal = new sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::addElement(sdl::V1_0::modsdl::CPrinterSpecificationBaseObject* item)
{
	append(item);
}


void sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::isEqualWithModel(sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::insert(int index, sdl::V1_0::modsdl::CPrinterSpecificationBaseObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::modsdl::CPrinterSpecificationBaseObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_name"){
		return Version_1_0->at(index)->name ? QVariant::fromValue(Version_1_0->at(index)->name.value()) : QVariant();
	}

	return QVariant();
}
CLinkObject::CLinkObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CLinkObject::linkChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CLinkObject::statusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CLinkObject::statusListChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CLinkObject::GetLink()
{
	return CLink::link ? CLink::link.value() : QVariant();
}


void CLinkObject::SetLink(const QVariant& v)
{
	CLink::link = v.value<QString>().toUtf8();
	linkChanged();
}


bool CLinkObject::hasLink()
{
	 return CLink::link.HasValue();
}


QVariant CLinkObject::GetStatus()
{
	if (!status) {return {};}
	sdl::V1_0::modsdl::LinkStatus valueType = CLink::status.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::modsdl::LinkStatus>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CLinkObject::SetStatus(const QVariant& v)
{
	CLink::status.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::modsdl::LinkStatus>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CLink::status = (sdl::V1_0::modsdl::LinkStatus)key;
	}
	statusChanged();
}


bool CLinkObject::hasStatus()
{
	 return CLink::status.HasValue();
}


QVariant CLinkObject::GetStatusList()
{
	if (!statusList) {return {};}

	QVariantList tempStatusListList;
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::modsdl::LinkStatus>();
	for (const auto& tempValue: statusList.value()){
		tempStatusListList << (tempValue ? QVariant(QString::fromUtf8(metaEnum.valueToKey(int(*tempValue)))) : QVariant());
	}

	return tempStatusListList;
}


void CLinkObject::SetStatusList(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){
		CLink::statusList.SetNull();
		statusListChanged();
		return;
	}
	if (!CLink::statusList){
		CLink::statusList.emplace();
	}
	else{
		CLink::statusList->clear();
	}
	for (const QVariant& tempValue: v.toList()){
		if (!tempValue.isValid() || tempValue.isNull()){
			CLink::statusList->AppendNull();
			continue;
		}
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::modsdl::LinkStatus>();
		const int key = metaEnum.keyToValue(tempValue.toString().toUtf8());
		if (key < 0){continue;}
		istd::TNullableValue<sdl::V1_0::modsdl::LinkStatus> tempItem(static_cast<sdl::V1_0::modsdl::LinkStatus>(key));
		CLink::statusList->append(tempItem);
	}

	statusListChanged();
}


bool CLinkObject::hasStatusList()
{
	 return CLink::statusList.HasValue();
}


QString CLinkObject::toJson() const
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


bool CLinkObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CLinkObject::fromObject(const QJsonObject& jsonObject)
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


QString CLinkObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CLinkObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CLinkObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "link")){
		return "link";
	}
	if (propertyName == (QStringLiteral("m_") + "status")){
		return "status";
	}
	if (propertyName == (QStringLiteral("m_") + "statusList")){
		return "statusList";
	}

	return propertyName;
}





bool CLinkObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CLinkObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CLinkObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CLinkObjectList::append(sdl::V1_0::modsdl::CLinkObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::modsdl::CLinkObjectList* sdl::V1_0::modsdl::CLinkObjectList::copyMe()
{
	sdl::V1_0::modsdl::CLinkObjectList* retVal = new sdl::V1_0::modsdl::CLinkObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::modsdl::CLinkObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::modsdl::CLinkObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::modsdl::CLinkObjectList::addElement(sdl::V1_0::modsdl::CLinkObject* item)
{
	append(item);
}


void sdl::V1_0::modsdl::CLinkObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::modsdl::CLinkObjectList::isEqualWithModel(sdl::V1_0::modsdl::CLinkObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::modsdl::CLinkObjectList::insert(int index, sdl::V1_0::modsdl::CLinkObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::modsdl::CLinkObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::modsdl::CLinkObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::modsdl::CLinkObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::modsdl::CLinkObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_link"){
		return Version_1_0->at(index)->link ? QVariant::fromValue(Version_1_0->at(index)->link.value()) : QVariant();
	}
	if (nameId == "m_status"){
		return Version_1_0->at(index)->status ? QVariant::fromValue(Version_1_0->at(index)->status.value()) : QVariant();
	}
	if (nameId == "m_statusList"){
		return Version_1_0->at(index)->statusList ? QVariant::fromValue(Version_1_0->at(index)->statusList.value()) : QVariant();
	}

	return QVariant();
}
CPrinterBaseObject::CPrinterBaseObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPrinterBaseObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::linkListChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::specificationChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::specificationListChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPrinterBaseObject::GetName()
{
	return CPrinterBase::name ? CPrinterBase::name.value() : QVariant();
}


void CPrinterBaseObject::SetName(const QVariant& v)
{
	CPrinterBase::name = v.value<QString>();
	nameChanged();
}


bool CPrinterBaseObject::hasName()
{
	 return CPrinterBase::name.HasValue();
}


QVariant CPrinterBaseObject::GetLinkList()
{
	if (!linkList) {return {};}
	if (!m_linkListQObjectPtr.isValid()){
		m_linkListQObjectPtr = CreateObject("linkList");
		auto itemPtr = m_linkListQObjectPtr.value<sdl::V1_0::modsdl::CLinkObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CPrinterBase::linkList;
	}
	return m_linkListQObjectPtr;
}


void CPrinterBaseObject::SetLinkList(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){return;}
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::modsdl::CLinkObjectList* itemPtr = v.value<sdl::V1_0::modsdl::CLinkObjectList*>();
		if (itemPtr != nullptr){
			if (!itemPtr->Version_1_0){return;}
			CPrinterBase::linkList = itemPtr->Version_1_0;
		}
	}
	else {
		CPrinterBase::linkList.SetNull();
	}
	m_linkListQObjectPtr = v;

	linkListChanged();
}


bool CPrinterBaseObject::hasLinkList()
{
	 return CPrinterBase::linkList.HasValue();
}


void CPrinterBaseObject::emplaceLinkList()
{
	ResetLinkList();
	CPrinterBase::linkList.emplace();
}


void CPrinterBaseObject::ResetLinkList()
{
	auto* itemPtr = m_linkListQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_linkListQObjectPtr = QVariant();
}


QVariant CPrinterBaseObject::createLinkListArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::modsdl::CLinkObject());
}


QVariant CPrinterBaseObject::GetSpecification()
{
	if (m_specificationQObjectPtr.isValid()){
		if (const CPrinterSpecificationBase* printerSpecificationBaseVal = std::get_if<CPrinterSpecificationBase>((CPrinterBase::specification).GetPtr())){
			CPrinterSpecificationBaseObject *newObjectPtr = new CPrinterSpecificationBaseObject(this);
						newObjectPtr->CPrinterSpecificationBase::name = printerSpecificationBaseVal->CPrinterSpecificationBase::name;

			m_specificationQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const CLink* linkVal = std::get_if<CLink>((CPrinterBase::specification).GetPtr())){
			CLinkObject *newObjectPtr = new CLinkObject(this);
						newObjectPtr->CLink::link = linkVal->CLink::link;
			newObjectPtr->CLink::status = linkVal->CLink::status;
			newObjectPtr->CLink::statusList = linkVal->CLink::statusList;

			m_specificationQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const QString* stringVal = std::get_if<QString>((CPrinterBase::specification).GetPtr())){
			m_specificationQObjectPtr = QVariant::fromValue(stringVal);
		}
		if (const double* doubleVal = std::get_if<double>((CPrinterBase::specification).GetPtr())){
			m_specificationQObjectPtr = QVariant::fromValue(doubleVal);
		}
	}
	return m_specificationQObjectPtr;
}


void CPrinterBaseObject::SetSpecification(const QVariant& v)
{
	if (v.isValid()){
		if (const CPrinterSpecificationBaseObject* printerSpecificationBaseVal = v.value<const CPrinterSpecificationBaseObject*>()){
			CPrinterBase::specification = *printerSpecificationBaseVal;
		}
		if (const CLinkObject* linkVal = v.value<const CLinkObject*>()){
			CPrinterBase::specification = *linkVal;
		}
		if (const QString* stringVal = v.value<const QString*>()){
			CPrinterBase::specification = *stringVal;
		}
		if (const double* doubleVal = v.value<const double*>()){
			CPrinterBase::specification = *doubleVal;
		}
	}
	else {
		CPrinterBase::specification.SetNull();
	}
	m_specificationQObjectPtr = v;

	specificationChanged();
}


bool CPrinterBaseObject::hasSpecification()
{
	 return CPrinterBase::specification.HasValue();
}


void CPrinterBaseObject::emplaceSpecification()
{
	ResetSpecification();
	CPrinterBase::specification.emplace();
}


void CPrinterBaseObject::ResetSpecification()
{
	auto* itemPtr = m_specificationQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_specificationQObjectPtr = QVariant();
}


QVariant CPrinterBaseObject::GetSpecificationList()
{
	if (!specificationList) {return {};}
	if (!m_specificationListQObjectPtr.isValid()){
		m_specificationListQObjectPtr = CreateObject("specificationList");
		auto itemPtr = m_specificationListQObjectPtr.value<sdl::V1_0::modsdl::CPrinterSpecificationObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CPrinterBase::specificationList;
	}
	return m_specificationListQObjectPtr;
}


void CPrinterBaseObject::SetSpecificationList(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){return;}
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::modsdl::CPrinterSpecificationObjectList* itemPtr = v.value<sdl::V1_0::modsdl::CPrinterSpecificationObjectList*>();
		if (itemPtr != nullptr){
			if (!itemPtr->Version_1_0){return;}
			CPrinterBase::specificationList = itemPtr->Version_1_0;
		}
	}
	else {
		CPrinterBase::specificationList.SetNull();
	}
	m_specificationListQObjectPtr = v;

	specificationListChanged();
}


bool CPrinterBaseObject::hasSpecificationList()
{
	 return CPrinterBase::specificationList.HasValue();
}


void CPrinterBaseObject::emplaceSpecificationList()
{
	ResetSpecificationList();
	CPrinterBase::specificationList.emplace();
}


void CPrinterBaseObject::ResetSpecificationList()
{
	auto* itemPtr = m_specificationListQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_specificationListQObjectPtr = QVariant();
}


QString CPrinterBaseObject::toJson() const
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


bool CPrinterBaseObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPrinterBaseObject::fromObject(const QJsonObject& jsonObject)
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


QString CPrinterBaseObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CPrinterBaseObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "linkList"){
		return QVariant::fromValue(new sdl::V1_0::modsdl::CLinkObjectList(this));
	}
	if (key == "specificationList"){
		return QVariant::fromValue(new sdl::V1_0::modsdl::CPrinterSpecificationObjectList(this));
	}
	return QVariant();
}


QString CPrinterBaseObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "name")){
		return "name";
	}
	if (propertyName == (QStringLiteral("m_") + "linkList")){
		return "linkList";
	}
	if (propertyName == (QStringLiteral("m_") + "specification")){
		return "specification";
	}
	if (propertyName == (QStringLiteral("m_") + "specificationList")){
		return "specificationList";
	}

	return propertyName;
}





bool CPrinterBaseObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPrinterBaseObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPrinterBaseObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPrinterBaseObjectList::append(sdl::V1_0::modsdl::CPrinterBaseObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::modsdl::CPrinterBaseObjectList* sdl::V1_0::modsdl::CPrinterBaseObjectList::copyMe()
{
	sdl::V1_0::modsdl::CPrinterBaseObjectList* retVal = new sdl::V1_0::modsdl::CPrinterBaseObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::modsdl::CPrinterBaseObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::modsdl::CPrinterBaseObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::modsdl::CPrinterBaseObjectList::addElement(sdl::V1_0::modsdl::CPrinterBaseObject* item)
{
	append(item);
}


void sdl::V1_0::modsdl::CPrinterBaseObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::modsdl::CPrinterBaseObjectList::isEqualWithModel(sdl::V1_0::modsdl::CPrinterBaseObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::modsdl::CPrinterBaseObjectList::insert(int index, sdl::V1_0::modsdl::CPrinterBaseObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::modsdl::CPrinterBaseObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::modsdl::CPrinterBaseObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::modsdl::CPrinterBaseObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::modsdl::CPrinterBaseObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_name"){
		return Version_1_0->at(index)->name ? QVariant::fromValue(Version_1_0->at(index)->name.value()) : QVariant();
	}
	if (nameId == "m_linkList"){
		return itemPtr->GetLinkList();
	}
	if (nameId == "m_specification"){
		return itemPtr->GetSpecification();
	}
	if (nameId == "m_specificationList"){
		return itemPtr->GetSpecificationList();
	}

	return QVariant();
}



QVariant CPrinterSpecificationObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const CPrinterSpecificationBase* printerSpecificationBaseVal = std::get_if<CPrinterSpecificationBase>(this->Version_1_0->at(index).GetPtr())){
			CPrinterSpecificationBaseObject *newObjectPtr = new CPrinterSpecificationBaseObject();
			static_cast<CPrinterSpecificationBase&>(*newObjectPtr) = *printerSpecificationBaseVal;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const CLink* linkVal = std::get_if<CLink>(this->Version_1_0->at(index).GetPtr())){
			CLinkObject *newObjectPtr = new CLinkObject();
			static_cast<CLink&>(*newObjectPtr) = *linkVal;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const QString* stringVal = std::get_if<QString>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(stringVal);
		}

		if (const double* doubleVal = std::get_if<double>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(doubleVal);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CPrinterSpecificationObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CPrinterSpecificationObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CPrinterSpecificationObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPrinterSpecificationObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CPrinterSpecificationBaseObject*>()){
		CPrinterSpecificationBaseObject* printerSpecificationBaseVal = item.value<CPrinterSpecificationBaseObject*>();
		CPrinterSpecificationBase* newItemPtr = dynamic_cast<CPrinterSpecificationBase*>(printerSpecificationBaseVal);
		Version_1_0->append(istd::TNullableValue<PrinterSpecification>(*newItemPtr));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* linkVal = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(linkVal);
		Version_1_0->append(istd::TNullableValue<PrinterSpecification>(*newItemPtr));
	}

	if (item.canConvert<QString>()){
		QString stringVal = item.value<QString>();
		Version_1_0->append(istd::TNullableValue<PrinterSpecification>(stringVal));
	}

	if (item.canConvert<double>()){
		double doubleVal = item.value<double>();
		Version_1_0->append(istd::TNullableValue<PrinterSpecification>(doubleVal));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::modsdl::CPrinterSpecificationObjectList* CPrinterSpecificationObjectList::copyMe()
{
	sdl::V1_0::modsdl::CPrinterSpecificationObjectList* objectListPtr = new sdl::V1_0::modsdl::CPrinterSpecificationObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<CPrinterSpecificationBaseObject*>()){
			CPrinterSpecificationBaseObject* printerSpecificationBaseVal = item.value<CPrinterSpecificationBaseObject*>();
			objectListPtr->addElement(QVariant::fromValue(printerSpecificationBaseVal->copyMe()));
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* linkVal = item.value<CLinkObject*>();
			objectListPtr->addElement(QVariant::fromValue(linkVal->copyMe()));
		}

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			objectListPtr->addElement(stringVal);
		}

		if (item.canConvert<double>()){
			double doubleVal = item.value<double>();
			objectListPtr->addElement(doubleVal);
		}
	}
	return objectListPtr;
}


QString CPrinterSpecificationObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CPrinterSpecificationBaseObject*>()){
			CPrinterSpecificationBaseObject* printerSpecificationBaseVal = item.value<CPrinterSpecificationBaseObject*>();
			retVal += printerSpecificationBaseVal->toJson();
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* linkVal = item.value<CLinkObject*>();
			retVal += linkVal->toJson();
		}

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			retVal += QString(stringVal);
		}

		if (item.canConvert<double>()){
			double doubleVal = item.value<double>();
			retVal += QString::number(doubleVal);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CPrinterSpecificationObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CPrinterSpecificationBaseObject*>()){
			CPrinterSpecificationBaseObject* printerSpecificationBaseVal = item.value<CPrinterSpecificationBaseObject*>();
			retVal += printerSpecificationBaseVal->toGraphQL();
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* linkVal = item.value<CLinkObject*>();
			retVal += linkVal->toGraphQL();
		}

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			retVal += QString(stringVal);
		}

		if (item.canConvert<double>()){
			double doubleVal = item.value<double>();
			retVal += QString::number(doubleVal);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CPrinterSpecificationObjectList::addElement(QVariant item)
{
	append(item);
}

void CPrinterSpecificationObjectList::removeElement(int index)
{
	remove(index);
}

bool CPrinterSpecificationObjectList::isEqualWithModel(sdl::V1_0::modsdl::CPrinterSpecificationObjectList* otherModelPtr)
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


void CPrinterSpecificationObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > Version_1_0->size()) return;
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CPrinterSpecificationBaseObject*>()){
		CPrinterSpecificationBaseObject* printerSpecificationBaseVal = item.value<CPrinterSpecificationBaseObject*>();
		CPrinterSpecificationBase* newItemPtr = dynamic_cast<CPrinterSpecificationBase*>(printerSpecificationBaseVal);
		Version_1_0->append(istd::TNullableValue<PrinterSpecification>(*newItemPtr));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* linkVal = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(linkVal);
		Version_1_0->append(istd::TNullableValue<PrinterSpecification>(*newItemPtr));
	}

	if (item.canConvert<QString>()){
		QString stringVal = item.value<QString>();
		Version_1_0->append(istd::TNullableValue<PrinterSpecification>(stringVal));
	}

	if (item.canConvert<double>()){
		double doubleVal = item.value<double>();
		Version_1_0->append(istd::TNullableValue<PrinterSpecification>(doubleVal));
	}

	ClearCache();
	endInsertRows();
}


void CPrinterSpecificationObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CPrinterSpecificationObjectList::clear()
{
	BaseClass::clear();
}


QVariant CPrinterSpecificationObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}
} // namespace sdl::V1_0::modsdl
