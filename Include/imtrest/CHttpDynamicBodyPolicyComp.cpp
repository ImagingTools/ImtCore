// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpDynamicBodyPolicyComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QStorageInfo>

// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpTemporaryFileBodySink.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


namespace
{


/**
	Receiver of a body to a temporary file that holds the space of the disk for the body while it is received.
	The space is held in the counter of the policy until the body is written, so the other requests do not take it: the free space of
	the disk is reduced only when the bytes are written. A written byte is taken off the counter, because the disk counts it itself now.
	The space is released when the body is received, refused or broken, and also when the receiver is destroyed.
*/
class CReservingFileBodySink: public CHttpTemporaryFileBodySink
{
public:
	typedef CHttpTemporaryFileBodySink BaseClass;

	CReservingFileBodySink(const QString& directory, const std::shared_ptr<std::atomic<qint64>>& reservedSizePtr)
		:BaseClass(directory),
		m_reservedSizePtr(reservedSizePtr)
	{
	}

	~CReservingFileBodySink()
	{
		ReleaseReservedSize();
	}

	/**
		Hold the space for the body if it fits into the limit together with the space that the other bodies hold.
		The check and the holding are one atomic operation, so the requests that are received at the same time cannot hold more than the limit.
	*/
	bool TryReserveSize(qint64 size, qint64 limit)
	{
		qint64 reservedSize = m_reservedSizePtr->load();
		do{
			if (size > limit - reservedSize){
				return false;
			}
		} while (!m_reservedSizePtr->compare_exchange_weak(reservedSize, reservedSize + size));

		m_ownReservedSize += size;

		return true;
	}

	// reimplemented (IHttpRequestBodySink)
	virtual bool Write(const char* data, qint64 size) override
	{
		if (!BaseClass::Write(data, size)){
			return false;
		}

		const qint64 writtenSize = qMin(size, m_ownReservedSize);
		m_ownReservedSize -= writtenSize;
		m_reservedSizePtr->fetch_sub(writtenSize);

		return true;
	}

	virtual bool Finish() override
	{
		const bool isFinished = BaseClass::Finish();

		ReleaseReservedSize();

		return isFinished;
	}

	virtual void Abort() override
	{
		BaseClass::Abort();

		ReleaseReservedSize();
	}

private:
	void ReleaseReservedSize()
	{
		m_reservedSizePtr->fetch_sub(m_ownReservedSize);
		m_ownReservedSize = 0;
	}

private:
	std::shared_ptr<std::atomic<qint64>> m_reservedSizePtr;
	qint64 m_ownReservedSize = 0;
};


} // namespace


// public methods

CHttpDynamicBodyPolicyComp::CHttpDynamicBodyPolicyComp()
	:m_reservedSizePtr(std::make_shared<std::atomic<qint64>>(0))
{
}


qint64 CHttpDynamicBodyPolicyComp::GetAnnouncedBodySize(const QByteArray& contentLength, const QByteArray& transferEncoding)
{
	// Transfer-Encoding overrides Content-Length (RFC 9112, section 6.3), the size of a chunked body is not known
	if (!transferEncoding.trimmed().isEmpty()){
		return -1;
	}

	const QByteArray value = contentLength.trimmed();
	if (value.isEmpty()){
		return 0;
	}

	// The size is a sequence of digits (RFC 9110, section 8.6), without a sign and without anything else
	for (const char symbol : value){
		if (symbol < '0' || symbol > '9'){
			return -1;
		}
	}

	bool isNumber = false;
	const qint64 size = value.toLongLong(&isNumber);

	return isNumber ? size : -1;
}


qint64 CHttpDynamicBodyPolicyComp::CalculateFileBodyLimit(qint64 totalBytes, qint64 availableBytes, int reservePercent)
{
	if (totalBytes < 0 || availableBytes <= 0){
		return 0;
	}

	const qint64 percent = qBound(qint64(0), qint64(reservePercent), qint64(100));

	// The share is calculated in two parts: the product of the size of a big disk and the percent does not fit into 64 bits
	const qint64 reservedBytes = (totalBytes / 100) * percent + ((totalBytes % 100) * percent) / 100;

	return qMax<qint64>(0, availableBytes - reservedBytes);
}


qint64 CHttpDynamicBodyPolicyComp::GetReservedSize() const
{
	return m_reservedSizePtr->load();
}


// reimplemented (IHttpRequestBodyPolicy)

IHttpRequestBodyPolicy::BodyDecision CHttpDynamicBodyPolicyComp::DecideBody(const CHttpRequest& requestHead) const
{
	const qint64 announcedSize = GetAnnouncedBodySize(requestHead.GetHeaderValue("content-length"), requestHead.GetHeaderValue("transfer-encoding"));

	// 0 is no limit
	const qint64 maxBodySize = qMax<qint64>(0, *m_maxBodySizeAttrPtr);
	if (maxBodySize > 0 && announcedSize > maxBodySize){
		return CreateRejection(IProtocolEngine::SC_PAYLOAD_TOO_LARGE);
	}

	const qint64 memoryLimit = qMax<qint64>(0, *m_memoryBodyLimitAttrPtr);
	if (announcedSize >= 0 && announcedSize <= memoryLimit){
		BodyDecision decision;
		decision.mode = BM_BUFFER;

		// For the request 0 is no limit, but the empty body does not need it
		decision.maxBodySize = qMax<qint64>(memoryLimit, 1);

		return decision;
	}

	return DecideFileBody(requestHead, announcedSize, maxBodySize);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CHttpDynamicBodyPolicyComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// The path is read once: the policy is used from many threads, so the directory must not change while it works
	const QString path = m_bodyDirectoryCompPtr.IsValid() ? m_bodyDirectoryCompPtr->GetPath() : QString();
	if (path.isEmpty()){
		SendErrorMessage(0, QStringLiteral("The directory of the bodies is not set, the requests with a body that does not go to the memory will be refused"));

		return;
	}

	m_directoryPath = QDir(path).absolutePath();

	// The directory is used only when a big body comes, but a mistake of the settings should be seen when the server starts
	if (!QDir().mkpath(m_directoryPath)){
		SendWarningMessage(0, QStringLiteral("The directory '%1' of the bodies cannot be created, the requests with a body that does not go to the memory will be refused").arg(m_directoryPath));
	}
}


// private methods

IHttpRequestBodyPolicy::BodyDecision CHttpDynamicBodyPolicyComp::DecideFileBody(const CHttpRequest& requestHead, qint64 announcedSize, qint64 maxBodySize) const
{
	qint64 totalBytes = 0;
	qint64 availableBytes = 0;
	if (!GetDiskSpace(totalBytes, availableBytes)){
		SendErrorMessage(0, QStringLiteral("The free space of the directory '%1' is not known, the body of the request '%2' is refused").arg(m_directoryPath, QString::fromUtf8(requestHead.GetCommandId())));

		return CreateRejection(IProtocolEngine::SC_INTERNAL_SERVER_ERROR);
	}

	const qint64 diskLimit = CalculateFileBodyLimit(totalBytes, availableBytes, *m_freeSpaceReservePercentAttrPtr);

	// The receiver holds the space from the moment it is created, so the space is released on every way out of the method
	const std::shared_ptr<CReservingFileBodySink> sinkPtr = std::make_shared<CReservingFileBodySink>(m_directoryPath, m_reservedSizePtr);

	qint64 bodyLimit = diskLimit;
	if (announcedSize >= 0){
		if (!sinkPtr->TryReserveSize(announcedSize, diskLimit)){
			SendWarningMessage(0, QStringLiteral("The body of %1 bytes of the request '%2' is refused: not more than %3 bytes of the disk of '%4' can be used for it")
						.arg(announcedSize)
						.arg(QString::fromUtf8(requestHead.GetCommandId()))
						.arg(qMax<qint64>(0, diskLimit - m_reservedSizePtr->load()))
						.arg(m_directoryPath));

			return CreateRejection(IProtocolEngine::SC_PAYLOAD_TOO_LARGE);
		}
	}
	else{
		// The size is not known, so the limit is checked while the body is received. The space is not held, but the one that is held by the others is not used
		bodyLimit = qMax<qint64>(0, diskLimit - m_reservedSizePtr->load());
	}

	if (maxBodySize > 0){
		bodyLimit = qMin(bodyLimit, maxBodySize);
	}

	// The request takes 0 as no limit, so a disk without free space must be refused here
	if (bodyLimit <= 0){
		SendWarningMessage(0, QStringLiteral("The body of the request '%1' is refused: there is no free space on the disk of '%2'")
					.arg(QString::fromUtf8(requestHead.GetCommandId()), m_directoryPath));

		return CreateRejection(IProtocolEngine::SC_PAYLOAD_TOO_LARGE);
	}

	// The client should not send the body that cannot be saved
	if (!sinkPtr->Open()){
		SendErrorMessage(0, QStringLiteral("The file of the body of the request '%1' cannot be created in '%2': %3")
					.arg(QString::fromUtf8(requestHead.GetCommandId()), m_directoryPath, sinkPtr->GetErrorString()));

		return CreateRejection(IProtocolEngine::SC_INTERNAL_SERVER_ERROR);
	}

	BodyDecision decision;
	decision.mode = BM_STREAM;
	decision.maxBodySize = bodyLimit;
	decision.sinkPtr = sinkPtr;

	return decision;
}


bool CHttpDynamicBodyPolicyComp::GetDiskSpace(qint64& totalBytes, qint64& availableBytes) const
{
	if (m_directoryPath.isEmpty()){
		return false;
	}

	// The object is created for every call: it reads the actual state of the disk and it is not shared between the threads
	const QStorageInfo storage(m_directoryPath);
	if (!storage.isValid() || !storage.isReady()){
		return false;
	}

	totalBytes = storage.bytesTotal();
	availableBytes = storage.bytesAvailable();

	return (totalBytes >= 0) && (availableBytes >= 0);
}


IHttpRequestBodyPolicy::BodyDecision CHttpDynamicBodyPolicyComp::CreateRejection(int statusCode)
{
	BodyDecision decision;
	decision.mode = BM_REJECT;
	decision.rejectStatusCode = statusCode;

	return decision;
}


} // namespace imtrest
