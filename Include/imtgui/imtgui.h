// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


