// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CBasicRequestsProxyProviderComp.h>


// ImtCore includes
#include <imtsdl/CSdlDocumentType.h>


namespace imtsdl
{


SdlRequestList CBasicRequestsProxyProviderComp::GetRequests(bool onlyLocal) const
{
	if (m_sdlRequestListCompPtr.IsValid() && m_sdlDocumentListCompPtr.IsValid()){
		SdlRequestList retVal = m_sdlRequestListCompPtr->GetRequests(onlyLocal);
		SdlDocumentTypeList documentList = m_sdlDocumentListCompPtr->GetDocumentTypes(false);

		for (const CSdlDocumentType& documentType: documentList){
			const SdlRequestList typeRequestList = documentType.GetOperationsList().values();
			for (const CSdlRequest& typeRequest: typeRequestList){
				retVal.removeAll(typeRequest);
			}
			const SdlDocumentTypeList subtypeList = documentType.GetSubtypes();
			for (const CSdlDocumentType& documentSubtype: subtypeList){
				const SdlRequestList subtypeRequestList = documentSubtype.GetOperationsList().values();
				for (const CSdlRequest& subtypeRequest: subtypeRequestList){
					retVal.removeAll(subtypeRequest);
				}
			}
		}

		return retVal;
	}

	I_CRITICAL();
	return SdlRequestList();
}



} // namespace imtsdl


