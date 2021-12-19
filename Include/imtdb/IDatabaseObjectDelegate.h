#pragma once


// Qt includes
#include <QtSql/QtSql>

// ACF includes
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
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
		Get selection query for the given object-ID.
	*/
	virtual QByteArray GetSelectionQueryForObject(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr) const = 0;

	/**
		Get ID of the object in the database from the SQL record.
	*/
	virtual QByteArray GetObjectIdFromRecord(const QByteArray& typeId, const QSqlRecord& record) const = 0;

	/**
		Create object meta-informations based on the SQL record.
	*/
	virtual bool CreateObjectInfoFromRecord(
				const QByteArray& typeId,
				const QSqlRecord& record,
				imtbase::IMetaInfoCreator::MetaInfoPtr& objectMetaInfoPtr,
				imtbase::IMetaInfoCreator::MetaInfoPtr& collectionItemMetaInfoPtr) const = 0;

	/**
		Create a data object for the given SQL record.
	*/
	virtual istd::IChangeable* CreateObjectFromRecord(
				const QByteArray& typeId,
				const QSqlRecord& record) const = 0;

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

	/**
	*	Create SQL query for the update of the object.
	*/
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object) const = 0;

	/**
	*	Create SQL query for the renaming of the object.
	*/
	virtual QByteArray CreateRenameObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& newObjectName) const = 0;

	/**
	*	Create SQL query for the settings description of the object.
	*/
	virtual QByteArray CreateDescriptionObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& description) const = 0;
};


} // namespace imtdb


