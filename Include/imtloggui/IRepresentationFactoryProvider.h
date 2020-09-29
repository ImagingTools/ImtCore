#pragma once


// ImtCore Includes
#include <imtbase/ICollectionInfo.h>
#include <imtloggui/IRepresentationFactory.h>


namespace imtloggui
{


class IRepresentationFactoryProvider: virtual public imtbase::ICollectionInfo
{
public:
	virtual IRepresentationFactory* GetRepresentationFactory(const QByteArray& id) const = 0;
};


} // namespace imtloggui


