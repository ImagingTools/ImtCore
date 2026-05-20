// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CRemoteFilterableSelectControllerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtservergql
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

		if (m_collectionIdAttrPtr.IsValid() && *m_collectionIdAttrPtr != ""){
			QByteArray collectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();
			return *m_collectionIdAttrPtr == collectionId;
		}
	}

	return false;
}


// reimplemented (sdl::imtbase::FilterableSelect::CGraphQlHandlerCompBase)

sdl::imtbase::FilterableSelect::CGetSelectableItemsPayload CRemoteFilterableSelectControllerComp::OnGetSelectableItems(
			const sdl::imtbase::FilterableSelect::CGetSelectableItemsGqlRequest& /*getSelectableItemsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::FilterableSelect::CGetSelectableItemsPayload>(gqlRequest, errorMessage);
}


} // namespace imtservergql
