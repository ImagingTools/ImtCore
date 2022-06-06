#pragma once


// ACF includes
#include <istd/ICHangeable.h>


class QByteArray;


namespace imtgui
{


class IPopupEventHandler: virtual public istd::IChangeable
{
public:
	virtual void OnPopupClosed(const QByteArray& popupId) = 0;
};


} // namespace imtgui


