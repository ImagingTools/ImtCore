#pragma once


// ImtCore Includes
#include <imtbase/ICollectionInfo.h>
#include <imtloggui/ILayerProvider.h>


namespace imtloggui
{


class IGroupProvider: virtual public imtbase::ICollectionInfo
{
public:
	enum ElementInfoType
	{
		EIT_COLOR = imtbase::ICollectionInfo::EIT_USER
	};

	virtual ILayerProvider* GetLayerProvider(const QByteArray& id) const = 0;
};


} // namespace imtloggui


