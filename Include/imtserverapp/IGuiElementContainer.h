#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtserverapp
{


class IGuiElementModel;


class IGuiElementContainer: virtual public istd::IChangeable
{
public:
	virtual QByteArrayList GetElementIds() const = 0;
	virtual const IGuiElementModel* GetGuiElementModel(const QByteArray& elementId) const = 0;
};


} // namespace imtserverapp


