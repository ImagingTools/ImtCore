// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CConnectedProductDocumentValidatorComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/IConnectedProduct.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtdoc/imtdoc.h>


namespace imtauth
{


static imtbase::ICollectionInfo::Ids SelectProductEntries(
			const imtbase::IObjectCollection& productCollection,
			const QByteArray& objectId,
			const QByteArray& productId)
{
	imtbase::CComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.logicalOperation = imtbase::IComplexCollectionFilter::LO_AND;
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("ProductId", QString::fromUtf8(productId));
	if (!objectId.isEmpty()){
		filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter(
					"DocumentId", QString::fromUtf8(objectId), imtbase::IComplexCollectionFilter::FO_NOT_EQUAL);
	}

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	return productCollection.GetElementIds(0, -1, &filterParams);
}


// reimplemented (imtdoc::IDocumentValidator)

bool CConnectedProductDocumentValidatorComp::ValidateDocumentData(
			const istd::IChangeable& document,
			QString& errorMessage,
			const iprm::IParamsSet* validationParams,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const IConnectedProduct* productPtr = dynamic_cast<const IConnectedProduct*>(&document);
	if (productPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type for product validation");

		return false;
	}

	return CheckProductIdUnique(*m_productCollectionCompPtr, imtdoc::GetObjectIdFromParams(validationParams), productPtr->GetProductId(), errorMessage);
}


bool CConnectedProductDocumentValidatorComp::CheckProductIdUnique(
			const imtbase::IObjectCollection& productCollection,
			const QByteArray& objectId,
			const QByteArray& productId,
			QString& errorMessage)
{
	if (productId.isEmpty()){
		errorMessage = QStringLiteral("Product-ID can't be empty!");

		return false;
	}

	if (!SelectProductEntries(productCollection, objectId, productId).isEmpty()){
		errorMessage = QStringLiteral("The product '%1' is already connected").arg(QString::fromUtf8(productId));

		return false;
	}

	return true;
}


bool CConnectedProductDocumentValidatorComp::HasProduct(const imtbase::IObjectCollection& productCollection, const QByteArray& productId)
{
	return !productId.isEmpty() && !SelectProductEntries(productCollection, QByteArray(), productId).isEmpty();
}


} // namespace imtauth
