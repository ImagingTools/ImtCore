#include <imtdb/CStandardSqlTableDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/TParamsPtr.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtdb
{


// reimplemented (imtdb::ISqlDatabaseTableDelegate)

QByteArrayList CStandardSqlTableDelegateComp::GetColumnIds() const
{
	QByteArrayList retVal;

	retVal << "Id" << "Name" << "Description" << "LastModified" << "Added" << "Checksum";

	return QByteArrayList();
}


imtdb::ISqlDatabaseTableDelegate::ColumnDataType CStandardSqlTableDelegateComp::GetColumnType(const QByteArray & columnId)
{
	return ColumnDataType();
}


int CStandardSqlTableDelegateComp::GetMetaInfoType(const QByteArray & columnId)
{
	return 0;
}


idoc::IDocumentMetaInfo* CStandardSqlTableDelegateComp::CreateItemItemInfo() const
{
	return nullptr;
}


} // namespace imtdb


