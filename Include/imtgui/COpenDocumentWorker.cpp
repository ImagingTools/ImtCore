// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgui/COpenDocumentWorker.h>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CSystem.h>

// ImtCore includes
#include <imtgui/CDocumentCollectionViewDelegateComp.h>


namespace imtgui
{

COpenDocumentWorker::COpenDocumentWorker(
			const CDocumentCollectionViewDelegateComp* parent,
			const QByteArray& objectId,
			const QByteArray& viewTypeId)
	:m_parent(parent),
	m_objectId(objectId),
	m_viewTypeId(viewTypeId)
{
}


void COpenDocumentWorker::Run()
{
	OpenDocumentResult result;
	result.objectId = m_objectId;
	result.viewTypeId = m_viewTypeId;

	if ((m_parent == nullptr) || (m_parent->m_collectionPtr == nullptr) || !m_parent->m_filePersistenceCompPtr.IsValid()){
		result.success = false;
		Q_EMIT Finished(result);
		return;
	}

	result.typeId = m_parent->m_collectionPtr->GetObjectTypeId(m_objectId);
	result.objectName = m_parent->m_collectionPtr->GetElementInfo(m_objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

	QStringList fileExtensions;
	if (!m_parent->m_filePersistenceCompPtr->GetFileExtensions(fileExtensions) || fileExtensions.isEmpty()){
		result.success = false;
		Q_EMIT Finished(result);
		return;
	}

	const QString fileExtension = fileExtensions[0];

	result.tempPath = QDir::tempPath() + "/ImtCore/" + QUuid::createUuid().toString();
	istd::CSystem::EnsurePathExists(result.tempPath);
	result.tempFilePath = result.tempPath + "/" + result.objectName + "." + fileExtension;

	imtbase::IObjectCollection::DataPtr documentDataPtr;
	if (!m_parent->m_collectionPtr->GetObjectData(m_objectId, documentDataPtr, &m_parent->m_loadFilterParamSet) || !documentDataPtr.IsValid()){
		result.success = false;
		Q_EMIT Finished(result);
		return;
	}

	const int state = m_parent->m_filePersistenceCompPtr->SaveToFile(*documentDataPtr, result.tempFilePath);
	result.success = (state == ifile::IFilePersistence::OS_OK);
	Q_EMIT Finished(result);
}


} // namespace imtgui


