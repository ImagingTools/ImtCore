// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtcache/ICacheTableBuilder.h>


namespace imtcache
{


/**
	Keeps a view over cache tables, for data that is a join of other cache tables.

	A view stores nothing, so it is always as current as the tables it reads. Changing one row of an input
	changes every row of the view that joins it, with no rule for which ones and nothing to recompute, and a
	row of an input that disappears disappears from the view too. The cost is paid when the view is queried.

	The definition is recreated on every update, so a change to the script takes effect without a migration.
	Tables can be renamed or dropped underneath the view freely: it looks its tables up when it is queried.
*/
class CViewBuilderComp:
			public ilog::CLoggerComponentBase,
			virtual public ICacheTableBuilder
{
public:
	using BaseClass = ilog::CLoggerComponentBase;

	I_BEGIN_COMPONENT(CViewBuilderComp)
		I_REGISTER_INTERFACE(ICacheTableBuilder)
		I_ASSIGN(m_viewNameAttrPtr, "ViewName", "Name of the view", true, "");
		I_ASSIGN(m_createViewScriptPathAttrPtr, "CreateViewScriptPath", "QRC path of the SQL script creating the view, as CREATE OR REPLACE VIEW. ${ViewName} is substituted", true, "");
		I_ASSIGN_MULTI_0(m_requiredTablesAttrPtr, "RequiredTables", "Cache tables the view reads; they are brought up to date first", false);
	I_END_COMPONENT;

	// reimplemented (imtcache::ICacheTableBuilder)
	virtual QString GetCacheTableName() const override;
	virtual QStringList GetRequiredCacheTables() const override;
	virtual BuildResult Rebuild(imtduckdb::IDuckConnection& connection) const override;
	virtual BuildResult ApplyChanges(imtduckdb::IDuckConnection& connection, const QDateTime& lastSourceUpdateTime) const override;

private:
	I_ATTR(QByteArray, m_viewNameAttrPtr);
	I_ATTR(QByteArray, m_createViewScriptPathAttrPtr);
	I_MULTIATTR(QByteArray, m_requiredTablesAttrPtr);
};


} // namespace imtcache
