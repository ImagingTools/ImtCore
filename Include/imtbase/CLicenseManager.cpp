#include <imtbase/CLicenseManager.h>


// ACF includes
#include <istd/TSingleFactory.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/CLicenseInfo.h>


namespace imtbase
{


static QByteArray s_objectTypeId = "LicenseInfo";


// public methods

CLicenseManager::CLicenseManager()
{
	m_licenses.RegisterFactory(new istd::TSingleFactory<istd::IChangeable, CLicenseInfo>(s_objectTypeId), true);

	m_types.InsertOption("License Info", s_objectTypeId);
}


// reimplemented (imtbase::ILicenseInfoProvider)

const imtbase::ICollectionInfo& CLicenseManager::GetLicenseList() const
{
	return m_licenses;
}


const imtbase::ILicenseInfo* CLicenseManager::GetLicenseInfo(const QByteArray& licenseId) const
{
	return dynamic_cast<const imtbase::ILicenseInfo*>(m_licenses.GetObjectPtr(licenseId));
}


// reimplemented (IObjectCollection)

const imtbase::IRevisionController* CLicenseManager::GetRevisionController() const
{
	return nullptr;
}


int CLicenseManager::GetOperationFlags(const QByteArray& objectId) const
{
	return m_licenses.GetOperationFlags(objectId);
}


bool CLicenseManager::GetDataMetaInfo(const QByteArray& objectId, MetaInfoPtr& metaInfoPtr) const
{
	return m_licenses.GetDataMetaInfo(objectId, metaInfoPtr);
}


QByteArray CLicenseManager::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* defaultValuePtr,
			const QByteArray& proposedObjectId)
{
	return m_licenses.InsertNewObject(typeId, name, description, defaultValuePtr, proposedObjectId);
}


bool CLicenseManager::RemoveObject(const QByteArray& objectId)
{
	return m_licenses.RemoveObject(objectId);
}


const istd::IChangeable* CLicenseManager::GetObjectPtr(const QByteArray& objectId) const
{
	return m_licenses.GetObjectPtr(objectId);
}


bool CLicenseManager::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const
{
	return m_licenses.GetObjectData(objectId, dataPtr);
}


bool CLicenseManager::SetObjectData(const QByteArray & objectId, const istd::IChangeable& object, CompatibilityMode mode)
{
	return m_licenses.SetObjectData(objectId, object, mode);
}


void CLicenseManager::SetObjectName(const QByteArray & objectId, const QString & objectName)
{
	m_licenses.SetObjectName(objectId, objectName);
}


void CLicenseManager::SetObjectDescription(const QByteArray& objectId, const QString& objectDescription)
{
	m_licenses.SetObjectDescription(objectId, objectDescription);
}


void CLicenseManager::SetObjectEnabled(const QByteArray& objectId, bool isEnabled)
{
	m_licenses.SetObjectEnabled(objectId, isEnabled);
}


bool CLicenseManager::RegisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler)
{
	return m_licenses.RegisterEventHandler(eventHandler);
}


bool CLicenseManager::UnregisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler)
{
	return m_licenses.UnregisterEventHandler(eventHandler);
}


// reimplemented (IObjectCollectionInfo)

bool CLicenseManager::GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const
{
	return m_licenses.GetCollectionItemMetaInfo(objectId, metaInfo);
}


const iprm::IOptionsList* CLicenseManager::GetObjectTypesInfo() const
{
	return &m_types;
}


ICollectionInfo::Id CLicenseManager::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	return s_objectTypeId;
}


// reimplemented (ICollectionInfo)

ICollectionInfo::Ids CLicenseManager::GetElementIds() const
{
	return m_licenses.GetElementIds();
}


QVariant CLicenseManager::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	return m_licenses.GetElementInfo(elementId, infoType);
}


// reimplemented (iser::ISerializable)

bool CLicenseManager::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag licensesTag("LicenseContainer", "Container of licenses", iser::CArchiveTag::TT_GROUP);
	bool retVal = archive.BeginTag(licensesTag);
	retVal = retVal && m_licenses.Serialize(archive);
	retVal = retVal && archive.EndTag(licensesTag);

	return retVal;
}

// reimplemented (istd::IChangeable)

bool CLicenseManager::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CLicenseManager* sourcePtr = dynamic_cast<const imtbase::CLicenseManager*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		return m_licenses.CopyFrom(sourcePtr->m_licenses);
	}

	return false;
}


bool CLicenseManager::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	return m_licenses.ResetData();
}


} // namespace imtbase


