#pragma once


// ACF includes
#include <iprm/IOptionsList.h>
#include <iqtgui/IMultiVisualStatusProvider.h>


namespace imtgui
{


class IGuiElementContainer: virtual public iprm::IOptionsList, virtual public iqtgui::IMultiVisualStatusProvider
{
public:
	virtual const IGuiElementContainer* GetElementCommands(const QByteArray& elementId) const = 0;
};


} // namespace imtgui


