// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CDocumentServiceComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>


namespace imtdoc
{


// protected methods

IDocumentService::OperationStatus CDocumentServiceComp::CloseDocumentInternal(
			const QByteArray& userId,
			const QByteArray& documentId)
{
	QByteArray objectId;

	{
		QMutexLocker locker(&m_mutex);
		OperationStatus validationStatus;
		if (!ValidateInputParams(userId, documentId, validationStatus)){
			return validationStatus;
		}

		const WorkingDocument& workingDocument = m_userDocuments[userId][documentId];
		objectId = workingDocument.objectId;
	}

	OperationStatus status = BaseClass::CloseDocumentInternal(userId, documentId);

	if (status != OS_OK){
		return status;
	}

	{
		QMutexLocker locker(&m_mutex);
		if (IsSingleCopyMode() && !objectId.isEmpty() && m_sharedDocuments.contains(objectId)){
			return status;
		}

		RemoveUndoManagerDocumentDirectory(documentId);
	}

	return status;
}


// private methods

void CDocumentServiceComp::RemoveUndoManagerDocumentDirectory(const QByteArray& documentId) const
{
	if (!m_undoManagerFolderCompPtr.IsValid() || documentId.isEmpty()){
		return;
	}

	const QString rootFolderPath = m_undoManagerFolderCompPtr->GetPath();
	if (rootFolderPath.isEmpty()){
		return;
	}

	QDir rootDirectory(rootFolderPath);
	if (!rootDirectory.exists()){
		return;
	}

	const QString documentDirectoryName(documentId);
	QStringList directoriesToRemove;
	QDirIterator it(rootFolderPath, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while (it.hasNext()){
		const QString directoryPath = it.next();
		if (QFileInfo(directoryPath).fileName() != documentDirectoryName){
			continue;
		}

		directoriesToRemove.push_back(directoryPath);
	}

	for (const QString& directoryPath : directoriesToRemove){
		QDir(directoryPath).removeRecursively();
	}
}


} // namespace imtdoc
