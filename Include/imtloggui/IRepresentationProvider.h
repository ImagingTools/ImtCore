#pragma once


// ImtCore Includes
#include <imtbase/ICollectionInfo.h>
#include <imtloggui/IRepresentation.h>


namespace imtloggui
{


class IRepresentationProvider: virtual public imtbase::ICollectionInfo
{
public:
	virtual IRepresentation* GetRepresentation(const QByteArray& id) const = 0;
};


} // namespace imtloggui


