// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CProductDocumentValidatorComp.h>


// ImtCore includes
#include <imtdoc/imtdoc.h>
#include <imtlic/IProductInfo.h>
#include <imtlicgql/ProductRepresentation.h>


namespace imtlicgql
{


// reimplemented (imtdoc::IDocumentValidator)

bool CProductDocumentValidatorComp::ValidateDocumentData(
			const istd::IChangeable& document,
			QString& errorMessage,
			const iprm::IParamsSet* paramsPtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(&document);
	if (productInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to validate the document. Error: It is not a product");

		return false;
	}

	// Empty for a document never saved: the product itself is not taken for a duplicate
	const QByteArray objectId = imtdoc::GetObjectIdFromParams(paramsPtr);

	return CheckProductIdentity(*productInfoPtr, objectId, *m_productCollectionCompPtr, errorMessage);
}


} // namespace imtlicgql


