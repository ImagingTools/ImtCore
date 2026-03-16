// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/COrderedObjectCollectionProxy.h>


namespace imtbase
{


class COrderedObjectCollectionProxyComp:
			public icomp::CComponentBase,
			public imtbase::COrderedObjectCollectionProxy
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(COrderedObjectCollectionProxyComp)
		I_REGISTER_INTERFACE(imtbase::COrderedObjectCollectionProxy)
		I_REGISTER_INTERFACE(imtbase::IOrderedObjectCollection)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection)
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionInfo)
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo)
		I_REGISTER_INTERFACE(iser::ISerializable)
		I_REGISTER_INTERFACE(istd::IChangeable)
		I_ASSIGN(m_collectionCompPtr, "ObjectCollection", "Source object collection", false, "ObjectCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
};


} // namespace imtbase


