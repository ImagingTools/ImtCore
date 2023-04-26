#pragma once


// ACF includes
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ICompanyInfo.h>
#include <imtauth/CContactBaseInfo.h>


namespace imtauth
{


class CCompanyBaseInfo: virtual public ICompanyBaseInfo, virtual public CContactBaseInfo
{
public:
	typedef CContactBaseInfo BaseClass;

	// reimplemented (ICompanyInfo)
	virtual const ICompanyInfo* GetParent() const override;
	virtual const istd::TPointerVector<const ICompanyBaseInfo>& GetChildren() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	void GetAllCompanyRecursive(const CompanyInfoList& allCompany, int maxDepth, int currentDepth, QByteArrayList& companyList) const;
	const ICompanyInfo* GetCompanyRecursive(const CompanyInfoList& allCompany, const QByteArray& subfeatureId, int maxDepth, int currentDepth = 1) const;

private:
	ICompanyInfo* m_parentCompanyPtr;
	CompanyInfoList m_companyChildren;
};


typedef imtbase::TIdentifiableWrap<CCompanyBaseInfo> CIdentifiableCompanyInfo;


}
