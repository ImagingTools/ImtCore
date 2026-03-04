// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlRepresentationDataControllerCompBase.h>


namespace imtservergql
{


// protected methods

imtbase::CTreeItemModel* CGqlRepresentationControllerCompBase::CreateRepresentationFromRequest(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return nullptr;
}


bool CGqlRepresentationControllerCompBase::UpdateModelFromRepresentation(
			const imtgql::CGqlRequest& /*request*/,
			imtbase::CTreeItemModel* /*representationPtr*/) const
{
	return false;
}


// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CGqlRepresentationControllerCompBase::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	imtgql::IGqlRequest::RequestType requestType = gqlRequest.GetRequestType();

	if (requestType == imtgql::IGqlRequest::RT_QUERY){
		return CreateRepresentationFromRequest(gqlRequest, errorMessage);
	}
	if (requestType == imtgql::IGqlRequest::RT_MUTATION){
		const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			return nullptr;
		}

		QByteArray itemData = inputParamPtr->GetParamArgumentValue("Item").toByteArray();
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

	return nullptr;
}


} // namespace imtservergql


