#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ibase/ICommandsProvider.h>
#include <iqtgui/CHierarchicalCommand.h>


namespace imtgui
{


class CDelegatedCommandsProviderComp:
			public icomp::CComponentBase,
			virtual public ibase::ICommandsProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedCommandsProviderComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_slaveCommandsProviderCompPtr, "SlaveCommandsProvider", "Slave provider of commands", false, "SlaveCommandsProvider");
	I_END_COMPONENT

	// reimplemented (ibase::ICommandsProvider)
	const ibase::IHierarchicalCommand* GetCommands() const override;

private:
	I_REF(ibase::ICommandsProvider, m_slaveCommandsProviderCompPtr);
};


} // namespace imtgui


