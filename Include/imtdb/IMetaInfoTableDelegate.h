#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace imtdb
{


/**
	Common interface for a SQL-table delegate.
	The delegate is responsible for description of a table in the SQL database.
*/
class IMetaInfoTableDelegate: virtual public istd::IPolymorphic
{
public:
	enum ColumnDataType
	{
		CDI_UNKNOWN,
		CDT_TEXT,
		CDI_ID,
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
	virtual ColumnDataType GetColumnType(const QByteArray& columnId) const = 0;
	virtual int GetMetaInfoType(const QByteArray& columnId) const = 0;
	virtual idoc::MetaInfoPtr CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId) const = 0;
};


} // namespace imtdb


