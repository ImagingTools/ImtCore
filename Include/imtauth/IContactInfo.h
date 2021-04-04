#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iser/IObject.h>


class QDate;


namespace imtauth
{


class IAddressProvider;


/**
	Interface describing the contact information.
	\ingroup Authentification
*/
class IContactInfo: virtual public iser::IObject
{
public:
	enum GenderType
	{
		GT_DIVERSE, // :)
		GT_FEMALE,
		GT_MALE
	};

	I_DECLARE_ENUM(GenderType, GT_DIVERSE, GT_FEMALE, GT_MALE);

	enum NameFieldType
	{
		NFT_FIRST_NAME,
		NFT_LAST_NAME,
		NFT_NICKNAME,
		NFT_SALUTATION,
		NFT_TITLE
	};

	I_DECLARE_ENUM(NameFieldType, NFT_FIRST_NAME, NFT_LAST_NAME, NFT_NICKNAME, NFT_SALUTATION, NFT_TITLE);

	enum MetaInfoTypes
	{
		/**
			Contact email given as QString.
		*/
		MIT_MAIL = idoc::IDocumentMetaInfo::MIT_USER + 1,

		/**
			Contact gender type given as GenderType.
		*/
		MIT_GENDER_TYPE,

		/**
			Contact birthday given as QDate.
		*/
		MIT_BIRTHDAY,

		/**
			Contact first name given as QString.
		*/
		MIT_FIRST_NAME,

		/**
			Contact last name given as QString.
		*/
		MIT_LAST_NAME,

		/**
			Contact nickname given as QString.
		*/
		MIT_NICKNAME
	};

	virtual QString GetMail() const = 0;
	virtual void SetEmail(const QString& email) = 0;
	virtual GenderType GetGenderType() const = 0;
	virtual void SetGenderType(GenderType genderType) = 0;
	virtual QDate GetBirthday() const = 0;
	virtual void SetBirthday(const QDate& birthday) = 0;
	virtual QString GetNameField(NameFieldType fieldType) const = 0;
	virtual void SetNameField(NameFieldType fieldType, const QString& value) = 0;
	virtual const IAddressProvider* GetAddresses() const = 0;
};


} // namespace imtauth


