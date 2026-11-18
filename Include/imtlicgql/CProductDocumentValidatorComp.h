// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentValidator.h>


namespace imtlicgql
{


/**
	Refuses a product without a product ID or a name, and one whose product ID or name
	another product of the collection already has.
*/
class CProductDocumentValidatorComp: public ilog::CLoggerComponentBase, virtual public imtdoc::IDocumentValidator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductDocumentValidatorComp);
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator);
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Products: the product ID and the name of a product are unique in it", true, "ProductCollection");
	I_END_COMPONENT;

	// reimplemented (imtdoc::IDocumentValidator)
	virtual bool ValidateDocumentData(
				const istd::IChangeable& document,
				QString& errorMessage,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

private:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
};


} // namespace imtlicgql


