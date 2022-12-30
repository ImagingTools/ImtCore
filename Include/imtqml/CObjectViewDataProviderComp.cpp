#include <imtqml/CObjectViewDataProviderComp.h>


namespace imtqml
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel *CObjectViewDataProviderComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest &gqlRequest, QString &errorMessage) const
{
	if (m_objectViewAttrPtr.IsValid()){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

		rootModelPtr->SetData("Id", "ObjectViewProvider");
		rootModelPtr->SetData("Path", *m_objectViewAttrPtr);
		rootModelPtr->SetData("CommandsId", *m_objectViewModelIdAttrPtr);

		return rootModelPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtqml


