#include <imtauth/CPersonCollection.h>


namespace imtauth
{


// public methods

CPersonCollection::CPersonCollection()
	:BaseClass(CPersonInfo::GetTypeId(), "Person", "Persons")
{
}


// reimplemented (IPersonInfoProvider)

const imtbase::ICollectionInfo& CPersonCollection::GetPersonList() const
{
	return m_collection;
}


const IPersonInfo* CPersonCollection::GetPerson(const QByteArray& personId) const
{
	return dynamic_cast<const IPersonInfo*>(m_collection.GetObjectPtr(personId));
}


} // namespace imtauth


