#pragma once


// ACF Includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IGroup: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetId() const = 0;
	virtual QString GetName() const = 0;
	virtual int GetHeight() const = 0;
	virtual QColor GetColor() const = 0;
	virtual QVector<int> GetSupportedMessageIds() const = 0;
};


} // namespace imtloggui


