#pragma once


// ImtCore Includes
#include <imtbase/ICollectionInfo.h>
#include <imtloggui/IRepresentationView.h>


namespace imtloggui
{


class IRepresentationViewProvider: virtual public imtbase::ICollectionInfo
{
public:
	virtual IRepresentationView* GetRepresentationView(const QByteArray& id) const = 0;
};


} // namespace imtloggui


