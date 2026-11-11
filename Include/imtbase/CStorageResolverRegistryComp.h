// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CStorageResolverRegistry.h>


namespace imtbase
{


/**
	Component of the storage resolver registry.
	The resolvers are registered for the logical storage IDs at the same position of the lists; an empty ID registers the default resolver.
	If IsDefaultProvider is set, the registry becomes the application-wide provider of the storage links
	(see StorageLinkBase::SetDefaultResolverProvider) while the component exists.
	\ingroup Collection
*/
class CStorageResolverRegistryComp:
			public icomp::CComponentBase,
			public CStorageResolverRegistry
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStorageResolverRegistryComp);
		I_REGISTER_INTERFACE(IStorageResolverProvider);
		I_ASSIGN_MULTI_0(m_storageIdsAttrPtr, "StorageIds", "List of the logical storage IDs for the corresponding resolvers", true);
		I_ASSIGN_MULTI_0(m_storageResolversCompPtr, "StorageResolvers", "List of the storage resolvers according to the specified storage IDs", true);
		I_ASSIGN(m_isDefaultProviderAttrPtr, "IsDefaultProvider", "If enabled, the registry is used by all storage links without own binding", true, false);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_MULTIATTR(QByteArray, m_storageIdsAttrPtr);
	I_MULTIREF(IStorageResolver, m_storageResolversCompPtr);
	I_ATTR(bool, m_isDefaultProviderAttrPtr);
};


} // namespace imtbase


