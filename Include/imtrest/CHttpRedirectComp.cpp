#include <imtrest/CHttpRedirectComp.h>


// ImtCore includes
#include <imthttp/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imthttp/IProtocolEngine.h>


namespace imtrest
{


// reimplemented (IProtocolEngine)

bool CHttpRedirectComp::IsCommandSupported(const QByteArray& commandId) const
{
	return commandId.isEmpty();
}


imthttp::ConstResponsePtr CHttpRedirectComp::ProcessRequest(const imthttp::IRequest& request, const QByteArray& /*subCommandId*/) const
{
	QByteArray commandId = request.GetCommandId();
	if (commandId.startsWith('/')){
		commandId = commandId.remove(0, 1);
	}

	if (commandId.endsWith('/')){
		commandId = commandId.remove(commandId.length() - 1, 1);
	}

	if (commandId.isEmpty()){
		QByteArray body;
		body = R"(<head> <meta http-equiv="refresh" content="0; URL=)";
		body.append(*m_redirectToCommandIdAttrPtr);
		body.append(R"(" /></head>)");

		imthttp::ConstResponsePtr retval = ConstResponsePtr(request.GetProtocolEngine()
			.CreateResponse(
				request,
				IProtocolEngine::SC_MOVED_PERMANENTLY,
				body,
				"text/html; charset=UTF-8").PopInterfacePtr());

		return retval;
	}

	imthttp::ConstResponsePtr retval = ConstResponsePtr(request.GetProtocolEngine()
		.CreateResponse(
			request,
			IProtocolEngine::SC_NOT_FOUND,
			QByteArray(),
			"text/html; charset=UTF-8").PopInterfacePtr());

	return retval;
}


} // namespace imtrest


