// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserDocumentNameProviderComp.h>


// ImtCore includes
#include <imtauth/IPrincipal.h>


namespace imtauth
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CUserDocumentNameProviderComp::GetDefaultDocumentName(const istd::IChangeable& document, const iprm::IParamsSet* /*nameGenerationParams*/) const
{
	const IPrincipal* userBaseInfoPtr = dynamic_cast<const IPrincipal*>(&document);
	if (userBaseInfoPtr != nullptr){
		return userBaseInfoPtr->GetName();
	}

	return QString();
}


} // namespace imtauth
