// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <atomic>
#include <memory>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IHttpRequestBodyPolicy.h>


namespace imtrest
{


/**
	Policy of the body that chooses how to receive the body by its size: a small body is received to the memory (the handler reads
	it with CHttpRequest::GetBody()), a big one is written to a file by parts (the handler gets the file from
	CHttpRequest::GetBodySink(), see CHttpTemporaryFileBodySink), so the memory of the server does not depend on the size of the uploads.
	The size is read from the headers, the decision is made before the body is received:
	- the body that is not bigger than \c MemoryBodyLimit (100 MiB by default) goes to the memory;
	- the bigger body goes to a file of the directory \c BodyDirectory, if it fits there: the body is saved only if the share
	  \c FreeSpaceReservePercent (10% by default) of the disk stays free after that. Otherwise the request is refused with \c 413 before
	  the body is received, the client does not send it;
	- the size of the body that is bigger than \c MaxBodySize (if it is set) is not accepted at all, in any mode.
	The size of the body that is received with \c Transfer-Encoding: chunked is not known in advance, so such a body always goes to
	a file, and the limits are checked while it is received: the request is refused with \c 413 as soon as the body is bigger than the limit.
	The bodies of the requests without a body (\c GET) and the requests with \c Content-Length: 0 do not use the disk.

	The free space of the disk is read for every request that goes to a file. Many requests can be received at the same time, so
	the space of the bodies that are received to the disk and are not written yet is counted too: the second upload does not get
	the space that the first one is going to use. The size of a chunked body is not known, such a body does not hold the space.
	The limit is the best effort: the other programs can use the disk while the body is received.

	The policy is thread-safe, as the requests are received in many threads at the same time.
*/
class CHttpDynamicBodyPolicyComp:
			public ilog::CLoggerComponentBase,
			virtual public IHttpRequestBodyPolicy
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpDynamicBodyPolicyComp);
		I_REGISTER_INTERFACE(IHttpRequestBodyPolicy);
		I_ASSIGN(m_memoryBodyLimitAttrPtr, "MemoryBodyLimit", "Maximal size of the body in bytes that is received to the memory (100 MiB by default). A bigger body is written to a file. 0 - every body that is not empty goes to a file", true, 100 * 1024 * 1024);
		I_ASSIGN(m_maxBodySizeAttrPtr, "MaxBodySize", "Maximal size of the body in bytes in any mode, a bigger body is refused with 413 before it is received. The free space of the disk limits the bodies that go to a file in any case. 0 - there is no other limit", true, 0);
		I_ASSIGN(m_freeSpaceReservePercentAttrPtr, "FreeSpaceReservePercent", "Share of the disk in percent that stays free after the body is written to a file (10 by default): a body is accepted only if it fits into the free space without this share. 0 - the disk can be filled up completely", true, 10);
		I_ASSIGN(m_bodyDirectoryAttrPtr, "BodyDirectory", "Directory of the files of the bodies that are bigger than MemoryBodyLimit, the free space of its disk limits the size of the bodies. The directory is created if it does not exist. Empty - the directory of the temporary files of the system", true, "");
	I_END_COMPONENT;

	CHttpDynamicBodyPolicyComp();

	/**
		Get the size of the body that the headers announce (RFC 9112, section 6.3).
		\param	contentLength		Value of \c Content-Length, empty if there is no such header.
		\param	transferEncoding	Value of \c Transfer-Encoding, empty if there is no such header. It overrides \c Content-Length.
		\return	Size of the body in bytes; 0 if the request has neither of the headers (a request without a body); \c -1 if the size is
				not known: the body is chunked or \c Content-Length is not a valid number.
	*/
	static qint64 GetAnnouncedBodySize(const QByteArray& contentLength, const QByteArray& transferEncoding);

	/**
		Calculate the maximal size of a body that can be written to the disk so that the share of the disk stays free.
		\param	totalBytes			Size of the disk.
		\param	availableBytes		Free space of the disk that a program can use.
		\param	reservePercent		Share of the size of the disk that should stay free after the body is written, from 0 to 100.
								A value out of this range is cut to it.
		\return	Size of the body in bytes, not less than 0. 0 - no body fits.
	*/
	static qint64 CalculateFileBodyLimit(qint64 totalBytes, qint64 availableBytes, int reservePercent);

	/**
		Get the size of the space of the disk that the bodies that are received to files now are going to use, they have not written it yet.
	*/
	qint64 GetReservedSize() const;

	// reimplemented (IHttpRequestBodyPolicy)
	virtual BodyDecision DecideBody(const CHttpRequest& requestHead) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	/**
		Decide about the body that does not go to the memory.
		\param	announcedSize	Size of the body, \c -1 if it is not known.
		\param	maxBodySize		Limit of the size of the body that is set by the attribute, 0 - there is no limit.
	*/
	BodyDecision DecideFileBody(const CHttpRequest& requestHead, qint64 announcedSize, qint64 maxBodySize) const;
	bool GetDiskSpace(qint64& totalBytes, qint64& availableBytes) const;
	static BodyDecision CreateRejection(int statusCode);

private:
	I_ATTR(int, m_memoryBodyLimitAttrPtr);
	I_ATTR(int, m_maxBodySizeAttrPtr);
	I_ATTR(int, m_freeSpaceReservePercentAttrPtr);
	I_ATTR(QString, m_bodyDirectoryAttrPtr);

	// Set when the component is created, the policy is used from many threads and does not change it after that.
	QString m_directoryPath;

	// The receivers of the bodies keep it too: they release the space after the policy is destroyed.
	std::shared_ptr<std::atomic<qint64>> m_reservedSizePtr;
};


} // namespace imtrest


