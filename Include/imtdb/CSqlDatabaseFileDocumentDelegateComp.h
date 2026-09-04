// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateCompBase.h>


namespace imtdb
{


/**
	Document delegate storing the serialized document content in a content-addressed
	file store instead of the database. The 'Document' column receives a small JSON
	descriptor (format, hash algorithm, content hash, size) referencing the stored file.

	Store files are immutable and named by the SHA-256 hash of their content. Content
	is always written and made durable before the referencing descriptor is handed to
	the SQL layer, so a committed row always points at existing, verified content.
	Files are never overwritten and never deleted by a writer: content whose referencing
	transaction failed or was rolled back is reclaimed by CFileDocumentGarbageCollectorComp.
*/
class CSqlDatabaseFileDocumentDelegateComp: public imtdb::CSqlDatabaseDocumentDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseDocumentDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CSqlDatabaseFileDocumentDelegateComp)
		I_ASSIGN(m_storageRootCompPtr, "StorageRoot", "Root folder of the document file store.\nMust be used exclusively by this collection's table: the garbage collector's liveness scan relies on this", true, "StorageRoot");
	I_END_COMPONENT

protected:
	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateCompBase)
	virtual bool WriteDataToMemory(const QByteArray& typeId, const istd::IChangeable& object, QByteArray& data) const override;
	virtual bool ReadDataFromMemory(const QByteArray& typeId, const QByteArray& data, istd::IChangeable& object) const override;

private:
	QString GetContentFilePath(const QByteArray& contentHashHex) const;

protected:
	/**
		Root folder of the content-addressed document file store.
	*/
	I_REF(ifile::IFileNameParam, m_storageRootCompPtr);
};


} // namespace imtdb
