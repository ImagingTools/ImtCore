#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrepo
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

	virtual bool IsRevisionHistoryEnabled() const = 0;
	virtual RevisionInfoList GetRevisionInfoList(const QByteArray& objectId) const = 0;
	virtual bool RestoreRevision(const QByteArray& objectId, int revision) = 0;
};


} // namespace imtrepo