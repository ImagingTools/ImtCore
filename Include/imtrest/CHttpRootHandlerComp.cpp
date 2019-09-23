#include <imtrest/CHttpRootHandlerComp.h>


namespace imtrest
{


// public methods

// reimplemented (IProtocolEngine)

bool CHttpRootHandlerComp::ProcessRequest(const IRequest& request) const
{
	return false;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CHttpRootHandlerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


} // namespace imtrest


