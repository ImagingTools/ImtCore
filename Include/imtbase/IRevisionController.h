#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


class IObjectCollection;


class IRevisionController: virtual public istd::IPolymorphic
{
public:
	struct RevisionInfo
	{
		int revision;
		QDateTime timestamp;
		QString user;
		QString comment;
	};
	
	typedef QVector<RevisionInfo> RevisionInfoList;

	/**
		Get list of revisions for a given data object in the collection.
	*/
	virtual RevisionInfoList GetRevisionInfoList(const imtbase::IObjectCollection& collection, const QByteArray& objectId) const = 0;

	/**
		Restore the data of an object in the collection from a given revision.
	*/
	virtual bool RestoreObject(const imtbase::IObjectCollection& collection const QByteArray& objectId, int revision) const = 0;

	/**
		\return Revision of the saved object of the operation was successfull, or a negative value otherwise.
	*/
	virtual int BackupObject(const imtbase::IObjectCollection& collection, const QByteArray& objectId, const QString& userComment = QString())  const = 0;
};


} // namespace imtbase


