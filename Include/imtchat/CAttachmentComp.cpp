// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CAttachmentComp.h>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtchat
{


// reimplemented (imtchat::IAttachment)

QByteArray CAttachmentComp::GetId() const
{
	return m_id;
}


void CAttachmentComp::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
	}
}


QString CAttachmentComp::GetFileName() const
{
	return m_fileName;
}


void CAttachmentComp::SetFileName(const QString& fileName)
{
	if (m_fileName != fileName){
		istd::CChangeNotifier notifier(this);

		m_fileName = fileName;
	}
}


QString CAttachmentComp::GetMimeType() const
{
	return m_mimeType;
}


void CAttachmentComp::SetMimeType(const QString& mimeType)
{
	if (m_mimeType != mimeType){
		istd::CChangeNotifier notifier(this);

		m_mimeType = mimeType;
	}
}


QByteArray CAttachmentComp::GetData() const
{
	return m_data;
}


void CAttachmentComp::SetData(const QByteArray& data)
{
	if (m_data != data){
		istd::CChangeNotifier notifier(this);

		m_data = data;
	}
}


qint64 CAttachmentComp::GetFileSize() const
{
	return m_fileSize;
}


void CAttachmentComp::SetFileSize(qint64 fileSize)
{
	if (m_fileSize != fileSize){
		istd::CChangeNotifier notifier(this);

		m_fileSize = fileSize;
	}
}


QString CAttachmentComp::GetCreatedAt() const
{
	return m_createdAt;
}


void CAttachmentComp::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);

		m_createdAt = createdAt;
	}
}


// reimplemented (iser::ISerializable)

bool CAttachmentComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag fileNameTag("FileName", "File name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fileNameTag);
	retVal = retVal && archive.Process(m_fileName);
	retVal = retVal && archive.EndTag(fileNameTag);

	static iser::CArchiveTag mimeTypeTag("MimeType", "MIME type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(mimeTypeTag);
	retVal = retVal && archive.Process(m_mimeType);
	retVal = retVal && archive.EndTag(mimeTypeTag);

	static iser::CArchiveTag dataTag("Data", "Binary data", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(dataTag);
	retVal = retVal && archive.Process(m_data);
	retVal = retVal && archive.EndTag(dataTag);

	static iser::CArchiveTag fileSizeTag("FileSize", "File size", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fileSizeTag);
	retVal = retVal && archive.Process(m_fileSize);
	retVal = retVal && archive.EndTag(fileSizeTag);

	static iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CAttachmentComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IAttachment* sourcePtr = dynamic_cast<const IAttachment*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

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
	istd::TUniqueInterfacePtr<CAttachmentComp> clonePtr(new CAttachmentComp());
	clonePtr->CopyFrom(*this, mode);
	return clonePtr;
}


bool CAttachmentComp::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_id.clear();
	m_fileName.clear();
	m_mimeType.clear();
	m_data.clear();
	m_fileSize = 0;
	m_createdAt.clear();
	return true;
}


} // namespace imtchat
