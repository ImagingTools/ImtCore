// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/CContractTypeInfo.h>


namespace imtauth
{


class CContractTypeInfoComp:
			public icomp::CComponentBase,
			public CIdentifiableContractTypeInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CContractTypeInfoComp);
		I_REGISTER_INTERFACE(IContractTypeInfo);
		I_REGISTER_INTERFACE(iser::ISerializable);
	I_END_COMPONENT;
};


} // namespace imtauth


