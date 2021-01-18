#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IRepresentationViewChangeNotifier: virtual public istd::IPolymorphic

{
public:
	virtual bool OnRepresentationViewChanged(const QByteArray& representationId) = 0;
};


} // namespace imtloggui


