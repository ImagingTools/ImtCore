#pragma once


// ImtCore Includes
#include <imtbase/ICollectionInfo.h>
#include <imtloggui/IRepresentationFactoryProvider.h>


namespace imtloggui
{


class ILayerProvider: virtual public imtbase::ICollectionInfo
{
public:
	virtual IRepresentationFactoryProvider* GetRepresentationFactoryProvider(const QByteArray& id) const = 0;
};


} // namespace imtloggui


