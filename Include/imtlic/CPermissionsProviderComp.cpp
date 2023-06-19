#include <imtlic/CPermissionsProviderComp.h>


namespace imtlic
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

bool CPermissionsProviderComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	return true;
}


imtbase::CTreeItemModel* CPermissionsProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_featurePackageCompPtr.IsValid() && m_featurePackageRepresentationCompPtr.IsValid()){
		istd::TDelPtr<imtbase::CTreeItemModel> representationPtr(new imtbase::CTreeItemModel);
		bool result = m_featurePackageRepresentationCompPtr->GetRepresentationFromDataModel(*m_featurePackageCompPtr, *representationPtr.GetPtr());
		QString json = representationPtr->toJSON();
		if (result){
			return representationPtr.PopPtr();
		}
	}

	return nullptr;
}


} // namespace imtlic


