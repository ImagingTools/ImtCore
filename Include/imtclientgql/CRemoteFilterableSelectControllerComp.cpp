// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteFilterableSelectControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FilterableSelect.h>


namespace imtclientgql
{


// reimplemented (imtgql::IGqlRequestHandler)

bool CRemoteFilterableSelectControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (isSupported){
		const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			return false;
		}

		if (m_collectionIdsAttrPtr.IsValid()){
			QByteArray collectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();
			return m_collectionIdsAttrPtr.FindValue(collectionId) != -1;
		}
	}

	return false;
}


// reimplemented (sdl::V1_0::imtbase::CGraphQlHandlerCompBase)

sdl::V1_0::imtbase::CGetSelectableItemsPayload CRemoteFilterableSelectControllerComp::OnGetSelectableItems(
			const sdl::V1_0::imtbase::CGetSelectableItemsGqlRequest& /*getSelectableItemsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CGetSelectableItemsPayload>(gqlRequest, errorMessage);
}


} // namespace imtclientgql


