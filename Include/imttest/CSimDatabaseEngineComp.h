// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QStringList>
#include <QtCore/QVariantList>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>


namespace imttest
{


/**
	Database engine without a database server. Every query is recorded; a query that contains
	the text of a rule gets the rows of that rule, other queries get an empty result.
	The rows come from an in-memory SQLite connection, so the callers read a real QSqlQuery.
*/
class CSimDatabaseEngineComp:
			public icomp::CComponentBase,
			virtual public imtdb::IDatabaseEngine
{
public:
	using BaseClass = icomp::CComponentBase;

	I_BEGIN_COMPONENT(CSimDatabaseEngineComp);
		I_REGISTER_INTERFACE(imtdb::IDatabaseEngine);
	I_END_COMPONENT;

	/**
		\brief Answer the queries that contain \c queryPart with \c rows. The first matching rule wins.
		\param columnNames - names of the columns, as some callers read the values by name.
		\param rows - rows of the result, one value per column.
	*/
	void AddRule(const QString& queryPart, const QStringList& columnNames, const QList<QVariantList>& rows);

	// reimplemented (imtdb::IDatabaseEngine)
	virtual bool BeginTransaction() const override;
	virtual bool FinishTransaction() const override;
	virtual bool CancelTransaction() const override;
	virtual QByteArray GetDatabaseDriverId() const override;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QString& filePath, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QString& filePath, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;

	/// Queries executed so far, in their order; files are recorded by their path.
	mutable QStringList executedQueries;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	/**
		\brief Record the query and build its result from the first rule the query matches.
	*/
	QSqlQuery CreateResult(const QString& queryString) const;

private:
	struct Rule
	{
		QString queryPart;
		QStringList columnNames;
		QList<QVariantList> rows;
	};

	QList<Rule> m_rules;
	QString m_connectionName;
};


} // namespace imttest

