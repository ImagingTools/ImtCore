#include <imtgql/CGqlRepresentationDataControllerCompBase.h>


namespace imtgql
{


// protected methods

imtbase::CTreeItemModel* CGqlRepresentationControllerCompBase::CreateRepresentationFromRequest(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	return nullptr;
}


bool CGqlRepresentationControllerCompBase::UpdateModelFromRepresentation(const imtgql::CGqlRequest& /*request*/, imtbase::CTreeItemModel* /*representationPtr*/) const
{
	return false;
}


// reimplemented (imtgql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CGqlRepresentationControllerCompBase::CreateInternalResponse(const CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	imtgql::IGqlRequest::RequestType requestType = gqlRequest.GetRequestType();

	if (requestType == imtgql::IGqlRequest::RT_QUERY){
		return CreateRepresentationFromRequest(gqlRequest, errorMessage);
	}
	else if (requestType == imtgql::IGqlRequest::RT_MUTATION){
		const QList<CGqlObject> paramsPtr = gqlRequest.GetParams();
		if (!paramsPtr.empty()){
			QByteArray itemData = paramsPtr.at(0).GetFieldArgumentValue("Item").toByteArray();
			if (!itemData.isEmpty()){
				istd::TDelPtr<imtbase::CTreeItemModel> representationPtr(new imtbase::CTreeItemModel);
				if (representationPtr->CreateFromJson(itemData)){
					bool result = UpdateModelFromRepresentation(gqlRequest, representationPtr.GetPtr());
					if (result){
						return representationPtr.PopPtr();
					}
				}
			}
		}
	}

	Q_ASSERT(false);

	return nullptr;
}


} // namespace imtgql


