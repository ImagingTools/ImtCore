#pragma once


// ImtCore Includes
#include <imtbase/ICollectionInfo.h>
#include <imtloggui/IGroup.h>


namespace imtloggui
{


class IGroupProvider: virtual public imtbase::ICollectionInfo
{
public:
	virtual IGroup* GetGroup(const QByteArray& id) const = 0;
};


} // namespace imtloggui


