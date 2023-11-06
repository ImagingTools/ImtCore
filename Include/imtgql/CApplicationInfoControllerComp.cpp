#include <imtgql/CApplicationInfoControllerComp.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CApplicationInfoControllerComp::CreateRepresentationFromRequest(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	if (m_applicationInfoCompPtr.IsValid() && m_applicationInfoRepresentationCompPtr.IsValid()){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
		imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
		Q_ASSERT(dataModelPtr != nullptr);
		if (m_applicationInfoRepresentationCompPtr->GetRepresentationFromApplicationInfo(*m_applicationInfoCompPtr, *dataModelPtr)){
			return rootModelPtr.PopPtr();
		}
	}

	return nullptr;
}


} // namespace imtgql


