#pragma once


// ACF includes
#include <iauth/IRightsProvider.h>
#include <idoc/IDocumentMetaInfo.h>
#include <iimg/IBitmap.h>
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>


namespace imtauth
{


/**
	Interface for describing an user account.
	\ingroup Authentification
*/
class IAccountInfo: virtual public iser::IObject
{
public:
	/**
		Supported account types.
	*/
	enum AccountType
	{
		/**
			A personal account.
		*/
		AT_PERSON,

		/**
			A company account.
		*/
		AT_COMPANY
	};

	enum MetaInfoTypes
	{
		/**
			Account type given as AccountType.
		*/
		MIT_ACCOUNT_TYPE = idoc::IDocumentMetaInfo::MIT_USER + 1,

		/**
			Account name given as QString.
		*/
		MIT_ACCOUNT_NAME,

		/**
			Account description given as QString.
		*/
		MIT_ACCOUNT_DESCRIPTION,

		/**
			Contact email given as QString.
		*/
		MIT_CONTACT_EMAIL,

		/**
			Contact birthday given as QDate.
		*/
		MIT_CONTACT_BIRTHDAY,

		/**
			Contact first name given as QString.
		*/
		MIT_CONTACT_FIRST_NAME,

		/**
			Contact last name given as QString.
		*/
		MIT_CONTACT_LAST_NAME,

		/**
			Contact nickname given as QString.
		*/
		MIT_CONTACT_NICKNAME
	};

	/**
		Get information about the type of the account.
		\sa AccountType
	*/
	virtual AccountType GetAccountType() const = 0;

	/**
		Set information about the type of the account.
		\sa AccountType
	*/
	virtual void SetAccountType(AccountType accountType) = 0;

	/**
		Get name of the account.
	*/
	virtual QString GetAccountName() const = 0;

	/**
		Set name of the account.
	*/
	virtual void SetAccountName(QString accountName) = 0;

	/**
		Get description of the account.
	*/
	virtual QString GetAccountDescription() const = 0;

	/**
		Set description of the account.
	*/
	virtual void SetAccountDescription(QString accountDescription) = 0;

	/**
		Get account picture.
	*/
	virtual const iimg::IBitmap& GetAccountPicture() const = 0;

	/**
		Set account picture.
	*/
	virtual void SetAccountPicture(const iimg::IBitmap& picture) = 0;

	/**
		Get information about the person who owns and manages this account.
	*/
	virtual const IContactInfo* GetAccountOwner() const = 0;

	/**
		Set informations about account owner.
	*/
	virtual void SetAccountOwner(const IContactInfo& ownerInfo) = 0;

	/**
		Get the access to the rights provider associated with this account.
	*/
	virtual const iauth::IRightsProvider& GetAccountRights() const = 0;
};


} // namespace imtauth


