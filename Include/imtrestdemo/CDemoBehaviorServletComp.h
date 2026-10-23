// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>


namespace imtrestdemo
{


/**
	Servlet \c /behavior/<case>: the answers that a server gives in the special situations, to see how the clients and
	the server itself behave:
	- \c status/<code>: an answer with the status (200, 201, 202, 204, 205, 301, 302, 304, 400, 401, 403, 404, 409, 412, 413, 416, 418, 500, 503);
	- \c throw: the servlet throws an exception, the server must answer with 500 and stay alive;
	- \c sleep?ms=<ms>: the servlet works for the time (if the server limits the processing time, the answer is 503);
	- \c headers: the servlet sets the headers that the server must control: its own \c Content-Length and \c Connection are replaced,
	  a header value with a line break (the injection of a header) is not sent;
	- \c big?size=<bytes>: a big body that is completely in memory (not a streaming response), up to 64 MiB;
	- \c empty: 200 with an empty body.
*/
class CDemoBehaviorServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CDemoBehaviorServletComp);
	I_END_COMPONENT;

	static const int s_maxBigBodySize = 64 * 1024 * 1024;

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual imtrest::ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
	virtual imtrest::ConstResponsePtr OnHead(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
};


} // namespace imtrestdemo


