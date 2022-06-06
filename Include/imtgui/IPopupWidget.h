#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>
#include <istd/TIFactory.h>


namespace imtgui
{


class IPopupWidget: virtual public istd::IPolymorphic
{
public:
	virtual const ilog::IMessageConsumer::MessagePtr& GetMessagePtr() const = 0;
	virtual void SetContent(const ilog::IMessageConsumer::MessagePtr& messagePtr, QWidget* contentWidgetPtr = nullptr) = 0;
	virtual void AllowClosingOnMouseClick(bool isAllowed = true) = 0;
};


typedef istd::TIFactory<IPopupWidget> IPopupWidgetFactory;


} // namespace imtgui


