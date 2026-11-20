// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CStorageResolverRegistry.h>


namespace imtbase
{


/**
	Registry of the storage resolvers filled by the composition.
	The resolver at each position of StorageResolvers is registered for the logical storage ID at the same position of StorageIds.
	The registry becomes the application-wide resolver provider of the storage links when it is created, and stops being it when it is destroyed.
	\sa CStorageResolverRegistry, StorageLinkBase::SetDefaultResolverProvider
	\ingroup Collection
*/
class CStorageResolverRegistryComp:
			public icomp::CComponentBase,
			public CStorageResolverRegistry
{
public:
	using BaseClass = icomp::CComponentBase;

	I_BEGIN_COMPONENT(CStorageResolverRegistryComp);
		I_REGISTER_INTERFACE(IStorageResolverProvider);
		I_ASSIGN_MULTI_0(m_storageIdsAttrPtr, "StorageIds", "Logical storage IDs in the order of StorageResolvers, an empty ID is the default storage", true);
		I_ASSIGN_MULTI_0(m_storageResolversCompPtr, "StorageResolvers", "Storage resolvers in the order of StorageIds", true);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_MULTIATTR(QByteArray, m_storageIdsAttrPtr);
	I_MULTIREF(IStorageResolver, m_storageResolversCompPtr);
};


} // namespace imtbase


