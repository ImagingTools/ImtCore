// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CStorageResolverRegistryComp.h>


// Qt includes
#include <QtCore/QtGlobal>

// ImtCore includes
#include <imtbase/StorageLinkBase.h>


namespace imtbase
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CStorageResolverRegistryComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	const int storageIdsCount = m_storageIdsAttrPtr.GetCount();
	const int resolversCount = m_storageResolversCompPtr.GetCount();
	Q_ASSERT_X(storageIdsCount == resolversCount, "CStorageResolverRegistryComp", "Each storage ID needs a resolver");

	const int count = qMin(storageIdsCount, resolversCount);
	for (int index = 0; index < count; ++index){
		RegisterResolver(m_storageResolversCompPtr[index], m_storageIdsAttrPtr[index]);
	}

	if (*m_isDefaultProviderAttrPtr){
		StorageLinkBase::SetDefaultResolverProvider(this);
	}
}


void CStorageResolverRegistryComp::OnComponentDestroyed()
{
	// Another registry may have become the default provider in the meantime: it stays.
	if (StorageLinkBase::GetDefaultResolverProvider() == this){
		StorageLinkBase::SetDefaultResolverProvider(nullptr);
	}

	BaseClass::OnComponentDestroyed();
}


} // namespace imtbase


