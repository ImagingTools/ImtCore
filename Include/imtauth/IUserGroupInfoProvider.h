#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtauth
{


class IUserGroupInfo;


class IUserGroupInfoProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetUserGroupList() const = 0;
	virtual const imtauth::IUserGroupInfo* GetUserGroup(const QByteArray& groupId) const = 0;
};


} // namespace imtauth


