#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CObjectContainer.h>


namespace imtbase
{


/**
	Component-based implementation of an object container. The sub-objects will be created using registered factories (via 'ObjectFactories')
*/
class CObjectContainerComp:
			public icomp::CComponentBase,
			public CObjectContainer
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CObjectContainer BaseClass2;

	I_BEGIN_COMPONENT(CObjectContainerComp);
		I_REGISTER_INTERFACE(IObjectManager);
		I_REGISTER_INTERFACE(IObjectProvider);
		I_ASSIGN_MULTI_0(m_objectFactoriesCompPtr, "ObjectFactories", "List of factories used for object creation", true);
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "TypeIds", "List of type-ID corresponding to the registered factories", true);
	I_END_COMPONENT;

protected:
	// reimplemented (CObjectContainer)
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const override;

private:
	I_MULTIFACT(istd::IChangeable, m_objectFactoriesCompPtr);
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);
};


} // namespace imtbase


