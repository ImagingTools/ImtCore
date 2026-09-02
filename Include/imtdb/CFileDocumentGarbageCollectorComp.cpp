// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CFileDocumentGarbageCollectorComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QDirIterator>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>
#include <QtConcurrent/QtConcurrent>

// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateCompBase.h>


namespace imtdb
{


namespace
{
	const QString descriptorHashKey = QStringLiteral("hash");

	// <sha256-hex>.bin - the only files the collector may ever delete inside the
	// grace period rules; everything else in the store is reported, not touched,
	// except stale staging leftovers ('<name>.bin.XXXXXX' from QSaveFile).
	const QRegularExpression contentFileNamePattern(QStringLiteral("^[0-9a-f]{64}\\.bin$"));
	const QRegularExpression stagingFileNamePattern(QStringLiteral("^[0-9a-f]{64}\\.bin\\.[^.]+$"));
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

		const QJsonDocument descriptorDoc = QJsonDocument::fromJson(documentValue);
		const QByteArray contentHash = descriptorDoc.isObject()
					? descriptorDoc.object().value(descriptorHashKey).toString().toLatin1()
					: QByteArray();
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

	SendInfoMessage(0, QString("Document store pass finished: %1 referenced, %2 unreferenced past grace period, %3 deleted, %4 missing")
				.arg(livenessInfo.referencedHashes.count())
				.arg(unreferencedCount)
				.arg(deletedCount)
				.arg(missingHashes.count()),
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
