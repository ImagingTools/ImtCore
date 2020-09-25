#pragma once


// ACF Includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class ILayer: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetId() const = 0;
	virtual QString GetName() const = 0;
	virtual QString GetDescription() const = 0;
	virtual quint64 GetMinimumVisualTimeSpan() const = 0;
	virtual quint64 GetMaximumVisualTimeSpan() const = 0;
};


} // namespace imtloggui


