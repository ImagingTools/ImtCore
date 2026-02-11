// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>
#include <imtbase/TFilterableCollectionWrap.h>


namespace imtbase
{


/**
	Component-based implementation of an object collection.
	The sub-objects will be created using registered factories (via 'ObjectFactories')
	\ingroup Collection
*/
class CObjectCollectionComp:
			public icomp::CComponentBase,
			public imtbase::TFilterableCollectionWrap<CObjectCollectionBase>,
			virtual public IObjectCollection::IDataFactory
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::TFilterableCollectionWrap <CObjectCollectionBase> BaseClass2;

	I_BEGIN_COMPONENT(CObjectCollectionComp);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectCollectionInfo);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(IObjectCollection::IDataFactory);
		I_ASSIGN_MULTI_0(m_objectFactoriesCompPtr, "ObjectFactories", "List of factories used for object creation", false);
		I_ASSIGN_MULTI_0(m_objectPersistenceListCompPtr, "ObjectPersistenceList", "List of persistence components used for object data persistence", false);
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "TypeIds", "List of type-ID corresponding to the registered factories", false);
		I_ASSIGN_MULTI_0(m_typeNamesAttrPtr, "TypeNames", "List of type names corresponding to the registered factories", false);
		I_ASSIGN_MULTI_0(m_fixedObjectsCompPtr, "FixedObjects", "List of static defined objects", false);
		I_ASSIGN_MULTI_0(m_fixedObjectIdsAttrPtr, "FixedObjectIds", "List of IDs for corresponding fixed object", false);
		I_ASSIGN_MULTI_0(m_fixedObjectTypeIdsAttrPtr, "FixedObjectTypeIds", "List of type-IDs for corresponding fixed object", false);
		I_ASSIGN_MULTI_0(m_fixedObjectNamesAttrPtr, "FixedObjectNames", "List of names for corresponding fixed object", false);
		I_ASSIGN_MULTI_0(m_fixedObjectTypeNamesAttrPtr, "FixedObjectTypeNames", "List of names for corresponding fixed object type", false);
		I_ASSIGN_MULTI_0(m_externalTypeIdsAttrPtr, "ExternaObjectTypeIds", "List of IDs for object types should be stored in the external storage", false);
		I_ASSIGN(m_externalStorageCompPtr, "ExternalStorage", "External storage used to persist objects of types specified in 'ExternaObjectTypeIds'", false, "ExternalStorage");
	I_END_COMPONENT;

	// reimplemented (IObjectCollection::IDataFactory)
	virtual istd::IChangeableUniquePtr CreateInstance(const QByteArray& keyId = "") const override;
	virtual istd::IFactoryInfo::KeyList GetFactoryKeys() const override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;

	// reimplemented (ICollectionDataController)
	virtual const ifile::IFilePersistence* GetPersistenceForObjectType(const QByteArray& typeId) const override;

	// reimplemented (istd::IChangeable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	// reimplemented (CObjectCollectionBase)
	virtual istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray& typeId) const override;
	virtual IObjectCollection* GetObjectStorage(const QByteArray& typeId, const istd::IChangeable* objectPtr) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_MULTIFACT(istd::IChangeable, m_objectFactoriesCompPtr);
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);
	I_MULTITEXTATTR(m_typeNamesAttrPtr);
	I_MULTIREF(istd::IChangeable, m_fixedObjectsCompPtr);
	I_MULTIATTR(QByteArray, m_fixedObjectIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_fixedObjectTypeIdsAttrPtr);
	I_MULTITEXTATTR(m_fixedObjectNamesAttrPtr);
	I_MULTITEXTATTR(m_fixedObjectTypeNamesAttrPtr);
	I_MULTIREF(ifile::IFilePersistence, m_objectPersistenceListCompPtr);
	I_MULTIATTR(QByteArray, m_externalTypeIdsAttrPtr);
	I_REF(imtbase::IObjectCollection, m_externalStorageCompPtr);

	iprm::COptionsManager m_typesInfo;
};


} // namespace imtbase


