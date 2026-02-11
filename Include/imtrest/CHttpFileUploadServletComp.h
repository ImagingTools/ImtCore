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

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual ConstResponsePtr OnPut(
		const QByteArray& commandId,
		const imtrest::IRequest::CommandParams& commandParams,
		const HeadersMap& headers,
		const imtrest::CHttpRequest& request) const override;

private:
	struct RangeData
	{
		QString units;
		qsizetype startIndex = -1;
		qsizetype endIndex = -1;
		qsizetype totalSize = -1;
	};

private:
	RangeData GetRangeDataFromString(const QString& dataString) const;
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


