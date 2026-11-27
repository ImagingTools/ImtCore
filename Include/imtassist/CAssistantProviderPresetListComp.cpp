// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantProviderPresetListComp.h>


namespace imtassist
{


CAssistantProviderPresetListComp::CAssistantProviderPresetListComp()
	:m_presets(CAssistantProviderPresets::GetPresets())
{
}


// reimplemented (iprm::IOptionsList)

int CAssistantProviderPresetListComp::GetOptionsFlags() const
{
	return SCF_SUPPORT_UNIQUE_ID;
}


int CAssistantProviderPresetListComp::GetOptionsCount() const
{
	return m_presets.size();
}


QString CAssistantProviderPresetListComp::GetOptionName(int index) const
{
	return ((index >= 0) && (index < m_presets.size())) ? m_presets.at(index).name : QString();
}


QString CAssistantProviderPresetListComp::GetOptionDescription(int index) const
{
	return ((index >= 0) && (index < m_presets.size())) ? m_presets.at(index).credentialHint : QString();
}


QByteArray CAssistantProviderPresetListComp::GetOptionId(int index) const
{
	return ((index >= 0) && (index < m_presets.size())) ? m_presets.at(index).id : QByteArray();
}


bool CAssistantProviderPresetListComp::IsOptionEnabled(int /*index*/) const
{
	return true;
}


} // namespace imtassist


