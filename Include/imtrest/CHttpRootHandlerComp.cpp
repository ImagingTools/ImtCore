#include <imtrest/CHttpRootHandlerComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponder.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

// reimplemented (IProtocolEngine)

bool CHttpRootHandlerComp::ProcessRequest(const IRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();

	istd::TDelPtr<IResponse> responsePtr(engine.CreateResponse(request, QByteArray("<html><head><title>A Simple HTML Document</title></head><body><p>This is a very simple HTML document</p><p>It only has two paragraphs</p></body></html>"), IProtocolEngine::SC_OK, QByteArray("text/html; charset=utf-8")));
	if (responsePtr.IsValid()){
		return engine.GetResponder().SendResponse(*responsePtr);
	}

	return false;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CHttpRootHandlerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


} // namespace imtrest


