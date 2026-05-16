// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CCollectionDocumentServiceComp.h>

namespace imtdoc
{


// protected methods

// reimplemented (imtdoc::CCollectionDocumentServiceBase)

imtbase::IObjectCollection* CCollectionDocumentServiceComp::GetCollection() const
{
return m_collectionCompPtr.IsValid() ? m_collectionCompPtr.GetPtr() : nullptr;
}


// reimplemented (imtdoc::CDocumentServiceBase)

bool CCollectionDocumentServiceComp::IsSingleCopyMode() const
{
	return m_isSingleCopyModeAttrPtr.IsValid() && *m_isSingleCopyModeAttrPtr;
}


} // namespace imtdoc
