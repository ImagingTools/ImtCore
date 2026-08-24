// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseBinaryDocumentDelegateComp.h>


// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtdb
{


bool CSqlDatabaseBinaryDocumentDelegateComp::WriteDataToMemory(const QByteArray& /*typeId*/, const istd::IChangeable& object, QByteArray& data) const
{
	auto serializableObjectPtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(&object));
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	iser::CMemoryWriteArchive writeArchive(m_versionInfoCompPtr.GetPtr());
	iser::CBinaryWriteArchiveBase& archive = writeArchive;
	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, "Unable to write data to memory. Error: Serialization failed", "CSqlDatabaseBinaryDocumentDelegateComp");
		return false;
	}

	data = QByteArray(static_cast<const char*>(writeArchive.GetBuffer()), writeArchive.GetBufferSize());

	return true;
}


bool CSqlDatabaseBinaryDocumentDelegateComp::ReadDataFromMemory(const QByteArray& /*typeId*/, const QByteArray& data, istd::IChangeable& object) const
{
	auto serializableObjectPtr = dynamic_cast<iser::ISerializable*>(&object);
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	const int bufferSize = imtbase::narrow_cast<int>(data.size());
	iser::CMemoryReadArchive readArchive(data.constData(), bufferSize);
	iser::CBinaryReadArchiveBase& archive = readArchive;
	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, "Unable to read data from memory. Error: Serialization failed", "CSqlDatabaseBinaryDocumentDelegateComp");
		return false;
	}

	return true;
}


} // namespace imtdb
