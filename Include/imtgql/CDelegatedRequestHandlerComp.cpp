#include <imtgql/CDelegatedRequestHandlerComp.h>


namespace imtgql
{


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

bool CDelegatedRequestHandlerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (m_slaveRequestHandlerCompPtr.IsValid()){
		return m_slaveRequestHandlerCompPtr->IsRequestSupported(gqlRequest);
	}

	return false;

}


imtbase::CTreeItemModel* CDelegatedRequestHandlerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_slaveRequestHandlerCompPtr.IsValid()){
		return m_slaveRequestHandlerCompPtr->CreateResponse(gqlRequest, errorMessage);
	}

	return nullptr;

}



} // namespace imtgql


