// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>


namespace imtrestdemo
{


/**
	Servlet \c /stream/<size>: sends a body that is generated while it is sent, so a gigabyte body does not take the memory
	(see imtrest::IStreamingResponse). The bytes are given by DemoProtocol::GetPatternByte(), a client checks them by position.
	The query:
	- \c part=<bytes>: the size of one part that the source gives (the default is the size that the streamer asks);
	- \c delay=<ms>: the pause after every part, the body is produced slowly (the source waits for the data);
	- \c chunked=1: the size is not announced, the body is sent with \c Transfer-Encoding: chunked.
	\c HEAD gives the same headers without the body.
*/
class CDemoStreamServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CDemoStreamServletComp);
	I_END_COMPONENT;

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


