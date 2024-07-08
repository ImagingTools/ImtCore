#pragma once


// ImtCore includes
#include <imtgui/IGuiElementModel.h>
#include <imtgui/IGuiElementContainer.h>


namespace imtbase
{


class ICommandGroup:
			virtual public imtgui::IGuiElementModel
{
public:
	virtual int GetAlignment() const = 0;
};


} // namespace imtbase


