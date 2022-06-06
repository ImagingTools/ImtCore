#pragma once


// Qt includes
#include <QtCore/qnamespace.h>

// ACF includes
#include <istd/IChangeable.h>


namespace imtgui
{


class IClientAreaProvider: virtual public istd::IChangeable
{
public:
	virtual QPoint GetAnchor() const = 0;
	virtual QSize GetSize() const = 0;
};


} // namespace imtgui


