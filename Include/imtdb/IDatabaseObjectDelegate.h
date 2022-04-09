#pragma once


// ACF includes
#include <iprm/IParamsSet.h>

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
	struct NewObjectQuery
	{
		/**
			Resulting query for object insertion.
		*/
		QByteArray query;

		/**
			Name of the object.
		*/
		QString objectName;
	};


	/**
		Get query for getting of count of objects in the collection, optionaly filtered by some criteria.
	*/
	virtual QByteArray GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const = 0;

	/**
		Get selection query for the listing objects in the collection.
		\param objectId		If non empty, the given object will be selected.
		\param offset		if positive, first N rows given by \c offset will be skipped from the result.
		\param count		if positive, only N rows given by \c count will be returned in the result object.
		\param paramsPtr	additional query parameters (e.g. filtering, searching, sorting and so on).
	*/
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const = 0;

	/**
		Create query for insertion of a new object into the database.
	*/
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr) const = 0;

	/**
	*	Create query for the deletion of the object from the database.
	*/
	virtual QByteArray CreateDeleteObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId) const = 0;

	/**
	*	Create query for the update of the object.
	*/
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object) const = 0;

	/**
	*	Create query for the renaming of the object.
	*/
	virtual QByteArray CreateRenameObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& newObjectName) const = 0;

	/**
	*	Create query for the settings description of the object.
	*/
	virtual QByteArray CreateDescriptionObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& description) const = 0;
};


} // namespace imtdb


