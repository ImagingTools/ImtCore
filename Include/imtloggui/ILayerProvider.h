#pragma once


// ImtCore Includes
#include <imtbase/ICollectionInfo.h>
#include <imtloggui/ILayer.h>


namespace imtloggui
{


class ILayerProvider: virtual public imtbase::ICollectionInfo
{
public:
	virtual ILayer* GetLayer(const QByteArray& id) const = 0;
};


} // namespace imtloggui


