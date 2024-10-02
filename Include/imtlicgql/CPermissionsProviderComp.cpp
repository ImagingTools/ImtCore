#include <imtlicgql/CPermissionsProviderComp.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtgql::CGqlRequestHandlerCompBase)

bool CPermissionsProviderComp::IsRequestSupported(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return true;
}


imtbase::CTreeItemModel* CPermissionsProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	if (m_featureContainerCompPtr.IsValid() && m_featurePackageRepresentationCompPtr.IsValid()){
		istd::TDelPtr<imtbase::CTreeItemModel> representationPtr(new imtbase::CTreeItemModel);
		bool result = m_featurePackageRepresentationCompPtr->GetRepresentationFromDataModel(*m_featureContainerCompPtr, *representationPtr.GetPtr());
		if (result){
			return representationPtr.PopPtr();
		}
	}

	return nullptr;
}


} // namespace imtlicgql


