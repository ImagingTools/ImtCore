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

	const int storageIdsCount	= m_storageIdsAttrPtr.GetCount();
	const int resolversCount	= qMin(storageIdsCount, m_storageResolversCompPtr.GetCount());
	if (storageIdsCount != m_storageResolversCompPtr.GetCount()){
		SendErrorMessage(0, QStringLiteral("StorageIds and StorageResolvers differ in count, only the first %1 storages are registered").arg(resolversCount));
	}

	for (int i = 0; i < resolversCount; ++i){
		RegisterResolver(m_storageResolversCompPtr[i], m_storageIdsAttrPtr[i]);
	}

	const IStorageResolverProvider* previousProviderPtr = CStorageLinkBase::GetDefaultResolverProvider();
	if ((previousProviderPtr != nullptr) && (previousProviderPtr != this)){
		SendWarningMessage(0, QStringLiteral("Another storage resolver registry was the default provider of the storage links and is replaced"));
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


