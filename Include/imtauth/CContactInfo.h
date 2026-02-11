// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDate>

// ACF includes
#include <imod/CModelUpdateBridge.h>

// ImtBase includes
#include <imtauth/IContactInfo.h>
#include <imtauth/CAddressCollection.h>


namespace imtauth
{


class CContactInfo: virtual public IContactInfo
{
public:
	static QByteArray GetTypeId();

	CContactInfo();

	// reimplemented (IContactInfo)
	virtual QString GetEmail() const override;
	virtual void SetEmail(const QString& email) override;
	virtual GenderType GetGenderType() const override;
	virtual void SetGenderType(GenderType genderType) override;
	virtual QDate GetBirthday() const override;
	virtual void SetBirthday(const QDate& birthday) override;
	virtual QString GetNameField(NameFieldType fieldType) const override;
	virtual void SetNameField(NameFieldType fieldType, const QString& value) override;
	virtual const IAddressProvider* GetAddresses() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_email;
	GenderType m_genderType;
	QDate m_birthday;
	QString m_firstName;
	QString m_lastName;
	QString m_nickName;

	imod::TModelWrap<CAddressCollection> m_addresses;

	imod::CModelUpdateBridge m_modelUpdateBridge;
};


} // namespace imtauth


