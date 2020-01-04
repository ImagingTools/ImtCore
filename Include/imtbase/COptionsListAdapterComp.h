#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


/**
	Adapter of the \c ICollectionInfo interface to \c iprmIOptionsList
	\ingroup Collection
*/
class COptionsListAdapterComp:
			public icomp::CComponentBase,
			private imod::CMultiModelDispatcherBase,
			virtual public iprm::IOptionsList
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(COptionsListAdapterComp);
		I_REGISTER_INTERFACE(iprm::IOptionsList);
		I_ASSIGN(m_collectionInfoCompPtr, "CollectionInfo", "Collection information", false, "CollectionInfo");
		I_ASSIGN_TO(m_collectionInfoModelCompPtr, m_collectionInfoCompPtr, false);
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "FilterByTypeIds", "List of type-IDs used for filtering the collection items", false);
	I_END_COMPONENT;

	// reimplemented (iprm::IOptionsList)
	virtual int GetOptionsFlags() const override;
	virtual int GetOptionsCount() const override;
	virtual QString GetOptionName(int index) const override;
	virtual QString GetOptionDescription(int index) const override;
	virtual QByteArray GetOptionId(int index) const override;
	virtual bool IsOptionEnabled(int index) const override;

protected:
	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet & changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

private:
	void UpdateList();

private:
	I_REF(ICollectionInfo, m_collectionInfoCompPtr);
	I_REF(imod::IModel, m_collectionInfoModelCompPtr);
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);

	iprm::COptionsManager m_options;
};


} // namespace imtbase


