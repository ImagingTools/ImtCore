#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtdb
{


class ISqlDatabaseObjectCollection: virtual public imtbase::IObjectCollection
{
public:
	/**
		Get the unique identifier of the database that the collection belongs to
	*/
	virtual QByteArray GetDatabaseId() const = 0;

	/**
		Get whether transactions are used to execute a SQL query
	*/
	virtual bool AreInternalTransactionsEnabled() const = 0;

	/**
		Set the use of transactions to execute sql query
	*/
	virtual bool SetInternalTransactionsEnabled(bool isEnabled) = 0;
};


} // namespace imtdb


