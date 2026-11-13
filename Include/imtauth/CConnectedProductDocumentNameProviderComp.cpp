// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CConnectedProductDocumentNameProviderComp.h>


// ImtCore includes
#include <imtauth/IConnectedProduct.h>


namespace imtauth
{


// public methods

// reimplemented (imtdoc::IDocumentNameProvider)

QString CConnectedProductDocumentNameProviderComp::GetDefaultDocumentName(const istd::IChangeable& document, const iprm::IParamsSet* /*nameParams*/) const
{
	const IConnectedProduct* productPtr = dynamic_cast<const IConnectedProduct*>(&document);

	return productPtr != nullptr ? productPtr->GetProductName() : QString();
}


} // namespace imtauth
