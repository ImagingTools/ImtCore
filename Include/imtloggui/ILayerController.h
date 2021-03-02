#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class ILayerController: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetActiveRepresentationId() const = 0;
	virtual bool SetActiveRepresentationId(const QByteArray& representationId) = 0;
};


} // namespace imtloggui


