#pragma once


// ACF includes
#include <istd/TIFactory.h>
#include <iqtgui/IGuiObject.h>

// ImtCore includes
#include <imtbase/IPluginInfo.h>


namespace imthypegui
{


/**
	Interface describing a task-editor Plug-In.
*/
class ITaskEditorPlugin: virtual public imtbase::IPluginInfo
{
public:
	/**
		Get access to the task editor factory.
	*/
	virtual const istd::TIFactory<iqtgui::IGuiObject>* GetTaskEditorFactory() const = 0;
};


} // namespace imthypegui


