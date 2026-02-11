// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
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
		QByteArray userId;
		std::vector<FileInfo> files;
		iprm::IParamsSetSharedPtr additionalParams;
	};

	virtual bool BeginCollectionImportSession(const SessionInfo& sessionInfo, QString& errorMessage) = 0;
	virtual bool CancelCollectionImportSession(const QByteArray& sessionId, QString& errorMessage) = 0;
};


} // namespace imtservergql


