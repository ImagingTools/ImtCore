#pragma once


// ImtCore includes
#include <imtgui/CPopupController.h>


namespace imtgui
{


template<class GuiObject>
class TPopupControllerCompWrap: public GuiObject
{
public:
	typedef GuiObject BaseClass;

	I_BEGIN_COMPONENT(TPopupControllerCompWrap);
		I_REGISTER_SUBELEMENT(PopupController)
		I_REGISTER_SUBELEMENT_INTERFACE(PopupController, IPopupController, ExtractPopupController);
		I_REGISTER_SUBELEMENT_INTERFACE(PopupController, ilog::IMessageConsumer, ExtractPopupController);
		I_ASSIGN(m_popupWidgetFactoryCompPtr, "PopupWidgetFactory", "Widget factory for popup messages and content widgets", false, "StandardPopupWidgetFactory");
		I_ASSIGN_MULTI_0(m_popupEventHandlerCompPtr, "PopupEventHandler", "Popup event handlers", false);
		I_ASSIGN(m_popupAnchorAttrPtr, "PopupAnchorCorner", "0 - TopLeft\n1 - TopRight\n2 - BottomLeft\n3 - BottomRight", true, 3);
		I_ASSIGN(m_popupSpacingAttrPtr, "PopupSpacing", "Spacing between adjacent popups", true, 5);
		I_ASSIGN(m_popupDefaultTimeoutAttrPtr, "PopupDefaultTimeout", "Timeout value in msecs before the popup closes, a zero or negative value requires user action to close", true, 3000);
	I_END_COMPONENT;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiShown() override;
	virtual void OnGuiHidden() override;
	virtual void OnGuiDestroyed() override;

	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;

private:
	template <class InteraceType>
	static InteraceType* ExtractPopupController(TPopupControllerCompWrap<GuiObject>& parent)
	{
		return &parent.m_controller;
	}

private:
	I_REF(IPopupWidgetFactory, m_popupWidgetFactoryCompPtr);
	I_MULTIREF(IPopupEventHandler, m_popupEventHandlerCompPtr);
	I_ATTR(int, m_popupAnchorAttrPtr);
	I_ATTR(int, m_popupSpacingAttrPtr);
	I_ATTR(int, m_popupDefaultTimeoutAttrPtr);

	CPopupController m_controller;
};


template<class GuiObject>
void TPopupControllerCompWrap<GuiObject>::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	m_controller.SetParentWidget(BaseClass::GetWidget());
}


template<class GuiObject>
void TPopupControllerCompWrap<GuiObject>::OnGuiShown()
{
	BaseClass::OnGuiShown();

	m_controller.SetEnabled(true);
}


template<class GuiObject>
void TPopupControllerCompWrap<GuiObject>::OnGuiHidden()
{
	m_controller.SetEnabled(false);

	BaseClass::OnGuiHidden();
}


template<class GuiObject>
void TPopupControllerCompWrap<GuiObject>::OnGuiDestroyed()
{
	m_controller.SetParentWidget(nullptr);

	BaseClass::OnGuiDestroyed();
}


template<class GuiObject>
void TPopupControllerCompWrap<GuiObject>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_controller.SetAnchorCorner((Qt::Corner)*m_popupAnchorAttrPtr);
	m_controller.SetSpacing(*m_popupSpacingAttrPtr);
	m_controller.SetDefaultTimeOut(*m_popupDefaultTimeoutAttrPtr);
	m_controller.SetPopupWidgetFactory(m_popupWidgetFactoryCompPtr.GetPtr());
	for (int i = 0; i < m_popupEventHandlerCompPtr.GetCount(); i++){
		if (m_popupEventHandlerCompPtr[i] != nullptr){
			m_controller.AddPopupEvenHandler(m_popupEventHandlerCompPtr[i]);
		}
	}
}


} // namespace imtgui


