#pragma once


// ImtCore Includes
#include <imtbase/ICollectionInfo.h>
#include <imtloggui/IRepresentationFactoryProvider.h>


namespace imtloggui
{


class ILayerProvider: virtual public imtbase::ICollectionInfo
{
public:
	enum ElementInfoType
	{
		EIT_MIN_TIME_SPAN = EIT_USER
	};

	virtual IRepresentationFactoryProvider* GetRepresentationFactoryProvider(const QByteArray& id) const = 0;
	virtual QList<int> GetMessageIdList() const = 0;
	virtual QByteArray GetIdForTimeSpan(uint64_t msecs) const = 0;
};


} // namespace imtloggui


