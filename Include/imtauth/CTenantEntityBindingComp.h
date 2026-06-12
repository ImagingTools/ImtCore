// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/CTenantEntityBinding.h>


namespace imtauth
{


class CTenantEntityBindingComp:
			public icomp::CComponentBase,
			public CIdentifiableTenantEntityBinding
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantEntityBindingComp);
		I_REGISTER_INTERFACE(ITenantEntityBinding);
		I_REGISTER_INTERFACE(iser::ISerializable);
	I_END_COMPONENT;
};


} // namespace imtauth
