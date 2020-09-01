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
		I_ASSIGN(m_physicalWidthAttrPtr, "MonitorPhysicalWidth", "Monitor physical width", false, 0);
		I_ASSIGN(m_physicalHeightAttrPtr, "MonitorPhysicalHeight", "Monitor physical height", false, 0);
	I_END_COMPONENT;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(int, m_physicalWidthAttrPtr);
	I_ATTR(int, m_physicalHeightAttrPtr);
};


} // namespace imtgui


