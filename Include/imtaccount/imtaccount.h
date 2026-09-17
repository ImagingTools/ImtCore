// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


/**
	Library containing interfaces and implementation of the party related entities:
	accounts, contacts, persons, companies and their addresses.
	It is independent of the authentification and authorization logic implemented in \ref imtauth.

	Entity model:
	- IContactInfo holds communication channels and is owned by every contactable entity.
	- IContactInfoProvider is implemented by everything owning a contact info.
	- IParty describes a subject and is implemented by IPersonInfo and ICompanyInfo.
	- ICustomerInfo is a role played by a party, extended by each product.
*/
namespace imtaccount
{


} // namespace imtaccount
