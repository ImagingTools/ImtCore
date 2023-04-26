#pragma once


// ACF includes
#include <imtauth/IPersonInfo.h>

// ImtCore includes
#include <imtauth/CPersonBaseInfo.h>


namespace imtauth
{


class CPersonInfo: virtual public IPersonInfo, virtual public CPersonBaseInfo
{
public:
	typedef CPersonBaseInfo BaseClass;

	// reimplemented (IPersonInfo)
	virtual const IAddressProvider* GetAddresses() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	imod::TModelWrap<CAddressCollection> m_addresses;
};


}


