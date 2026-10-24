// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtaccount/IAccountInfo.h>
#include <imtaccount/IContactInfoProvider.h>
#include <imtbase/IUserGroupScope.h>


namespace imtaccount
{


/**
	Role of an account acting as a customer.
	Product independent part only: concrete products derive from this interface and add their own data.
	\ingroup Account
*/
class ICustomerInfo:
	virtual public IContactInfoProvider,
	virtual public imtbase::IUserGroupScope,
	virtual public iser::IObject
{
public:
	/**
		Get the account acting as this customer.
		IAccountInfo::GetAccountType() tells whether it is a person or a company.
	*/
	virtual IAccountInfo* GetAccount() const = 0;

	/**
		Get accounts representing this customer, for example employees.
		Unrelated to GetContactInfo(), which describes how to reach the customer itself.
	*/
	virtual const AccountInfoList& GetRepresentatives() const = 0;
};


} // namespace imtaccount
