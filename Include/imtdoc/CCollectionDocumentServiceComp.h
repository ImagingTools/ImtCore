// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/TCollectionDocumentServiceWrap.h>
#include <imtdoc/CDocumentServiceComp.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Concrete ACF component that implements \c IDocumentService on top of an \c IObjectCollection.

	\c CCollectionDocumentServiceComp is the standard ready-to-use document
	service component for server-side multi-user scenarios.  It inherits
	the full implementation from
	\c TCollectionDocumentServiceWrap<CDocumentServiceComp> and adds only
	the \c "Collection" reference to the \c IObjectCollection that provides
	persistent storage (required).

	Single-copy mode (\c "IsSingleCopyMode") and state persistence of the opened
	documents (\c "StateFilePath", \c "StatePersistenceEnabler") are inherited
	from \c CDocumentServiceCompBase;
	the data of a restored document is loaded from the collection object
	(or its revision) addressed by the document source reference.

	Register it in the component descriptor with the interface
	\c imtdoc::IDocumentService so that clients can obtain it via the ACF
	service locator.
*/
class CCollectionDocumentServiceComp: public TCollectionDocumentServiceWrap<CDocumentServiceComp>
{
public:
	typedef TCollectionDocumentServiceWrap<CDocumentServiceComp> BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentServiceComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentService)
		I_ASSIGN(m_collectionCompPtr, "Collection", "Document collection containing related documents", true, "Collection");
	I_END_COMPONENT

protected:
	// reimplemented (imtdoc::TCollectionDocumentServiceWrap)
	virtual imtbase::IObjectCollection* GetCollection() const override;

private:
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
};


} // namespace imtdoc
