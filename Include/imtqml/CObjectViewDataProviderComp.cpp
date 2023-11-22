#include <imtqml/CObjectViewDataProviderComp.h>


namespace imtqml
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

bool CObjectViewDataProviderComp::IsRequestSupported(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return true;
}


imtbase::CTreeItemModel* CObjectViewDataProviderComp::CreateInternalResponse(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	if (m_objectViewPathAttrPtr.IsValid()){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

		rootModelPtr->SetData("Path", *m_objectViewPathAttrPtr);
		rootModelPtr->SetData("CommandId", *m_objectViewModelIdAttrPtr);

		return rootModelPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtqml


