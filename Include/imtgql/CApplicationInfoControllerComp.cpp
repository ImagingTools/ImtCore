#include <imtgql/CApplicationInfoControllerComp.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CApplicationInfoControllerComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_applicationInfoProviderCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	dataModelPtr->SetData("Id", "About");
	dataModelPtr->SetData("Name", "About");

	imtbase::CTreeItemModel* representationPtr = m_applicationInfoProviderCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (representationPtr != nullptr){
		dataModelPtr->SetExternTreeModel("Parameters", representationPtr);
	}

	return rootModelPtr.PopPtr();
}


bool CApplicationInfoControllerComp::UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const
{
	return true;
}


} // namespace imtgql


