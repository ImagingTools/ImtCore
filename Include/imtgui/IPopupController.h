#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>


namespace imtgui
{


class IPopupController: virtual public ilog::IMessageConsumer
{
public:
	/**
		Adds a popup item with a timeout.
		\param	message			the message placed in the popup container.
		\param	widgetPtr		the widget placed in the popup container. Widget ownership goes to popup manager.
		\param	timeout			timeout value in msecs, a negative value requires user action to close the popup.
		\return					assigned popup ID.
	*/
	virtual QByteArray AddPopup(const MessagePtr& messagePtr, int timeout = -1, bool isClosingOnMouseClickAllowed = true, QWidget* contentWidgetPtr = nullptr) = 0;
	virtual bool ClosePopup(const QByteArray& popupId) = 0;
};


} // namespace imtgui


