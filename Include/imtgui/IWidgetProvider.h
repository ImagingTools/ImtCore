// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QWidget>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtgui
{


class IWidgetProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Get information about the list of widgets in the container.
	*/
	virtual const imtbase::ICollectionInfo* GetWidgetList() const = 0;

	/**
		Get widget from the container according to the given ID.
	*/
	virtual QWidget* GetWidgetPtr(const QByteArray& widgetId) const = 0;
};


} // namespace imtgui


