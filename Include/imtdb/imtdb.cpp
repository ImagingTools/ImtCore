#include <imtdb/imtdb.h>



namespace imtdb
{


QString SqlEncode(const QString& sqlQuery)
{
	QString retVal = sqlQuery;
	return retVal.replace("'", "''").replace(";", "\b");
}


} // namespace imtdb


