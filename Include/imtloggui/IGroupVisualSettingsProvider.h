#pragma once


// Qt includes
#include <QtGui/QColor>


namespace imtloggui
{


class IGroupVisualSettingsProvider: virtual public istd::IChangeable
{
public:
	virtual QColor GetBackgroundColor() const = 0;
	virtual int GetHeight() const = 0;
};


} // namespace imtloggui


