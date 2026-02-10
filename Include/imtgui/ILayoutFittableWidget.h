// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


//Qt includes
#include <QtWidgets/QGridLayout>

// ACF includes
#include <istd/IPolymorphic.h>



namespace imtgui
{


class ILayoutFittableWidget: public virtual istd::IPolymorphic
{

public:
	virtual bool PlaceWidgetToGridLayout(QGridLayout& layout, int rowIndex = -1, int columnIndex = 1) = 0;

};


} // namespace imtgui



