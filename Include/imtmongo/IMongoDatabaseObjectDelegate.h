#pragma once

// mongocxx includes
#include <bsoncxx/json.hpp>
#include <bsoncxx/document/view_or_value.hpp>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtdb/IDatabaseObjectDelegate.h>


namespace imtmongo
{


/**
	Common interface for a Mongo database object delegate.
*/
class IMongoDatabaseObjectDelegate: virtual public istd::IPolymorphic
{
public:
	/**
		Get information about supported object types.
	*/
	virtual const iprm::IOptionsList* GetObjectTypeInfos() const = 0;

	/**
		Get type-ID of the object.
	*/
	virtual QByteArray GetObjectTypeId(const QByteArray& objectId) const = 0;

	/**
		Get query for getting of count of objects in the collection, optionaly filtered by some criteria.
	*/
	virtual bsoncxx::document::view_or_value GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const = 0;

	/**
		Get selection query for the listing objects in the collection.
		\param objectId		If non empty, the given object will be selected.
		\param offset		if positive, first N rows given by \c offset will be skipped from the result.
		\param count		if positive, only N rows given by \c count will be returned in the result object.
		\param paramsPtr	additional query parameters (e.g. filtering, searching, sorting and so on).
	*/
	virtual bsoncxx::document::view_or_value GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const = 0;

	/**
		Create query for insertion of a new object into the database.
	*/
	virtual bsoncxx::document::view_or_value CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr) const = 0;

	/**
		Create query for the deletion of the object from the database.
	*/
	virtual bsoncxx::document::view_or_value CreateDeleteObjectQuery(
				const QByteArray& objectId) const = 0;

	/**
		Create query for the update an existing object in the collection.
	*/
	virtual bsoncxx::document::view_or_value CreateUpdateObjectQuery(
				const QByteArray& objectId,
				const istd::IChangeable& object,
				bool useExternDelegate = true) const = 0;


	virtual bsoncxx::document::view_or_value CreateResetQuery(const imtbase::IObjectCollection& collection) const = 0;

	/**
		Create query for setting the meta-information for the given object in the database.
	*/
	virtual bsoncxx::document::view_or_value CreateDataMetaInfoQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr) const = 0;

	/**
		Create query for setting the collection-related meta-information for the given object in the database.
	*/
	virtual bsoncxx::document::view_or_value CreateCollectionItemMetaInfoQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr) const = 0;

	/**
		Get ID of the object in the database from the bson document .
	*/
	virtual QByteArray GetObjectIdFromRecord(const bsoncxx::document::view& doc) const = 0;

	/**
		Create object meta-informations based on the bson document.
	*/
	virtual bool CreateObjectInfoFromRecord(
				const bsoncxx::document::view& doc,
				idoc::MetaInfoPtr& objectMetaInfoPtr,
				idoc::MetaInfoPtr& collectionItemMetaInfoPtr) const = 0;

	/**
		Create a data object for the given bson document.
	*/
	virtual istd::IChangeable* CreateObjectFromRecord(const bsoncxx::document::view& doc) const = 0;

	/**
		Create object meta-information element based on the bson document.
	*/
	virtual QVariant GetElementInfoFromRecord(const bsoncxx::document::view& doc, const QString& infoId) const = 0;

};


} // namespace imtmongo


