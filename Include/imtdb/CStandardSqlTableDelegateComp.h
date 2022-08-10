#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/ISqlDatabaseTableDelegate.h>


namespace imtdb
{


class CStandardSqlTableDelegateComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::ISqlDatabaseTableDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CStandardSqlTableDelegateComp)
		I_REGISTER_INTERFACE(imtdb::ISqlDatabaseTableDelegate);
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseTableDelegate)
	virtual QByteArrayList GetColumnIds() const override;
	virtual ColumnDataType GetColumnType(const QByteArray& columnId) override;
	virtual int GetMetaInfoType(const QByteArray& columnId) override;
	virtual idoc::IDocumentMetaInfo* CreateItemItemInfo() const override;

protected:

protected:
};


} // namespace imtdb


