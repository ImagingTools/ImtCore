// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/IDeviceBasedPersistence.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateCompBase.h>


namespace imtdb
{


/**
	Document delegate storing the document content in a content-addressed file store
	instead of the database. The 'Document' column receives a small JSON descriptor
	(format, hash algorithm, content hash, size) referencing the stored file.

	The content is written in the document's native format via the first assigned
	ifile::IDeviceBasedPersistence component supporting the object - store files are
	unwrapped, directly usable native files, and deduplication is over the native bytes.

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
		I_ASSIGN_MULTI_0(m_persistenceListCompPtr, "PersistenceList", "List of persistence components used to read and write the document content in its native format;\nthe first component supporting the object is used", true);
	I_END_COMPONENT

protected:
	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateCompBase)
	virtual bool WriteDataToMemory(const QByteArray& typeId, const istd::IChangeable& object, QByteArray& data) const override;
	virtual bool ReadDataFromMemory(const QByteArray& typeId, const QByteArray& data, istd::IChangeable& object) const override;

private:
	QString GetContentFilePath(const QByteArray& contentHashHex) const;
	const ifile::IDeviceBasedPersistence* GetObjectPersistence(const istd::IChangeable& object, QIODevice& device, int deviceOperation) const;

protected:
	/**
		Root folder of the content-addressed document file store.
	*/
	I_REF(ifile::IFileNameParam, m_storageRootCompPtr);

	/**
		Persistence components writing and reading the native document representation.
	*/
	I_MULTIREF(ifile::IDeviceBasedPersistence, m_persistenceListCompPtr);
};


} // namespace imtdb
