// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CConnectedProductDocumentValidatorComp.h>


// ACF includes
#include <ilog/CMessageContainer.h>
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
			const QByteArray& productId,
			bool& isRead)
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

	ilog::CMessageContainer readErrors;
	const imtbase::ICollectionInfo::Ids ids = productCollection.GetElementIds(0, -1, &filterParams, &readErrors);
	isRead = readErrors.GetWorstCategory() < istd::IInformationProvider::IC_ERROR;

	return ids;
}


// reimplemented (imtdoc::IDocumentNameProvider)

QString CConnectedProductDocumentNameProviderComp::GetDefaultDocumentName(const istd::IChangeable& document, const iprm::IParamsSet* /*nameParams*/) const
{
	const IConnectedProduct* productPtr = dynamic_cast<const IConnectedProduct*>(&document);

	return productPtr != nullptr ? productPtr->GetProductName() : QString();
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

	bool isRead = false;
	const imtbase::ICollectionInfo::Ids ids = SelectProductEntries(productCollection, objectId, productId, isRead);
	if (!isRead){
		errorMessage = QStringLiteral("The catalog of products could not be read. Try again.");

		return false;
	}

	if (!ids.isEmpty()){
		errorMessage = QStringLiteral("The product '%1' is already connected").arg(QString::fromUtf8(productId));

		return false;
	}

	return true;
}


bool CConnectedProductDocumentValidatorComp::HasProduct(const imtbase::IObjectCollection& productCollection, const QByteArray& productId, bool* isReadPtr)
{
	bool isRead = true;
	const bool hasProduct = !productId.isEmpty() && !SelectProductEntries(productCollection, QByteArray(), productId, isRead).isEmpty();
	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
	}

	return hasProduct;
}


} // namespace imtauth
