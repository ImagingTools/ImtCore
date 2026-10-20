// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QString>

// ImtCore includes
#include <imtrest/CHttpConditionalTools.h>
#include <imtrest/CHttpServletCompBase.h>
#include <imtrest/IHttpFileResolver.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IResponse.h>


namespace imtrest
{


/**
	Servlet that sends files. The file is sent by parts without loading it into memory (see IStreamingResponse),
	the requests are processed as RFC 9110 requires:
	- \c GET sends the file, \c HEAD sends the same headers without the body;
	- \c Range and \c If-Range: a part of the file is sent with 206, a range that is outside of the file gets 416
	  (a header with several ranges is answered with the whole file: this is allowed by RFC 9110, section 14.2);
	- the conditions \c If-Match, \c If-None-Match, \c If-Modified-Since and \c If-Unmodified-Since give 304, 412 or 400.

	Which file is sent and who can get it is decided by the resolver (see IHttpFileResolver), it is the reference \c FileResolver.
	Without the resolver every request gets 500.

	The size of the file and the time of its last change are taken from the file system. The entity tag is made of them if
	the resolver does not set its own one.
	If the file is changed while the response is being sent, the connection is aborted, because the announced length does not
	match the data anymore: the client sees the broken response and does not take it for a complete file.
*/
class CHttpFileServletComp: public CHttpServletCompBase
{
public:
	typedef CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpFileServletComp);
		I_ASSIGN(m_fileResolverCompPtr, "FileResolver", "Finds the file of a request and checks the access to it", true, "FileResolver");
	I_END_COMPONENT;

	/**
		The file and the headers of the request that decide how the file is sent. It is separated from the request to check
		the decisions without a connection.
	*/
	struct FileRepresentation
	{
		QString filePath;
		qint64 size = 0;
		QDateTime lastModified;
		QByteArray eTag;
		QByteArray dataTypeId;
		IResponse::Headers headers;
	};

	/**
		Values of the headers of the request that decide how the file is sent. An empty value means that the header is absent.
	*/
	struct RequestConditions
	{
		CHttpConditionalTools::Preconditions preconditions;
		QByteArray ifRange;
		QByteArray range;
	};

	/**
		Create the entity tag of a file from its size and the time of the last change with the resolution of a millisecond.
		The tag changes when the file is replaced or edited, so it is a strong validator for the ranges (\c If-Range).
		\return The value of \c ETag with the quotes.
	*/
	static QByteArray CreateDefaultETag(qint64 size, const QDateTime& lastModified);

	/**
		Decide how a file is sent and create the response: 200, 206, 304, 400, 412 or 416.
		The body is not read here: a response with the body (200 and 206) is a streaming response that opens the file
		when the body is sent. The method does not check that the file exists.
		\param	isHeadRequest	The method is \c HEAD (otherwise \c GET): the same headers without the body, \c Range is ignored.
	*/
	static ConstResponsePtr CreateFileResponse(
				const IProtocolEngine& engine,
				bool isHeadRequest,
				const RequestConditions& conditions,
				const FileRepresentation& file);

	/**
		Process the request of a file: ask the resolver, take the size and the time of the file from the file system and
		create the response, see CreateFileResponse(). The refusal of the resolver, a file that does not exist
		(or is a directory) and a file that cannot be read are answered without a body.
	*/
	static ConstResponsePtr ProcessFileRequest(const IHttpFileResolver& resolver, const imtrest::CHttpRequest& request, bool isHeadRequest);

protected:
	// reimplemented (CHttpServletCompBase)
	virtual ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
	virtual ConstResponsePtr OnHead(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;

private:
	ConstResponsePtr ProcessRequestWithResolver(const imtrest::CHttpRequest& request, bool isHeadRequest) const;

private:
	I_REF(IHttpFileResolver, m_fileResolverCompPtr);
};


} // namespace imtrest


