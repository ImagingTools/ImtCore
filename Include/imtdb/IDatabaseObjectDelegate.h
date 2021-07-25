#pragma once


// Qt includes
#include <QtSql/QtSql>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{


/**
	Common interface for a database object delegate.
	The delegate is responsible for the binding of the database layer to the object representation in the C++ data layer (ORM).
*/
class IDatabaseObjectDelegate: virtual public istd::IPolymorphic
{
public:
	/**
		Create a data object for the given SQL record.
	*/
	virtual istd::IChangeable* CreateObjectFromRecord(const QByteArray& typeId, const QSqlRecord& record) const = 0;
};


} // namespace imtdb


