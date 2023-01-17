#pragma once


// ImtCore includes
#include <imtgui/IGuiElementModel.h>


namespace imtgui
{


class IGuiElementContainer: virtual public istd::IChangeable
{
public:
	virtual QByteArrayList GetElementIds() const = 0;
	virtual const imtgui::IGuiElementModel* GetGuiElementModel(const QByteArray& elementId) const = 0;
};


} // namespace imtgui


