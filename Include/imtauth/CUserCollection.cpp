#include <imtauth/CUserCollection.h>


namespace imtauth
{


// public methods

CUserCollection::CUserCollection()
	:BaseClass("UserInfo", "User", "Users")
{
}


// reimplemented (IAccountInfoProvider)

const imtbase::ICollectionInfo& CUserCollection::GetUserList() const
{
	return m_collection;
}


const IUserInfo* CUserCollection::GetUser(const QByteArray& userId) const
{
	return dynamic_cast<const IUserInfo*>(m_collection.GetObjectPtr(userId));
}


} // namespace imtauth


