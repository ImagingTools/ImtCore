#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcol
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

	struct TransactionInfo
	{
		QByteArray transactionId;
		QByteArray collectionId;
		std::vector<FileInfo> files;
	};

	virtual bool BeginCollectionImportTransaction(const TransactionInfo& transactionInfo, QString& errorMessage) = 0;
	virtual bool CancelCollectionImportTransaction(const QByteArray& transactionId, QString& errorMessage) = 0;
};


} // namespace imtcol


