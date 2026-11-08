// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/CStaticPermissionsProviderCompBase.h>


namespace imtauth
{


/**
	What the server of a product may do at the authorization server: the permissions of its token.
	They are given to server tokens only, never to roles: no product tree holds them.
*/
class CProductServerPermissionsProviderComp:
			public CStaticPermissionsProviderCompBase
{
public:
	typedef CStaticPermissionsProviderCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductServerPermissionsProviderComp)
	I_END_COMPONENT

protected:
	// reimplemented (CStaticPermissionsProviderCompBase)
	virtual void CreateFeatures() override;
};


} // namespace imtauth


