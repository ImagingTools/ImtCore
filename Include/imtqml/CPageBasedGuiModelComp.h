#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtqml/IPageElementContainer.h>
#include <imtgui/CGuiElementModelComp.h>


namespace imtqml
{


class CPageBasedGuiModelComp: virtual public imtqml::IPageElementContainer, public imtgui::CGuiElementModelComp
{
public:
	typedef imtgui::CGuiElementModelComp BaseClass;

	I_BEGIN_COMPONENT(CPageBasedGuiModelComp);
		I_REGISTER_INTERFACE(imtqml::IPageElementContainer);
		I_ASSIGN_MULTI_0(m_pageSourceItemAttrPtr, "PageSourceItem", "Path of source item", true);
		I_ASSIGN_MULTI_0(m_startSourceItemAttrPtr, "StartSourceItem", "Start of source item", true);
	I_END_COMPONENT;

	// reimplemented (imtqml::IPageElementContainer)
	virtual QString GetStartSourceItem(const QByteArray& elementId) const override;
	virtual QString GetPageSourceItem(const QByteArray& elementId) const override;

protected:
	I_MULTITEXTATTR(m_pageSourceItemAttrPtr);
	I_MULTITEXTATTR(m_startSourceItemAttrPtr);
};


} // namespace imtqml


