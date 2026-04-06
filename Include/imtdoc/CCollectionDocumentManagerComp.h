// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/CCollectionDocumentManagerBase.h>
#include <imtdoc/CDocumentManagerCompBase.h>


namespace imtdoc
{


class CCollectionDocumentManagerComp: public CDocumentManagerCompBase, public CCollectionDocumentManagerBase
{
public:
typedef CDocumentManagerCompBase BaseClass;
typedef CCollectionDocumentManagerBase BaseClass2;

I_BEGIN_COMPONENT(CCollectionDocumentManagerComp)
I_REGISTER_INTERFACE(imtdoc::IDocumentManager)
I_ASSIGN(m_collectionCompPtr, "Collection", "Document collection containing related documents", true, "Collection");
I_END_COMPONENT

protected:
// reimplemented (imtdoc::CCollectionDocumentManagerBase)
virtual imtbase::IObjectCollection* GetCollection() const override;

private:
I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
};


} // namespace imtdoc
