// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoBehaviorServletComp.h>


// STL includes
#include <stdexcept>

// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QThread>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>
#include <imtrestdemo/DemoServletTools.h>


namespace imtrestdemo
{


namespace
{


typedef imtrest::IProtocolEngine Engine;


bool GetStatusByCode(int code, Engine::StatusCode& status)
{
	static const QMap<int, Engine::StatusCode> statuses = {
		{200, Engine::SC_OK},
		{201, Engine::SC_CREATED},
		{202, Engine::SC_ACCEPTED},
		{204, Engine::SC_NO_CONTENT},
		{205, Engine::SC_RESET_CONTENT},
		{301, Engine::SC_MOVED_PERMANENTLY},
		{302, Engine::SC_MOVED_TEMPORARILY},
		{304, Engine::SC_NOT_MODIFIED},
		{400, Engine::SC_BAD_REQUEST},
		{401, Engine::SC_UNAUTHORIZED},
		{403, Engine::SC_FORBIDDEN},
		{404, Engine::SC_NOT_FOUND},
		{409, Engine::SC_CONFLICT},
		{412, Engine::SC_PRECONDITION_FAILED},
		{413, Engine::SC_PAYLOAD_TOO_LARGE},
		{416, Engine::SC_RANGE_NOT_SATISFIABLE},
		{418, Engine::SC_I_AM_A_TEAPOT},
		{500, Engine::SC_INTERNAL_SERVER_ERROR},
		{503, Engine::SC_SERVICE_UNAVAILABLE}};

	QMap<int, Engine::StatusCode>::ConstIterator iter = statuses.constFind(code);
	if (iter == statuses.constEnd()){
		return false;
	}

	status = iter.value();

	return true;
}


imtrest::ConstResponsePtr CreateBehaviorResponse(const imtrest::IRequest::CommandParams& params, const imtrest::CHttpRequest& request)
{
	const Engine& engine = request.GetProtocolEngine();
	const QByteArrayList arguments = DemoServletTools::GetPathArguments(request);
	const QByteArray caseId = arguments.value(0);

	if (caseId == "status" && arguments.count() == 2){
		Engine::StatusCode status = Engine::SC_NONE;
		if (!GetStatusByCode(arguments.at(1).toInt(), status)){
			return DemoServletTools::CreateTextResponse(engine, Engine::SC_BAD_REQUEST, "The status is not supported");
		}

		imtrest::IResponse::Headers headers;
		if (status == Engine::SC_MOVED_PERMANENTLY || status == Engine::SC_MOVED_TEMPORARILY){
			headers.insert("Location", "/echo");
		}

		if (status == Engine::SC_UNAUTHORIZED){
			headers.insert("WWW-Authenticate", "Bearer realm=\"imtrestdemo\"");
		}

		return DemoServletTools::CreateResponse(engine, status, "status " + arguments.at(1), "text/plain; charset=utf-8", headers);
	}

	if (caseId == "throw"){
		throw std::runtime_error("The demo servlet failed on purpose");
	}

	if (caseId == "sleep"){
		const int delayMs = qBound(0, params.value("ms").toInt(), 60000);

		// The wait is cut when a server stops, so the worker does not outlive the components
		const int stopCount = DemoServletTools::GetServerStopCount();
		QElapsedTimer timer;
		timer.start();
		while (timer.elapsed() < delayMs && stopCount == DemoServletTools::GetServerStopCount()){
			QThread::msleep(10);
		}

		return DemoServletTools::CreateTextResponse(engine, Engine::SC_OK, QStringLiteral("slept %1 ms").arg(delayMs));
	}

	if (caseId == "headers"){
		imtrest::IResponse::Headers headers;
		headers.insert("X-Demo-Header", "demo value");
		// The server controls the length and the connection: these values must be replaced or removed
		headers.insert("Content-Length", "99999");
		headers.insert("Connection", "keep-alive");
		// A value with a line break would add a header to the response
		headers.insert("X-Demo-Injected", "value\r\nX-Injected: yes");

		return DemoServletTools::CreateResponse(engine, Engine::SC_OK, "headers", "text/plain; charset=utf-8", headers);
	}

	if (caseId == "big"){
		const qint64 size = params.value("size").toLongLong();
		if (size < 0 || size > CDemoBehaviorServletComp::s_maxBigBodySize){
			return DemoServletTools::CreateTextResponse(engine, Engine::SC_BAD_REQUEST, QStringLiteral("size is from 0 to %1").arg(CDemoBehaviorServletComp::s_maxBigBodySize));
		}

		return DemoServletTools::CreateResponse(engine, Engine::SC_OK, DemoProtocol::CreatePatternData(0, size), "application/octet-stream");
	}

	if (caseId == "empty"){
		return DemoServletTools::CreateResponse(engine, Engine::SC_OK, QByteArray(), "text/plain; charset=utf-8");
	}

	return DemoServletTools::CreateTextResponse(engine, Engine::SC_NOT_FOUND, "Unknown case of the behavior");
}


} // namespace


// protected methods

// reimplemented (imtrest::CHttpServletCompBase)

imtrest::ConstResponsePtr CDemoBehaviorServletComp::OnGet(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	return CreateBehaviorResponse(commandParams, request);
}


imtrest::ConstResponsePtr CDemoBehaviorServletComp::OnHead(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	return CreateBehaviorResponse(commandParams, request);
}


} // namespace imtrestdemo
