#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtcol
{


class IObjectTypeIdFilter: virtual public iser::ISerializable
{
public:
	virtual QByteArray GetObjectTypeId() const = 0;
	virtual void SetObjectTypeId(const QByteArray& typeId) = 0;
};


} // namespace imtcol


