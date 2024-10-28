#include <imtservergql/CGqlControllerComp.h>


namespace imtservergql
{


// protected methods

// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CGqlControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	dataModelPtr->SetData("Status", "OK");

	return rootModelPtr.PopPtr();
}


} // namespace imtservergql


