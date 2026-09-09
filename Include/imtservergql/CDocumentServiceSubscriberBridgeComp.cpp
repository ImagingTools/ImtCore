// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CDocumentServiceSubscriberBridgeComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlSubscriberController via CGqlPublisherCompBase)

bool CDocumentServiceSubscriberBridgeComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (!BaseClass::IsRequestSupported(gqlRequest)){
		return false;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		return false;
	}

	const QByteArray collectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();
	if (collectionId.isEmpty()){
		return false;
	}

	return m_collectionIdsAttrPtr.FindValue(collectionId) != -1;
}


} // namespace imtservergql
