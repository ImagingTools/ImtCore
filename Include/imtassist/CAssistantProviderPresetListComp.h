// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtassist/CAssistantProviderPresets.h>


namespace imtassist
{


/**
	The supported model providers as options, e.g. for the provider selection in the server settings.
	The option ID is the preset ID of CAssistantProviderPresets.

	\ingroup imtassist
*/
class CAssistantProviderPresetListComp:
			public icomp::CComponentBase,
			virtual public iprm::IOptionsList
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAssistantProviderPresetListComp);
		I_REGISTER_INTERFACE(iprm::IOptionsList);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT;

	CAssistantProviderPresetListComp();

	// reimplemented (iprm::IOptionsList)
	virtual int GetOptionsFlags() const override;
	virtual int GetOptionsCount() const override;
	virtual QString GetOptionName(int index) const override;
	virtual QString GetOptionDescription(int index) const override;
	virtual QByteArray GetOptionId(int index) const override;
	virtual bool IsOptionEnabled(int index) const override;

private:
	CAssistantProviderPresets::Presets m_presets;
};


} // namespace imtassist


