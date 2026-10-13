// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CModelUpdateBridge.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtaccount/CAddressCollection.h>
#include <imtaccount/IContactInfo.h>


namespace imtaccount
{


class CContactInfo: virtual public IContactInfo
{
public:
	static QByteArray GetTypeId();

	CContactInfo();

	// reimplemented (IContactInfo)
	virtual QString GetEmail() const override;
	virtual void SetEmail(const QString& email) override;
	virtual QString GetPhone() const override;
	virtual void SetPhone(const QString& phone) override;
	virtual QString GetWebPage() const override;
	virtual void SetWebPage(const QString& webPage) override;
	virtual IAddressProvider* GetAddresses() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_email;
	QString m_phone;
	QString m_webPage;

	imod::TModelWrap<CAddressCollection> m_addresses;

	imod::CModelUpdateBridge m_modelUpdateBridge;
};


} // namespace imtaccount
