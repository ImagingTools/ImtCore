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
	ObjectInterface* CreateInstance(const QByteArray& keyId = "") const override;

	// reimplemented (istd::IFactoryInfo)
	KeyList GetFactoryKeys() const override;

private:
	I_TFACT(ObjectInterface, m_factCompPtr);
	I_ATTR(QByteArray, m_typeIdAttrPtr);
};


// reimplemented (istd::TIFactory)

template<typename ObjectInterface>
ObjectInterface* TComponentFactoryComp<ObjectInterface>::CreateInstance(const QByteArray& /*keyId*/) const
{
	return m_factCompPtr.CreateInstance();
}


template<typename ObjectInterface>
istd::IFactoryInfo::KeyList TComponentFactoryComp<ObjectInterface>::GetFactoryKeys() const
{
	KeyList keys;

	keys.insert(*m_typeIdAttrPtr);

	return keys;
}


} // namespace imtbase


