#include <imtauth/CAccountCollection.h>


namespace imtauth
{


// public methods

CAccountCollection::CAccountCollection()
	:BaseClass(CAccountInfo::GetTypeId(), "Account", "Accounts")
{
}


// reimplemented (IAccountInfoProvider)

const imtbase::ICollectionInfo& CAccountCollection::GetAccountList() const
{
	return m_collection;
}


const IAccountInfo* CAccountCollection::GetAccount(const QByteArray& accountId) const
{
	return dynamic_cast<const IAccountInfo*>(m_collection.GetObjectPtr(accountId));
}


} // namespace imtauth


