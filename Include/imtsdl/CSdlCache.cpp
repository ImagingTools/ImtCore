#include <imtsdl/CSdlCache.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>

/// \file CCacheEntry.cpp

namespace imtsdl
{


// public methods

QString CCacheEntry::GetTypeId() const
{
	return m_typeId;
}


void CCacheEntry::SetTypeId(const QString& typeId)
{
	if (typeId != m_typeId || !_m_settedFields.contains("TypeId")){
		istd::CChangeNotifier notifier(this);

		m_typeId = typeId;
		_m_settedFields << "TypeId";
	}
}


void CCacheEntry::ResetTypeId()
{
	istd::CChangeNotifier notifier(this);

	_m_settedFields.remove("TypeId");
	m_typeId.clear();
}



bool CCacheEntry::HasTypeId() const
{
	return _m_settedFields.contains("TypeId");
}


QString CCacheEntry::GetQmlEntryName() const
{
	return m_qmlEntryName;
}


void CCacheEntry::SetQmlEntryName(const QString& qmlEntryName)
{
	if (qmlEntryName != m_qmlEntryName || !_m_settedFields.contains("QmlEntryName")){
		istd::CChangeNotifier notifier(this);

		m_qmlEntryName = qmlEntryName;
		_m_settedFields << "QmlEntryName";
	}
}


void CCacheEntry::ResetQmlEntryName()
{
	istd::CChangeNotifier notifier(this);

	_m_settedFields.remove("QmlEntryName");
	m_qmlEntryName.clear();
}



bool CCacheEntry::HasQmlEntryName() const
{
	return _m_settedFields.contains("QmlEntryName");
}


QString CCacheEntry::GetTargetHeaderPath() const
{
	return m_targetHeaderPath;
}


void CCacheEntry::SetTargetHeaderPath(const QString& targetHeaderPath)
{
	if (targetHeaderPath != m_targetHeaderPath || !_m_settedFields.contains("TargetHeaderPath")){
		istd::CChangeNotifier notifier(this);

		m_targetHeaderPath = targetHeaderPath;
		_m_settedFields << "TargetHeaderPath";
	}
}


void CCacheEntry::ResetTargetHeaderPath()
{
	istd::CChangeNotifier notifier(this);

	_m_settedFields.remove("TargetHeaderPath");
	m_targetHeaderPath.clear();
}



bool CCacheEntry::HasTargetHeaderPath() const
{
	return _m_settedFields.contains("TargetHeaderPath");
}


QString CCacheEntry::GetNamespace() const
{
	return m_namespace;
}


void CCacheEntry::SetNamespace(const QString& aNamespace)
{
	if (aNamespace != m_namespace || !_m_settedFields.contains("Namespace")){
		istd::CChangeNotifier notifier(this);

		m_namespace = aNamespace;
		_m_settedFields << "Namespace";
	}
}


void CCacheEntry::ResetNamespace()
{
	istd::CChangeNotifier notifier(this);

	_m_settedFields.remove("Namespace");
	m_namespace.clear();
}



bool CCacheEntry::HasNamespace() const
{
	return _m_settedFields.contains("Namespace");
}


QString CCacheEntry::GetTargetSourceRelativePath() const
{
	return m_targetSourceRelativePath;
}


void CCacheEntry::SetTargetSourceRelativePath(const QString& targetSourceRelativePath)
{
	if (targetSourceRelativePath != m_targetSourceRelativePath || !_m_settedFields.contains("TargetSourceRelativePath")){
		istd::CChangeNotifier notifier(this);

		m_targetSourceRelativePath = targetSourceRelativePath;
		_m_settedFields << "TargetSourceRelativePath";
	}
}


void CCacheEntry::ResetTargetSourceRelativePath()
{
	istd::CChangeNotifier notifier(this);

	_m_settedFields.remove("TargetSourceRelativePath");
	m_targetSourceRelativePath.clear();
}



bool CCacheEntry::HasTargetSourceRelativePath() const
{
	return _m_settedFields.contains("TargetSourceRelativePath");
}


QString CCacheEntry::GetVersion() const
{
	return m_version;
}


void CCacheEntry::SetVersion(const QString& version)
{
	if (version != m_version || !_m_settedFields.contains("Version")){
		istd::CChangeNotifier notifier(this);

		m_version = version;
		_m_settedFields << "Version";
	}
}


void CCacheEntry::ResetVersion()
{
	istd::CChangeNotifier notifier(this);

	_m_settedFields.remove("Version");
	m_version.clear();
}



bool CCacheEntry::HasVersion() const
{
	return _m_settedFields.contains("Version");
}


bool CCacheEntry::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!_m_settedFields.contains("TypeId")){
		return false;
	}
	jsonObject["TypeId"] = QJsonValue::fromVariant(m_typeId);

	if (!_m_settedFields.contains("QmlEntryName")){
		return false;
	}
	jsonObject["QmlEntryName"] = QJsonValue::fromVariant(m_qmlEntryName);

	if (!_m_settedFields.contains("TargetHeaderPath")){
		return false;
	}
	jsonObject["TargetHeaderPath"] = QJsonValue::fromVariant(m_targetHeaderPath);

	if (!_m_settedFields.contains("Namespace")){
		return false;
	}
	jsonObject["Namespace"] = QJsonValue::fromVariant(m_namespace);

	if (_m_settedFields.contains("TargetSourceRelativePath")){
		jsonObject["TargetSourceRelativePath"] = QJsonValue::fromVariant(m_targetSourceRelativePath);
	}

	if (_m_settedFields.contains("Version")){
		jsonObject["Version"] = QJsonValue::fromVariant(m_version);
	}

	return true;
}


bool CCacheEntry::ReadFromJsonObject(CCacheEntry& object, const QJsonObject& jsonObject)
{
	istd::CChangeGroup changeGroup(&object);

	QVariant typeIdData = jsonObject.value("TypeId").toVariant();
	if (typeIdData.isNull()){
		return false;
	}
	object.SetTypeId(typeIdData.toString());

	QVariant qmlEntryNameData = jsonObject.value("QmlEntryName").toVariant();
	if (qmlEntryNameData.isNull()){
		return false;
	}
	object.SetQmlEntryName(qmlEntryNameData.toString());

	QVariant targetHeaderPathData = jsonObject.value("TargetHeaderPath").toVariant();
	if (targetHeaderPathData.isNull()){
		return false;
	}
	object.SetTargetHeaderPath(targetHeaderPathData.toString());

	QVariant namespaceData = jsonObject.value("Namespace").toVariant();
	if (namespaceData.isNull()){
		return false;
	}
	object.SetNamespace(namespaceData.toString());

	QVariant targetSourceRelativePathData = jsonObject.value("TargetSourceRelativePath").toVariant();
	if (!targetSourceRelativePathData.isNull()){
		object.SetTargetSourceRelativePath(targetSourceRelativePathData.toString());
	}

	QVariant versionData = jsonObject.value("Version").toVariant();
	if (!versionData.isNull()){
		object.SetVersion(versionData.toString());
	}

	return true;
}


bool CCacheEntry::operator==(const CCacheEntry& other) const
{
	bool retVal = true;
	retVal = retVal && (m_typeId == other.m_typeId);
	retVal = retVal && (m_qmlEntryName == other.m_qmlEntryName);
	retVal = retVal && (m_targetHeaderPath == other.m_targetHeaderPath);
	retVal = retVal && (m_namespace == other.m_namespace);
	retVal = retVal && (m_targetSourceRelativePath == other.m_targetSourceRelativePath);
	retVal = retVal && (m_version == other.m_version);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CCacheEntry::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_RESET;
}


bool CCacheEntry::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	try {
		const CCacheEntry& otherObjectRef = dynamic_cast<const CCacheEntry&>(object);

		istd::CChangeGroup changeGroup(this);
		*this = otherObjectRef;
	}
	catch (const std::bad_cast&) {
		return false;
	}

	I_CRITICAL();
	return false;
}


bool CCacheEntry::IsEqual(const IChangeable& object) const
{
	try {
		const CCacheEntry& otherObjectRef = dynamic_cast<const CCacheEntry&>(object);
		return this->operator==(otherObjectRef);
	}
	catch (const std::bad_cast&) {
		return false;
	}

	I_CRITICAL();
	return false;
}


bool CCacheEntry::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	m_typeId.clear();
	m_qmlEntryName.clear();
	m_targetHeaderPath.clear();
	m_namespace.clear();
	m_targetSourceRelativePath.clear();
	m_version.clear();

	return true;
}


/// \file CCache.cpp


// public methods

CCache::CCache()
	:m_updateBridge(this)
{
}


QList<QString> CCache::GetIncludePaths() const
{
	return m_includePaths;
}


void CCache::SetIncludePaths(const QList<QString>& includePaths)
{
	if (includePaths != m_includePaths || !_m_settedFields.contains("IncludePaths")){
		istd::CChangeNotifier notifier(this);

		m_includePaths = includePaths;
		_m_settedFields << "IncludePaths";
	}
}


void CCache::AddIncludePathsElement(const QString& element)
{
	istd::CChangeNotifier notifier(this);

	m_includePaths << element;
}


void CCache::ResetIncludePaths()
{
	istd::CChangeNotifier notifier(this);

	_m_settedFields.remove("IncludePaths");
	m_includePaths.clear();
}



bool CCache::HasIncludePaths() const
{
	return _m_settedFields.contains("IncludePaths");
}


istd::TPointerVector<CCacheEntry> CCache::GetTypes() const
{
	return m_types;
}


void CCache::AddTypesElement(const CCacheEntry& element)
{
	istd::CChangeNotifier notifier(this);

	imod::TModelWrap<CCacheEntry>* newElementPtr = new imod::TModelWrap<CCacheEntry>;
	bool isCopied = newElementPtr->CopyFrom(element);
	Q_ASSERT_X(isCopied, "Unable to copy cache entry", __func__);

	newElementPtr->AttachObserver(&m_updateBridge);

	m_types.PushBack(newElementPtr);
}


void CCache::ResetTypes()
{
	istd::CChangeNotifier notifier(this);

	_m_settedFields.remove("Types");
	m_types.Reset();
}



bool CCache::HasTypes() const
{
	return _m_settedFields.contains("Types");
}


bool CCache::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!_m_settedFields.contains("IncludePaths") || m_includePaths.isEmpty()){
		return false;
	}
	QJsonArray newIncludePathsJsonArray;
	for (qsizetype includePathsIndex = 0; includePathsIndex < m_includePaths.size(); ++includePathsIndex){
		newIncludePathsJsonArray << QJsonValue::fromVariant(m_includePaths[includePathsIndex]);
	}
	jsonObject["IncludePaths"] = newIncludePathsJsonArray;

	if (!_m_settedFields.contains("Types") || m_types.IsEmpty()){
		return false;
	}
	QJsonArray newTypesJsonArray;
	for (qsizetype typesIndex = 0; typesIndex < m_types.GetCount(); ++typesIndex){
		QJsonObject newTypesJsonObject;
		if (!m_types.GetAt(typesIndex)->WriteToJsonObject(newTypesJsonObject)){
			return false;
		}
		newTypesJsonArray << newTypesJsonObject;
	}
	jsonObject["Types"] = newTypesJsonArray;

	return true;
}


bool CCache::ReadFromJsonObject(CCache& object, const QJsonObject& jsonObject)
{
	istd::CChangeGroup changeGroup(&object);

	if (!jsonObject.contains("IncludePaths")){
		return false;
	}
	const QJsonArray includePathsArray = jsonObject["IncludePaths"].toArray();
	qsizetype includePathsCount = includePathsArray.size();
	if (includePathsCount <= 0){
		return false;
	}
	QList<QString> includePathsList;
	for (qsizetype includePathsIndex = 0; includePathsIndex < includePathsCount; ++includePathsIndex){
		includePathsList << includePathsArray[includePathsIndex].toString();
	}
	object.SetIncludePaths(includePathsList);

	if (!jsonObject.contains("Types")){
		return false;
	}
	const QJsonArray typesArray = jsonObject["Types"].toArray();
	qsizetype typesCount = typesArray.size();
	if (typesCount <= 0){
		return false;
	}
	for (int typesIndex = 0; typesIndex < typesCount; ++typesIndex){
		CCacheEntry typeEntry;
		if (!CCacheEntry::ReadFromJsonObject(typeEntry, typesArray[typesIndex].toObject())){
			return false;
		}
		object.AddTypesElement(typeEntry);
	}

	return true;
}


bool CCache::operator==(const CCache& other) const
{
	bool retVal = true;
	retVal = retVal && (m_includePaths == other.m_includePaths);
	retVal = retVal && (m_types.GetCount() == other.m_types.GetCount());
	if (retVal){
		for (int index = 0; index < m_types.GetCount(); ++index){
			retVal = retVal && m_types.GetAt(index)->IsEqual(*other.m_types.GetAt(index));
		}
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CCache::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_RESET;
}


bool CCache::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	try {
		const CCache& otherObjectRef = dynamic_cast<const CCache&>(object);
		istd::CChangeGroup changeGroup(this);

		m_includePaths = otherObjectRef.m_includePaths;

		for (int index = 0; index < otherObjectRef.m_types.GetCount(); ++index){
			CCacheEntry* clonedPtr = dynamic_cast<CCacheEntry*>(otherObjectRef.m_types.GetAt(index)->CloneMe(mode));
			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(clonedPtr);
			if (clonedPtr == nullptr || modelPtr == nullptr){
				I_CRITICAL();

				return false;
			}
			modelPtr->AttachObserver(&m_updateBridge);
			m_types.PushBack(clonedPtr);
		}
	}
	catch (const std::bad_cast&) {
		return false;
	}

	I_CRITICAL();
	return false;
}


bool CCache::IsEqual(const IChangeable& object) const
{
	try {
		const CCache& otherObjectRef = dynamic_cast<const CCache&>(object);
		return this->operator==(otherObjectRef);
	}
	catch (const std::bad_cast&) {
		return false;
	}

	I_CRITICAL();
	return false;
}


bool CCache::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_includePaths.clear();
	m_types.Reset();

	return true;
}


} // namespace imtsdl




