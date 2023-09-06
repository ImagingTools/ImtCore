#include <imtgql/CGqlControllerComp.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CGqlControllerComp::CreateInternalResponse(const CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	Q_UNUSED(dataModelPtr);

	dataModelPtr->SetData("Status", "OK");

	return rootModelPtr.PopPtr();
}


} // namespace imtgql


