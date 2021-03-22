#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iser/IObject.h>


class QDate;


namespace imtauth
{


class IAddressProvider;


/**
	Interface descibing a natural person.
	\ingroup Authentification
*/
class IPersonInfo: virtual public iser::IObject
{
public:
	enum GenderType
	{
		GT_DIVERSE, // :)
		GT_FEMALE,
		GT_MALE
	};

	enum NameFieldType
	{
		NFT_FIRST_NAME,
		NFT_LAST_NAME,
		NFT_NICKNAME
	};

	enum MetaInfoTypes
	{
		/**
			Person gender type given as GenderType.
		*/
		MIT_GENDER_TYPE = idoc::IDocumentMetaInfo::MIT_USER + 1,

		/**
			Person birthday given as QDate.
		*/
		MIT_BIRTHDAY,

		/**
			Person first name given as QString.
		*/
		MIT_FIRST_NAME,

		/**
			Person last name given as QString.
		*/
		MIT_LAST_NAME,

		/**
			Person nickname given as QString.
		*/
		MIT_NICKNAME
	};

	virtual GenderType GetGenderType() const = 0;
	virtual void SetGenderType(GenderType genderType) = 0;
	virtual QDate GetBirthday() const = 0;
	virtual void SetBirthday(const QDate& birthday) = 0;
	virtual QString GetNameField(NameFieldType fieldType) const = 0;
	virtual void SetNameField(NameFieldType fieldType, const QString& value) = 0;
	virtual const IAddressProvider* GetAddresses() const = 0;
};


} // namespace imtauth


