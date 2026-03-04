// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CDelegatedRequestHandlerComp.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

bool CDelegatedRequestHandlerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (m_slaveRequestHandlerCompPtr.IsValid()){
		return m_slaveRequestHandlerCompPtr->IsRequestSupported(gqlRequest);
	}

	return false;

}


imtbase::CTreeItemModel* CDelegatedRequestHandlerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_slaveRequestHandlerCompPtr.IsValid()){
		return m_slaveRequestHandlerCompPtr->CreateResponse(gqlRequest, errorMessage);
	}

	return nullptr;
}


} // namespace imtservergql


