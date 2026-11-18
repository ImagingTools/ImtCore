// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CProductDocumentNameProviderComp.h>


// ImtCore includes
#include <imtlic/IProductInfo.h>


namespace imtlicgql
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CProductDocumentNameProviderComp::GetDefaultDocumentName(
			const istd::IChangeable& document,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(&document);
	if (productInfoPtr == nullptr){
		return QString();
	}

	return productInfoPtr->GetName();
}


} // namespace imtlicgql


