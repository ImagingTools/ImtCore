#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>


namespace imtbase
{


/**
	Component-based implementation of an object container. The sub-objects will be created using registered factories (via 'ObjectFactories')
*/
class CObjectCollectionComp:
			public icomp::CComponentBase,
			public CObjectCollection
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CObjectCollection BaseClass2;

	I_BEGIN_COMPONENT(CObjectCollectionComp);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectProvider);
		I_ASSIGN_MULTI_0(m_objectFactoriesCompPtr, "ObjectFactories", "List of factories used for object creation", true);
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "TypeIds", "List of type-ID corresponding to the registered factories", true);
		I_ASSIGN_MULTI_0(m_typeNamesAttrPtr, "TypeNames", "List of type names corresponding to the registered factories", true);
		I_ASSIGN_MULTI_0(m_fixedObjectsCompPtr, "FixedObjects", "List of static defined objects", false);
		I_ASSIGN_MULTI_0(m_fixedObjectIdsAttrPtr, "FixedObjectIds", "List of IDs for corresponding fixed object", false);
		I_ASSIGN_MULTI_0(m_fixedObjectTypeIdsAttrPtr, "FixedObjectTypeIds", "List of type-IDs for corresponding fixed object", false);
		I_ASSIGN_MULTI_0(m_fixedObjectTypeNamesAttrPtr, "FixedObjectTypeNames", "List of names for corresponding fixed object type", false);
	I_END_COMPONENT;

protected:
	// reimplemented (CObjectCollection)
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIFACT(istd::IChangeable, m_objectFactoriesCompPtr);
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);
	I_MULTITEXTATTR(m_typeNamesAttrPtr);
	I_MULTIREF(istd::IChangeable, m_fixedObjectsCompPtr);
	I_MULTIATTR(QByteArray, m_fixedObjectIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_fixedObjectTypeIdsAttrPtr);
	I_MULTITEXTATTR(m_fixedObjectTypeNamesAttrPtr);
};


} // namespace imtbase


