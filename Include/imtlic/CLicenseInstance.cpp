#include <imtlic/CLicenseInstance.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtlic
{


// public methods

QByteArray CLicenseInstance::GetTypeId()
{
	return "LicenseInstance";
}


// reimplemented (imtlic::ILicenseInstance)

QDateTime CLicenseInstance::GetExpiration() const
{
	return m_expirationTime;
}


void CLicenseInstance::SetExpiration(const QDateTime& expirationTime)
{
	if (m_expirationTime != expirationTime){
		istd::CChangeNotifier notifier(this);

		m_expirationTime = expirationTime;
	}
}


// reimplemented (iser::IObject)

QByteArray CLicenseInstance::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (iser::ISerializable)

bool CLicenseInstance::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);

	static iser::CArchiveTag expirationTimeTag("ExpirationTime", "Expired time stamp", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(expirationTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_expirationTime);
	retVal = retVal && archive.EndTag(expirationTimeTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CLicenseInstance::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::ILicenseInstance* sourcePtr = dynamic_cast<const imtlic::ILicenseInstance*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		if (BaseClass::CopyFrom(object)){
			m_expirationTime = sourcePtr->GetExpiration();

			return true;
		}
	}
	else{
		const imtlic::ILicenseInfo* interfacePtr = dynamic_cast<const imtlic::ILicenseInfo*>(&object);
		if (interfacePtr != nullptr){
			istd::CChangeNotifier changeNotifier(this);

			if (BaseClass::CopyFrom(object)){
				m_expirationTime = QDateTime();

				return true;
			}
		}
	}

	return false;
}


istd::IChangeable* CLicenseInstance::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CLicenseInstance> clonePtr(new CLicenseInstance);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CLicenseInstance::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	if (BaseClass::ResetData(mode)){
		m_expirationTime = QDateTime();

		return true;
	}

	 return false;
}


} // namespace imtlic


