#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CAuthorizationOptionsControllerComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	rootModelPtr.SetPtr(BaseClass::CreateRepresentationFromRequest(gqlRequest, errorMessage));
	if (!rootModelPtr.IsValid()){
		return nullptr;
	}

	if (m_superuserProviderCompPtr.IsValid()){
		imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->GetTreeItemModel("data");
		Q_ASSERT(dataModelPtr != nullptr);

		bool superuserExists = m_superuserProviderCompPtr->SuperuserExists();
		dataModelPtr->SetData("SuperUserExists", superuserExists);
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


