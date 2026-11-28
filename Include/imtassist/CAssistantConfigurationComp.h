// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtassist/CAssistantConfiguration.h>


namespace imtassist
{


/**
	Component of the assistant configuration of a tenant, used as factory of the store.

	\ingroup imtassist
*/
class CAssistantConfigurationComp:
			public icomp::CComponentBase,
			public CIdentifiableAssistantConfiguration
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAssistantConfigurationComp);
		I_REGISTER_INTERFACE(IAssistantConfiguration);
		I_REGISTER_INTERFACE(iser::ISerializable);
	I_END_COMPONENT;
};


} // namespace imtassist


