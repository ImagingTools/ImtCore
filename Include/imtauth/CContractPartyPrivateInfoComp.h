// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/CContractPartyPrivateInfo.h>


namespace imtauth
{


class CContractPartyPrivateInfoComp:
			public icomp::CComponentBase,
			public CIdentifiableContractPartyPrivateInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CContractPartyPrivateInfoComp);
		I_REGISTER_INTERFACE(IContractPartyPrivateInfo);
		I_REGISTER_INTERFACE(iser::ISerializable);
	I_END_COMPONENT;
};


} // namespace imtauth


