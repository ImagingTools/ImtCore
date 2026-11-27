// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// STL includes
#include <atomic>
#include <functional>

// ImtCore includes
#include <imtfile/IFileDownloadLinkProvider.h>


namespace imtcom
{


/**
	Downloads a file of a remote file system by the links of its imtfile::IFileDownloadLinkProvider and goes on where an
	interrupted download stopped.

	The bytes go into "<file>.part"; "<file>.part.etag" keeps the entity tag of the link they came with. A download goes on
	from the end of the part file (\c Range with \c If-Range), also after a restart of the application. A part file of another
	entity tag, or one for a link without an entity tag, is started anew. The size and the SHA-256 of the link are checked
	as the bytes come (the bytes of a part file are read first), and the complete part file replaces the file.

	A link the server refuses (401, 403, 404, 410), or one it answers with another \c ETag (the file has changed), is replaced
	by a new one, not more than 'maxLinkRenewals' times per download. A connection that fails, or brings no data for the idle
	timeout, is tried with the next URL of the link; when no URL is left, the download fails and keeps its part file for
	the next attempt.

	Download() blocks the calling thread for the whole download without an event loop: each transfer runs in a thread of its
	own, so the queued calls of the calling thread do not run meanwhile. Call it from a worker thread. The links are requested
	in the calling thread (with its request context); the progress is reported from the thread of a transfer. One file is
	downloaded by one call at a time: two calls for the same file would write into the same part file.
*/
class CFileDownloader
{
public:
	enum class Result
	{
		Completed,
		Canceled,
		Failed
	};

	/**
		\param receivedBytes - the bytes of the file received so far, those of the part file included
		\param totalBytes - the size of the file; -1 if it is not known
	*/
	typedef std::function<void(qint64 receivedBytes, qint64 totalBytes)> ProgressFunction;

	static constexpr int s_defaultIdleTimeoutMs = 30 * 1000;
	static constexpr int s_defaultMaxLinkRenewals = 3;
	static constexpr int s_defaultProgressIntervalMs = 250;

	explicit CFileDownloader(const imtfile::IFileDownloadLinkProvider& linkProvider);

	/**
		Time without data after which a connection is given up, ms.
	*/
	void SetIdleTimeout(int idleTimeoutMs);
	/**
		How many new links a download may take after the first one.
	*/
	void SetMaxLinkRenewals(int maxLinkRenewals);
	/**
		Time between two reports of the progress, ms.
	*/
	void SetProgressInterval(int progressIntervalMs);

	/**
		\param remotePath [INPUT] - path of the file for the link provider
		\param filePath [INPUT] - the local file; it is replaced when the download completes
		\param isCanceled [INPUT] - set by another thread to stop the download; the part file stays for the next attempt
		\param progress [INPUT] - optional
		\param error [OUTPUT] - why the download failed
	*/
	Result Download(
				const QString& remotePath,
				const QString& filePath,
				const std::atomic<bool>& isCanceled,
				const ProgressFunction& progress,
				QString& error) const;

private:
	typedef imtfile::IFileDownloadLinkProvider::DownloadLink DownloadLink;

	bool RequestLink(const QString& remotePath, DownloadLink& link, QString& error) const;

private:
	const imtfile::IFileDownloadLinkProvider& m_linkProvider;
	int m_idleTimeoutMs = s_defaultIdleTimeoutMs;
	int m_maxLinkRenewals = s_defaultMaxLinkRenewals;
	int m_progressIntervalMs = s_defaultProgressIntervalMs;
};


} // namespace imtcom
