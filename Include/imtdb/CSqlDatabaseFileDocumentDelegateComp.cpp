// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseFileDocumentDelegateComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSaveFile>

// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtbase/imtbase.h>


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
	auto serializableObjectPtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(&object));
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	if (!m_storageRootCompPtr.IsValid() || m_storageRootCompPtr->GetPath().isEmpty()){
		SendErrorMessage(0, "Attribute 'StorageRoot' was not set or is empty", "CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}

	iser::CMemoryWriteArchive writeArchive(m_versionInfoCompPtr.GetPtr());
	if (!serializableObjectPtr->Serialize(writeArchive)){
		SendErrorMessage(0, "Unable to write document data. Error: Serialization failed", "CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}
	const char* bufferPtr = static_cast<const char*>(writeArchive.GetBuffer());
	const qint64 bufferSize = writeArchive.GetBufferSize();

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

	if (targetInfo.exists()){
		// Identical content is already in the store.
		if (targetInfo.size() != bufferSize){
			SendErrorMessage(0, QString("Store integrity error: '%1' exists with size %2, expected %3")
						.arg(targetFilePath).arg(targetInfo.size()).arg(bufferSize),
						"CSqlDatabaseFileDocumentDelegateComp");
			return false;
		}
	}
	else{
		if (!QDir().mkpath(targetInfo.absolutePath())){
			SendErrorMessage(0, QString("Unable to create store folder '%1'").arg(targetInfo.absolutePath()),
						"CSqlDatabaseFileDocumentDelegateComp");
			return false;
		}

		// QSaveFile stages in a temporary file next to the target (same volume, so
		// the promotion is an atomic rename) and removes that temporary itself on
		// cancel, commit failure or destruction - staging cannot outlive this scope.
		QSaveFile stagingFile(targetFilePath);
		if (!stagingFile.open(QIODevice::WriteOnly)){
			SendErrorMessage(0, QString("Unable to open staging file for '%1': %2")
						.arg(targetFilePath, stagingFile.errorString()),
						"CSqlDatabaseFileDocumentDelegateComp");
			return false;
		}
		if (stagingFile.write(bufferPtr, bufferSize) != bufferSize){
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
			if (!raceInfo.exists() || raceInfo.size() != bufferSize){
				SendErrorMessage(0, QString("Unable to promote document content to '%1': %2")
							.arg(targetFilePath, stagingFile.errorString()),
							"CSqlDatabaseFileDocumentDelegateComp");
				return false;
			}
		}
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
	const QByteArray content = contentFile.readAll();
	if (content.size() != expectedSize
			|| QCryptographicHash::hash(content, QCryptographicHash::Sha256).toHex() != expectedHash){
		SendErrorMessage(0, QString("Integrity check failed for document content '%1'").arg(contentFile.fileName()),
					"CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}

	auto serializableObjectPtr = dynamic_cast<iser::ISerializable*>(&object);
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	iser::CMemoryReadArchive readArchive(content.constData(), imtbase::narrow_cast<int>(content.size()));
	if (!serializableObjectPtr->Serialize(readArchive)){
		SendErrorMessage(0, "Unable to read document data. Error: Deserialization failed", "CSqlDatabaseFileDocumentDelegateComp");
		return false;
	}

	return true;
}


// private methods

QString CSqlDatabaseFileDocumentDelegateComp::GetContentFilePath(const QByteArray& contentHashHex) const
{
	// Two-character fan-out keeps single folder sizes manageable at scale.
	return QDir(m_storageRootCompPtr->GetPath()).filePath(
				QString("%1/%2%3")
					.arg(QString::fromLatin1(contentHashHex.left(2)),
						QString::fromLatin1(contentHashHex),
						contentFileSuffix));
}


} // namespace imtdb
