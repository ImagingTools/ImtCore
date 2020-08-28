#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


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

	virtual RevisionInfoList GetRevisionInfoList(const QByteArray& objectId) const = 0;
	virtual bool RestoreObject(const QByteArray& objectId, int revision) const = 0;
	virtual bool BackupObject(const QByteArray& objectId, const QString& userComment = QString())  const = 0;
};


} // namespace imtbase


