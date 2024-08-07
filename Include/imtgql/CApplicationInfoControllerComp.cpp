#include <imtgql/CApplicationInfoControllerComp.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CApplicationInfoControllerComp::CreateRepresentationFromRequest(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		errorMessage = QString("Unable to create representaion for application info. Component attribute 'm_applicationInfoCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CApplicationInfoControllerComp");

		return nullptr;
	}

	if (!m_applicationInfoRepresentationCompPtr.IsValid()){
		errorMessage = QString("Unable to create representaion for application info. Component attribute 'm_applicationInfoRepresentationCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CApplicationInfoControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	if (!m_applicationInfoRepresentationCompPtr->GetRepresentationFromApplicationInfo(*m_applicationInfoCompPtr, *dataModelPtr)){
		errorMessage = QString("Unable to create representaion for application info");
		SendErrorMessage(0, errorMessage, "CApplicationInfoControllerComp");

		return nullptr;
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtgql


