#include <imtgql/Test/CGqlSdlRequestTest.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <ipackage/CComponentAccessor.h>

// ImtCore includes
#include <imtgql/IGqlRequestHandler.h>


namespace imtgql
{


// protected methods

ipackage::CComponentAccessor* CGqlSdlRequestTest::GetComponentAccessor() const
{
	if (!m_componentAccessorPtr.IsValid()){
		m_componentAccessorPtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	}

	return m_componentAccessorPtr.GetPtr();
}


const imtgql::IGqlRequestHandler* CGqlSdlRequestTest::GetRequestHandler() const
{
	ipackage::CComponentAccessor* componentAccessorPtr = GetComponentAccessor();
	if (componentAccessorPtr == nullptr){
		return nullptr;
	}

	return componentAccessorPtr->GetComponentInterface<imtgql::IGqlRequestHandler>();
}


QSqlQuery CGqlSdlRequestTest::ExecuteQuery(const QString& query, QSqlError* sqlError) const
{
	ipackage::CComponentAccessor* accessorPtr = GetComponentAccessor();
	if (accessorPtr != nullptr){
		imtdb::IDatabaseEngine* databaseEnginePtr = accessorPtr->GetComponentInterface<imtdb::IDatabaseEngine>();
		if (databaseEnginePtr != nullptr){
			return databaseEnginePtr->ExecSqlQuery(query.toUtf8(), sqlError);
		}
	}

	return QSqlQuery();
}


}
