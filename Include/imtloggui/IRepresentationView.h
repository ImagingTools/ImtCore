#pragma once


// ACF includes
#include <imod/IObserver.h>

// ImtCore includes
#include <imtloggui/IGraphicsItemProvider.h>


namespace imtloggui
{


class IRepresentationView: virtual public IGraphicsItemProvider
{
public:
	virtual imod::IObserver* GetRepresentationObserver() = 0;
};


} // namespace imtloggui


