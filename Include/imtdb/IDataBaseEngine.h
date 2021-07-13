#pragma once

// Qt includes
#include <QtSql/QtSql>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{


/**
	Interfaces and basic implementations for general SQL database management.
	This package is system independent.
*/
class IDataBaseEngine: virtual public istd::IPolymorphic
{
public:
	virtual QString GetConnectionName() const = 0;
	virtual QSqlDatabase GetDatabase() const = 0;
	virtual bool OpenDataBase() = 0;
	virtual void CloseDataBase() = 0;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError = nullptr) const = 0;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError = nullptr) const = 0;
	virtual QSqlQuery ExecSqlQueryFromFile(const QByteArray& filePath, QSqlError* sqlError = nullptr) const = 0;
	virtual QSqlQuery ExecSqlQueryFromFile(const QByteArray& filePath, const QVariantMap& bindValues, QSqlError* sqlError = nullptr) const = 0;
};


} // namespace imtdb


