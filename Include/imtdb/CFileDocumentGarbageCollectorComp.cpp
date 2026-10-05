// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CFileDocumentGarbageCollectorComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QDirIterator>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QRegularExpression>
#include <QtConcurrent/QtConcurrent>

// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateCompBase.h>


namespace imtdb
{


namespace
{
	// Store descriptor schema written by CSqlDatabaseFileDocumentDelegateComp. Every
	// field is validated before a row is trusted to reference content: an incomplete
	// or malformed descriptor gives no safe way to know which file it points at, so it
	// must count as foreign content rather than silently permitting deletion.
	const QString descriptorFormatKey = QStringLiteral("fmt");
	const QString descriptorAlgorithmKey = QStringLiteral("alg");
	const QString descriptorHashKey = QStringLiteral("hash");
	const QString descriptorSizeKey = QStringLiteral("size");

	const int descriptorFormatVersion = 1;
	const QString descriptorAlgorithm = QStringLiteral("sha256");

	// The hash names the content file, so it must be exactly the 64-character
	// lowercase SHA-256 hex the store uses everywhere else.
	const QRegularExpression contentHashPattern(QStringLiteral("^[0-9a-f]{64}$"));

	// <sha256-hex>.bin - the only files the collector may ever delete inside the
	// grace period rules; everything else in the store is reported, not touched,
	// except stale staging leftovers ('<name>.bin.XXXXXX' from QSaveFile).
	const QRegularExpression contentFileNamePattern(QStringLiteral("^[0-9a-f]{64}\\.bin$"));
	const QRegularExpression stagingFileNamePattern(QStringLiteral("^[0-9a-f]{64}\\.bin\\.[^.]+$"));

	// Two-hex-character fan-out folders created by the delegate; emptied ones are
	// removed at the end of a deletion pass.
	const QRegularExpression fanOutFolderNamePattern(QStringLiteral("^[0-9a-f]{2}$"));

	// Returns the referenced content hash of a fully valid store descriptor, or an
	// empty array when the value is not one. A caller must treat the empty result as
	// foreign content: the referenced file cannot be determined, so nothing may be
	// deleted on this row's behalf.
	QByteArray GetValidatedContentHash(const QByteArray& descriptorValue)
	{
		const QJsonDocument descriptorDoc = QJsonDocument::fromJson(descriptorValue);
		if (!descriptorDoc.isObject()){
			return QByteArray();
		}

		const QJsonObject descriptor = descriptorDoc.object();

		if (descriptor.value(descriptorFormatKey).toInt(-1) != descriptorFormatVersion){
			return QByteArray();
		}

		if (descriptor.value(descriptorAlgorithmKey).toString() != descriptorAlgorithm){
			return QByteArray();
		}

		const QJsonValue sizeValue = descriptor.value(descriptorSizeKey);
		if (!sizeValue.isDouble() || sizeValue.toDouble() < 0.0){
			return QByteArray();
		}

		const QString contentHash = descriptor.value(descriptorHashKey).toString();
		if (!contentHashPattern.match(contentHash).hasMatch()){
			return QByteArray();
		}

		return contentHash.toLatin1();
	}
}


// reimplemented (icomp::CComponentBase)

void CFileDocumentGarbageCollectorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	connect(&m_timer, &QTimer::timeout, this, &CFileDocumentGarbageCollectorComp::OnTimeout);

	m_timer.setInterval(*m_checkIntervalAttrPtr);
	m_timer.start();
}


void CFileDocumentGarbageCollectorComp::OnComponentDestroyed()
{
	m_timer.stop();

	if (m_sweepWatcher.isStarted()){
		m_sweepWatcher.waitForFinished();
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CFileDocumentGarbageCollectorComp::GetLivenessInfo(LivenessInfo& livenessInfo) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		SendErrorMessage(0, "Attribute 'DatabaseEngine' was not set", "CFileDocumentGarbageCollectorComp");

		return false;
	}

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid() && !(*m_tableSchemaAttrPtr).isEmpty()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	// All rows, all states: inactive revision rows and soft-deleted rows keep their
	// content alive - restoring either must always find its file.
	const QByteArray query = QString("SELECT DISTINCT \"%1\" FROM %2\"%3\";")
				.arg(
					qPrintable(CSqlDatabaseDocumentDelegateCompBase::s_documentColumn),
					schemaPrefix,
					qPrintable(*m_tableNameAttrPtr)).toUtf8();

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError, true);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "CFileDocumentGarbageCollectorComp");

		return false;
	}

	while (sqlQuery.next()){
		const QByteArray documentValue = sqlQuery.value(0).toByteArray();
		if (documentValue.isEmpty()){
			continue;
		}

		const QByteArray contentHash = GetValidatedContentHash(documentValue);
		if (contentHash.isEmpty()){
			livenessInfo.hasForeignContent = true;

			continue;
		}

		livenessInfo.referencedHashes.insert(contentHash);
	}

	return true;
}


bool CFileDocumentGarbageCollectorComp::SweepStore(const LivenessInfo& livenessInfo, bool isDeletionAllowed)
{
	const QString storageRootPath = m_storageRootCompPtr->GetPath();
	const QDateTime graceLimit = QDateTime::currentDateTimeUtc().addSecs(-qint64(*m_gracePeriodHoursAttrPtr) * 3600);

	QSet<QByteArray> foundHashes;
	int unreferencedCount = 0;
	int deletedCount = 0;

	QDirIterator storeIterator(storageRootPath, QDir::Files, QDirIterator::Subdirectories);
	while (storeIterator.hasNext()){
		storeIterator.next();
		const QFileInfo fileInfo = storeIterator.fileInfo();
		const QString fileName = fileInfo.fileName();

		const bool isContentFile = contentFileNamePattern.match(fileName).hasMatch();
		if (!isContentFile && !stagingFileNamePattern.match(fileName).hasMatch()){
			SendWarningMessage(0, QString("Foreign file in document store: '%1'").arg(fileInfo.absoluteFilePath()),
						"CFileDocumentGarbageCollectorComp");

			continue;
		}

		const QByteArray contentHash = fileName.left(64).toLatin1();
		if (isContentFile && livenessInfo.referencedHashes.contains(contentHash)){
			foundHashes.insert(contentHash);

			continue;
		}

		// Unreferenced content file, or a staging leftover from a crashed writer.
		// Age gates the deletion: a young unreferenced file may belong to a
		// transaction that has not committed yet.
		if (fileInfo.lastModified().toUTC() >= graceLimit){
			continue;
		}

		++unreferencedCount;

		if (!isDeletionAllowed){
			SendInfoMessage(0, QString("Unreferenced document content (audit): '%1'").arg(fileInfo.absoluteFilePath()),
						"CFileDocumentGarbageCollectorComp");

			continue;
		}

		// Re-stat immediately before removal. A writer that reuses this content
		// refreshes the file's modification time - its store lease - and that refresh
		// can land after the SQL liveness snapshot and the directory walk above. This
		// final, fresh check must observe it, or reuse and deletion could both act on
		// the same blob and leave the writer's transaction committing a dangling
		// reference. A file that vanished, or was re-dated within the grace period, is
		// left alone.
		const QDateTime currentModified = QFileInfo(fileInfo.absoluteFilePath()).lastModified().toUTC();
		if (!currentModified.isValid() || currentModified >= graceLimit){
			continue;
		}

		if (QFile::remove(fileInfo.absoluteFilePath())){
			++deletedCount;
		}
		else{
			SendWarningMessage(0, QString("Unable to delete unreferenced document content '%1'").arg(fileInfo.absoluteFilePath()),
						"CFileDocumentGarbageCollectorComp");
		}
	}

	// Integrity audit: every referenced hash must exist in the store.
	const QSet<QByteArray> missingHashes = livenessInfo.referencedHashes - foundHashes;
	for (const QByteArray& missingHash: missingHashes){
		SendErrorMessage(0, QString("Referenced document content is missing from the store: '%1'").arg(QString::fromLatin1(missingHash)),
					"CFileDocumentGarbageCollectorComp");
	}

	// Remove fan-out folders emptied by this or an earlier pass. QDir::rmdir fails
	// on a non-empty folder, so a folder that received a file concurrently is left
	// alone; the delegate re-creates a folder removed between its mkpath and the
	// staging file creation.
	int removedFolderCount = 0;
	if (isDeletionAllowed){
		QDir storageRootDir(storageRootPath);
		const QStringList folderNames = storageRootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (const QString& folderName: folderNames){
			if (fanOutFolderNamePattern.match(folderName).hasMatch() && storageRootDir.rmdir(folderName)){
				++removedFolderCount;
			}
		}
	}

	SendInfoMessage(0, QString("Document store pass finished: %1 referenced, %2 unreferenced past grace period, %3 deleted, %4 missing, %5 empty folders removed")
				.arg(livenessInfo.referencedHashes.count())
				.arg(unreferencedCount)
				.arg(deletedCount)
				.arg(missingHashes.count())
				.arg(removedFolderCount),
				"CFileDocumentGarbageCollectorComp");

	return missingHashes.isEmpty();
}


// private slots

void CFileDocumentGarbageCollectorComp::OnTimeout()
{
	if (m_sweepWatcher.isStarted() && !m_sweepWatcher.isFinished()){
		return;
	}

	if (!m_storageRootCompPtr.IsValid() || m_storageRootCompPtr->GetPath().isEmpty()){
		SendErrorMessage(0, "Attribute 'StorageRoot' was not set or is empty", "CFileDocumentGarbageCollectorComp");

		return;
	}

	// The liveness scan runs on this thread: the SQL connection of the engine is
	// bound to the thread it was created on. Only the file sweep is offloaded.
	LivenessInfo livenessInfo;
	if (!GetLivenessInfo(livenessInfo)){
		return;
	}

	if (livenessInfo.hasForeignContent){
		SendWarningMessage(0, "Document table contains non-descriptor content; deletion is disabled for this pass",
					"CFileDocumentGarbageCollectorComp");
	}

	const bool isAuditOnly = m_auditOnlyAttrPtr.IsValid() ? *m_auditOnlyAttrPtr : true;
	const bool isDeletionAllowed = !isAuditOnly && !livenessInfo.hasForeignContent;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	m_sweepWatcher.setFuture(QtConcurrent::run(this, &CFileDocumentGarbageCollectorComp::SweepStore, livenessInfo, isDeletionAllowed));
#else
	m_sweepWatcher.setFuture(QtConcurrent::run(&CFileDocumentGarbageCollectorComp::SweepStore, this, livenessInfo, isDeletionAllowed));
#endif
}


} // namespace imtdb
