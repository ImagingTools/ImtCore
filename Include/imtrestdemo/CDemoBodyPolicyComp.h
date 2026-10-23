// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtrest/IHttpRequestBodyPolicy.h>


namespace imtrestdemo
{


/**
	Policy of the body of the demo server, the mode is chosen by the attribute so the same component gives the different
	rules of the paths (see imtrest::CHttpBodyPolicyRouterComp):
	- \c buffer: the body is received to the memory with the limit \c MaxBodySize (413 before the body is read if the
	  announced length is bigger);
	- \c stream: the body of \c PUT and \c POST is written to a file of the upload directory, the name of the file is the last
	  segment of the path (see CDemoUploadSink);
	- \c reject: every request is refused at once with \c RejectStatusCode, the body is never received.
	If \c RequiredToken is set, a request without the header \c Authorization: Bearer <token> is refused with 401 before
	the body is received, in any mode.
*/
class CDemoBodyPolicyComp:
			public icomp::CComponentBase,
			virtual public imtrest::IHttpRequestBodyPolicy
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDemoBodyPolicyComp);
		I_REGISTER_INTERFACE(imtrest::IHttpRequestBodyPolicy);
		I_ASSIGN(m_modeAttrPtr, "Mode", "Mode of the policy: buffer, stream or reject", true, "buffer");
		I_ASSIGN(m_maxBodySizeAttrPtr, "MaxBodySize", "Limit of the body in bytes: less than 0 - the limit of the server, 0 - no limit", true, -1);
		I_ASSIGN(m_rejectStatusCodeAttrPtr, "RejectStatusCode", "HTTP status of the refusal in the mode 'reject' (400, 401, 403, 404, 409, 413, 500 or 503)", true, 403);
		I_ASSIGN(m_requiredTokenAttrPtr, "RequiredToken", "Token of the header Authorization: Bearer, empty - the access is free", true, "");
		I_ASSIGN(m_uploadDirectoryAttrPtr, "UploadDirectory", "Directory of the uploaded files in the mode 'stream'", true, "");
	I_END_COMPONENT;

	/**
		Get the name of the uploaded file from the path of the request: the last segment of the path.
		\return Empty string if the name is not allowed (hidden file, separators, empty name).
	*/
	static QString GetUploadFileName(const QByteArray& commandId);

	// reimplemented (imtrest::IHttpRequestBodyPolicy)
	virtual BodyDecision DecideBody(const imtrest::CHttpRequest& requestHead) const override;

private:
	BodyDecision DecideRangedUpload(const imtrest::CHttpRequest& requestHead, const QString& fileName, const QByteArray& contentRange) const;
	static BodyDecision CreateRejection(int statusCode);
	static int ToEngineStatusCode(int httpStatusCode);

private:
	I_ATTR(QByteArray, m_modeAttrPtr);
	I_ATTR(int, m_maxBodySizeAttrPtr);
	I_ATTR(int, m_rejectStatusCodeAttrPtr);
	I_ATTR(QByteArray, m_requiredTokenAttrPtr);
	I_ATTR(QString, m_uploadDirectoryAttrPtr);
};


} // namespace imtrestdemo


