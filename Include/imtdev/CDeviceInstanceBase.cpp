// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdev/CDeviceInstanceBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtdev
{


// public methods

CDeviceInstanceBase::CDeviceInstanceBase()
	:m_updateBridge(this)
{
	m_versionInfo.AttachObserver(&m_updateBridge);
}


CDeviceInstanceBase::~CDeviceInstanceBase()
{
	m_updateBridge.EnsureModelsDetached();
}


// reimplemented (imtdev::IEditableDeviceInstance)

bool CDeviceInstanceBase::SetIdentifier(IdentifierTypes idType, const QByteArray& id)
{
	istd::CChangeNotifier notifier(this);

	m_identifiers[idType] = id;

	return true;
}


bool CDeviceInstanceBase::SetVersion(int versionId, quint32 version, const QString& description)
{
	m_versionInfo.SetVersion(versionId,  version, description);

	return true;
}


iattr::IAttributesManager* CDeviceInstanceBase::GetAttributesManager()
{
	EnsureAttributesCreated();

	return dynamic_cast<iattr::IAttributesManager*>(m_attributesPtr.get());
}


// reimplemented (imtdev::IDeviceInstance)

QByteArray CDeviceInstanceBase::GetIdentifier(int idType) const
{
	if (m_identifiers.contains(idType)){
		return m_identifiers[idType];
	}

	return QByteArray();
}


const iser::IVersionInfo& CDeviceInstanceBase::GetVersion() const
{
	return m_versionInfo;
}


const iattr::IAttributesProvider* CDeviceInstanceBase::GetAttributes() const
{
	EnsureAttributesCreated();

	return m_attributesPtr.get();
}


// reimplemented (istd::IChangeable)

int CDeviceInstanceBase::GetSupportedOperations() const
{
	return SO_COPY | SO_RESET;
}


bool CDeviceInstanceBase::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CDeviceInstanceBase* sourcePtr = dynamic_cast<const CDeviceInstanceBase*>(&object);
	if (sourcePtr != nullptr){
		if (GetDeviceSpecification().GetTypeId() != sourcePtr->GetDeviceSpecification().GetTypeId()){
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
			else{
				m_attributesPtr.reset();
			}
		}

		return retVal;
	}

	return false;
}


bool CDeviceInstanceBase::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_identifiers.clear();
	m_attributesPtr.reset();

	bool retVal = m_versionInfo.ResetData();

	return retVal;
}


// protected methods

bool CDeviceInstanceBase::EnsureAttributesCreated() const
{
	return false;
}


// public methods of the embedded class VersionInfo

bool CDeviceInstanceBase::VersionInfo::SetVersion(
	int versionId,
	quint32 version,
	const QString& description)
{
	m_versions[versionId].versionNumber = version;
	m_versions[versionId].versionDescription = description;

	return true;
}


// reimplemenbted (iser::IVersionInfo)

iser::IVersionInfo::VersionIds CDeviceInstanceBase::VersionInfo::GetVersionIds() const
{
	QList<int> ids = m_versions.keys();

	VersionIds retVal(ids.cbegin(), ids.cend());

	return retVal;
}


bool CDeviceInstanceBase::VersionInfo::GetVersionNumber(int versionId, quint32& result) const
{
	if (m_versions.contains(versionId)){
		result = m_versions[versionId].versionNumber;

		return true;
	}

	return false;
}


QString CDeviceInstanceBase::VersionInfo::GetVersionIdDescription(int versionId) const
{
	if (m_versions.contains(versionId)){
		return m_versions[versionId].versionDescription;
	}

	return QString();
}


QString CDeviceInstanceBase::VersionInfo::GetEncodedVersionName(int versionId, quint32 versionNumber) const
{
	if (m_versions.contains(versionId)){
		return QString::number(versionNumber);
	}

	return QString();
}


// reimplemented (istd::IChangeable)

int CDeviceInstanceBase::VersionInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CDeviceInstanceBase::VersionInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const VersionInfo* sourcePtr = dynamic_cast<const VersionInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this);

		m_versions = sourcePtr->m_versions;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CDeviceInstanceBase::VersionInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new VersionInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CDeviceInstanceBase::VersionInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_versions.clear();

	return true;
}


} // namespace imtdev


