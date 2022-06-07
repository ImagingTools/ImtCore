#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


class QByteArray;


namespace imtgui
{


class IPopupEventHandler: virtual public istd::IPolymorphic
{
public:
	virtual void OnPopupClosed(const QByteArray& popupId) = 0;
};


} // namespace imtgui


