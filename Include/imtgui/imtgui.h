#pragma once


// Qt includes
#include <QtWidgets/QWidget>

// ACF includes
#include <istd/TIFactory.h>


/**
	Basic UI-components
	This package is system independent.
*/
namespace imtgui
{


typedef istd::TIFactory<QWidget> IWidgetFactory;


} // namespace imtgui


