// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CAttachmentComp.h>

// ACF includes
#include <iser/IArchive.h>


namespace imtchat
{


// reimplemented (imtchat::IAttachment)

QByteArray CAttachmentComp::GetId() const
{
	return m_id;
}

void CAttachmentComp::SetId(const QByteArray& id)
{
	m_id = id;
}

QString CAttachmentComp::GetFileName() const
{
	return m_fileName;
}

void CAttachmentComp::SetFileName(const QString& fileName)
{
	m_fileName = fileName;
}

QString CAttachmentComp::GetMimeType() const
{
	return m_mimeType;
}

void CAttachmentComp::SetMimeType(const QString& mimeType)
{
	m_mimeType = mimeType;
}

QByteArray CAttachmentComp::GetData() const
{
	return m_data;
}

void CAttachmentComp::SetData(const QByteArray& data)
{
	m_data = data;
}

qint64 CAttachmentComp::GetFileSize() const
{
	return m_fileSize;
}

void CAttachmentComp::SetFileSize(qint64 fileSize)
{
	m_fileSize = fileSize;
}

QString CAttachmentComp::GetCreatedAt() const
{
	return m_createdAt;
}

void CAttachmentComp::SetCreatedAt(const QString& createdAt)
{
	m_createdAt = createdAt;
}


// reimplemented (iser::ISerializable)

bool CAttachmentComp::Serialize(iser::IArchive& archive)
{
	bool retVal = true;
	retVal = retVal && archive.Process("Id", m_id);
	retVal = retVal && archive.Process("FileName", m_fileName);
	retVal = retVal && archive.Process("MimeType", m_mimeType);
	retVal = retVal && archive.Process("Data", m_data);
	retVal = retVal && archive.Process("FileSize", m_fileSize);
	retVal = retVal && archive.Process("CreatedAt", m_createdAt);
	return retVal;
}


// reimplemented (istd::IChangeable)

bool CAttachmentComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IAttachment* sourcePtr = dynamic_cast<const IAttachment*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	m_id = sourcePtr->GetId();
	m_fileName = sourcePtr->GetFileName();
	m_mimeType = sourcePtr->GetMimeType();
	m_data = sourcePtr->GetData();
	m_fileSize = sourcePtr->GetFileSize();
	m_createdAt = sourcePtr->GetCreatedAt();

	return true;
}

bool CAttachmentComp::IsEqual(const IChangeable& object) const
{
	const IAttachment* otherPtr = dynamic_cast<const IAttachment*>(&object);
	if (otherPtr == nullptr){
		return false;
	}

	return m_id == otherPtr->GetId()
			&& m_fileName == otherPtr->GetFileName()
			&& m_mimeType == otherPtr->GetMimeType()
			&& m_data == otherPtr->GetData()
			&& m_fileSize == otherPtr->GetFileSize()
			&& m_createdAt == otherPtr->GetCreatedAt();
}

istd::IChangeableUniquePtr CAttachmentComp::CloneMe(CompatibilityMode mode) const
{
	Q_UNUSED(mode);
	return nullptr;
}

bool CAttachmentComp::ResetData(CompatibilityMode /*mode*/)
{
	m_id.clear();
	m_fileName.clear();
	m_mimeType.clear();
	m_data.clear();
	m_fileSize = 0;
	m_createdAt.clear();
	return true;
}


} // namespace imtchat
