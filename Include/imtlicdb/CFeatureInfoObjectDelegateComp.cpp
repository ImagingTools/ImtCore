#include <imtlicdb/CFeatureInfoObjectDelegateComp.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::IDatabaseObjectDelegate)

istd::IChangeable* CFeatureInfoObjectDelegateComp::CreateObjectFromRecord(const QByteArray& typeId, const QSqlRecord& record) const
{
	return nullptr;
}


} // namespace imtlicdb


