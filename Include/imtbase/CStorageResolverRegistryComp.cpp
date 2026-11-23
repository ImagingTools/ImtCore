// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CStorageResolverRegistryComp.h>


// ImtCore includes
#include <imtbase/CStorageLinkBase.h>


namespace imtbase
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CStorageResolverRegistryComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	const int resolversCount = qMin(m_storageIdsAttrPtr.GetCount(), m_storageResolversCompPtr.GetCount());
	for (int i = 0; i < resolversCount; ++i){
		RegisterResolver(m_storageResolversCompPtr[i], m_storageIdsAttrPtr[i]);
	}

	CStorageLinkBase::SetDefaultResolverProvider(this);
}


void CStorageResolverRegistryComp::OnComponentDestroyed()
{
	if (CStorageLinkBase::GetDefaultResolverProvider() == this){
		CStorageLinkBase::SetDefaultResolverProvider(nullptr);
	}

	BaseClass::OnComponentDestroyed();
}


} // namespace imtbase


