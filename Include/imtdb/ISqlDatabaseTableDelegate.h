#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace imtdb
{


/**
	Common interface for a SQL-table delegate.
	The delegate is responsible for description of a table in the SQL database.
*/
class ISqlDatabaseTableDelegate: virtual public istd::IPolymorphic
{
public:
	enum ColumnDataType
	{
		CDT_TEXT,
		CDT_BINARY,
		CDT_JSON,
		CDT_XML,
		CDT_TIMESTAMP,
		CDT_DATE,
		CDT_INT32,
		CDT_INT64,
		CDT_REAL
	};

	virtual QByteArrayList GetColumnIds() const = 0;
	virtual ColumnDataType GetColumnType(const QByteArray& columnId) = 0;
	virtual int GetMetaInfoType(const QByteArray& columnId) = 0;
	virtual idoc::IDocumentMetaInfo* CreateItemItemInfo() const = 0; 
};


} // namespace imtdb


