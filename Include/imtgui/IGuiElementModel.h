#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtgui
{


class IGuiElementModel: virtual public istd::IChangeable
{
public:
	virtual QByteArray GetElementId() const = 0;
	virtual QString GetElementName() const = 0;
	virtual QString GetElementDescription() const = 0;
	virtual QString GetElementItemPath() const = 0;
	virtual QString GetElementStatus() const = 0;
	virtual bool IsEnabled() const = 0;
	virtual bool IsVisible() const = 0;
	virtual const IGuiElementModel* GetSubElements() const = 0;
};


} // namespace imtgui


