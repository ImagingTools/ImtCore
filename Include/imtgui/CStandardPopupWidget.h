#pragma once


// ImtCore includes
#include <imtgui/CPopupWidgetBase.h>


namespace imtgui
{


class CStandardPopupWidget: public CPopupWidgetBase
{
public:
	// reimplemented (IPopupWidget)
	virtual void SetContent(const ilog::IMessageConsumer::MessagePtr& messagePtr, QWidget* contentWidgetPtr = nullptr) override;

protected:
	// reimplemented (QWidget)
	virtual void paintEvent(QPaintEvent* event) override;
};


} // namespace imtgui


