// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlDemultiplexerComp.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

bool CGqlDemultiplexerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	for (int i = 0; i < m_gqlRequestHandlersCompPtr.GetCount(); i++){
		const imtgql::IGqlRequestHandler* gqlRequestHahdlerPtr = m_gqlRequestHandlersCompPtr[i];
		if (gqlRequestHahdlerPtr != nullptr){
			if (gqlRequestHahdlerPtr->IsRequestSupported(gqlRequest)){
				return true;
			}
		}
	}

	return false;
}


// protected methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CGqlDemultiplexerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	for (int i = 0; i < m_gqlRequestHandlersCompPtr.GetCount(); i++){
		const imtgql::IGqlRequestHandler* gqlRequestHahdlerPtr = m_gqlRequestHandlersCompPtr[i];
		if (gqlRequestHahdlerPtr != nullptr){
			if (gqlRequestHahdlerPtr->IsRequestSupported(gqlRequest)){
				return gqlRequestHahdlerPtr->CreateResponse(gqlRequest, errorMessage);
			}
		}
	}

	return nullptr;
}


} // namespace imtservergql


