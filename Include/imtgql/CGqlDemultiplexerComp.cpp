#include <imtgql/CGqlDemultiplexerComp.h>


namespace imtgql
{


// public methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

bool CGqlDemultiplexerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	for (int i = 0; i < m_gqlRequestHandlersCompPtr.GetCount(); i++){
		const imtgql::IGqlRequestHandler* gqlRequestHahdlerPtr = m_gqlRequestHandlersCompPtr[i];
		if (gqlRequestHahdlerPtr != nullptr){
			if (gqlRequestHahdlerPtr->IsRequestSupported(gqlRequest)){
				return true;
			}
		}
	}

	return false;
}


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CGqlDemultiplexerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	for (int i = 0; i < m_gqlRequestHandlersCompPtr.GetCount(); i++){
		const imtgql::IGqlRequestHandler* gqlRequestHahdlerPtr = m_gqlRequestHandlersCompPtr[i];
		if (gqlRequestHahdlerPtr != nullptr){
			if (gqlRequestHahdlerPtr->IsRequestSupported(gqlRequest)){
				return gqlRequestHahdlerPtr->CreateResponse(gqlRequest, errorMessage);
			}
		}
	}

	return nullptr;
}


} // namespace imtgql


