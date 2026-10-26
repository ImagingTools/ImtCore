// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imtbase/TIdentifiableWrap.h>

// ImtCore includes
#include <imtaccount/IPersonInfo.h>


namespace imtaccount
{


class CPersonInfo: virtual public IPersonInfo
{
public:
	static QByteArray GetTypeId();

	CPersonInfo();

	// reimplemented (IPersonInfo)
	virtual GenderType GetGenderType() const override;
	virtual void SetGenderType(GenderType genderType) override;
	virtual QDate GetBirthday() const override;
	virtual void SetBirthday(const QDate& birthday) override;
	virtual QString GetNameField(NameFieldType fieldType) const override;
	virtual void SetNameField(NameFieldType fieldType, const QString& value) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	GenderType m_genderType;

	QDate m_birthday;

	QString m_firstName;
	QString m_lastName;
	QString m_nickName;
	QString m_salutation;
	QString m_title;
};


using CIdentifiablePersonInfo = imtbase::TIdentifiableWrap<CPersonInfo>;


} // namespace imtaccount

