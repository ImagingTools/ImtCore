#include <imtsdl/CSdlCache.h>


/// \file CCacheEntry.cpp


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>


namespace imtsdl
{


// public methods

QString CCacheEntry::GetTypeId() const
{
	return m_typeId;
}


void CCacheEntry::SetTypeId(const QString& typeId)
{
	if (typeId != m_typeId){
		istd::CChangeNotifier notifier(this);

		m_typeId = typeId;
	}
}


QString CCacheEntry::GetQmlEntryName() const
{
	return m_qmlEntryName;
}


void CCacheEntry::SetQmlEntryName(const QString& qmlEntryName)
{
	if (qmlEntryName != m_qmlEntryName){
		istd::CChangeNotifier notifier(this);

		m_qmlEntryName = qmlEntryName;
	}
}


QString CCacheEntry::GetTargetHeaderPath() const
{
	return m_targetHeaderPath;
}


void CCacheEntry::SetTargetHeaderPath(const QString& targetHeaderPath)
{
	if (targetHeaderPath != m_targetHeaderPath){
		istd::CChangeNotifier notifier(this);

		m_targetHeaderPath = targetHeaderPath;
	}
}


QString CCacheEntry::GetNamespace() const
{
	return m_namespace;
}


void CCacheEntry::SetNamespace(const QString& aNamespace)
{
	if (aNamespace != m_namespace){
		istd::CChangeNotifier notifier(this);

		m_namespace = aNamespace;
	}
}


QString CCacheEntry::GetTargetSourceRelativePath() const
{
	return m_targetSourceRelativePath;
}


void CCacheEntry::SetTargetSourceRelativePath(const QString& targetSourceRelativePath)
{
	if (targetSourceRelativePath != m_targetSourceRelativePath){
		istd::CChangeNotifier notifier(this);

		m_targetSourceRelativePath = targetSourceRelativePath;
	}
}


QString CCacheEntry::GetVersion() const
{
	return m_version;
}


void CCacheEntry::SetVersion(const QString& version)
{
	if (version != m_version){
		istd::CChangeNotifier notifier(this);

		m_version = version;
	}
}


bool CCacheEntry::WriteToJsonObject(QJsonObject& jsonObject) const
{	
	jsonObject["TypeId"] = QJsonValue::fromVariant(m_typeId);
	jsonObject["QmlEntryName"] = QJsonValue::fromVariant(m_qmlEntryName);
	jsonObject["TargetHeaderPath"] = QJsonValue::fromVariant(m_targetHeaderPath);
	jsonObject["Namespace"] = QJsonValue::fromVariant(m_namespace);
	jsonObject["TargetSourceRelativePath"] = QJsonValue::fromVariant(m_targetSourceRelativePath);
	jsonObject["Version"] = QJsonValue::fromVariant(m_version);

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

		istd::CChangeNotifier changeNotifier(this);

		m_typeId = otherObjectRef.m_typeId;
		m_qmlEntryName = otherObjectRef.m_qmlEntryName;
		m_targetHeaderPath = otherObjectRef.m_targetHeaderPath;
		m_namespace = otherObjectRef.m_namespace;
		m_targetSourceRelativePath = otherObjectRef.m_targetSourceRelativePath;
		m_version = otherObjectRef.m_version;

		return true;
	}
	catch (const std::bad_cast&){
		return false;
	}

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


CCache::CCache(const CCache& other)
	:m_updateBridge(this)
{
	m_includePaths = other.m_includePaths;

	m_types = other.m_types;
}


QList<QString> CCache::GetIncludePaths() const
{
	return m_includePaths;
}


void CCache::SetIncludePaths(const QList<QString>& includePaths)
{
	if (includePaths != m_includePaths){
		istd::CChangeNotifier notifier(this);

		m_includePaths = includePaths;
	}
}


void CCache::AddIncludePathsElement(const QString& element)
{
	istd::CChangeNotifier notifier(this);

	m_includePaths << element;
}


int CCache::GetTypesCount() const
{
	return m_types.GetCount();
}


const istd::TPointerVector<CCacheEntry>& CCache::GetTypes() const
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

bool CCache::HasTypesElement(const CCacheEntry& element)
{
	for (int index = 0; index < m_types.GetCount(); ++index){
		if (m_types.GetAt(index)->IsEqual(element)){
			return true;
		}
	}

	return false;
}


bool CCache::WriteToJsonObject(QJsonObject& jsonObject) const
{	
	QJsonArray newIncludePathsJsonArray;
	for (qsizetype includePathsIndex = 0; includePathsIndex < m_includePaths.size(); ++includePathsIndex){
		newIncludePathsJsonArray << QJsonValue::fromVariant(m_includePaths[includePathsIndex]);
	}
	jsonObject["IncludePaths"] = newIncludePathsJsonArray;

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

		ResetData(mode);

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

		return true;
	}
	catch (const std::bad_cast&) {
		return false;
	}

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




