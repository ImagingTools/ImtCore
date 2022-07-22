#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


class IObjectCollection;


/**
	Common interface for the services around the objects in the collection.
	\ingroup Collection
*/
class ICollectionDataController: virtual public istd::IPolymorphic
{
public:
	/**
		Get the file persistence for a given object type.
	*/
	virtual const ifile::IFilePersistence* GetPersistenceForObjectType(const QByteArray& typeId) const = 0;

	/**
		Export a file from an object collection.
		\param collection	Collection containing the object to be exported.
		\param objectId		ID of the object to be exported from a collection.
		\param targetPath	Path to the exported file.
		\return \c true if the file object was successfully exported or \c false otherwise.
	*/
	virtual bool ExportFile(
				const imtbase::IObjectCollection& collection,
				const ICollectionInfo::Id& objectId,
				const QString& targetFilePath = QString()) const = 0;

	/**
		Import an object in the collection from a file.
		\param collection	Collection instance where the will be imported.
		\param typeId		Object type ID.
		\param sourcePath	Path to the imported file.
		\return \c true if the file object was successfully imported or \c false otherwise.
	*/
	virtual ICollectionInfo::Id ImportFile(
				imtbase::IObjectCollection& collection,
				const QByteArray& typeId,
				const QString& sourceFilePath = QString(),
				const ICollectionInfo::Id& parentId = ICollectionInfo::Id()) const = 0;
};


} // namespace imtbase


