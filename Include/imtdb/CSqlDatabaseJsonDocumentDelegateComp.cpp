// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseJsonDocumentDelegateComp.h>


// ACF includes
#include <iser/CJsonMemWriteArchive.h>
#include <iser/CJsonMemReadArchive.h>


namespace imtdb
{


bool CSqlDatabaseJsonDocumentDelegateComp::WriteDataToMemory(const QByteArray& /*typeId*/, const istd::IChangeable& object, QByteArray& data) const
{
	auto serializableObjectPtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(&object));
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	iser::CJsonMemWriteArchive archive(m_versionInfoCompPtr.GetPtr());
	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, "Unable to write data to memory. Error: Serialization failed", "CSqlDatabaseJsonDocumentDelegateComp");
		return false;
	}

	data = archive.GetData();

	return true;
}


bool CSqlDatabaseJsonDocumentDelegateComp::ReadDataFromMemory(const QByteArray& /*typeId*/, const QByteArray& data, istd::IChangeable& object) const
{
	auto serializableObjectPtr = dynamic_cast<iser::ISerializable*>(&object);
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	iser::CJsonMemReadArchive archive(data);
	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, "Unable to read data from memory. Error: Serialization failed", "CSqlDatabaseJsonDocumentDelegateComp");
		return false;
	}

	return true;
}


} // namespace imtdb
