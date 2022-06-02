#include <imttest/CTestCollection.h>


namespace imttest
{


// public methods

CTestCollection::CTestCollection()
	:BaseClass("TestInfo", "Test Info", "Tests")
{
}


// reimplemented (imtbase::ITestInfoProvider)

const imtbase::ICollectionInfo& CTestCollection::GetTestList() const
{
	return m_collection;
}


const imttest::ITestInfo* CTestCollection::GetTest(const QByteArray& instanceId) const
{
	return dynamic_cast<const imttest::ITestInfo*>(m_collection.GetObjectPtr(instanceId));
}


} // namespace imttest


