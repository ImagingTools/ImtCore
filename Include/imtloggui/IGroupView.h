#pragma once


// ImtCore Includes
#include <imtbase/ICollectionInfo.h>
#include <imtloggui/ILayerViewProvider.h>


namespace imtloggui
{


class IGroupViewProvider: virtual public imtbase::ICollectionInfo
{
public:
	enum ElementInfoType
	{
		EIT_COLOR = imtbase::ICollectionInfo::EIT_USER
	};

	virtual ILayerViewProvider* GetLayerViewProvider(const QByteArray& id) const = 0;
};


} // namespace imtloggui


