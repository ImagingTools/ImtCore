#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtdb
{


class IDatabaseLoginSettings: virtual public istd::IChangeable
{
public:
	virtual QString GetHost() const = 0;
	virtual void SetHost(const QString& host) = 0;
	virtual int GetPort() const = 0;
	virtual void SetPort(int port) = 0;
	virtual QString GetDatabaseName() const = 0;
	virtual void SetDatabaseName(const QString& databaseName) = 0;
	virtual QString GetUserName() const = 0;
	virtual void SetUserName(const QString& userName) = 0;
	virtual QString GetPassword() const = 0;
	virtual void SetPassword(const QString& password) = 0;
};


} // namespace imtdb


