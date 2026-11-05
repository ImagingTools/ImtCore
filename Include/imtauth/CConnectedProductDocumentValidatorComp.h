// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentValidator.h>


namespace imtauth
{


/**
	Validates the catalog entries of products: an entry names a product, and only one entry names it.
*/
class CConnectedProductDocumentValidatorComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentValidator
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CConnectedProductDocumentValidatorComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator);
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Catalog of connected products", true, "ProductCollection");
	I_END_COMPONENT

	// reimplemented (imtdoc::IDocumentValidator)
	virtual bool ValidateDocumentData(
				const istd::IChangeable& document,
				QString& errorMessage,
				const iprm::IParamsSet* validationParams = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

	/**
		Check that the product ID is given and no other entry of the catalog uses it.
		\param objectId ID of the checked entry, empty for a new one.
	*/
	static bool CheckProductIdUnique(
				const imtbase::IObjectCollection& productCollection,
				const QByteArray& objectId,
				const QByteArray& productId,
				QString& errorMessage);

	/**
		Tell whether the catalog has an entry of the product.
	*/
	static bool HasProduct(const imtbase::IObjectCollection& productCollection, const QByteArray& productId);

private:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
};


} // namespace imtauth
