#pragma once


// Qt includes
#include <QtSql/QtSql>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


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
	virtual istd::IChangeable* CreateObjectFromRecord(
				const QByteArray& typeId,
				const QSqlRecord& record,
				QString& objectName,
				QString& objectDescription) const = 0;

	/**
		Create SQL query for insertion of a new object into the database.
	*/
	virtual QByteArray CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr) const = 0;

	/**
	*	Create SQL query for the deletion of the object from the database.
	*/
	virtual QByteArray CreateDeleteObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId) const = 0;
};


} // namespace imtdb


