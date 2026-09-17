// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtaccount/IContactInfoProvider.h>
#include <imtaccount/IParty.h>
#include <imtbase/IGroupScopedObject.h>


namespace imtaccount
{


/**
	Role of a party acting as a customer.
	Product independent part only: concrete products derive from this interface and add their own data.
	\ingroup Account
*/
class ICustomerInfo:
	virtual public IContactInfoProvider,
	virtual public imtbase::IGroupScopedObject,
	virtual public iser::IObject
{
public:
	/**
		Get the party acting as this customer.
	*/
	virtual IParty* GetParty() const = 0;

	/**
		Get parties acting on behalf of this customer, for example employees or branch offices.
		Unrelated to GetContactInfo(), which describes how to reach the customer itself.
	*/
	virtual const PartyList& GetRepresentatives() const = 0;
};


} // namespace imtaccount
