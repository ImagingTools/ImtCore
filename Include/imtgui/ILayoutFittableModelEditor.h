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



