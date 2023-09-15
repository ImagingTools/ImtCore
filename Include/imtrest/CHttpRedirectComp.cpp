#include <imtrest/CHttpRedirectComp.h>


// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// reimplemented (IProtocolEngine)

ConstResponsePtr CHttpRedirectComp::ProcessRequest(const IRequest& request) const
{
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

QByteArray CHttpRedirectComp::GetSupportedCommandId() const
{
	return *m_commandIdAttrPtr;
}


} // namespace imtrest


