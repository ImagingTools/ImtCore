#pragma once


// ImtCore includes
#include <imtauth/ICompanyInfo.h>
#include <imtauth/CCompanyBaseInfo.h>


namespace imtauth
{


class CCompanyInfo: virtual public ICompanyInfo, virtual public CCompanyBaseInfo
{
public:
	typedef CCompanyBaseInfo BaseClass;

	// reimplemented (ICompanyInfo)
	virtual const IAddressProvider* GetAddresses() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	imod::TModelWrap<CAddressCollection> m_addresses;
};


}
