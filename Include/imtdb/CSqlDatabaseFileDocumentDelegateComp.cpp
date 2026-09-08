// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseFileDocumentDelegateComp.h>


// Qt includes
#include <QtCore/QBuffer>
#include <QtCore/QCryptographicHash>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSaveFile>


namespace imtdb
{


namespace
{
	const QString descriptorFormatKey = QStringLiteral("fmt");
	const QString descriptorAlgorithmKey = QStringLiteral("alg");
	const QString descriptorHashKey = QStringLiteral("hash");
	const QString descriptorSizeKey = QStringLiteral("size");

	const int descriptorFormatVersion = 1;
	const QString descriptorAlgorithm = QStringLiteral("sha256");
	const QString contentFileSuffix = QStringLiteral(".bin");
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateCompBase)

bool CSqlDatabaseFileDocumentDelegateComp::WriteDataToMemory(
			const QByteArray& /*typeId*/,
			const istd::IChangeable& object,
			QByteArray& data) const
{
	if (!m_storageRootCompPtr.IsValid() || m_storageRootCompPtr->GetPath().isEmpty()){
		SendErrorMessage(0, "Attribute 'StorageRoot' was not set or is empty", "CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}

	// The content is produced in the document's native format - the store holds
	// unwrapped, directly usable files, and the content hash is over native bytes.
	QByteArray contentBuffer;
	QBuffer contentDevice(&contentBuffer);
	if (!contentDevice.open(QIODevice::WriteOnly)){
		return false;
	}

	const ifile::IDeviceBasedPersistence* persistencePtr = GetObjectPersistence(object, contentDevice, ifile::IDeviceBasedPersistence::WriteOperation);
	if (persistencePtr == nullptr){
		SendErrorMessage(0, "No assigned persistence component supports writing the document", "CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}
	if (persistencePtr->WriteToDevice(object, contentDevice) != ifile::IDeviceBasedPersistence::Successful){
		SendErrorMessage(0, "Unable to write document data. Error: Persistence write failed", "CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}
	contentDevice.close();
	const char* bufferPtr = contentBuffer.constData();
	const qint64 bufferSize = contentBuffer.size();

	// The file name is the hash of the content. This single decision carries the
	// crash-safety of the store: writes are idempotent, files are immutable, and a
	// file promoted here whose referencing SQL never commits is reclaimed by the
	// garbage collector - never by a writer, since deduplication means a writer
	// cannot know it is the sole referencer of the content.
	const QByteArray contentHash = QCryptographicHash::hash(
				QByteArray::fromRawData(bufferPtr, bufferSize),
				QCryptographicHash::Sha256).toHex();

	const QString targetFilePath = GetContentFilePath(contentHash);
	const QFileInfo targetInfo(targetFilePath);

	// The modification time of stored content is its reuse lease: the garbage
	// collector's grace period protects any freshly written - or freshly re-dated -
	// file for the current, not-yet-committed transaction. A plain existence check is
	// not enough for a deduplicated hit, because the blob may be an old orphan the
	// collector is about to reclaim; refreshing its lease re-arms that protection, and
	// if the collector has already won the race the content is simply re-written.
	if (targetInfo.exists()){
		// Identical content is already in the store.
		QFile existingFile(targetFilePath);
		if (!existingFile.open(QIODevice::ReadOnly)
				|| existingFile.size() != bufferSize
				|| QCryptographicHash::hash(existingFile.readAll(), QCryptographicHash::Sha256).toHex() != contentHash){
			SendErrorMessage(0, QString("Store integrity error: '%1' does not match the expected content")
						.arg(targetFilePath),
						"CSqlDatabaseFileDocumentDelegateComp");
			return false;
		}

		if (!RefreshContentLease(targetFilePath) && !WriteContentFile(targetFilePath, bufferPtr, bufferSize)){
			return false;
		}
	}
	else if (!WriteContentFile(targetFilePath, bufferPtr, bufferSize)){
		return false;
	}

	// Only now, with the content durably in the store, is the reference handed to
	// the SQL layer: content always precedes reference.
	QJsonObject descriptor;
	descriptor[descriptorFormatKey] = descriptorFormatVersion;
	descriptor[descriptorAlgorithmKey] = descriptorAlgorithm;
	descriptor[descriptorHashKey] = QString::fromLatin1(contentHash);
	descriptor[descriptorSizeKey] = double(bufferSize);
	data = QJsonDocument(descriptor).toJson(QJsonDocument::Compact);

	return true;
}


bool CSqlDatabaseFileDocumentDelegateComp::ReadDataFromMemory(
			const QByteArray& /*typeId*/,
			const QByteArray& data,
			istd::IChangeable& object) const
{
	QJsonParseError parseError;
	const QJsonDocument descriptorDoc = QJsonDocument::fromJson(data, &parseError);
	if (parseError.error != QJsonParseError::NoError || !descriptorDoc.isObject()){
		SendErrorMessage(0, "Document column does not contain a valid store descriptor", "CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}

	const QJsonObject descriptor = descriptorDoc.object();
	if (descriptor.value(descriptorAlgorithmKey).toString() != descriptorAlgorithm){
		SendErrorMessage(0, QString("Unsupported store descriptor algorithm '%1'")
					.arg(descriptor.value(descriptorAlgorithmKey).toString()),
					"CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}
	const QByteArray expectedHash = descriptor.value(descriptorHashKey).toString().toLatin1();
	const qint64 expectedSize = qint64(descriptor.value(descriptorSizeKey).toDouble());
	if (expectedHash.isEmpty()){
		SendErrorMessage(0, "Store descriptor does not contain a content hash", "CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}

	QFile contentFile(GetContentFilePath(expectedHash));
	if (!contentFile.open(QIODevice::ReadOnly)){
		SendErrorMessage(0, QString("Referenced document content '%1' is missing").arg(contentFile.fileName()),
					"CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}
	QByteArray content = contentFile.readAll();
	if (content.size() != expectedSize
			|| QCryptographicHash::hash(content, QCryptographicHash::Sha256).toHex() != expectedHash){
		SendErrorMessage(0, QString("Integrity check failed for document content '%1'").arg(contentFile.fileName()),
					"CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}

	QBuffer contentDevice(&content);
	if (!contentDevice.open(QIODevice::ReadOnly)){
		return false;
	}

	const ifile::IDeviceBasedPersistence* persistencePtr = GetObjectPersistence(object, contentDevice, ifile::IDeviceBasedPersistence::ReadOperation);
	if (persistencePtr == nullptr){
		SendErrorMessage(0, "No assigned persistence component supports reading the document", "CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}
	if (persistencePtr->ReadFromDevice(object, contentDevice) != ifile::IDeviceBasedPersistence::Successful){
		SendErrorMessage(0, "Unable to read document data. Error: Persistence read failed", "CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}

	return true;
}


// private methods

bool CSqlDatabaseFileDocumentDelegateComp::WriteContentFile(
			const QString& targetFilePath,
			const char* contentPtr,
			qint64 contentSize) const
{
	const QFileInfo targetInfo(targetFilePath);
	if (!QDir().mkpath(targetInfo.absolutePath())){
		SendErrorMessage(0, QString("Unable to create store folder '%1'").arg(targetInfo.absolutePath()),
					"CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}

	// QSaveFile stages in a temporary file next to the target (same volume, so
	// the promotion is an atomic rename) and removes that temporary itself on
	// cancel, commit failure or destruction - staging cannot outlive this scope.
	// The open is retried once with a re-created folder: the garbage collector
	// may remove an emptied fan-out folder between the mkpath above and here.
	QSaveFile stagingFile(targetFilePath);
	if (!stagingFile.open(QIODevice::WriteOnly)
			&& (!QDir().mkpath(targetInfo.absolutePath()) || !stagingFile.open(QIODevice::WriteOnly))){
		SendErrorMessage(0, QString("Unable to open staging file for '%1': %2")
					.arg(targetFilePath, stagingFile.errorString()),
					"CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}
	if (stagingFile.write(contentPtr, contentSize) != contentSize){
		stagingFile.cancelWriting();
		SendErrorMessage(0, QString("Unable to write document content to '%1': %2")
					.arg(targetFilePath, stagingFile.errorString()),
					"CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}
	if (!stagingFile.commit()){
		// A concurrent writer of identical content may have won the rename race;
		// the store is correct either way if the target now exists with our size.
		const QFileInfo raceInfo(targetFilePath);
		if (!raceInfo.exists() || raceInfo.size() != contentSize){
			SendErrorMessage(0, QString("Unable to promote document content to '%1': %2")
						.arg(targetFilePath, stagingFile.errorString()),
						"CSqlDatabaseFileDocumentDelegateComp");
			return false;
		}
	}

	return true;
}


bool CSqlDatabaseFileDocumentDelegateComp::RefreshContentLease(const QString& targetFilePath) const
{
	// ExistingOnly keeps a lost race - the collector reclaimed the blob between the
	// existence check and here - from silently re-creating an empty file; the caller
	// re-writes the real content instead.
	QFile contentFile(targetFilePath);
	if (!contentFile.open(QIODevice::ReadWrite | QIODevice::ExistingOnly)){
		return false;
	}
	if (!contentFile.setFileTime(QDateTime::currentDateTimeUtc(), QFileDevice::FileModificationTime)){
		return false;
	}
	contentFile.close();

	// On POSIX a racing collector can unlink the path while this open handle keeps the
	// inode - and thus setFileTime - alive; re-confirm the entry still resolves so a
	// descriptor is never returned for content that has left the store.
	return QFileInfo::exists(targetFilePath);
}


QString CSqlDatabaseFileDocumentDelegateComp::GetContentFilePath(const QByteArray& contentHashHex) const
{
	// Two-character fan-out keeps single folder sizes manageable at scale.
	return QDir(m_storageRootCompPtr->GetPath()).filePath(
				QString("%1/%2%3")
					.arg(QString::fromLatin1(contentHashHex.left(2)),
						QString::fromLatin1(contentHashHex),
						contentFileSuffix));
}


const ifile::IDeviceBasedPersistence* CSqlDatabaseFileDocumentDelegateComp::GetObjectPersistence(
			const istd::IChangeable& object,
			QIODevice& device,
			int deviceOperation) const
{
	for (int i = 0; i < m_persistenceListCompPtr.GetCount(); ++i){
		const ifile::IDeviceBasedPersistence* persistencePtr = m_persistenceListCompPtr[i];
		if (persistencePtr != nullptr && persistencePtr->IsDeviceOperationSupported(object, device, deviceOperation)){
			return persistencePtr;
		}
	}

	return nullptr;
}


} // namespace imtdb
