#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtqml/IPageGuiElementModel.h>
#include <imtgui/CGuiElementModelComp.h>


namespace imtqml
{


class CPageGuiElementModelComp: virtual public imtqml::IPageGuiElementModel, public imtgui::CGuiElementModelComp
{
public:
	typedef imtgui::CGuiElementModelComp BaseClass;

	I_BEGIN_COMPONENT(CPageGuiElementModelComp);
		I_REGISTER_INTERFACE(imtqml::IPageGuiElementModel);
		I_ASSIGN(m_pageSourceItemAttrPtr, "PageQmlItem", "Path to the QML file item describing this page", false, "PageQmlItem");
		I_ASSIGN(m_startSourceItemAttrPtr, "StartQmlItem", "Path to the QML file start on this page", false, "StartQmlItem");
	I_END_COMPONENT;

	// reimplemented (imtqml::IPageElementContainer)
	virtual QString GetStartSourceItem() const override;
	virtual QString GetPageQmlItemFilePath() const override;

protected:
	I_TEXTATTR(m_pageSourceItemAttrPtr);
	I_TEXTATTR(m_startSourceItemAttrPtr);
};


} // namespace imtqml


