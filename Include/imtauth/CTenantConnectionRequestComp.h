// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/CTenantConnectionRequest.h>


namespace imtauth
{


class CTenantConnectionRequestComp:
			public icomp::CComponentBase,
			public CIdentifiableTenantConnectionRequest
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantConnectionRequestComp);
		I_REGISTER_INTERFACE(ITenantConnectionRequestInfo);
		I_REGISTER_INTERFACE(iser::ISerializable);
	I_END_COMPONENT;
};


} // namespace imtauth
