#include <imtrest/CHttpProtocolEngineComp.h>


// ImtCore includes
#include <imtrest/CHttpRequest.h>


namespace imtrest
{


// public methods

// reimplemented (IProtocolEngine)

QByteArray CHttpProtocolEngineComp::GetProtocolTypeId() const
{
	return "http";
}


const iser::IVersionInfo* CHttpProtocolEngineComp::GetProtocolVersion() const
{
	return nullptr;
}


IRequest* CHttpProtocolEngineComp::CreateRequest(const QAbstractSocket* socketPtr, const IRequestHandler& requestHandler) const
{
	if (socketPtr != nullptr){
		return new CHttpRequest(*socketPtr, requestHandler, *this);
	}

	return nullptr;
}


IResponse* CHttpProtocolEngineComp::CreateResponse(const QIODevice* devicePtr, const QByteArray& data, const IRequest& request, int statusCode) const
{
	return nullptr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CHttpProtocolEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


} // namespace imtrest


