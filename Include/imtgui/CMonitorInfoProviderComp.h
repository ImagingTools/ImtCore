#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtgui/CMonitorInfoProvider.h>


namespace imtgui
{


class CMonitorInfoProviderComp: public icomp::CComponentBase, public CMonitorInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CMonitorInfoProviderComp);
		I_REGISTER_INTERFACE(IMonitorInfoProvider);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT;
};


} // namespace imtgui


