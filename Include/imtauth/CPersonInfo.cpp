#include <imtauth/CPersonInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// reimplemented (IPersonInfo)

const IAddressProvider* CPersonInfo::GetAddresses() const
{
	return &m_addresses;
}


// reimplemented (iser::ISerializable)

bool CPersonInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);

	iser::CArchiveTag addressesTag("Addresses", "Addresses", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(addressesTag);
	retVal = retVal && m_addresses.Serialize(archive);
	retVal = retVal && archive.EndTag(addressesTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CPersonInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_RESET;
}


bool CPersonInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CPersonInfo* sourcePtr = dynamic_cast<const CPersonInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		bool retVal = BaseClass::CopyFrom(object);

		retVal = retVal && m_addresses.CopyFrom(sourcePtr->m_addresses);

		return retVal;
	}

	return false;
}


istd::IChangeable* CPersonInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CPersonInfo> clonePtr(new CPersonInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CPersonInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	bool retVal = BaseClass::ResetData();

	retVal = retVal && m_addresses.ResetData();

	return retVal;
}


}


