// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtcache/ICacheTableBuilder.h>


namespace imtcache
{


/**
	Keeps a table that is a join of other cache tables, stored rather than recomputed for every query.

	The table is built by a select script. After that only the rows a change touched are recomputed:
	a second script names the keys of the rows affected by the entries the table builders put in
	CCacheChangeLog since this table last read it. Those rows are deleted and selected again, so a row
	whose source is gone disappears and an update never walks the whole table.

	The whole table is built only for the first population, when the script's columns changed, when
	a table it reads was rebuilt (the log cannot say which rows changed) and on an explicit full update.

	Select script: a SELECT of every column of the table. It contains ${KeyFilter}, a condition to put
	where the table that holds the key is read. It is TRUE for a full build, and limits the key to the
	affected ones otherwise.

	Affected keys script: a SELECT of one column, the keys (UBIGINT) of the rows to recompute, which may
	include keys of rows that no longer exist. ${LastChangeId} and ${LatestChangeId} bound the entries of
	CCacheChangeLog to read: ChangeId > ${LastChangeId} AND ChangeId <= ${LatestChangeId}.
*/
class CMaterializedTableBuilderComp:
			public ilog::CLoggerComponentBase,
			virtual public ICacheTableBuilder
{
public:
	using BaseClass = ilog::CLoggerComponentBase;

	I_BEGIN_COMPONENT(CMaterializedTableBuilderComp)
		I_REGISTER_INTERFACE(ICacheTableBuilder)
		I_ASSIGN(m_tableNameAttrPtr, "TableName", "Name of the cache table", true, "");
		I_ASSIGN(m_keyColumnAttrPtr, "KeyColumn", "Column of the table that identifies a row (UBIGINT)", true, "");
		I_ASSIGN(m_keyExpressionAttrPtr, "KeyExpression", "The key as the select script writes it, which ${KeyFilter} is tested against", true, "");
		I_ASSIGN(m_selectRowsScriptPathAttrPtr, "SelectRowsScriptPath", "QRC path of the SQL script selecting the rows of the table. ${KeyFilter} is substituted", true, "");
		I_ASSIGN(m_selectAffectedKeysScriptPathAttrPtr, "SelectAffectedKeysScriptPath", "QRC path of the SQL script selecting the keys of the rows to recompute. ${LastChangeId} and ${LatestChangeId} are substituted", true, "");
		I_ASSIGN_MULTI_0(m_requiredTablesAttrPtr, "RequiredTables", "Cache tables the rows are computed from; they are brought up to date first", false);
	I_END_COMPONENT;

	// reimplemented (imtcache::ICacheTableBuilder)
	virtual QString GetCacheTableName() const override;
	virtual QStringList GetRequiredCacheTables() const override;
	virtual BuildResult Rebuild(imtduckdb::IDuckConnection& connection) const override;
	virtual BuildResult ApplyChanges(imtduckdb::IDuckConnection& connection, const QDateTime& lastSourceUpdateTime) const override;

private:
	bool ReadScript(const QByteArray& path, QString& script, QString& errorMessage) const;

	/// The select script with ${KeyFilter} replaced by \a keyFilter.
	bool CreateSelectQuery(const QString& keyFilter, QString& query, QString& errorMessage) const;

	/// False when the select script no longer yields the columns of the table, in the order of the table.
	bool HasExpectedColumns(imtduckdb::IDuckConnection& connection) const;

	QString GetAffectedTableName() const;

private:
	I_ATTR(QByteArray, m_tableNameAttrPtr);
	I_ATTR(QByteArray, m_keyColumnAttrPtr);
	I_ATTR(QByteArray, m_keyExpressionAttrPtr);
	I_ATTR(QByteArray, m_selectRowsScriptPathAttrPtr);
	I_ATTR(QByteArray, m_selectAffectedKeysScriptPathAttrPtr);
	I_MULTIATTR(QByteArray, m_requiredTablesAttrPtr);
};


} // namespace imtcache
