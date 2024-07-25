#include <imtrest/CHttpRedirectComp.h>


// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// reimplemented (IProtocolEngine)

bool CHttpRedirectComp::IsCommandSupported(const QByteArray& commandId) const
{
	return commandId.isEmpty();
}


ConstResponsePtr CHttpRedirectComp::ProcessRequest(const IRequest& request, const QByteArray& /*subCommandId*/) const
{
	QByteArray commandId = request.GetCommandId();
	if (commandId.startsWith('/')) {
		commandId = commandId.remove(0, 1);
	}

	if (commandId.endsWith('/')) {
		commandId = commandId.remove(commandId.length() - 1, 1);
	}

	if (commandId.isEmpty()) {
		QByteArray body;
		body = R"(<head> <meta http-equiv="refresh" content="0; URL=)";
		body.append(*m_redirectToCommandIdAttrPtr);
		body.append(R"(" /></head>)");

		ConstResponsePtr retval = ConstResponsePtr(request.GetProtocolEngine()
			.CreateResponse(
				request,
				IProtocolEngine::SC_MOVED_PERMANENTLY,
				body,
				"text/html; charset=UTF-8"));

		return retval;
	}

	ConstResponsePtr retval = ConstResponsePtr(request.GetProtocolEngine()
		.CreateResponse(
			request,
			IProtocolEngine::SC_NOT_FOUND,
			QByteArray(),
			"text/html; charset=UTF-8"));

	return retval;
}


} // namespace imtrest


