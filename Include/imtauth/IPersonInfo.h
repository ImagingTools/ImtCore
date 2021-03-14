#pragma once


// ACF includes
#include <iser/IObject.h>


class QDate;


namespace imtauth
{



/**
	Interface descibing a natural person.
	\ingroup Authentification
*/
class IPersonInfo: virtual public iser::IObject
{
public:
	enum GenderType
	{
		GT_MALE,
		GT_FEMALE,
		GT_DIVERSE // :)
	};

	enum NameFieldType
	{
		NFT_FIRST_NAME,
		NFT_LAST_NAME,
		NFT_NICKNAME
	};

	enum ContactFieldType
	{
		CFT_PRIVATE,
		CFT_BUSINESS
	};

	virtual GenderType GetGenderType() const = 0;
	virtual void SetGenderType(GenderType genderType) = 0;
	virtual QDate GetBirthday() const = 0;
	virtual void SetBirthday(const QDate& birthday) = 0;
	virtual QString GetNameField(NameFieldType) const = 0;
	virtual void SetNameField(NameFieldType fieldType, const QString& value) = 0;
};


} // namespace imtauth


