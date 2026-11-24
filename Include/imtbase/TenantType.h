// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


namespace imtbase
{


/**
	Kind of the tenant that owns data.
	TT_ORGANIZATION: an organization, the tenant ID is the ID of the organization.
	TT_USER: the personal space of a user outside of any organization, the tenant ID is the ID of the user.
*/
enum TenantType
{
	TT_ORGANIZATION = 0,
	TT_USER = 1
};


} // namespace imtbase


