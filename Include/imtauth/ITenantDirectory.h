// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/ITenantDirectoryProfile.h>


namespace imtauth
{


/**
	Filter parameters of the collection of the profiles (iprm::ITextParam each).
	The search text, a part of the name, the description or the industry: with it only the found organizations are selected,
	an empty one selects none.
*/
static const inline QByteArray s_directorySearchTextParamId = QByteArrayLiteral("DirectorySearchText");
/// The organization searching, left out of the result
static const inline QByteArray s_directoryExcludedTenantIdParamId = QByteArrayLiteral("DirectoryExcludedTenantId");


/**
	The directory of the organizations that want to be found.
	\ingroup Tenant
*/
class ITenantDirectory: virtual public istd::IChangeable
{
public:
	/**
		The profile of the organization; an organization that never set one has the default profile.
		\return Null when the profile could not be read.
	*/
	virtual ITenantDirectoryProfileUniquePtr GetProfile(const QByteArray& tenantId) const = 0;

	virtual bool SetProfile(const ITenantDirectoryProfile& profile, QString& errorMessage) = 0;
};


} // namespace imtauth


