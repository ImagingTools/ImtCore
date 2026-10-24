#include "PrinterBaseObjects.h"


namespace sdl::V1_0::modsdl
{


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

	return QVariant();
}
CPrinterBaseObject::CPrinterBaseObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPrinterBaseObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::specificationChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::simpleTestChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::mixedTestChanged, this, &CItemModelBase::OnInternalModelChanged);
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

			m_specificationQObjectPtr = QVariant::fromValue(newObjectPtr);
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


QVariant CPrinterBaseObject::GetSimpleTest()
{
	if (m_simpleTestQObjectPtr.isValid()){
		if (const QString* stringVal = std::get_if<QString>((CPrinterBase::simpleTest).GetPtr())){
			m_simpleTestQObjectPtr = QVariant::fromValue(stringVal);
		}
		if (const double* doubleVal = std::get_if<double>((CPrinterBase::simpleTest).GetPtr())){
			m_simpleTestQObjectPtr = QVariant::fromValue(doubleVal);
		}
	}
	return m_simpleTestQObjectPtr;
}


void CPrinterBaseObject::SetSimpleTest(const QVariant& v)
{
	if (v.isValid()){
		if (const QString* stringVal = v.value<const QString*>()){
			CPrinterBase::simpleTest = *stringVal;
		}
		if (const double* doubleVal = v.value<const double*>()){
			CPrinterBase::simpleTest = *doubleVal;
		}
	}
	else {
		CPrinterBase::simpleTest.SetNull();
	}
	m_simpleTestQObjectPtr = v;

	simpleTestChanged();
}


bool CPrinterBaseObject::hasSimpleTest()
{
	 return CPrinterBase::simpleTest.HasValue();
}


void CPrinterBaseObject::emplaceSimpleTest()
{
	ResetSimpleTest();
	CPrinterBase::simpleTest.emplace();
}


void CPrinterBaseObject::ResetSimpleTest()
{
	auto* itemPtr = m_simpleTestQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_simpleTestQObjectPtr = QVariant();
}


QVariant CPrinterBaseObject::GetMixedTest()
{
	if (m_mixedTestQObjectPtr.isValid()){
		if (const QString* stringVal = std::get_if<QString>((CPrinterBase::mixedTest).GetPtr())){
			m_mixedTestQObjectPtr = QVariant::fromValue(stringVal);
		}
		if (const CLink* linkVal = std::get_if<CLink>((CPrinterBase::mixedTest).GetPtr())){
			CLinkObject *newObjectPtr = new CLinkObject(this);
						newObjectPtr->CLink::link = linkVal->CLink::link;

			m_mixedTestQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
	}
	return m_mixedTestQObjectPtr;
}


void CPrinterBaseObject::SetMixedTest(const QVariant& v)
{
	if (v.isValid()){
		if (const QString* stringVal = v.value<const QString*>()){
			CPrinterBase::mixedTest = *stringVal;
		}
		if (const CLinkObject* linkVal = v.value<const CLinkObject*>()){
			CPrinterBase::mixedTest = *linkVal;
		}
	}
	else {
		CPrinterBase::mixedTest.SetNull();
	}
	m_mixedTestQObjectPtr = v;

	mixedTestChanged();
}


bool CPrinterBaseObject::hasMixedTest()
{
	 return CPrinterBase::mixedTest.HasValue();
}


void CPrinterBaseObject::emplaceMixedTest()
{
	ResetMixedTest();
	CPrinterBase::mixedTest.emplace();
}


void CPrinterBaseObject::ResetMixedTest()
{
	auto* itemPtr = m_mixedTestQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_mixedTestQObjectPtr = QVariant();
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
	Q_UNUSED(key);	return QVariant();
}


QString CPrinterBaseObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "name")){
		return "name";
	}
	if (propertyName == (QStringLiteral("m_") + "specification")){
		return "specification";
	}
	if (propertyName == (QStringLiteral("m_") + "simpleTest")){
		return "simpleTest";
	}
	if (propertyName == (QStringLiteral("m_") + "mixedTest")){
		return "mixedTest";
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
	if (nameId == "m_specification"){
		return itemPtr->GetSpecification();
	}
	if (nameId == "m_simpleTest"){
		return itemPtr->GetSimpleTest();
	}
	if (nameId == "m_mixedTest"){
		return itemPtr->GetMixedTest();
	}

	return QVariant();
}
CPrinterListObject::CPrinterListObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPrinterListObject::dataChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPrinterListObject::GetData()
{
	if (!data) {return {};}
	if (!m_dataQObjectPtr.isValid()){
		m_dataQObjectPtr = CreateObject("data");
		auto itemPtr = m_dataQObjectPtr.value<sdl::V1_0::modsdl::CPrinterBaseObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CPrinterList::data;
	}
	return m_dataQObjectPtr;
}


void CPrinterListObject::SetData(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::modsdl::CPrinterBaseObjectList* itemPtr = v.value<sdl::V1_0::modsdl::CPrinterBaseObjectList*>();
		if (itemPtr != nullptr){
			CPrinterList::data = itemPtr->Version_1_0;
		}
	}
	else {
		CPrinterList::data.SetNull();
	}
	m_dataQObjectPtr = v;

	dataChanged();
}


bool CPrinterListObject::hasData()
{
	 return CPrinterList::data.HasValue();
}


void CPrinterListObject::emplaceData()
{
	ResetData();
	CPrinterList::data.emplace();
}


void CPrinterListObject::ResetData()
{
	auto* itemPtr = m_dataQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_dataQObjectPtr = QVariant();
}


QVariant CPrinterListObject::createDataArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::modsdl::CPrinterBaseObject());
}


QString CPrinterListObject::toJson() const
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


bool CPrinterListObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPrinterListObject::fromObject(const QJsonObject& jsonObject)
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


QString CPrinterListObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CPrinterListObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "data"){
		return QVariant::fromValue(new sdl::V1_0::modsdl::CPrinterBaseObjectList(this));
	}
	return QVariant();
}


QString CPrinterListObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "data")){
		return "data";
	}

	return propertyName;
}





bool CPrinterListObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPrinterListObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPrinterListObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPrinterListObjectList::append(sdl::V1_0::modsdl::CPrinterListObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::modsdl::CPrinterListObjectList* sdl::V1_0::modsdl::CPrinterListObjectList::copyMe()
{
	sdl::V1_0::modsdl::CPrinterListObjectList* retVal = new sdl::V1_0::modsdl::CPrinterListObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::modsdl::CPrinterListObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::modsdl::CPrinterListObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::modsdl::CPrinterListObjectList::addElement(sdl::V1_0::modsdl::CPrinterListObject* item)
{
	append(item);
}


void sdl::V1_0::modsdl::CPrinterListObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::modsdl::CPrinterListObjectList::isEqualWithModel(sdl::V1_0::modsdl::CPrinterListObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::modsdl::CPrinterListObjectList::insert(int index, sdl::V1_0::modsdl::CPrinterListObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::modsdl::CPrinterListObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::modsdl::CPrinterListObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::modsdl::CPrinterListObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::modsdl::CPrinterListObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_data"){
		return itemPtr->GetData();
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



QVariant CSimpleUnionObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
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


bool CSimpleUnionObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CSimpleUnionObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CSimpleUnionObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSimpleUnionObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<QString>()){
		QString stringVal = item.value<QString>();
		Version_1_0->append(istd::TNullableValue<SimpleUnion>(stringVal));
	}

	if (item.canConvert<double>()){
		double doubleVal = item.value<double>();
		Version_1_0->append(istd::TNullableValue<SimpleUnion>(doubleVal));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::modsdl::CSimpleUnionObjectList* CSimpleUnionObjectList::copyMe()
{
	sdl::V1_0::modsdl::CSimpleUnionObjectList* objectListPtr = new sdl::V1_0::modsdl::CSimpleUnionObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

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


QString CSimpleUnionObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

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


QString CSimpleUnionObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

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


void CSimpleUnionObjectList::addElement(QVariant item)
{
	append(item);
}

void CSimpleUnionObjectList::removeElement(int index)
{
	remove(index);
}

bool CSimpleUnionObjectList::isEqualWithModel(sdl::V1_0::modsdl::CSimpleUnionObjectList* otherModelPtr)
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


void CSimpleUnionObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > Version_1_0->size()) return;
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<QString>()){
		QString stringVal = item.value<QString>();
		Version_1_0->append(istd::TNullableValue<SimpleUnion>(stringVal));
	}

	if (item.canConvert<double>()){
		double doubleVal = item.value<double>();
		Version_1_0->append(istd::TNullableValue<SimpleUnion>(doubleVal));
	}

	ClearCache();
	endInsertRows();
}


void CSimpleUnionObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CSimpleUnionObjectList::clear()
{
	BaseClass::clear();
}


QVariant CSimpleUnionObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}



QVariant CMixedUnionObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const QString* stringVal = std::get_if<QString>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(stringVal);
		}

		if (const CLink* linkVal = std::get_if<CLink>(this->Version_1_0->at(index).GetPtr())){
			CLinkObject *newObjectPtr = new CLinkObject();
			static_cast<CLink&>(*newObjectPtr) = *linkVal;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CMixedUnionObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CMixedUnionObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CMixedUnionObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CMixedUnionObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<QString>()){
		QString stringVal = item.value<QString>();
		Version_1_0->append(istd::TNullableValue<MixedUnion>(stringVal));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* linkVal = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(linkVal);
		Version_1_0->append(istd::TNullableValue<MixedUnion>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::modsdl::CMixedUnionObjectList* CMixedUnionObjectList::copyMe()
{
	sdl::V1_0::modsdl::CMixedUnionObjectList* objectListPtr = new sdl::V1_0::modsdl::CMixedUnionObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			objectListPtr->addElement(stringVal);
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* linkVal = item.value<CLinkObject*>();
			objectListPtr->addElement(QVariant::fromValue(linkVal->copyMe()));
		}
	}
	return objectListPtr;
}


QString CMixedUnionObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			retVal += QString(stringVal);
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* linkVal = item.value<CLinkObject*>();
			retVal += linkVal->toJson();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CMixedUnionObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<QString>()){
			QString stringVal = item.value<QString>();
			retVal += QString(stringVal);
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* linkVal = item.value<CLinkObject*>();
			retVal += linkVal->toGraphQL();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CMixedUnionObjectList::addElement(QVariant item)
{
	append(item);
}

void CMixedUnionObjectList::removeElement(int index)
{
	remove(index);
}

bool CMixedUnionObjectList::isEqualWithModel(sdl::V1_0::modsdl::CMixedUnionObjectList* otherModelPtr)
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


void CMixedUnionObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > Version_1_0->size()) return;
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<QString>()){
		QString stringVal = item.value<QString>();
		Version_1_0->append(istd::TNullableValue<MixedUnion>(stringVal));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* linkVal = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(linkVal);
		Version_1_0->append(istd::TNullableValue<MixedUnion>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


void CMixedUnionObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CMixedUnionObjectList::clear()
{
	BaseClass::clear();
}


QVariant CMixedUnionObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}
} // namespace sdl::V1_0::modsdl
