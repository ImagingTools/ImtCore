#pragma once


// Qt includes
#include <QtGui/QColor>

// ACF includes
#include <istd/IChangeable.h>


namespace imtloggui
{


class IGroupVisualSettings: virtual public istd::IChangeable
{
public:
	virtual QColor GetBackgroundColor() const = 0;
	virtual int GetHeight() const = 0;
};


} // namespace imtloggui


