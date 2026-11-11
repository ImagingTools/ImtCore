// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CCollectionStorageResolver.h>


namespace imtbase
{


/**
	Component of the storage resolver working on top of an object collection.
	It makes a collection of the composition available to the storage links, see CCollectionStorageResolver.
	\ingroup Collection
*/
class CCollectionStorageResolverComp:
			public icomp::CComponentBase,
			public CCollectionStorageResolver
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionStorageResolverComp);
		I_REGISTER_INTERFACE(IStorageResolver);
		I_ASSIGN(m_collectionCompPtr, "Collection", "Collection used as the storage of the linked objects", true, "Collection");
		I_ASSIGN(m_typeIdAttrPtr, "TypeId", "Type-ID of the objects created in the collection", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(IObjectCollection, m_collectionCompPtr);
	I_ATTR(QByteArray, m_typeIdAttrPtr);
};


} // namespace imtbase


