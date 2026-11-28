// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcom/CFileDownloader.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QElapsedTimer>
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

// STL includes
#include <memory>

// ImtCore includes
#include <imtcom/CRequestSender.h>
#include <imtrest/HttpHeaderIds.h>


namespace imtcom
{
namespace
{


typedef imtfile::IFileDownloadLinkProvider::DownloadLink DownloadLink;


const qint64 s_readChunkSize = Q_INT64_C(1024) * 1024;
const int s_cancelCheckIntervalMs = 100;
// Restarts from the first byte per download: the server answered 416, or with another range than asked for
const int s_maxRestarts = 3;
const int s_millisecondsPerSecond = 1000;


enum class TransferOutcome
{
	// The body came to its end
	Received,
	Canceled,
	// The server refused the link, or answered with the bytes of another entity tag
	LinkRefused,
	// The bytes start anew from the first one
	Restart,
	// No answer, a broken connection, no data for the idle timeout or an error of the server: the next URL is tried
	ConnectionFailed,
	// More bytes than the size of the link
	Oversized,
	WriteFailed
};


struct TransferResult
{
	TransferOutcome outcome = TransferOutcome::ConnectionFailed;
	QString error;
	// The size of the file the server announced; -1 if it did not
	qint64 announcedSize = -1;
};


struct TransferSettings
{
	QString url;
	// The entity tag of the link: the bytes of the part file are of it
	QByteArray entityTag;
	qint64 linkSize = -1;
	int idleTimeoutMs = 0;
	int progressIntervalMs = 0;
};


QString GetPartFilePath(const QString& filePath)
{
	return filePath + QStringLiteral(".part");
}


QString GetEntityTagFilePath(const QString& partFilePath)
{
	return partFilePath + QStringLiteral(".etag");
}


QByteArray ReadEntityTag(const QString& entityTagFilePath)
{
	QFile file(entityTagFilePath);
	if (!file.open(QIODevice::ReadOnly)){
		return QByteArray();
	}

	return file.readAll();
}


bool WriteEntityTag(const QString& entityTagFilePath, const QByteArray& entityTag, QString& error)
{
	QFile file(entityTagFilePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate) || file.write(entityTag) != entityTag.size()){
		error = QStringLiteral("The file '%1' cannot be written: %2").arg(entityTagFilePath, file.errorString());

		return false;
	}

	return true;
}


void DiscardPartFile(QFile& partFile)
{
	partFile.close();
	partFile.remove();
	QFile::remove(GetEntityTagFilePath(partFile.fileName()));
}


/**
	Opens the part file at its end, with its bytes in the hash. The bytes of another entity tag, or for a link without one,
	are discarded: they cannot be continued.
	\return \c false on failure or when canceled
*/
bool PreparePartFile(
			const DownloadLink& link,
			QFile& partFile,
			QCryptographicHash& hash,
			const std::atomic<bool>& isCanceled,
			QString& error)
{
	partFile.close();

	const QString entityTagFilePath = GetEntityTagFilePath(partFile.fileName());
	const QByteArray entityTag = link.etag.toUtf8();
	if (entityTag.isEmpty() || ReadEntityTag(entityTagFilePath) != entityTag){
		DiscardPartFile(partFile);

		// The entity tag is written before the first byte: no byte is kept without it
		if (!entityTag.isEmpty() && !WriteEntityTag(entityTagFilePath, entityTag, error)){
			return false;
		}
	}

	if (!partFile.open(QIODevice::ReadWrite)){
		error = QStringLiteral("The file '%1' cannot be written: %2").arg(partFile.fileName(), partFile.errorString());

		return false;
	}

	if (link.size >= 0 && partFile.size() > link.size && !partFile.resize(0)){
		error = QStringLiteral("The file '%1' cannot be written: %2").arg(partFile.fileName(), partFile.errorString());

		return false;
	}

	hash.reset();
	while (!partFile.atEnd()){
		if (isCanceled){
			return false;
		}

		const QByteArray data = partFile.read(s_readChunkSize);
		if (data.isEmpty()){
			error = QStringLiteral("The file '%1' cannot be read: %2").arg(partFile.fileName(), partFile.errorString());

			return false;
		}

		hash.addData(data);
	}

	return true;
}


/**
	The first byte and the size of the file in "bytes <first>-<last>/<size>"; the size is -1 for "*".
*/
bool ParseContentRange(const QByteArray& contentRange, qint64& first, qint64& size)
{
	static const QByteArray s_unit = QByteArrayLiteral("bytes ");
	first = -1;
	size = -1;

	const qsizetype dashIndex = contentRange.indexOf('-');
	const qsizetype slashIndex = contentRange.indexOf('/');
	if (!contentRange.startsWith(s_unit) || dashIndex < 0 || slashIndex < dashIndex){
		return false;
	}

	bool isNumber = false;
	first = contentRange.mid(s_unit.size(), dashIndex - s_unit.size()).trimmed().toLongLong(&isNumber);
	if (!isNumber){
		return false;
	}

	const QByteArray sizeText = contentRange.mid(slashIndex + 1).trimmed();
	if (sizeText != "*"){
		size = sizeText.toLongLong(&isNumber);
		if (!isNumber){
			size = -1;
		}
	}

	return true;
}


bool IsLinkRefusal(int status)
{
	static const QList<int> s_refusals = {401, 403, 404, 410};

	return s_refusals.contains(status);
}


/**
	One GET of the file into the part file, from its end. It runs in a thread of its own with a local event loop.
*/
class CTransfer
{
public:
	CTransfer(
				const TransferSettings& settings,
				QFile& partFile,
				QCryptographicHash& hash,
				const std::atomic<bool>& isCanceled,
				const CFileDownloader::ProgressFunction& progress)
		:m_settings(settings),
		m_partFile(partFile),
		m_hash(hash),
		m_isCanceled(isCanceled),
		m_progress(progress),
		m_offset(partFile.pos())
	{
	}

	TransferResult Run()
	{
		QNetworkRequest request{QUrl(m_settings.url)};
		// The bytes as they are on the server: the offsets of a compressed answer would not match
		request.setRawHeader("Accept-Encoding", "identity");
		if (m_offset > 0){
			request.setRawHeader(imtrest::HttpHeaderIds::s_range, "bytes=" + QByteArray::number(m_offset) + '-');
			request.setRawHeader(imtrest::HttpHeaderIds::s_ifRange, m_settings.entityTag);
		}

		QNetworkAccessManager manager;
		QEventLoop loop;
		const std::unique_ptr<QNetworkReply> replyPtr(manager.get(request));
		m_replyPtr = replyPtr.get();

		QTimer idleTimer;
		idleTimer.setSingleShot(true);
		idleTimer.setInterval(m_settings.idleTimeoutMs);
		QObject::connect(&idleTimer, &QTimer::timeout, &loop, [this](){
			Stop(TransferOutcome::ConnectionFailed, QStringLiteral("No data came from '%1' for %2 s")
						.arg(m_settings.url).arg(m_settings.idleTimeoutMs / s_millisecondsPerSecond));
		});

		QTimer checkTimer;
		checkTimer.setInterval(s_cancelCheckIntervalMs);
		QObject::connect(&checkTimer, &QTimer::timeout, &loop, [this](){
			if (m_isCanceled){
				Stop(TransferOutcome::Canceled, QString());
			}
			else{
				ReportProgress(false);
			}
		});

		QObject::connect(m_replyPtr, &QNetworkReply::readyRead, &loop, [this, &idleTimer](){
			idleTimer.start();
			Receive();
		});
		QObject::connect(m_replyPtr, &QNetworkReply::finished, &loop, &QEventLoop::quit);

		m_progressTimer.start();
		idleTimer.start();
		checkTimer.start();
		if (!m_replyPtr->isFinished()){
			loop.exec();
		}

		if (!m_isStopped){
			// The last bytes, or an answer without a body
			Receive();
		}

		if (!m_isStopped){
			if (m_replyPtr->error() == QNetworkReply::NoError){
				m_result.outcome = TransferOutcome::Received;
			}
			else{
				Stop(TransferOutcome::ConnectionFailed, QStringLiteral("The download from '%1' broke off: %2").arg(m_settings.url, m_replyPtr->errorString()));
			}
		}

		m_partFile.flush();
		ReportProgress(true);

		return m_result;
	}

private:
	/**
		Decides on the status and the headers of the answer, once.
		\return \c true if the bytes of the answer go into the part file
	*/
	bool CheckAnswer()
	{
		if (m_isAnswerChecked){
			return !m_isStopped;
		}

		m_isAnswerChecked = true;

		const int status = m_replyPtr->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (status == 0){
			Stop(TransferOutcome::ConnectionFailed, QStringLiteral("Unable to download from '%1': %2").arg(m_settings.url, m_replyPtr->errorString()));

			return false;
		}

		if (IsLinkRefusal(status)){
			Stop(TransferOutcome::LinkRefused, QStringLiteral("The server refused the link (HTTP %1)").arg(status));

			return false;
		}

		if (status == 416){
			Stop(TransferOutcome::Restart, QStringLiteral("The server refused the range of the download (HTTP 416)"));

			return false;
		}

		if (status != 200 && status != 206){
			Stop(TransferOutcome::ConnectionFailed, QStringLiteral("The server at '%1' answered HTTP %2").arg(m_settings.url).arg(status));

			return false;
		}

		// The file has changed since the link was given
		const QByteArray entityTag = m_replyPtr->rawHeader("ETag");
		if (!m_settings.entityTag.isEmpty() && !entityTag.isEmpty() && entityTag != m_settings.entityTag){
			Stop(TransferOutcome::LinkRefused, QStringLiteral("The file has changed since its link was given"));

			return false;
		}

		if (status == 206){
			qint64 first = -1;
			if (!ParseContentRange(m_replyPtr->rawHeader(imtrest::HttpHeaderIds::s_contentRange), first, m_result.announcedSize) || first != m_offset){
				Stop(TransferOutcome::Restart, QStringLiteral("The server sent another range than asked for"));

				return false;
			}

			return true;
		}

		// The whole file
		bool isNumber = false;
		const qint64 contentLength = m_replyPtr->rawHeader(imtrest::HttpHeaderIds::s_contentLength).toLongLong(&isNumber);
		m_result.announcedSize = isNumber ? contentLength : -1;

		if (!m_partFile.resize(0) || !m_partFile.seek(0)){
			Stop(TransferOutcome::WriteFailed, QStringLiteral("The file '%1' cannot be written: %2").arg(m_partFile.fileName(), m_partFile.errorString()));

			return false;
		}

		m_hash.reset();

		return true;
	}

	void Receive()
	{
		if (m_isCanceled){
			Stop(TransferOutcome::Canceled, QString());

			return;
		}

		if (!CheckAnswer()){
			return;
		}

		const QByteArray data = m_replyPtr->readAll();
		if (data.isEmpty()){
			return;
		}

		if (m_settings.linkSize >= 0 && m_partFile.pos() + data.size() > m_settings.linkSize){
			Stop(TransferOutcome::Oversized, QStringLiteral("The server sent more bytes than the size of the file"));

			return;
		}

		if (m_partFile.write(data) != data.size()){
			Stop(TransferOutcome::WriteFailed, QStringLiteral("The file '%1' cannot be written: %2").arg(m_partFile.fileName(), m_partFile.errorString()));

			return;
		}

		m_hash.addData(data);
	}

	void Stop(TransferOutcome outcome, const QString& error)
	{
		if (m_isStopped){
			return;
		}

		m_isStopped = true;
		m_result.outcome = outcome;
		m_result.error = error;
		m_replyPtr->abort();
	}

	void ReportProgress(bool isFinal)
	{
		if (!m_progress){
			return;
		}

		const qint64 receivedBytes = m_partFile.pos();
		if (!isFinal && (receivedBytes == m_reportedBytes || m_progressTimer.elapsed() < m_settings.progressIntervalMs)){
			return;
		}

		m_reportedBytes = receivedBytes;
		m_progressTimer.restart();
		m_progress(receivedBytes, m_settings.linkSize >= 0 ? m_settings.linkSize : m_result.announcedSize);
	}

private:
	const TransferSettings& m_settings;
	QFile& m_partFile;
	QCryptographicHash& m_hash;
	const std::atomic<bool>& m_isCanceled;
	const CFileDownloader::ProgressFunction& m_progress;
	const qint64 m_offset;

	QNetworkReply* m_replyPtr = nullptr;
	TransferResult m_result;
	bool m_isAnswerChecked = false;
	bool m_isStopped = false;
	QElapsedTimer m_progressTimer;
	qint64 m_reportedBytes = -1;
};


/**
	Runs the transfer in a thread of its own and waits for it: the calling thread runs no event loop, so its queued calls
	do not run meanwhile.
*/
TransferResult RunTransfer(
			const TransferSettings& settings,
			QFile& partFile,
			QCryptographicHash& hash,
			const std::atomic<bool>& isCanceled,
			const CFileDownloader::ProgressFunction& progress)
{
	TransferResult result;
	const std::unique_ptr<QThread> threadPtr(QThread::create([&settings, &partFile, &hash, &isCanceled, &progress, &result](){
		CTransfer transfer(settings, partFile, hash, isCanceled, progress);
		result = transfer.Run();
	}));
	threadPtr->start();
	threadPtr->wait();

	return result;
}


CFileDownloader::Result CompleteDownload(
			const DownloadLink& link,
			QFile& partFile,
			QCryptographicHash& hash,
			const QString& filePath,
			QString& error)
{
	if (!link.sha256.isEmpty() && hash.result().toHex() != link.sha256.toLatin1().toLower()){
		// The bytes are wrong: the next download starts anew
		DiscardPartFile(partFile);
		error = QStringLiteral("The SHA-256 of the downloaded file '%1' is not the one of its link").arg(filePath);

		return CFileDownloader::Result::Failed;
	}

	const QString entityTagFilePath = GetEntityTagFilePath(partFile.fileName());
	partFile.close();

	if (QFile::exists(filePath) && !QFile::remove(filePath)){
		error = QStringLiteral("The file '%1' cannot be replaced").arg(filePath);

		return CFileDownloader::Result::Failed;
	}

	if (!partFile.rename(filePath)){
		error = QStringLiteral("The file '%1' cannot be written: %2").arg(filePath, partFile.errorString());

		return CFileDownloader::Result::Failed;
	}

	QFile::remove(entityTagFilePath);

	return CFileDownloader::Result::Completed;
}


} // namespace


// public methods

CFileDownloader::CFileDownloader(const imtfile::IFileDownloadLinkProvider& linkProvider)
	:m_linkProvider(linkProvider)
{
}


void CFileDownloader::SetIdleTimeout(int idleTimeoutMs)
{
	m_idleTimeoutMs = idleTimeoutMs;
}


void CFileDownloader::SetMaxLinkRenewals(int maxLinkRenewals)
{
	m_maxLinkRenewals = maxLinkRenewals;
}


void CFileDownloader::SetProgressInterval(int progressIntervalMs)
{
	m_progressIntervalMs = progressIntervalMs;
}


CFileDownloader::Result CFileDownloader::Download(
			const QString& remotePath,
			const QString& filePath,
			const std::atomic<bool>& isCanceled,
			const ProgressFunction& progress,
			QString& error) const
{
	error.clear();

	DownloadLink link;
	if (!RequestLink(remotePath, link, error)){
		return Result::Failed;
	}

	// The network objects of the process are made in the thread of the application, see CRequestSender
	CRequestSender::InitializeNetworkBackend();

	QFile partFile(GetPartFilePath(filePath));
	int linkRenewals = 0;
	int restarts = 0;
	int urlIndex = 0;

	for (;;){
		if (isCanceled){
			return Result::Canceled;
		}

		QCryptographicHash hash(QCryptographicHash::Sha256);
		if (!PreparePartFile(link, partFile, hash, isCanceled, error)){
			partFile.close();

			return isCanceled ? Result::Canceled : Result::Failed;
		}

		const QStringList urls = QStringList(link.url) + link.alternativeUrls;

		TransferSettings settings;
		settings.url = urls[urlIndex];
		settings.entityTag = link.etag.toUtf8();
		settings.linkSize = link.size;
		settings.idleTimeoutMs = m_idleTimeoutMs;
		settings.progressIntervalMs = m_progressIntervalMs;

		const TransferResult transfer = RunTransfer(settings, partFile, hash, isCanceled, progress);
		switch (transfer.outcome){
		case TransferOutcome::Received:{
			const qint64 size = (link.size >= 0) ? link.size : transfer.announcedSize;
			if (size < 0 || partFile.size() >= size){
				return CompleteDownload(link, partFile, hash, filePath, error);
			}

			// The answer ended before the end of the file: the part file is continued
			error = QStringLiteral("The download from '%1' ended after %2 of %3 bytes").arg(settings.url).arg(partFile.size()).arg(size);
			if (++urlIndex >= urls.size()){
				partFile.close();

				return Result::Failed;
			}
			break;
		}

		case TransferOutcome::Canceled:
			partFile.close();

			return Result::Canceled;

		case TransferOutcome::LinkRefused:
			if (++linkRenewals > m_maxLinkRenewals){
				partFile.close();
				error = QStringLiteral("No link to '%1' was accepted: %2").arg(remotePath, transfer.error);

				return Result::Failed;
			}

			if (!RequestLink(remotePath, link, error)){
				partFile.close();

				return Result::Failed;
			}

			urlIndex = 0;
			break;

		case TransferOutcome::Restart:
			DiscardPartFile(partFile);
			if (++restarts > s_maxRestarts){
				error = transfer.error;

				return Result::Failed;
			}
			break;

		case TransferOutcome::ConnectionFailed:
			error = transfer.error;
			if (++urlIndex >= urls.size()){
				partFile.close();

				return Result::Failed;
			}
			break;

		case TransferOutcome::Oversized:
			DiscardPartFile(partFile);
			error = transfer.error;

			return Result::Failed;

		case TransferOutcome::WriteFailed:
			partFile.close();
			error = transfer.error;

			return Result::Failed;
		}
	}
}


// private methods

bool CFileDownloader::RequestLink(const QString& remotePath, DownloadLink& link, QString& error) const
{
	link = DownloadLink();
	if (!m_linkProvider.GetDownloadLink(remotePath, link, error)){
		if (error.isEmpty()){
			error = QStringLiteral("No download link was given for '%1'").arg(remotePath);
		}

		return false;
	}

	if (link.url.isEmpty()){
		error = QStringLiteral("The download link of '%1' has no URL").arg(remotePath);

		return false;
	}

	return true;
}


} // namespace imtcom
