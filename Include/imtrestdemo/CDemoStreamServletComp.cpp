// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoStreamServletComp.h>


// STL includes
#include <memory>

// ImtCore includes
#include <imtrest/CHttpStreamingResponse.h>
#include <imtrestdemo/CDemoGeneratedBodySource.h>
#include <imtrestdemo/DemoServletTools.h>


namespace imtrestdemo
{


namespace
{


imtrest::ConstResponsePtr CreateStreamResponse(const imtrest::IRequest::CommandParams& params, const imtrest::CHttpRequest& request)
{
	typedef imtrest::IProtocolEngine Engine;

	const QByteArrayList arguments = DemoServletTools::GetPathArguments(request);

	bool isSizeValid = false;
	const qint64 size = arguments.count() == 1 ? arguments.first().toLongLong(&isSizeValid) : 0;
	if (!isSizeValid || size < 0){
		return DemoServletTools::CreateTextResponse(request.GetProtocolEngine(), Engine::SC_BAD_REQUEST, "Use /stream/<size in bytes>");
	}

	const qint64 partSize = qMax<qint64>(0, params.value("part").toLongLong());
	const int delayMs = qMax(0, params.value("delay").toInt());
	const bool isChunked = params.value("chunked") == "1";

	// A body of unknown size is sent with Transfer-Encoding: chunked
	std::unique_ptr<imtrest::CHttpStreamingResponse> responsePtr(new imtrest::CHttpStreamingResponse(
				Engine::SC_OK,
				"application/octet-stream",
				request.GetProtocolEngine(),
				isChunked ? -1 : size,
				[size, partSize, delayMs](){
		return imtrest::IResponseBodySourcePtr(new CDemoGeneratedBodySource(size, partSize, delayMs));
	}));

	imtrest::IResponse::Headers headers;
	headers.insert("X-Demo-Size", QByteArray::number(size));
	headers.insert("Cache-Control", "no-store");
	responsePtr->SetHeaders(headers);

	return imtrest::ConstResponsePtr(responsePtr.release());
}


} // namespace


// protected methods

// reimplemented (imtrest::CHttpServletCompBase)

imtrest::ConstResponsePtr CDemoStreamServletComp::OnGet(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	return CreateStreamResponse(commandParams, request);
}


imtrest::ConstResponsePtr CDemoStreamServletComp::OnHead(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	return CreateStreamResponse(commandParams, request);
}


} // namespace imtrestdemo
