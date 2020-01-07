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


