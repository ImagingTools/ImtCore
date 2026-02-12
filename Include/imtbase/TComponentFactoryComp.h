// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>


namespace imtbase
{


/**
	Generic adapter of the component factorization to istd::TIFactory<> interface.
*/
template <typename ObjectInterface>
class TComponentFactoryComp:
			public icomp::CComponentBase,
			public istd::TIFactory<ObjectInterface>
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef istd::TIFactory<ObjectInterface> FactoryInterface;

	I_BEGIN_COMPONENT(TComponentFactoryComp);
		I_REGISTER_INTERFACE(FactoryInterface);
		I_ASSIGN(m_factCompPtr, "Factory", "Factory used for creation of the object instance", true, "Factory");
		I_ASSIGN(m_typeIdAttrPtr, "TypeId", "Type-ID of the instance to be created", true, "TypeId");
	I_END_COMPONENT;

	// reimplemented (istd::TIFactory)
	virtual istd::TUniqueInterfacePtr<ObjectInterface> CreateInstance(const QByteArray& keyId = "") const override;

	// reimplemented (istd::IFactoryInfo)
	virtual istd::IFactoryInfo::KeyList GetFactoryKeys() const override;

private:
	I_TFACT(ObjectInterface, m_factCompPtr);
	I_ATTR(QByteArray, m_typeIdAttrPtr);
};


// reimplemented (istd::TIFactory)

template<typename ObjectInterface>
istd::TUniqueInterfacePtr<ObjectInterface> TComponentFactoryComp<ObjectInterface>::CreateInstance(const QByteArray& /*keyId*/) const
{
	auto componentPtr = m_factCompPtr.CreateInstance();
	istd::IPolymorphic* rawBasePtr = componentPtr.PopInterfacePtr();
	ObjectInterface* rawPtr = dynamic_cast<ObjectInterface*>(rawBasePtr);
	
	// If dynamic_cast fails, delete the original object to prevent leak
	if (rawPtr == nullptr && rawBasePtr != nullptr) {
		delete rawBasePtr;
	}
	
	return istd::TUniqueInterfacePtr<ObjectInterface>(rawPtr);
}


template<typename ObjectInterface>
istd::IFactoryInfo::KeyList TComponentFactoryComp<ObjectInterface>::GetFactoryKeys() const
{
	istd::IFactoryInfo::KeyList keys;

	keys.insert(*m_typeIdAttrPtr);

	return keys;
}


} // namespace imtbase


