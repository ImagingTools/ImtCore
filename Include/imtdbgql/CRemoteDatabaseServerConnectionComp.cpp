#include <imtdbgql/CRemoteDatabaseServerConnectionComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtdbgql
{


// reimplemented (IDatabaseServerConnectionChecker)

bool CRemoteDatabaseServerConnectionComp::CheckDatabaseConnection(QString& errorMessage) const
{
	return false;
}


} // namespace imtdbgql


