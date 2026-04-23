// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CAttachmentBinaryDataProviderComp.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QUuid>


namespace imtchat
{


// reimplemented (imtrest::IBinaryDataProvider)

bool CAttachmentBinaryDataProviderComp::GetData(
			QByteArray& data,
			const QByteArray& dataId,
			qint64 readFromPosition,
			qint64 readMaxLength) const
{
	if (!m_attachmentStorageCompPtr.IsValid()){
		return false;
	}

	QByteArray attachmentId = StripExtension(dataId);

	QString fileName;
	QString mimeType;
	if (!m_attachmentStorageCompPtr->GetAttachment(attachmentId, data, fileName, mimeType)){
		return false;
	}

	if (readFromPosition > 0){
		data = data.mid(static_cast<int>(readFromPosition));
	}

	if (readMaxLength >= 0 && data.size() > readMaxLength){
		data = data.left(static_cast<int>(readMaxLength));
	}

	return true;
}


// reimplemented (imtrest::IBinaryDataController)

bool CAttachmentBinaryDataProviderComp::SetData(const QByteArray& data, QByteArray& dataId) const
{
	if (!m_attachmentStorageCompPtr.IsValid()){
		return false;
	}

	if (data.isEmpty()){
		return false;
	}

	QString fileName = QFileInfo(QString::fromUtf8(dataId)).fileName();
	if (fileName.isEmpty()){
		fileName = "attachment";
	}

	QString mimeType = DetectMimeType(fileName);

	QByteArray storedId = m_attachmentStorageCompPtr->StoreAttachment(data, fileName, mimeType);
	if (storedId.isEmpty()){
		return false;
	}

	// Append the file extension so that the GET endpoint returns the correct MIME type
	QString ext = QFileInfo(fileName).suffix().toLower();
	if (!ext.isEmpty()){
		dataId = storedId + "." + ext.toUtf8();
	}
	else{
		dataId = storedId;
	}

	return true;
}


bool CAttachmentBinaryDataProviderComp::RemoveData(const QByteArray& dataId) const
{
	if (!m_attachmentStorageCompPtr.IsValid()){
		return false;
	}

	QByteArray attachmentId = StripExtension(dataId);
	return m_attachmentStorageCompPtr->DeleteAttachment(attachmentId);
}


bool CAttachmentBinaryDataProviderComp::EnsureDataExists(const QByteArray& dataId) const
{
	if (!m_attachmentStorageCompPtr.IsValid()){
		return false;
	}

	QByteArray attachmentId = StripExtension(dataId);
	QByteArray data;
	QString fileName;
	QString mimeType;
	return m_attachmentStorageCompPtr->GetAttachment(attachmentId, data, fileName, mimeType);
}


// private methods

QByteArray CAttachmentBinaryDataProviderComp::StripExtension(const QByteArray& dataId)
{
	int dotIdx = dataId.lastIndexOf('.');
	if (dotIdx > 0){
		QByteArray uuidPart = dataId.left(dotIdx);
		QUuid uuid = QUuid::fromString(QString::fromUtf8(uuidPart));
		if (!uuid.isNull()){
			return uuidPart;
		}
	}

	return dataId;
}


QString CAttachmentBinaryDataProviderComp::DetectMimeType(const QString& fileName)
{
	QString ext = QFileInfo(fileName).suffix().toLower();

	if (ext == "png") return "image/png";
	if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
	if (ext == "gif") return "image/gif";
	if (ext == "bmp") return "image/bmp";
	if (ext == "svg") return "image/svg+xml";
	if (ext == "webp") return "image/webp";
	if (ext == "tiff" || ext == "tif") return "image/tiff";
	if (ext == "ico") return "image/x-icon";
	if (ext == "pdf") return "application/pdf";

	return "application/octet-stream";
}


} // namespace imtchat
