#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TRange.h>
#include <iprm/IParamsSet.h>


namespace imtservergql
{


class ICollectionImportController: virtual public istd::IPolymorphic
{
public:
	struct FileInfo
	{
		QByteArray id;
		QString name;
		QString path; // Filled by CollectionImportController
		int size;
		QByteArray objectTypeId;
		iprm::IParamsSetSharedPtr additionalParams;
	};

	struct SessionInfo
	{
		QByteArray sessionId;
		QByteArray collectionId;
		std::vector<FileInfo> files;
		iprm::IParamsSetSharedPtr additionalParams;
	};

	virtual bool BeginCollectionImportSession(const SessionInfo& sessionInfo, QString& errorMessage) = 0;
	virtual bool CancelCollectionImportSession(const QByteArray& sessionId, QString& errorMessage) = 0;
};


} // namespace imtservergql


