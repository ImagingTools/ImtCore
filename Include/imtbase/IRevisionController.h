#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


class IObjectCollection;


/**
	Common interface for management revisions of the objects and the whole object collection.
	\ingroup Collection
*/
class IRevisionController: virtual public istd::IPolymorphic
{
public:
	struct RevisionInfo
	{
		RevisionInfo()
			:revision(0),
			isRevisionAvailable(false)
		{
		}

		int revision;
		QDateTime timestamp;
		QString user;
		QString comment;
		bool isRevisionAvailable;
	};

	typedef QVector<RevisionInfo> RevisionInfoList;

	/**
		Get list of revisions available for restore for a given data object in the collection.
	*/
	virtual RevisionInfoList GetRevisionInfoList(
				const IObjectCollection& collection,
				const ICollectionInfo::Id& objectId) const = 0;

	/**
		Create backup of the given object in the collection.
		\return Revision of the saved object if the operation was successfull, or a negative value otherwise.
	*/
	virtual int BackupObject(
				const IObjectCollection& collection,
				const ICollectionInfo::Id& objectId,
				const QString& userComment = QString()) const = 0;

	/**
		Restore the data of an object in the collection from a given revision.
	*/
	virtual bool RestoreObject(
				IObjectCollection& collection,
				const ICollectionInfo::Id& objectId,
				int revision) const = 0;

	/**
		Exporting object data of a given revision from a collection to a file
	*/
	virtual bool ExportObject(
				const IObjectCollection& collection,
				const ICollectionInfo::Id& objectId,
				int revision,
				const QString& filePath) const = 0;
};


} // namespace imtbase


