// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CAttachmentStorageComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>


namespace imtchat
{


// reimplemented (imtchat::IAttachmentStorage)

QByteArray CAttachmentStorageComp::StoreAttachment(
		const QByteArray& data,
		const QString& fileName,
		const QString& mimeType)
{
	if (!m_attachmentCollectionCompPtr.IsValid() || !m_attachmentFactCompPtr.IsValid()){
		return QByteArray();
	}

	istd::TUniqueInterfacePtr<IAttachment> attachPtr = m_attachmentFactCompPtr.CreateInstance();
	if (!attachPtr.IsValid()){
		return QByteArray();
	}

	QByteArray attachId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	attachPtr->SetId(attachId);
	attachPtr->SetFileName(fileName);
	attachPtr->SetMimeType(mimeType);
	attachPtr->SetData(data);
	attachPtr->SetFileSize(data.size());
	attachPtr->SetCreatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

	imtbase::IObjectCollection::OperationResult result =
			m_attachmentCollectionCompPtr->InsertObject("Attachment", attachId, fileName, QString(), attachPtr.GetPtr());
	if (result != imtbase::IObjectCollection::OR_OK){
		return QByteArray();
	}

	return attachId;
}


bool CAttachmentStorageComp::GetAttachment(
		const QByteArray& attachmentId,
		QByteArray& data,
		QString& fileName,
		QString& mimeType) const
{
	if (!m_attachmentCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_attachmentCollectionCompPtr->GetObjectData(attachmentId, dataPtr)){
		return false;
	}

	const IAttachment* attachPtr = dynamic_cast<const IAttachment*>(dataPtr.GetPtr());
	if (attachPtr == nullptr){
		return false;
	}

	data = attachPtr->GetData();
	fileName = attachPtr->GetFileName();
	mimeType = attachPtr->GetMimeType();

	return true;
}


bool CAttachmentStorageComp::DeleteAttachment(const QByteArray& attachmentId)
{
	if (!m_attachmentCollectionCompPtr.IsValid()){
		return false;
	}

	return m_attachmentCollectionCompPtr->RemoveObjects(QByteArrayList() << attachmentId)
			== imtbase::IObjectCollection::OR_OK;
}


} // namespace imtchat
