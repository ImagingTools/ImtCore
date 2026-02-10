// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imtgui/ILayoutFittableWidget.h>
#include <imod/IModelEditor.h>
#include <imod/IObserver.h>


namespace imtgui
{


class ILayoutFittableModelEditor:
			public virtual ILayoutFittableWidget,
			public virtual imod::IModelEditor,
			public virtual imod::IObserver
{
};


} // namespace imtgui



