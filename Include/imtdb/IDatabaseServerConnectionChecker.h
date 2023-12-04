#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{


class IDatabaseServerConnectionChecker: virtual public istd::IPolymorphic
{
public:
	/**
		Check if the database server is connected.
	*/
	virtual bool CheckDatabaseConnection(QString& errorMessage) const = 0;
};


} // namespace imtdb


