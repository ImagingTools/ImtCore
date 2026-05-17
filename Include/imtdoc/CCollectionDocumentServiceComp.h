// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/TCollectionDocumentServiceWrap.h>
#include <imtdoc/CDocumentServiceCompBase.h>


namespace imtdoc
{


class CCollectionDocumentServiceComp: public TCollectionDocumentServiceWrap<CDocumentServiceCompBase>
{
public:
	typedef TCollectionDocumentServiceWrap<CDocumentServiceCompBase> BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentServiceComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentService)
		I_ASSIGN(m_collectionCompPtr, "Collection", "Document collection containing related documents", true, "Collection");
		I_ASSIGN(m_isSingleCopyModeAttrPtr, "IsSingleCopyMode", "When enabled, all users share a single copy of each document", false, false);
	I_END_COMPONENT

protected:
	// reimplemented (imtdoc::TCollectionDocumentServiceWrap)
	virtual imtbase::IObjectCollection* GetCollection() const override;

	// reimplemented (imtdoc::CDocumentServiceBase)
	virtual bool IsSingleCopyMode() const override;

private:
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
	I_ATTR(bool, m_isSingleCopyModeAttrPtr);
};


} // namespace imtdoc


