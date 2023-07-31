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



