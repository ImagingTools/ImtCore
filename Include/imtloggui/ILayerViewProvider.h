#pragma once


// ImtCore Includes
#include <imtbase/ICollectionInfo.h>
#include <imtloggui/IRepresentationViewProvider.h>


namespace imtloggui
{


class ILayerViewProvider: virtual public imtbase::ICollectionInfo
{
public:
	virtual IRepresentationViewProvider* GetRepresentationViewProvider(const QByteArray& id) const = 0;
};


} // namespace imtloggui


