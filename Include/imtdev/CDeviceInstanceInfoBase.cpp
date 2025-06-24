#include <imtdev/CDeviceInstanceInfoBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>


namespace imtdev
{


// public methods

CDeviceInstanceInfoBase::CDeviceInstanceInfoBase()
	:m_updateBridge(this)
{
	m_versionInfo.AttachObserver(&m_updateBridge);
}


CDeviceInstanceInfoBase::~CDeviceInstanceInfoBase()
{
	m_updateBridge.EnsureModelsDetached();
}


// reimplemented (imtdev::IDeviceInstanceInfoEditor)

bool CDeviceInstanceInfoBase::SetIdentifier(IdentifierTypes idType, const QByteArray& value)
{
	istd::CChangeNotifier notifier(this);

	m_identifiers[idType] = value;

	return true;
}


bool CDeviceInstanceInfoBase::SetVersion(int /*versionId*/, quint32 /*version*/, const QString& /*name*/, const QString& /*description*/)
{
	return false;
}


iattr::IAttributesManager* CDeviceInstanceInfoBase::GetAttributesManager()
{
	EnsureAttributesCreated();

	return dynamic_cast<iattr::IAttributesManager*>(m_attributesPtr.get());
}


// reimplemented (imtdev::IDeviceInstanceInfo)

QByteArray CDeviceInstanceInfoBase::GetIdentifier(int idType) const
{
	if (m_identifiers.contains(idType)){
		return m_identifiers[idType];
	}

	return QByteArray();
}


const iser::IVersionInfo& CDeviceInstanceInfoBase::GetVersion() const
{
	return m_versionInfo;
}


const iattr::IAttributesProvider* CDeviceInstanceInfoBase::GetAttributes() const
{
	EnsureAttributesCreated();

	return m_attributesPtr.get();
}


// reimplemented (istd::IChangeable)

int CDeviceInstanceInfoBase::GetSupportedOperations() const
{
	return SO_COPY | SO_RESET;
}


bool CDeviceInstanceInfoBase::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CDeviceInstanceInfoBase* sourcePtr = dynamic_cast<const CDeviceInstanceInfoBase*>(&object);
	if (sourcePtr != nullptr){
		if (GetStaticInfo().GetTypeId() != sourcePtr->GetStaticInfo().GetTypeId()){
			return false;
		}

		istd::CChangeNotifier notifier(this);

		m_identifiers = sourcePtr->m_identifiers;

		bool retVal = m_versionInfo.CopyFrom(sourcePtr->m_versionInfo);
		if (retVal){
			const iattr::IAttributesProvider* sourceAttributesPtr = sourcePtr->GetAttributes();
			if (sourceAttributesPtr != nullptr){
				retVal = EnsureAttributesCreated();
				if (retVal){
					retVal = m_attributesPtr->CopyFrom(*sourceAttributesPtr);
				}
			}
		}

		return retVal;
	}

	return false;
}


bool CDeviceInstanceInfoBase::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_identifiers.clear();
	m_attributesPtr.reset();

	bool retVal = m_versionInfo.ResetData();

	return retVal;
}


// private metods

bool CDeviceInstanceInfoBase::EnsureAttributesCreated() const
{
	if (m_attributesPtr == nullptr){
		m_attributesPtr.reset(GetStaticInfo().CreateInstanceAttributes());
	}

	return m_attributesPtr != nullptr;
}


// public methods of the embedded class VersionInfo

// reimplemenbted (iser::IVersionInfo)

iser::IVersionInfo::VersionIds CDeviceInstanceInfoBase::VersionInfo::GetVersionIds() const
{
	QList<int> ids = m_versions.keys();

	VersionIds retVal(ids.cbegin(), ids.cend());

	return retVal;
}


bool CDeviceInstanceInfoBase::VersionInfo::GetVersionNumber(int versionId, quint32& result) const
{
	if (m_versions.contains(versionId)){
		result = m_versions[versionId].versionNumber;

		return true;
	}

	return false;
}


QString CDeviceInstanceInfoBase::VersionInfo::GetVersionIdDescription(int versionId) const
{
	if (m_versions.contains(versionId)){
		return m_versions[versionId].versionDescription;
	}

	return QString();
}


QString CDeviceInstanceInfoBase::VersionInfo::GetEncodedVersionName(int versionId, quint32 versionNumber) const
{
	if (m_versions.contains(versionId)){
		return QString::number(versionNumber);
	}

	return QString();
}


// reimplemented (istd::IChangeable)

int CDeviceInstanceInfoBase::VersionInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CDeviceInstanceInfoBase::VersionInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const VersionInfo* sourcePtr = dynamic_cast<const VersionInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this);

		m_versions = sourcePtr->m_versions;

		return true;
	}

	return false;
}


istd::IChangeable* CDeviceInstanceInfoBase::VersionInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<istd::IChangeable> clonePtr(new VersionInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CDeviceInstanceInfoBase::VersionInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_versions.clear();

	return true;
}


} // namespace imtdev


