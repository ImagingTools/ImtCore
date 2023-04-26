#pragma once


// Qt includes
#include <QtCore/QDate>

// ACF includes
#include <imtauth/IPersonBaseInfo.h>

// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/CContactBaseInfo.h>


namespace imtauth
{


class CPersonBaseInfo: virtual public IPersonBaseInfo, virtual public CContactBaseInfo
{
public:
	typedef CContactBaseInfo BaseClass;

	// reimplemented (IPersonInfo)
	virtual GenderType GetGenderType() const override;
	virtual void SetGenderType(GenderType genderType) override;
	virtual QDate GetBirthday() const override;
	virtual void SetBirthday(const QDate& birthday) override;
	virtual QString GetNameField(NameFieldType fieldType) const override;
	virtual void SetNameField(NameFieldType fieldType, const QString& value) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_lastName;
	QString m_nick;
	QDate m_birthday;
	GenderType m_genderType;
};


typedef imtbase::TIdentifiableWrap<CPersonBaseInfo> CIdentifiablePersonInfo;


}


