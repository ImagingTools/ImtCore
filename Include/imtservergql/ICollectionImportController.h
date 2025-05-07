#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TRange.h>


namespace imtservergql
{


class ICollectionImportController: virtual public istd::IPolymorphic
{
public:
	struct FileInfo
	{
		QByteArray id;
		QString name;
		int size;
		QByteArray objectTypeId;
	};

	struct SessionInfo
	{
		QByteArray sessionId;
		QByteArray collectionId;
		std::vector<FileInfo> files;
	};

	virtual bool BeginCollectionImportSession(const SessionInfo& sessionInfo, QString& errorMessage) = 0;
	virtual bool CancelCollectionImportSession(const QByteArray& sessionId, QString& errorMessage) = 0;
};


} // namespace imtservergql


