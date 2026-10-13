// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CModelUpdateBridge.h>
#include <imod/TModelWrap.h>
#include <istd/TPointerVector.h>

// ImtCore includes
#include <imtaccount/CContactInfo.h>
#include <imtaccount/ICompanyInfo.h>
#include <imtbase/TIdentifiableWrap.h>


namespace imtaccount
{


class CCompanyInfo: virtual public ICompanyInfo
{
public:
	static QByteArray GetTypeId();

	CCompanyInfo();

	// reimplemented (ICompanyInfo)
	virtual QString GetLegalName() const override;
	virtual void SetLegalName(const QString& legalName) override;
	virtual const ICompanyInfo* GetParent() const override;
	virtual const istd::TPointerVector<const ICompanyInfo>& GetChildren() const override;

	// reimplemented (IContactInfoProvider)
	virtual const IContactInfo* GetContactInfo() const override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_legalName;

	ICompanyInfo* m_parentCompanyPtr;
	CompanyInfoList m_companyChildren;

	imod::TModelWrap<CContactInfo> m_contactInfo;

	imod::CModelUpdateBridge m_modelUpdateBridge;
};


typedef imtbase::TIdentifiableWrap<CCompanyInfo> CIdentifiableCompanyInfo;


} // namespace imtaccount
