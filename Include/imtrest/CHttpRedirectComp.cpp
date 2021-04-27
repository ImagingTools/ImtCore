#include <imtrest/CHttpRedirectComp.h>


// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/IProtocolEngine.h>



namespace imtrest
{


// reimplemented (IProtocolEngine)


// reimplemented (icomp::CComponentBase)

void CHttpRedirectComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}

CHttpRedirectComp::ConstResponsePtr CHttpRedirectComp::ProcessRequest(const IRequest& request) const
{
	QByteArray body;
	body = R"(<head> <meta http-equiv="refresh" content="0; URL=)";
	body.append(*m_redirectToCommandID);
	body.append(R"(" /></head>)");

	ConstResponsePtr retval = ConstResponsePtr(request.GetProtocolEngine()
											   .CreateResponse(request,
															   IProtocolEngine::SC_MOVED_PERMANENTLY,
															   body,
															   "text/html; charset=UTF-8")
											   );
//	const CHttpResponse *httpResp = dynamic_cast<const CHttpResponse*>(retval.GetPtr());
//	if(httpResp != nullptr)
//	{
//		httpResp->
//	}
	return retval;
}

QByteArray CHttpRedirectComp::GetSupportedCommandId() const
{
	return *m_commandIdAttrPtr;
}


} // namespace imtrest


