#pragma once


// ACF includes
#include <iser/IObject.h>
#include <iimg/IBitmap.h>
#include <iauth/IRightsProvider.h>

// ImtCore includes
#include <imtauth/IPersonInfo.h>


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

	/**
		Get information about the type of the account.
		\sa AccountType
	*/
	virtual AccountType GetAccountType() const = 0;

	/**
	Get name of the account.
	*/
	virtual QString GetAccountName() const = 0;

	/**
		Get description of the account.
	*/
	virtual QString GetAccountDescription() const = 0;

	/**
		Get information about the person who owns and manages this account.
	*/
	virtual const IPersonInfo* GetAccountOwner() const = 0;

	/**
		Get the access to the rights provider associated with this account.
	*/
	virtual const iauth::IRightsProvider& GetAccountRights() const = 0;

	/**
		Get account picture.
	*/
	virtual const iimg::IBitmap& GetAccountPicture() const = 0;
};


} // namespace imtauth


