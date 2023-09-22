#include <imtauth/CCompanyInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods


CCompanyInfo::CCompanyInfo():
	m_modelUpdateBridge(this, imod::CModelUpdateBridge::UF_SOURCE)
{
	m_addresses.AttachObserver(&m_modelUpdateBridge);
}


void CCompanyInfo::AddAddress(IAddress& address)
{
	m_addresses.AddAddress(&address);
}


// reimplemented (ICompanyInfo)

const IAddressProvider* CCompanyInfo::GetAddresses() const
{
	return &m_addresses;
}


// reimplemented (iser::ISerializable)

bool CCompanyInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();
	quint32 identifiableVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, identifiableVersion)){
		identifiableVersion = 0;
	}

	if (identifiableVersion > 6362){
		retVal = retVal && BaseClass::Serialize(archive);

		iser::CArchiveTag addressesTag("Addresses", "Addresses", iser::CArchiveTag::TT_GROUP);
		retVal = retVal && archive.BeginTag(addressesTag);
		retVal = retVal && m_addresses.Serialize(archive);
		retVal = retVal && archive.EndTag(addressesTag);
	}

	return retVal;
}


bool CCompanyInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CCompanyInfo* sourcePtr = dynamic_cast<const CCompanyInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		bool retVal = BaseClass::CopyFrom(object);

		retVal = retVal && m_addresses.CopyFrom(sourcePtr->m_addresses);

		return retVal;
	}

	return false;
}


istd::IChangeable *CCompanyInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CCompanyInfo> clonePtr(new CCompanyInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CCompanyInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	bool retVal = BaseClass::ResetData();

	retVal = retVal && m_addresses.ResetData();

	return retVal;
}


}


