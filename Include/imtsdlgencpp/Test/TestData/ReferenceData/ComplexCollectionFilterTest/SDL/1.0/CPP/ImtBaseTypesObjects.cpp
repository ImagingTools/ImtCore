#include "ImtBaseTypesObjects.h"


namespace sdl::V1_0::imtbase
{


CTimeRangeObject::CTimeRangeObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CTimeRangeObject::beginChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeRangeObject::endChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CTimeRangeObject::GetBegin()
{
	return CTimeRange::Begin ? CTimeRange::Begin.value() : QVariant();
}


void CTimeRangeObject::SetBegin(const QVariant& v)
{
	CTimeRange::Begin = v.value<QString>();
	beginChanged();
}


bool CTimeRangeObject::hasBegin()
{
	 return CTimeRange::Begin.HasValue();
}


QVariant CTimeRangeObject::GetEnd()
{
	return CTimeRange::End ? CTimeRange::End.value() : QVariant();
}


void CTimeRangeObject::SetEnd(const QVariant& v)
{
	CTimeRange::End = v.value<QString>();
	endChanged();
}


bool CTimeRangeObject::hasEnd()
{
	 return CTimeRange::End.HasValue();
}


QString CTimeRangeObject::toJson() const
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


bool CTimeRangeObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CTimeRangeObject::fromObject(const QJsonObject& jsonObject)
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


QString CTimeRangeObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CTimeRangeObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CTimeRangeObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "begin")){
		return "Begin";
	}
	if (propertyName == (QStringLiteral("m_") + "end")){
		return "End";
	}

	return propertyName;
}





bool CTimeRangeObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CTimeRangeObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CTimeRangeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CTimeRangeObjectList::append(sdl::V1_0::imtbase::CTimeRangeObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CTimeRangeObjectList* sdl::V1_0::imtbase::CTimeRangeObjectList::copyMe()
{
	sdl::V1_0::imtbase::CTimeRangeObjectList* retVal = new sdl::V1_0::imtbase::CTimeRangeObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CTimeRangeObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CTimeRangeObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CTimeRangeObjectList::addElement(sdl::V1_0::imtbase::CTimeRangeObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CTimeRangeObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CTimeRangeObjectList::isEqualWithModel(sdl::V1_0::imtbase::CTimeRangeObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CTimeRangeObjectList::insert(int index, sdl::V1_0::imtbase::CTimeRangeObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CTimeRangeObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CTimeRangeObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CTimeRangeObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CTimeRangeObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_begin"){
		return Version_1_0->at(index)->Begin ? QVariant::fromValue(Version_1_0->at(index)->Begin.value()) : QVariant();
	}
	if (nameId == "m_end"){
		return Version_1_0->at(index)->End ? QVariant::fromValue(Version_1_0->at(index)->End.value()) : QVariant();
	}

	return QVariant();
}
CSdlSizeObject::CSdlSizeObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSdlSizeObject::widthChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSdlSizeObject::heightChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSdlSizeObject::GetWidth()
{
	return CSdlSize::width ? CSdlSize::width.value() : QVariant();
}


void CSdlSizeObject::SetWidth(const QVariant& v)
{
	CSdlSize::width = v.value<double>();
	widthChanged();
}


bool CSdlSizeObject::hasWidth()
{
	 return CSdlSize::width.HasValue();
}


QVariant CSdlSizeObject::GetHeight()
{
	return CSdlSize::height ? CSdlSize::height.value() : QVariant();
}


void CSdlSizeObject::SetHeight(const QVariant& v)
{
	CSdlSize::height = v.value<double>();
	heightChanged();
}


bool CSdlSizeObject::hasHeight()
{
	 return CSdlSize::height.HasValue();
}


QString CSdlSizeObject::toJson() const
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


bool CSdlSizeObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSdlSizeObject::fromObject(const QJsonObject& jsonObject)
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


QString CSdlSizeObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSdlSizeObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSdlSizeObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "width")){
		return "width";
	}
	if (propertyName == (QStringLiteral("m_") + "height")){
		return "height";
	}

	return propertyName;
}





bool CSdlSizeObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSdlSizeObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSdlSizeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSdlSizeObjectList::append(sdl::V1_0::imtbase::CSdlSizeObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CSdlSizeObjectList* sdl::V1_0::imtbase::CSdlSizeObjectList::copyMe()
{
	sdl::V1_0::imtbase::CSdlSizeObjectList* retVal = new sdl::V1_0::imtbase::CSdlSizeObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CSdlSizeObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CSdlSizeObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CSdlSizeObjectList::addElement(sdl::V1_0::imtbase::CSdlSizeObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CSdlSizeObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CSdlSizeObjectList::isEqualWithModel(sdl::V1_0::imtbase::CSdlSizeObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CSdlSizeObjectList::insert(int index, sdl::V1_0::imtbase::CSdlSizeObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CSdlSizeObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CSdlSizeObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CSdlSizeObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CSdlSizeObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_width"){
		return Version_1_0->at(index)->width ? QVariant::fromValue(Version_1_0->at(index)->width.value()) : QVariant();
	}
	if (nameId == "m_height"){
		return Version_1_0->at(index)->height ? QVariant::fromValue(Version_1_0->at(index)->height.value()) : QVariant();
	}

	return QVariant();
}
CSdlPointObject::CSdlPointObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSdlPointObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSdlPointObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSdlPointObject::GetX()
{
	return CSdlPoint::x ? CSdlPoint::x.value() : QVariant();
}


void CSdlPointObject::SetX(const QVariant& v)
{
	CSdlPoint::x = v.value<double>();
	xChanged();
}


bool CSdlPointObject::hasX()
{
	 return CSdlPoint::x.HasValue();
}


QVariant CSdlPointObject::GetY()
{
	return CSdlPoint::y ? CSdlPoint::y.value() : QVariant();
}


void CSdlPointObject::SetY(const QVariant& v)
{
	CSdlPoint::y = v.value<double>();
	yChanged();
}


bool CSdlPointObject::hasY()
{
	 return CSdlPoint::y.HasValue();
}


QString CSdlPointObject::toJson() const
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


bool CSdlPointObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSdlPointObject::fromObject(const QJsonObject& jsonObject)
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


QString CSdlPointObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSdlPointObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSdlPointObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "x")){
		return "x";
	}
	if (propertyName == (QStringLiteral("m_") + "y")){
		return "y";
	}

	return propertyName;
}





bool CSdlPointObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSdlPointObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSdlPointObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSdlPointObjectList::append(sdl::V1_0::imtbase::CSdlPointObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CSdlPointObjectList* sdl::V1_0::imtbase::CSdlPointObjectList::copyMe()
{
	sdl::V1_0::imtbase::CSdlPointObjectList* retVal = new sdl::V1_0::imtbase::CSdlPointObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CSdlPointObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CSdlPointObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CSdlPointObjectList::addElement(sdl::V1_0::imtbase::CSdlPointObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CSdlPointObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CSdlPointObjectList::isEqualWithModel(sdl::V1_0::imtbase::CSdlPointObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CSdlPointObjectList::insert(int index, sdl::V1_0::imtbase::CSdlPointObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CSdlPointObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CSdlPointObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CSdlPointObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CSdlPointObject*>();
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

	return QVariant();
}
CParamTypeIdsObject::CParamTypeIdsObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CParamTypeIdsObject::urlParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::idParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::textParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::textViewChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::selectionParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::schedulerParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::backupSettingsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::databaseAccessSettingsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::paramsSetChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::fileNameParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::integerParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::doubleParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::passwordParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::enableableParamChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CParamTypeIdsObject::GetUrlParam()
{
	return CParamTypeIds::UrlParam ? CParamTypeIds::UrlParam.value() : QVariant();
}


void CParamTypeIdsObject::SetUrlParam(const QVariant& v)
{
	CParamTypeIds::UrlParam = v.value<QString>().toUtf8();
	urlParamChanged();
}


bool CParamTypeIdsObject::hasUrlParam()
{
	 return CParamTypeIds::UrlParam.HasValue();
}


QVariant CParamTypeIdsObject::GetIdParam()
{
	return CParamTypeIds::IdParam ? CParamTypeIds::IdParam.value() : QVariant();
}


void CParamTypeIdsObject::SetIdParam(const QVariant& v)
{
	CParamTypeIds::IdParam = v.value<QString>().toUtf8();
	idParamChanged();
}


bool CParamTypeIdsObject::hasIdParam()
{
	 return CParamTypeIds::IdParam.HasValue();
}


QVariant CParamTypeIdsObject::GetTextParam()
{
	return CParamTypeIds::TextParam ? CParamTypeIds::TextParam.value() : QVariant();
}


void CParamTypeIdsObject::SetTextParam(const QVariant& v)
{
	CParamTypeIds::TextParam = v.value<QString>().toUtf8();
	textParamChanged();
}


bool CParamTypeIdsObject::hasTextParam()
{
	 return CParamTypeIds::TextParam.HasValue();
}


QVariant CParamTypeIdsObject::GetTextView()
{
	return CParamTypeIds::TextView ? CParamTypeIds::TextView.value() : QVariant();
}


void CParamTypeIdsObject::SetTextView(const QVariant& v)
{
	CParamTypeIds::TextView = v.value<QString>().toUtf8();
	textViewChanged();
}


bool CParamTypeIdsObject::hasTextView()
{
	 return CParamTypeIds::TextView.HasValue();
}


QVariant CParamTypeIdsObject::GetSelectionParam()
{
	return CParamTypeIds::SelectionParam ? CParamTypeIds::SelectionParam.value() : QVariant();
}


void CParamTypeIdsObject::SetSelectionParam(const QVariant& v)
{
	CParamTypeIds::SelectionParam = v.value<QString>().toUtf8();
	selectionParamChanged();
}


bool CParamTypeIdsObject::hasSelectionParam()
{
	 return CParamTypeIds::SelectionParam.HasValue();
}


QVariant CParamTypeIdsObject::GetSchedulerParam()
{
	return CParamTypeIds::SchedulerParam ? CParamTypeIds::SchedulerParam.value() : QVariant();
}


void CParamTypeIdsObject::SetSchedulerParam(const QVariant& v)
{
	CParamTypeIds::SchedulerParam = v.value<QString>().toUtf8();
	schedulerParamChanged();
}


bool CParamTypeIdsObject::hasSchedulerParam()
{
	 return CParamTypeIds::SchedulerParam.HasValue();
}


QVariant CParamTypeIdsObject::GetBackupSettings()
{
	return CParamTypeIds::BackupSettings ? CParamTypeIds::BackupSettings.value() : QVariant();
}


void CParamTypeIdsObject::SetBackupSettings(const QVariant& v)
{
	CParamTypeIds::BackupSettings = v.value<QString>().toUtf8();
	backupSettingsChanged();
}


bool CParamTypeIdsObject::hasBackupSettings()
{
	 return CParamTypeIds::BackupSettings.HasValue();
}


QVariant CParamTypeIdsObject::GetDatabaseAccessSettings()
{
	return CParamTypeIds::DatabaseAccessSettings ? CParamTypeIds::DatabaseAccessSettings.value() : QVariant();
}


void CParamTypeIdsObject::SetDatabaseAccessSettings(const QVariant& v)
{
	CParamTypeIds::DatabaseAccessSettings = v.value<QString>().toUtf8();
	databaseAccessSettingsChanged();
}


bool CParamTypeIdsObject::hasDatabaseAccessSettings()
{
	 return CParamTypeIds::DatabaseAccessSettings.HasValue();
}


QVariant CParamTypeIdsObject::GetParamsSet()
{
	return CParamTypeIds::ParamsSet ? CParamTypeIds::ParamsSet.value() : QVariant();
}


void CParamTypeIdsObject::SetParamsSet(const QVariant& v)
{
	CParamTypeIds::ParamsSet = v.value<QString>().toUtf8();
	paramsSetChanged();
}


bool CParamTypeIdsObject::hasParamsSet()
{
	 return CParamTypeIds::ParamsSet.HasValue();
}


QVariant CParamTypeIdsObject::GetFileNameParam()
{
	return CParamTypeIds::FileNameParam ? CParamTypeIds::FileNameParam.value() : QVariant();
}


void CParamTypeIdsObject::SetFileNameParam(const QVariant& v)
{
	CParamTypeIds::FileNameParam = v.value<QString>().toUtf8();
	fileNameParamChanged();
}


bool CParamTypeIdsObject::hasFileNameParam()
{
	 return CParamTypeIds::FileNameParam.HasValue();
}


QVariant CParamTypeIdsObject::GetIntegerParam()
{
	return CParamTypeIds::IntegerParam ? CParamTypeIds::IntegerParam.value() : QVariant();
}


void CParamTypeIdsObject::SetIntegerParam(const QVariant& v)
{
	CParamTypeIds::IntegerParam = v.value<QString>().toUtf8();
	integerParamChanged();
}


bool CParamTypeIdsObject::hasIntegerParam()
{
	 return CParamTypeIds::IntegerParam.HasValue();
}


QVariant CParamTypeIdsObject::GetDoubleParam()
{
	return CParamTypeIds::DoubleParam ? CParamTypeIds::DoubleParam.value() : QVariant();
}


void CParamTypeIdsObject::SetDoubleParam(const QVariant& v)
{
	CParamTypeIds::DoubleParam = v.value<QString>().toUtf8();
	doubleParamChanged();
}


bool CParamTypeIdsObject::hasDoubleParam()
{
	 return CParamTypeIds::DoubleParam.HasValue();
}


QVariant CParamTypeIdsObject::GetPasswordParam()
{
	return CParamTypeIds::PasswordParam ? CParamTypeIds::PasswordParam.value() : QVariant();
}


void CParamTypeIdsObject::SetPasswordParam(const QVariant& v)
{
	CParamTypeIds::PasswordParam = v.value<QString>().toUtf8();
	passwordParamChanged();
}


bool CParamTypeIdsObject::hasPasswordParam()
{
	 return CParamTypeIds::PasswordParam.HasValue();
}


QVariant CParamTypeIdsObject::GetEnableableParam()
{
	return CParamTypeIds::EnableableParam ? CParamTypeIds::EnableableParam.value() : QVariant();
}


void CParamTypeIdsObject::SetEnableableParam(const QVariant& v)
{
	CParamTypeIds::EnableableParam = v.value<QString>().toUtf8();
	enableableParamChanged();
}


bool CParamTypeIdsObject::hasEnableableParam()
{
	 return CParamTypeIds::EnableableParam.HasValue();
}


QString CParamTypeIdsObject::toJson() const
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


bool CParamTypeIdsObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CParamTypeIdsObject::fromObject(const QJsonObject& jsonObject)
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


QString CParamTypeIdsObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CParamTypeIdsObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CParamTypeIdsObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "urlParam")){
		return "UrlParam";
	}
	if (propertyName == (QStringLiteral("m_") + "idParam")){
		return "IdParam";
	}
	if (propertyName == (QStringLiteral("m_") + "textParam")){
		return "TextParam";
	}
	if (propertyName == (QStringLiteral("m_") + "textView")){
		return "TextView";
	}
	if (propertyName == (QStringLiteral("m_") + "selectionParam")){
		return "SelectionParam";
	}
	if (propertyName == (QStringLiteral("m_") + "schedulerParam")){
		return "SchedulerParam";
	}
	if (propertyName == (QStringLiteral("m_") + "backupSettings")){
		return "BackupSettings";
	}
	if (propertyName == (QStringLiteral("m_") + "databaseAccessSettings")){
		return "DatabaseAccessSettings";
	}
	if (propertyName == (QStringLiteral("m_") + "paramsSet")){
		return "ParamsSet";
	}
	if (propertyName == (QStringLiteral("m_") + "fileNameParam")){
		return "FileNameParam";
	}
	if (propertyName == (QStringLiteral("m_") + "integerParam")){
		return "IntegerParam";
	}
	if (propertyName == (QStringLiteral("m_") + "doubleParam")){
		return "DoubleParam";
	}
	if (propertyName == (QStringLiteral("m_") + "passwordParam")){
		return "PasswordParam";
	}
	if (propertyName == (QStringLiteral("m_") + "enableableParam")){
		return "EnableableParam";
	}

	return propertyName;
}





bool CParamTypeIdsObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CParamTypeIdsObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CParamTypeIdsObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CParamTypeIdsObjectList::append(sdl::V1_0::imtbase::CParamTypeIdsObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CParamTypeIdsObjectList* sdl::V1_0::imtbase::CParamTypeIdsObjectList::copyMe()
{
	sdl::V1_0::imtbase::CParamTypeIdsObjectList* retVal = new sdl::V1_0::imtbase::CParamTypeIdsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CParamTypeIdsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CParamTypeIdsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CParamTypeIdsObjectList::addElement(sdl::V1_0::imtbase::CParamTypeIdsObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CParamTypeIdsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CParamTypeIdsObjectList::isEqualWithModel(sdl::V1_0::imtbase::CParamTypeIdsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CParamTypeIdsObjectList::insert(int index, sdl::V1_0::imtbase::CParamTypeIdsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CParamTypeIdsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CParamTypeIdsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CParamTypeIdsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CParamTypeIdsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_urlParam"){
		return Version_1_0->at(index)->UrlParam ? QVariant::fromValue(Version_1_0->at(index)->UrlParam.value()) : QVariant();
	}
	if (nameId == "m_idParam"){
		return Version_1_0->at(index)->IdParam ? QVariant::fromValue(Version_1_0->at(index)->IdParam.value()) : QVariant();
	}
	if (nameId == "m_textParam"){
		return Version_1_0->at(index)->TextParam ? QVariant::fromValue(Version_1_0->at(index)->TextParam.value()) : QVariant();
	}
	if (nameId == "m_textView"){
		return Version_1_0->at(index)->TextView ? QVariant::fromValue(Version_1_0->at(index)->TextView.value()) : QVariant();
	}
	if (nameId == "m_selectionParam"){
		return Version_1_0->at(index)->SelectionParam ? QVariant::fromValue(Version_1_0->at(index)->SelectionParam.value()) : QVariant();
	}
	if (nameId == "m_schedulerParam"){
		return Version_1_0->at(index)->SchedulerParam ? QVariant::fromValue(Version_1_0->at(index)->SchedulerParam.value()) : QVariant();
	}
	if (nameId == "m_backupSettings"){
		return Version_1_0->at(index)->BackupSettings ? QVariant::fromValue(Version_1_0->at(index)->BackupSettings.value()) : QVariant();
	}
	if (nameId == "m_databaseAccessSettings"){
		return Version_1_0->at(index)->DatabaseAccessSettings ? QVariant::fromValue(Version_1_0->at(index)->DatabaseAccessSettings.value()) : QVariant();
	}
	if (nameId == "m_paramsSet"){
		return Version_1_0->at(index)->ParamsSet ? QVariant::fromValue(Version_1_0->at(index)->ParamsSet.value()) : QVariant();
	}
	if (nameId == "m_fileNameParam"){
		return Version_1_0->at(index)->FileNameParam ? QVariant::fromValue(Version_1_0->at(index)->FileNameParam.value()) : QVariant();
	}
	if (nameId == "m_integerParam"){
		return Version_1_0->at(index)->IntegerParam ? QVariant::fromValue(Version_1_0->at(index)->IntegerParam.value()) : QVariant();
	}
	if (nameId == "m_doubleParam"){
		return Version_1_0->at(index)->DoubleParam ? QVariant::fromValue(Version_1_0->at(index)->DoubleParam.value()) : QVariant();
	}
	if (nameId == "m_passwordParam"){
		return Version_1_0->at(index)->PasswordParam ? QVariant::fromValue(Version_1_0->at(index)->PasswordParam.value()) : QVariant();
	}
	if (nameId == "m_enableableParam"){
		return Version_1_0->at(index)->EnableableParam ? QVariant::fromValue(Version_1_0->at(index)->EnableableParam.value()) : QVariant();
	}

	return QVariant();
}
CUrlParamObject::CUrlParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CUrlParamObject::schemeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUrlParamObject::hostChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUrlParamObject::portChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUrlParamObject::pathChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CUrlParamObject::GetScheme()
{
	return CUrlParam::scheme ? CUrlParam::scheme.value() : QVariant();
}


void CUrlParamObject::SetScheme(const QVariant& v)
{
	CUrlParam::scheme = v.value<QString>();
	schemeChanged();
}


bool CUrlParamObject::hasScheme()
{
	 return CUrlParam::scheme.HasValue();
}


QVariant CUrlParamObject::GetHost()
{
	return CUrlParam::host ? CUrlParam::host.value() : QVariant();
}


void CUrlParamObject::SetHost(const QVariant& v)
{
	CUrlParam::host = v.value<QString>();
	hostChanged();
}


bool CUrlParamObject::hasHost()
{
	 return CUrlParam::host.HasValue();
}


QVariant CUrlParamObject::GetPort()
{
	return CUrlParam::port ? CUrlParam::port.value() : QVariant();
}


void CUrlParamObject::SetPort(const QVariant& v)
{
	CUrlParam::port = v.value<int>();
	portChanged();
}


bool CUrlParamObject::hasPort()
{
	 return CUrlParam::port.HasValue();
}


QVariant CUrlParamObject::GetPath()
{
	return CUrlParam::path ? CUrlParam::path.value() : QVariant();
}


void CUrlParamObject::SetPath(const QVariant& v)
{
	CUrlParam::path = v.value<QString>();
	pathChanged();
}


bool CUrlParamObject::hasPath()
{
	 return CUrlParam::path.HasValue();
}


QString CUrlParamObject::toJson() const
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


bool CUrlParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CUrlParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CUrlParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CUrlParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CUrlParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "scheme")){
		return "scheme";
	}
	if (propertyName == (QStringLiteral("m_") + "host")){
		return "host";
	}
	if (propertyName == (QStringLiteral("m_") + "port")){
		return "port";
	}
	if (propertyName == (QStringLiteral("m_") + "path")){
		return "path";
	}

	return propertyName;
}





bool CUrlParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CUrlParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CUrlParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CUrlParamObjectList::append(sdl::V1_0::imtbase::CUrlParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CUrlParamObjectList* sdl::V1_0::imtbase::CUrlParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CUrlParamObjectList* retVal = new sdl::V1_0::imtbase::CUrlParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CUrlParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CUrlParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CUrlParamObjectList::addElement(sdl::V1_0::imtbase::CUrlParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CUrlParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CUrlParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CUrlParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CUrlParamObjectList::insert(int index, sdl::V1_0::imtbase::CUrlParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CUrlParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CUrlParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CUrlParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CUrlParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_scheme"){
		return Version_1_0->at(index)->scheme ? QVariant::fromValue(Version_1_0->at(index)->scheme.value()) : QVariant();
	}
	if (nameId == "m_host"){
		return Version_1_0->at(index)->host ? QVariant::fromValue(Version_1_0->at(index)->host.value()) : QVariant();
	}
	if (nameId == "m_port"){
		return Version_1_0->at(index)->port ? QVariant::fromValue(Version_1_0->at(index)->port.value()) : QVariant();
	}
	if (nameId == "m_path"){
		return Version_1_0->at(index)->path ? QVariant::fromValue(Version_1_0->at(index)->path.value()) : QVariant();
	}

	return QVariant();
}
CIdParamObject::CIdParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CIdParamObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CIdParamObject::GetId()
{
	return CIdParam::id ? CIdParam::id.value() : QVariant();
}


void CIdParamObject::SetId(const QVariant& v)
{
	CIdParam::id = v.value<QString>().toUtf8();
	idChanged();
}


bool CIdParamObject::hasId()
{
	 return CIdParam::id.HasValue();
}


QString CIdParamObject::toJson() const
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


bool CIdParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CIdParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CIdParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CIdParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CIdParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "id")){
		return "id";
	}

	return propertyName;
}





bool CIdParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CIdParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CIdParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CIdParamObjectList::append(sdl::V1_0::imtbase::CIdParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CIdParamObjectList* sdl::V1_0::imtbase::CIdParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CIdParamObjectList* retVal = new sdl::V1_0::imtbase::CIdParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CIdParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CIdParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CIdParamObjectList::addElement(sdl::V1_0::imtbase::CIdParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CIdParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CIdParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CIdParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CIdParamObjectList::insert(int index, sdl::V1_0::imtbase::CIdParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CIdParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CIdParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CIdParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CIdParamObject*>();
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
CTextParamObject::CTextParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CTextParamObject::textChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CTextParamObject::GetText()
{
	return CTextParam::text ? CTextParam::text.value() : QVariant();
}


void CTextParamObject::SetText(const QVariant& v)
{
	CTextParam::text = v.value<QString>();
	textChanged();
}


bool CTextParamObject::hasText()
{
	 return CTextParam::text.HasValue();
}


QString CTextParamObject::toJson() const
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


bool CTextParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CTextParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CTextParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CTextParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CTextParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "text")){
		return "text";
	}

	return propertyName;
}





bool CTextParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CTextParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CTextParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CTextParamObjectList::append(sdl::V1_0::imtbase::CTextParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CTextParamObjectList* sdl::V1_0::imtbase::CTextParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CTextParamObjectList* retVal = new sdl::V1_0::imtbase::CTextParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CTextParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CTextParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CTextParamObjectList::addElement(sdl::V1_0::imtbase::CTextParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CTextParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CTextParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CTextParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CTextParamObjectList::insert(int index, sdl::V1_0::imtbase::CTextParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CTextParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CTextParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CTextParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CTextParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_text"){
		return Version_1_0->at(index)->text ? QVariant::fromValue(Version_1_0->at(index)->text.value()) : QVariant();
	}

	return QVariant();
}
CEnableableParamObject::CEnableableParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CEnableableParamObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CEnableableParamObject::GetValue()
{
	return CEnableableParam::value ? CEnableableParam::value.value() : QVariant();
}


void CEnableableParamObject::SetValue(const QVariant& v)
{
	CEnableableParam::value = v.value<bool>();
	valueChanged();
}


bool CEnableableParamObject::hasValue()
{
	 return CEnableableParam::value.HasValue();
}


QString CEnableableParamObject::toJson() const
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


bool CEnableableParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CEnableableParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CEnableableParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CEnableableParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CEnableableParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "value")){
		return "value";
	}

	return propertyName;
}





bool CEnableableParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CEnableableParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CEnableableParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CEnableableParamObjectList::append(sdl::V1_0::imtbase::CEnableableParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CEnableableParamObjectList* sdl::V1_0::imtbase::CEnableableParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CEnableableParamObjectList* retVal = new sdl::V1_0::imtbase::CEnableableParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CEnableableParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CEnableableParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CEnableableParamObjectList::addElement(sdl::V1_0::imtbase::CEnableableParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CEnableableParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CEnableableParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CEnableableParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CEnableableParamObjectList::insert(int index, sdl::V1_0::imtbase::CEnableableParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CEnableableParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CEnableableParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CEnableableParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CEnableableParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_value"){
		return Version_1_0->at(index)->value ? QVariant::fromValue(Version_1_0->at(index)->value.value()) : QVariant();
	}

	return QVariant();
}
CIntegerParamObject::CIntegerParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CIntegerParamObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CIntegerParamObject::GetValue()
{
	return CIntegerParam::value ? CIntegerParam::value.value() : QVariant();
}


void CIntegerParamObject::SetValue(const QVariant& v)
{
	CIntegerParam::value = v.value<int>();
	valueChanged();
}


bool CIntegerParamObject::hasValue()
{
	 return CIntegerParam::value.HasValue();
}


QString CIntegerParamObject::toJson() const
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


bool CIntegerParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CIntegerParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CIntegerParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CIntegerParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CIntegerParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "value")){
		return "value";
	}

	return propertyName;
}





bool CIntegerParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CIntegerParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CIntegerParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CIntegerParamObjectList::append(sdl::V1_0::imtbase::CIntegerParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CIntegerParamObjectList* sdl::V1_0::imtbase::CIntegerParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CIntegerParamObjectList* retVal = new sdl::V1_0::imtbase::CIntegerParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CIntegerParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CIntegerParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CIntegerParamObjectList::addElement(sdl::V1_0::imtbase::CIntegerParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CIntegerParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CIntegerParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CIntegerParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CIntegerParamObjectList::insert(int index, sdl::V1_0::imtbase::CIntegerParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CIntegerParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CIntegerParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CIntegerParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CIntegerParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_value"){
		return Version_1_0->at(index)->value ? QVariant::fromValue(Version_1_0->at(index)->value.value()) : QVariant();
	}

	return QVariant();
}
CDoubleParamObject::CDoubleParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CDoubleParamObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CDoubleParamObject::GetValue()
{
	return CDoubleParam::value ? CDoubleParam::value.value() : QVariant();
}


void CDoubleParamObject::SetValue(const QVariant& v)
{
	CDoubleParam::value = v.value<double>();
	valueChanged();
}


bool CDoubleParamObject::hasValue()
{
	 return CDoubleParam::value.HasValue();
}


QString CDoubleParamObject::toJson() const
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


bool CDoubleParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CDoubleParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CDoubleParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CDoubleParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CDoubleParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "value")){
		return "value";
	}

	return propertyName;
}





bool CDoubleParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CDoubleParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CDoubleParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CDoubleParamObjectList::append(sdl::V1_0::imtbase::CDoubleParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CDoubleParamObjectList* sdl::V1_0::imtbase::CDoubleParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CDoubleParamObjectList* retVal = new sdl::V1_0::imtbase::CDoubleParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CDoubleParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CDoubleParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CDoubleParamObjectList::addElement(sdl::V1_0::imtbase::CDoubleParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CDoubleParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CDoubleParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CDoubleParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CDoubleParamObjectList::insert(int index, sdl::V1_0::imtbase::CDoubleParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CDoubleParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CDoubleParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CDoubleParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CDoubleParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_value"){
		return Version_1_0->at(index)->value ? QVariant::fromValue(Version_1_0->at(index)->value.value()) : QVariant();
	}

	return QVariant();
}
COptionObject::COptionObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &COptionObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionObject::descriptionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionObject::enabledChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant COptionObject::GetId()
{
	return COption::id ? COption::id.value() : QVariant();
}


void COptionObject::SetId(const QVariant& v)
{
	COption::id = v.value<QString>().toUtf8();
	idChanged();
}


bool COptionObject::hasId()
{
	 return COption::id.HasValue();
}


QVariant COptionObject::GetName()
{
	return COption::name ? COption::name.value() : QVariant();
}


void COptionObject::SetName(const QVariant& v)
{
	COption::name = v.value<QString>();
	nameChanged();
}


bool COptionObject::hasName()
{
	 return COption::name.HasValue();
}


QVariant COptionObject::GetDescription()
{
	return COption::description ? COption::description.value() : QVariant();
}


void COptionObject::SetDescription(const QVariant& v)
{
	COption::description = v.value<QString>();
	descriptionChanged();
}


bool COptionObject::hasDescription()
{
	 return COption::description.HasValue();
}


QVariant COptionObject::GetEnabled()
{
	return COption::enabled ? COption::enabled.value() : QVariant();
}


void COptionObject::SetEnabled(const QVariant& v)
{
	COption::enabled = v.value<bool>();
	enabledChanged();
}


bool COptionObject::hasEnabled()
{
	 return COption::enabled.HasValue();
}


QString COptionObject::toJson() const
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


bool COptionObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool COptionObject::fromObject(const QJsonObject& jsonObject)
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


QString COptionObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant COptionObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString COptionObject::getJSONKeyForProperty(const QString& propertyName) const
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
	if (propertyName == (QStringLiteral("m_") + "enabled")){
		return "enabled";
	}

	return propertyName;
}





bool COptionObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int COptionObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap COptionObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void COptionObjectList::append(sdl::V1_0::imtbase::COptionObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::COptionObjectList* sdl::V1_0::imtbase::COptionObjectList::copyMe()
{
	sdl::V1_0::imtbase::COptionObjectList* retVal = new sdl::V1_0::imtbase::COptionObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::COptionObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::COptionObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::COptionObjectList::addElement(sdl::V1_0::imtbase::COptionObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::COptionObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::COptionObjectList::isEqualWithModel(sdl::V1_0::imtbase::COptionObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::COptionObjectList::insert(int index, sdl::V1_0::imtbase::COptionObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::COptionObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::COptionObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::COptionObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::COptionObject*>();
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
	if (nameId == "m_enabled"){
		return Version_1_0->at(index)->enabled ? QVariant::fromValue(Version_1_0->at(index)->enabled.value()) : QVariant();
	}

	return QVariant();
}
COptionsListObject::COptionsListObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &COptionsListObject::totalCountChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionsListObject::offsetChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionsListObject::optionsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant COptionsListObject::GetTotalCount()
{
	return COptionsList::totalCount ? COptionsList::totalCount.value() : QVariant();
}


void COptionsListObject::SetTotalCount(const QVariant& v)
{
	COptionsList::totalCount = v.value<int>();
	totalCountChanged();
}


bool COptionsListObject::hasTotalCount()
{
	 return COptionsList::totalCount.HasValue();
}


QVariant COptionsListObject::GetOffset()
{
	return COptionsList::offset ? COptionsList::offset.value() : QVariant();
}


void COptionsListObject::SetOffset(const QVariant& v)
{
	COptionsList::offset = v.value<int>();
	offsetChanged();
}


bool COptionsListObject::hasOffset()
{
	 return COptionsList::offset.HasValue();
}


QVariant COptionsListObject::GetOptions()
{
	if (!options) {return {};}
	if (!m_optionsQObjectPtr.isValid()){
		m_optionsQObjectPtr = CreateObject("options");
		auto itemPtr = m_optionsQObjectPtr.value<sdl::V1_0::imtbase::COptionObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = COptionsList::options;
	}
	return m_optionsQObjectPtr;
}


void COptionsListObject::SetOptions(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::imtbase::COptionObjectList* itemPtr = v.value<sdl::V1_0::imtbase::COptionObjectList*>();
		if (itemPtr != nullptr){
			COptionsList::options = itemPtr->Version_1_0;
		}
	}
	else {
		COptionsList::options.SetNull();
	}
	m_optionsQObjectPtr = v;

	optionsChanged();
}


bool COptionsListObject::hasOptions()
{
	 return COptionsList::options.HasValue();
}


void COptionsListObject::emplaceOptions()
{
	ResetOptions();
	COptionsList::options.emplace();
}


void COptionsListObject::ResetOptions()
{
	auto* itemPtr = m_optionsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_optionsQObjectPtr = QVariant();
}


QVariant COptionsListObject::createOptionsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::imtbase::COptionObject());
}


QString COptionsListObject::toJson() const
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


bool COptionsListObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool COptionsListObject::fromObject(const QJsonObject& jsonObject)
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


QString COptionsListObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant COptionsListObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "options"){
		return QVariant::fromValue(new sdl::V1_0::imtbase::COptionObjectList(this));
	}
	return QVariant();
}


QString COptionsListObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "totalCount")){
		return "totalCount";
	}
	if (propertyName == (QStringLiteral("m_") + "offset")){
		return "offset";
	}
	if (propertyName == (QStringLiteral("m_") + "options")){
		return "options";
	}

	return propertyName;
}





bool COptionsListObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int COptionsListObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap COptionsListObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void COptionsListObjectList::append(sdl::V1_0::imtbase::COptionsListObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::COptionsListObjectList* sdl::V1_0::imtbase::COptionsListObjectList::copyMe()
{
	sdl::V1_0::imtbase::COptionsListObjectList* retVal = new sdl::V1_0::imtbase::COptionsListObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::COptionsListObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::COptionsListObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::COptionsListObjectList::addElement(sdl::V1_0::imtbase::COptionsListObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::COptionsListObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::COptionsListObjectList::isEqualWithModel(sdl::V1_0::imtbase::COptionsListObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::COptionsListObjectList::insert(int index, sdl::V1_0::imtbase::COptionsListObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::COptionsListObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::COptionsListObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::COptionsListObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::COptionsListObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_totalCount"){
		return Version_1_0->at(index)->totalCount ? QVariant::fromValue(Version_1_0->at(index)->totalCount.value()) : QVariant();
	}
	if (nameId == "m_offset"){
		return Version_1_0->at(index)->offset ? QVariant::fromValue(Version_1_0->at(index)->offset.value()) : QVariant();
	}
	if (nameId == "m_options"){
		return itemPtr->GetOptions();
	}

	return QVariant();
}
CSelectionParamObject::CSelectionParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSelectionParamObject::selectedIndexChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSelectionParamObject::constraintsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSelectionParamObject::GetSelectedIndex()
{
	return CSelectionParam::selectedIndex ? CSelectionParam::selectedIndex.value() : QVariant();
}


void CSelectionParamObject::SetSelectedIndex(const QVariant& v)
{
	CSelectionParam::selectedIndex = v.value<int>();
	selectedIndexChanged();
}


bool CSelectionParamObject::hasSelectedIndex()
{
	 return CSelectionParam::selectedIndex.HasValue();
}


QVariant CSelectionParamObject::GetConstraints()
{
	if (!constraints) {return {};}
	
	if (!m_constraintsQObjectPtr.isValid()){
		m_constraintsQObjectPtr = CreateObject("constraints");
		auto itemPtr = m_constraintsQObjectPtr.value<sdl::V1_0::imtbase::COptionsListObject*>();
		if (itemPtr != nullptr) {
			itemPtr->totalCount = constraints->totalCount;
			itemPtr->offset = constraints->offset;
			itemPtr->options = constraints->options;
		}
	}
	return m_constraintsQObjectPtr;
}


void CSelectionParamObject::SetConstraints(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::imtbase::COptionsListObject* itemPtr = v.value<sdl::V1_0::imtbase::COptionsListObject*>();
		if (itemPtr != nullptr) CSelectionParam::constraints = (*itemPtr);
	}
	else {
		CSelectionParam::constraints.SetNull();
	}
	m_constraintsQObjectPtr = v;

	constraintsChanged();
}


bool CSelectionParamObject::hasConstraints()
{
	 return CSelectionParam::constraints.HasValue();
}


void CSelectionParamObject::emplaceConstraints()
{
	ResetConstraints();
	CSelectionParam::constraints.emplace();
}


void CSelectionParamObject::ResetConstraints()
{
	auto* itemPtr = m_constraintsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_constraintsQObjectPtr = QVariant();
}


QString CSelectionParamObject::toJson() const
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


bool CSelectionParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSelectionParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CSelectionParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSelectionParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "constraints"){
		return QVariant::fromValue(new sdl::V1_0::imtbase::COptionsListObject(this));
	}
	return QVariant();
}


QString CSelectionParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "selectedIndex")){
		return "selectedIndex";
	}
	if (propertyName == (QStringLiteral("m_") + "constraints")){
		return "constraints";
	}

	return propertyName;
}





bool CSelectionParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSelectionParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSelectionParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSelectionParamObjectList::append(sdl::V1_0::imtbase::CSelectionParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CSelectionParamObjectList* sdl::V1_0::imtbase::CSelectionParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CSelectionParamObjectList* retVal = new sdl::V1_0::imtbase::CSelectionParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CSelectionParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CSelectionParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CSelectionParamObjectList::addElement(sdl::V1_0::imtbase::CSelectionParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CSelectionParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CSelectionParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CSelectionParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CSelectionParamObjectList::insert(int index, sdl::V1_0::imtbase::CSelectionParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CSelectionParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CSelectionParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CSelectionParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CSelectionParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_selectedIndex"){
		return Version_1_0->at(index)->selectedIndex ? QVariant::fromValue(Version_1_0->at(index)->selectedIndex.value()) : QVariant();
	}
	if (nameId == "m_constraints"){
		return itemPtr->GetConstraints();
	}

	return QVariant();
}
CSchedulerParamObject::CSchedulerParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSchedulerParamObject::startTimeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSchedulerParamObject::intervalChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSchedulerParamObject::GetStartTime()
{
	return CSchedulerParam::startTime ? CSchedulerParam::startTime.value() : QVariant();
}


void CSchedulerParamObject::SetStartTime(const QVariant& v)
{
	CSchedulerParam::startTime = v.value<QString>();
	startTimeChanged();
}


bool CSchedulerParamObject::hasStartTime()
{
	 return CSchedulerParam::startTime.HasValue();
}


QVariant CSchedulerParamObject::GetInterval()
{
	return CSchedulerParam::interval ? CSchedulerParam::interval.value() : QVariant();
}


void CSchedulerParamObject::SetInterval(const QVariant& v)
{
	CSchedulerParam::interval = v.value<int>();
	intervalChanged();
}


bool CSchedulerParamObject::hasInterval()
{
	 return CSchedulerParam::interval.HasValue();
}


QString CSchedulerParamObject::toJson() const
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


bool CSchedulerParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSchedulerParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CSchedulerParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSchedulerParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSchedulerParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "startTime")){
		return "startTime";
	}
	if (propertyName == (QStringLiteral("m_") + "interval")){
		return "interval";
	}

	return propertyName;
}





bool CSchedulerParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSchedulerParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSchedulerParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSchedulerParamObjectList::append(sdl::V1_0::imtbase::CSchedulerParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CSchedulerParamObjectList* sdl::V1_0::imtbase::CSchedulerParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CSchedulerParamObjectList* retVal = new sdl::V1_0::imtbase::CSchedulerParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CSchedulerParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CSchedulerParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CSchedulerParamObjectList::addElement(sdl::V1_0::imtbase::CSchedulerParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CSchedulerParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CSchedulerParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CSchedulerParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CSchedulerParamObjectList::insert(int index, sdl::V1_0::imtbase::CSchedulerParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CSchedulerParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CSchedulerParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CSchedulerParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CSchedulerParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_startTime"){
		return Version_1_0->at(index)->startTime ? QVariant::fromValue(Version_1_0->at(index)->startTime.value()) : QVariant();
	}
	if (nameId == "m_interval"){
		return Version_1_0->at(index)->interval ? QVariant::fromValue(Version_1_0->at(index)->interval.value()) : QVariant();
	}

	return QVariant();
}
CBackupSettingsObject::CBackupSettingsObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CBackupSettingsObject::schedulerParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CBackupSettingsObject::folderPathChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CBackupSettingsObject::GetSchedulerParam()
{
	if (!schedulerParam) {return {};}
	
	if (!m_schedulerParamQObjectPtr.isValid()){
		m_schedulerParamQObjectPtr = CreateObject("schedulerParam");
		auto itemPtr = m_schedulerParamQObjectPtr.value<sdl::V1_0::imtbase::CSchedulerParamObject*>();
		if (itemPtr != nullptr) {
			itemPtr->startTime = schedulerParam->startTime;
			itemPtr->interval = schedulerParam->interval;
		}
	}
	return m_schedulerParamQObjectPtr;
}


void CBackupSettingsObject::SetSchedulerParam(const QVariant& v)
{
	if (v.isValid() && !v.isNull()){
		sdl::V1_0::imtbase::CSchedulerParamObject* itemPtr = v.value<sdl::V1_0::imtbase::CSchedulerParamObject*>();
		if (itemPtr != nullptr) CBackupSettings::schedulerParam = (*itemPtr);
	}
	else {
		CBackupSettings::schedulerParam.SetNull();
	}
	m_schedulerParamQObjectPtr = v;

	schedulerParamChanged();
}


bool CBackupSettingsObject::hasSchedulerParam()
{
	 return CBackupSettings::schedulerParam.HasValue();
}


void CBackupSettingsObject::emplaceSchedulerParam()
{
	ResetSchedulerParam();
	CBackupSettings::schedulerParam.emplace();
}


void CBackupSettingsObject::ResetSchedulerParam()
{
	auto* itemPtr = m_schedulerParamQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_schedulerParamQObjectPtr = QVariant();
}


QVariant CBackupSettingsObject::GetFolderPath()
{
	return CBackupSettings::folderPath ? CBackupSettings::folderPath.value() : QVariant();
}


void CBackupSettingsObject::SetFolderPath(const QVariant& v)
{
	CBackupSettings::folderPath = v.value<QString>();
	folderPathChanged();
}


bool CBackupSettingsObject::hasFolderPath()
{
	 return CBackupSettings::folderPath.HasValue();
}


QString CBackupSettingsObject::toJson() const
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


bool CBackupSettingsObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CBackupSettingsObject::fromObject(const QJsonObject& jsonObject)
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


QString CBackupSettingsObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CBackupSettingsObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "schedulerParam"){
		return QVariant::fromValue(new sdl::V1_0::imtbase::CSchedulerParamObject(this));
	}
	return QVariant();
}


QString CBackupSettingsObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "schedulerParam")){
		return "schedulerParam";
	}
	if (propertyName == (QStringLiteral("m_") + "folderPath")){
		return "folderPath";
	}

	return propertyName;
}





bool CBackupSettingsObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CBackupSettingsObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CBackupSettingsObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CBackupSettingsObjectList::append(sdl::V1_0::imtbase::CBackupSettingsObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CBackupSettingsObjectList* sdl::V1_0::imtbase::CBackupSettingsObjectList::copyMe()
{
	sdl::V1_0::imtbase::CBackupSettingsObjectList* retVal = new sdl::V1_0::imtbase::CBackupSettingsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CBackupSettingsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CBackupSettingsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CBackupSettingsObjectList::addElement(sdl::V1_0::imtbase::CBackupSettingsObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CBackupSettingsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CBackupSettingsObjectList::isEqualWithModel(sdl::V1_0::imtbase::CBackupSettingsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CBackupSettingsObjectList::insert(int index, sdl::V1_0::imtbase::CBackupSettingsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CBackupSettingsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CBackupSettingsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CBackupSettingsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CBackupSettingsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_schedulerParam"){
		return itemPtr->GetSchedulerParam();
	}
	if (nameId == "m_folderPath"){
		return Version_1_0->at(index)->folderPath ? QVariant::fromValue(Version_1_0->at(index)->folderPath.value()) : QVariant();
	}

	return QVariant();
}
CDatabaseAccessSettingsObject::CDatabaseAccessSettingsObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CDatabaseAccessSettingsObject::dbNameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::hostChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::portChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::dbPathChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::usernameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::passwordChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CDatabaseAccessSettingsObject::GetDbName()
{
	return CDatabaseAccessSettings::dbName ? CDatabaseAccessSettings::dbName.value() : QVariant();
}


void CDatabaseAccessSettingsObject::SetDbName(const QVariant& v)
{
	CDatabaseAccessSettings::dbName = v.value<QString>();
	dbNameChanged();
}


bool CDatabaseAccessSettingsObject::hasDbName()
{
	 return CDatabaseAccessSettings::dbName.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetHost()
{
	return CDatabaseAccessSettings::host ? CDatabaseAccessSettings::host.value() : QVariant();
}


void CDatabaseAccessSettingsObject::SetHost(const QVariant& v)
{
	CDatabaseAccessSettings::host = v.value<QString>();
	hostChanged();
}


bool CDatabaseAccessSettingsObject::hasHost()
{
	 return CDatabaseAccessSettings::host.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetPort()
{
	return CDatabaseAccessSettings::port ? CDatabaseAccessSettings::port.value() : QVariant();
}


void CDatabaseAccessSettingsObject::SetPort(const QVariant& v)
{
	CDatabaseAccessSettings::port = v.value<int>();
	portChanged();
}


bool CDatabaseAccessSettingsObject::hasPort()
{
	 return CDatabaseAccessSettings::port.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetDbPath()
{
	return CDatabaseAccessSettings::dbPath ? CDatabaseAccessSettings::dbPath.value() : QVariant();
}


void CDatabaseAccessSettingsObject::SetDbPath(const QVariant& v)
{
	CDatabaseAccessSettings::dbPath = v.value<QString>();
	dbPathChanged();
}


bool CDatabaseAccessSettingsObject::hasDbPath()
{
	 return CDatabaseAccessSettings::dbPath.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetUsername()
{
	return CDatabaseAccessSettings::username ? CDatabaseAccessSettings::username.value() : QVariant();
}


void CDatabaseAccessSettingsObject::SetUsername(const QVariant& v)
{
	CDatabaseAccessSettings::username = v.value<QString>();
	usernameChanged();
}


bool CDatabaseAccessSettingsObject::hasUsername()
{
	 return CDatabaseAccessSettings::username.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetPassword()
{
	return CDatabaseAccessSettings::password ? CDatabaseAccessSettings::password.value() : QVariant();
}


void CDatabaseAccessSettingsObject::SetPassword(const QVariant& v)
{
	CDatabaseAccessSettings::password = v.value<QString>();
	passwordChanged();
}


bool CDatabaseAccessSettingsObject::hasPassword()
{
	 return CDatabaseAccessSettings::password.HasValue();
}


QString CDatabaseAccessSettingsObject::toJson() const
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


bool CDatabaseAccessSettingsObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CDatabaseAccessSettingsObject::fromObject(const QJsonObject& jsonObject)
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


QString CDatabaseAccessSettingsObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CDatabaseAccessSettingsObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CDatabaseAccessSettingsObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "dbName")){
		return "dbName";
	}
	if (propertyName == (QStringLiteral("m_") + "host")){
		return "host";
	}
	if (propertyName == (QStringLiteral("m_") + "port")){
		return "port";
	}
	if (propertyName == (QStringLiteral("m_") + "dbPath")){
		return "dbPath";
	}
	if (propertyName == (QStringLiteral("m_") + "username")){
		return "username";
	}
	if (propertyName == (QStringLiteral("m_") + "password")){
		return "password";
	}

	return propertyName;
}





bool CDatabaseAccessSettingsObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CDatabaseAccessSettingsObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CDatabaseAccessSettingsObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CDatabaseAccessSettingsObjectList::append(sdl::V1_0::imtbase::CDatabaseAccessSettingsObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList* sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::copyMe()
{
	sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList* retVal = new sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::addElement(sdl::V1_0::imtbase::CDatabaseAccessSettingsObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::isEqualWithModel(sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::insert(int index, sdl::V1_0::imtbase::CDatabaseAccessSettingsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CDatabaseAccessSettingsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_dbName"){
		return Version_1_0->at(index)->dbName ? QVariant::fromValue(Version_1_0->at(index)->dbName.value()) : QVariant();
	}
	if (nameId == "m_host"){
		return Version_1_0->at(index)->host ? QVariant::fromValue(Version_1_0->at(index)->host.value()) : QVariant();
	}
	if (nameId == "m_port"){
		return Version_1_0->at(index)->port ? QVariant::fromValue(Version_1_0->at(index)->port.value()) : QVariant();
	}
	if (nameId == "m_dbPath"){
		return Version_1_0->at(index)->dbPath ? QVariant::fromValue(Version_1_0->at(index)->dbPath.value()) : QVariant();
	}
	if (nameId == "m_username"){
		return Version_1_0->at(index)->username ? QVariant::fromValue(Version_1_0->at(index)->username.value()) : QVariant();
	}
	if (nameId == "m_password"){
		return Version_1_0->at(index)->password ? QVariant::fromValue(Version_1_0->at(index)->password.value()) : QVariant();
	}

	return QVariant();
}
CFileNameParamObject::CFileNameParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CFileNameParamObject::pathTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFileNameParamObject::pathChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CFileNameParamObject::GetPathType()
{
	return CFileNameParam::pathType ? CFileNameParam::pathType.value() : QVariant();
}


void CFileNameParamObject::SetPathType(const QVariant& v)
{
	CFileNameParam::pathType = v.value<int>();
	pathTypeChanged();
}


bool CFileNameParamObject::hasPathType()
{
	 return CFileNameParam::pathType.HasValue();
}


QVariant CFileNameParamObject::GetPath()
{
	return CFileNameParam::path ? CFileNameParam::path.value() : QVariant();
}


void CFileNameParamObject::SetPath(const QVariant& v)
{
	CFileNameParam::path = v.value<QString>();
	pathChanged();
}


bool CFileNameParamObject::hasPath()
{
	 return CFileNameParam::path.HasValue();
}


QString CFileNameParamObject::toJson() const
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


bool CFileNameParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CFileNameParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CFileNameParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CFileNameParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CFileNameParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "pathType")){
		return "pathType";
	}
	if (propertyName == (QStringLiteral("m_") + "path")){
		return "path";
	}

	return propertyName;
}





bool CFileNameParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CFileNameParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CFileNameParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CFileNameParamObjectList::append(sdl::V1_0::imtbase::CFileNameParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CFileNameParamObjectList* sdl::V1_0::imtbase::CFileNameParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CFileNameParamObjectList* retVal = new sdl::V1_0::imtbase::CFileNameParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CFileNameParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CFileNameParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CFileNameParamObjectList::addElement(sdl::V1_0::imtbase::CFileNameParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CFileNameParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CFileNameParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CFileNameParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CFileNameParamObjectList::insert(int index, sdl::V1_0::imtbase::CFileNameParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CFileNameParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CFileNameParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CFileNameParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CFileNameParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_pathType"){
		return Version_1_0->at(index)->pathType ? QVariant::fromValue(Version_1_0->at(index)->pathType.value()) : QVariant();
	}
	if (nameId == "m_path"){
		return Version_1_0->at(index)->path ? QVariant::fromValue(Version_1_0->at(index)->path.value()) : QVariant();
	}

	return QVariant();
}
CParamsSetObject::CParamsSetObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CParamsSetObject::paramIdsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::paramTypeIdsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::paramNamesChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::paramDescriptionsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::parametersChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CParamsSetObject::GetParamIds()
{
	if (!paramIds ){return {};}

	QVariantList tempParamIdsList;
	for (const auto& tempValue: paramIds.value()){
		tempParamIdsList << (tempValue ? QVariant(QString::fromUtf8(*tempValue)) : QVariant());
	}

	return tempParamIdsList;
}


void CParamsSetObject::SetParamIds(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){
		CParamsSet::paramIds.SetNull();
		paramIdsChanged();
		return;
	}
	if (!CParamsSet::paramIds){
		CParamsSet::paramIds.emplace();
	}
	else{
		CParamsSet::paramIds->clear();
	}
	for (const QVariant& tempValue: v.toList()){
		if (!tempValue.isValid() || tempValue.isNull()){
			CParamsSet::paramIds->AppendNull();
			continue;
		}
		istd::TNullableValue<QByteArray> tempItem(tempValue.toString().toUtf8());
		CParamsSet::paramIds->append(tempItem);
	}

	paramIdsChanged();
}


bool CParamsSetObject::hasParamIds()
{
	 return CParamsSet::paramIds.HasValue();
}


QVariant CParamsSetObject::GetParamTypeIds()
{
	if (!paramTypeIds ){return {};}

	QVariantList tempParamTypeIdsList;
	for (const auto& tempValue: paramTypeIds.value()){
		tempParamTypeIdsList << (tempValue ? QVariant(QString::fromUtf8(*tempValue)) : QVariant());
	}

	return tempParamTypeIdsList;
}


void CParamsSetObject::SetParamTypeIds(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){
		CParamsSet::paramTypeIds.SetNull();
		paramTypeIdsChanged();
		return;
	}
	if (!CParamsSet::paramTypeIds){
		CParamsSet::paramTypeIds.emplace();
	}
	else{
		CParamsSet::paramTypeIds->clear();
	}
	for (const QVariant& tempValue: v.toList()){
		if (!tempValue.isValid() || tempValue.isNull()){
			CParamsSet::paramTypeIds->AppendNull();
			continue;
		}
		istd::TNullableValue<QByteArray> tempItem(tempValue.toString().toUtf8());
		CParamsSet::paramTypeIds->append(tempItem);
	}

	paramTypeIdsChanged();
}


bool CParamsSetObject::hasParamTypeIds()
{
	 return CParamsSet::paramTypeIds.HasValue();
}


QVariant CParamsSetObject::GetParamNames()
{
	if (!paramNames ){return {};}

	QVariantList tempParamNamesList;
	for (const auto& tempValue: paramNames.value()){
		tempParamNamesList << (tempValue ? QVariant::fromValue(*tempValue) : QVariant());
	}

	return tempParamNamesList;
}


void CParamsSetObject::SetParamNames(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){
		CParamsSet::paramNames.SetNull();
		paramNamesChanged();
		return;
	}
	if (!CParamsSet::paramNames){
		CParamsSet::paramNames.emplace();
	}
	else{
		CParamsSet::paramNames->clear();
	}
	for (const QVariant& tempValue: v.toList()){
		if (!tempValue.isValid() || tempValue.isNull()){
			CParamsSet::paramNames->AppendNull();
			continue;
		}
		istd::TNullableValue<QString> tempItem(tempValue.value<QString>());
		CParamsSet::paramNames->append(tempItem);
	}

	paramNamesChanged();
}


bool CParamsSetObject::hasParamNames()
{
	 return CParamsSet::paramNames.HasValue();
}


QVariant CParamsSetObject::GetParamDescriptions()
{
	if (!paramDescriptions ){return {};}

	QVariantList tempParamDescriptionsList;
	for (const auto& tempValue: paramDescriptions.value()){
		tempParamDescriptionsList << (tempValue ? QVariant::fromValue(*tempValue) : QVariant());
	}

	return tempParamDescriptionsList;
}


void CParamsSetObject::SetParamDescriptions(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){
		CParamsSet::paramDescriptions.SetNull();
		paramDescriptionsChanged();
		return;
	}
	if (!CParamsSet::paramDescriptions){
		CParamsSet::paramDescriptions.emplace();
	}
	else{
		CParamsSet::paramDescriptions->clear();
	}
	for (const QVariant& tempValue: v.toList()){
		if (!tempValue.isValid() || tempValue.isNull()){
			CParamsSet::paramDescriptions->AppendNull();
			continue;
		}
		istd::TNullableValue<QString> tempItem(tempValue.value<QString>());
		CParamsSet::paramDescriptions->append(tempItem);
	}

	paramDescriptionsChanged();
}


bool CParamsSetObject::hasParamDescriptions()
{
	 return CParamsSet::paramDescriptions.HasValue();
}


QVariant CParamsSetObject::GetParameters()
{
	if (!parameters ){return {};}

	QVariantList tempParametersList;
	for (const auto& tempValue: parameters.value()){
		tempParametersList << (tempValue ? QVariant::fromValue(*tempValue) : QVariant());
	}

	return tempParametersList;
}


void CParamsSetObject::SetParameters(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){
		CParamsSet::parameters.SetNull();
		parametersChanged();
		return;
	}
	if (!CParamsSet::parameters){
		CParamsSet::parameters.emplace();
	}
	else{
		CParamsSet::parameters->clear();
	}
	for (const QVariant& tempValue: v.toList()){
		if (!tempValue.isValid() || tempValue.isNull()){
			CParamsSet::parameters->AppendNull();
			continue;
		}
		istd::TNullableValue<QString> tempItem(tempValue.value<QString>());
		CParamsSet::parameters->append(tempItem);
	}

	parametersChanged();
}


bool CParamsSetObject::hasParameters()
{
	 return CParamsSet::parameters.HasValue();
}


QString CParamsSetObject::toJson() const
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


bool CParamsSetObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CParamsSetObject::fromObject(const QJsonObject& jsonObject)
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


QString CParamsSetObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CParamsSetObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CParamsSetObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "paramIds")){
		return "paramIds";
	}
	if (propertyName == (QStringLiteral("m_") + "paramTypeIds")){
		return "paramTypeIds";
	}
	if (propertyName == (QStringLiteral("m_") + "paramNames")){
		return "paramNames";
	}
	if (propertyName == (QStringLiteral("m_") + "paramDescriptions")){
		return "paramDescriptions";
	}
	if (propertyName == (QStringLiteral("m_") + "parameters")){
		return "parameters";
	}

	return propertyName;
}





bool CParamsSetObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CParamsSetObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CParamsSetObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CParamsSetObjectList::append(sdl::V1_0::imtbase::CParamsSetObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CParamsSetObjectList* sdl::V1_0::imtbase::CParamsSetObjectList::copyMe()
{
	sdl::V1_0::imtbase::CParamsSetObjectList* retVal = new sdl::V1_0::imtbase::CParamsSetObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CParamsSetObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CParamsSetObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CParamsSetObjectList::addElement(sdl::V1_0::imtbase::CParamsSetObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CParamsSetObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CParamsSetObjectList::isEqualWithModel(sdl::V1_0::imtbase::CParamsSetObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CParamsSetObjectList::insert(int index, sdl::V1_0::imtbase::CParamsSetObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CParamsSetObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CParamsSetObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CParamsSetObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CParamsSetObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_paramIds"){
		return Version_1_0->at(index)->paramIds ? QVariant::fromValue(Version_1_0->at(index)->paramIds.value()) : QVariant();
	}
	if (nameId == "m_paramTypeIds"){
		return Version_1_0->at(index)->paramTypeIds ? QVariant::fromValue(Version_1_0->at(index)->paramTypeIds.value()) : QVariant();
	}
	if (nameId == "m_paramNames"){
		return Version_1_0->at(index)->paramNames ? QVariant::fromValue(Version_1_0->at(index)->paramNames.value()) : QVariant();
	}
	if (nameId == "m_paramDescriptions"){
		return Version_1_0->at(index)->paramDescriptions ? QVariant::fromValue(Version_1_0->at(index)->paramDescriptions.value()) : QVariant();
	}
	if (nameId == "m_parameters"){
		return Version_1_0->at(index)->parameters ? QVariant::fromValue(Version_1_0->at(index)->parameters.value()) : QVariant();
	}

	return QVariant();
}
CMimeTypeObject::CMimeTypeObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CMimeTypeObject::typeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::treeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::subTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::suffixChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::parametersChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CMimeTypeObject::GetType()
{
	return CMimeType::type ? CMimeType::type.value() : QVariant();
}


void CMimeTypeObject::SetType(const QVariant& v)
{
	CMimeType::type = v.value<QString>();
	typeChanged();
}


bool CMimeTypeObject::hasType()
{
	 return CMimeType::type.HasValue();
}


QVariant CMimeTypeObject::GetTree()
{
	if (!tree ){return {};}

	QVariantList tempTreeList;
	for (const auto& tempValue: tree.value()){
		tempTreeList << (tempValue ? QVariant::fromValue(*tempValue) : QVariant());
	}

	return tempTreeList;
}


void CMimeTypeObject::SetTree(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){
		CMimeType::tree.SetNull();
		treeChanged();
		return;
	}
	if (!CMimeType::tree){
		CMimeType::tree.emplace();
	}
	else{
		CMimeType::tree->clear();
	}
	for (const QVariant& tempValue: v.toList()){
		if (!tempValue.isValid() || tempValue.isNull()){
			CMimeType::tree->AppendNull();
			continue;
		}
		istd::TNullableValue<QString> tempItem(tempValue.value<QString>());
		CMimeType::tree->append(tempItem);
	}

	treeChanged();
}


bool CMimeTypeObject::hasTree()
{
	 return CMimeType::tree.HasValue();
}


QVariant CMimeTypeObject::GetSubType()
{
	return CMimeType::subType ? CMimeType::subType.value() : QVariant();
}


void CMimeTypeObject::SetSubType(const QVariant& v)
{
	CMimeType::subType = v.value<QString>();
	subTypeChanged();
}


bool CMimeTypeObject::hasSubType()
{
	 return CMimeType::subType.HasValue();
}


QVariant CMimeTypeObject::GetSuffix()
{
	return CMimeType::suffix ? CMimeType::suffix.value() : QVariant();
}


void CMimeTypeObject::SetSuffix(const QVariant& v)
{
	CMimeType::suffix = v.value<QString>();
	suffixChanged();
}


bool CMimeTypeObject::hasSuffix()
{
	 return CMimeType::suffix.HasValue();
}


QVariant CMimeTypeObject::GetParameters()
{
	if (!parameters ){return {};}

	QVariantList tempParametersList;
	for (const auto& tempValue: parameters.value()){
		tempParametersList << (tempValue ? QVariant::fromValue(*tempValue) : QVariant());
	}

	return tempParametersList;
}


void CMimeTypeObject::SetParameters(const QVariant& v)
{
	if (!v.isValid() || v.isNull()){
		CMimeType::parameters.SetNull();
		parametersChanged();
		return;
	}
	if (!CMimeType::parameters){
		CMimeType::parameters.emplace();
	}
	else{
		CMimeType::parameters->clear();
	}
	for (const QVariant& tempValue: v.toList()){
		if (!tempValue.isValid() || tempValue.isNull()){
			CMimeType::parameters->AppendNull();
			continue;
		}
		istd::TNullableValue<QString> tempItem(tempValue.value<QString>());
		CMimeType::parameters->append(tempItem);
	}

	parametersChanged();
}


bool CMimeTypeObject::hasParameters()
{
	 return CMimeType::parameters.HasValue();
}


QString CMimeTypeObject::toJson() const
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


bool CMimeTypeObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CMimeTypeObject::fromObject(const QJsonObject& jsonObject)
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


QString CMimeTypeObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CMimeTypeObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CMimeTypeObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QStringLiteral("m_") + "type")){
		return "type";
	}
	if (propertyName == (QStringLiteral("m_") + "tree")){
		return "tree";
	}
	if (propertyName == (QStringLiteral("m_") + "subType")){
		return "subType";
	}
	if (propertyName == (QStringLiteral("m_") + "suffix")){
		return "suffix";
	}
	if (propertyName == (QStringLiteral("m_") + "parameters")){
		return "parameters";
	}

	return propertyName;
}





bool CMimeTypeObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CMimeTypeObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CMimeTypeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CMimeTypeObjectList::append(sdl::V1_0::imtbase::CMimeTypeObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CMimeTypeObjectList* sdl::V1_0::imtbase::CMimeTypeObjectList::copyMe()
{
	sdl::V1_0::imtbase::CMimeTypeObjectList* retVal = new sdl::V1_0::imtbase::CMimeTypeObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CMimeTypeObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CMimeTypeObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CMimeTypeObjectList::addElement(sdl::V1_0::imtbase::CMimeTypeObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CMimeTypeObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CMimeTypeObjectList::isEqualWithModel(sdl::V1_0::imtbase::CMimeTypeObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CMimeTypeObjectList::insert(int index, sdl::V1_0::imtbase::CMimeTypeObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CMimeTypeObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CMimeTypeObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CMimeTypeObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CMimeTypeObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_type"){
		return Version_1_0->at(index)->type ? QVariant::fromValue(Version_1_0->at(index)->type.value()) : QVariant();
	}
	if (nameId == "m_tree"){
		return Version_1_0->at(index)->tree ? QVariant::fromValue(Version_1_0->at(index)->tree.value()) : QVariant();
	}
	if (nameId == "m_subType"){
		return Version_1_0->at(index)->subType ? QVariant::fromValue(Version_1_0->at(index)->subType.value()) : QVariant();
	}
	if (nameId == "m_suffix"){
		return Version_1_0->at(index)->suffix ? QVariant::fromValue(Version_1_0->at(index)->suffix.value()) : QVariant();
	}
	if (nameId == "m_parameters"){
		return Version_1_0->at(index)->parameters ? QVariant::fromValue(Version_1_0->at(index)->parameters.value()) : QVariant();
	}

	return QVariant();
}
} // namespace sdl::V1_0::imtbase
