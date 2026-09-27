// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


/**
	Library containing interfaces and implementation of the party related entities:
	accounts, contacts, persons, companies and their addresses.
	It is independent of the authentification and authorization logic implemented in \ref imtauth.

	Entity model:
	- IContactInfo holds communication channels and is owned by every contactable entity.
	- IContactInfoProvider is implemented by everything owning a contact info.
	- IPersonInfo and ICompanyInfo describe a subject; only ICompanyInfo owns a contact info directly.
	- IAccountInfo already distinguishes person/company/representative accounts via AccountType.
	- ICustomerInfo is a role played by an account, extended by each product.
*/
namespace imtaccount
{


} // namespace imtaccount
