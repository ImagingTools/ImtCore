// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>


namespace imtrestdemo
{


class CDemoResumableUploadSink;


/**
	Servlet \c /upload/<name>: the files that the server receives. The body of \c PUT and \c POST is received by the policy
	(see CDemoBodyPolicyComp) directly to the file, so it never is in memory; the servlet answers with the size and
	SHA-256 of the saved file and with \c 201. \c GET gives the list of the files, \c DELETE removes a file.

	An upload that can be resumed: \c PUT with \c Content-Range: bytes first-last/total sends a part of the file (see CDemoResumableUploadSink).
	The part must start where the data of the server end, otherwise the answer is \c 409 with \c Upload-Offset (where the part must start) and
	\c Upload-State (\c offset-mismatch, \c total-mismatch or \c in-progress: another request writes the file now). The part that is
	not the last one is answered with \c 202 and \c Upload-Offset, the last one with \c 201 and the SHA-256 of the whole file.
	If the connection breaks, the received data stay. \c GET /upload/<name> tells how much the server has (\c received, \c total,
	\c complete, headers \c Upload-Offset and \c Upload-Length, \c 404 if there is nothing), \c DELETE removes the file and the data of
	the upload that is not finished.
	The uploaded file can be downloaded with \c /files/uploaded/<name> (ranges, conditions).
*/
class CDemoUploadServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CDemoUploadServletComp);
		I_ASSIGN(m_uploadDirectoryAttrPtr, "UploadDirectory", "Directory of the uploaded files", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual imtrest::ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
	virtual imtrest::ConstResponsePtr OnPut(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
	virtual imtrest::ConstResponsePtr OnPost(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
	virtual imtrest::ConstResponsePtr OnDelete(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;

private:
	imtrest::ConstResponsePtr SaveFile(const imtrest::CHttpRequest& request) const;
	imtrest::ConstResponsePtr CreatePartResponse(const imtrest::CHttpRequest& request, const CDemoResumableUploadSink& sink) const;
	imtrest::ConstResponsePtr CreateStateResponse(const imtrest::CHttpRequest& request, const QString& name) const;

private:
	I_ATTR(QString, m_uploadDirectoryAttrPtr);
};


} // namespace imtrestdemo


