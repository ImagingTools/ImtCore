#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqtgui/IVisualStatus.h>


namespace imtgui
{


class CVisualStatusComp: public ilog::CLoggerComponentBase, virtual public iqtgui::IVisualStatus
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CVisualStatusComp);
		I_REGISTER_INTERFACE(iqtgui::IVisualStatus);
		I_ASSIGN(m_statusIconAttrPtr, "StatusIcon", "Provide visual information for status icon", true, "StatusIcon");
		I_ASSIGN(m_statusTextAttrPtr, "StatusText", "Provide visual information for status text", true, "StatusText");
	I_END_COMPONENT;

	// reimplemented (iqtgui::IVisualStatus)
	virtual QIcon GetStatusIcon() const override;
	virtual QString GetStatusText() const override;

protected:
	I_ATTR(QString, m_statusIconAttrPtr);
	I_ATTR(QString, m_statusTextAttrPtr);
};


} // namespace imtgui


