// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


