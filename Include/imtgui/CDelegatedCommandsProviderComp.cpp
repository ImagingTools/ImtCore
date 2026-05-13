// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgui/CDelegatedCommandsProviderComp.h>


namespace imtgui
{


// public methods

// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CDelegatedCommandsProviderComp::GetCommands() const
{
	if (m_slaveCommandsProviderCompPtr.IsValid()){
		return m_slaveCommandsProviderCompPtr->GetCommands();
	}

	return nullptr;
}


} // namespace imtgui


