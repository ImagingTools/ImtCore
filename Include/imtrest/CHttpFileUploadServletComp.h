// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>
#include <imtrest/CHttpResponse.h>
#include <imtservergql/IFileUploadHandler.h>


namespace imtrest
{


class CHttpFileUploadServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpFileUploadServletComp)
		I_ASSIGN(m_fileUploadHandlerPtr, "FileUploadHandler", "File upload handler", false, "FileUploadHandlerPtr");
	I_END_COMPONENT

	/**
		Get the range of an uploaded chunk from the value of the \c Content-Range header.
		The positions of the header are inclusive (RFC 9110, section 14.4): the first chunk of 1000 bytes of a file
		of 5000 bytes is \c bytes \c 0-999/5000. The range of the chunk is returned in the same form, as it is used by
		imtservergql::IFileUploadHandler.
		The header is rejected if the size of the file is unknown, if the range does not fit in the file or if the
		number of bytes of the range differs from the size of the chunk.
		\param	contentRange	Value of the header.
		\param	chunkSize		Number of the received bytes.
		\param	range			Inclusive range of the chunk in the file.
		\param	fileSize		Size of the file.
	*/
	static bool GetChunkRange(const QByteArray& contentRange, qint64 chunkSize, imtservergql::IFileUploadHandler::ByteRange& range, qint64& fileSize);

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual ConstResponsePtr OnPut(
		const QByteArray& commandId,
		const imtrest::IRequest::CommandParams& commandParams,
		const HeadersMap& headers,
		const imtrest::CHttpRequest& request) const override;

private:
	CHttpResponse::Headers GetRequestHeaders(const CHttpRequest& request) const;
	ConstResponsePtr CreateResponse(
		const IRequest& request,
		int statusCode,
		const imtrest::CHttpResponse::Headers& headers,
		const QByteArray& data,
		const QByteArray& dataTypeId) const;

private:
	I_REF(imtservergql::IFileUploadHandler, m_fileUploadHandlerPtr);

	static QByteArray s_plainTextReponseTypeId;
	static QByteArray s_octetStreamTypeId;
};


} // namespace imtrest


