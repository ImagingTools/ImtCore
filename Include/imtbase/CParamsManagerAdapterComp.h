#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <iprm/IParamsManager.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Adapter for the object collections to \c iprm::IParamsManager interface.
	\ingroup Collection
*/
class CParamsManagerAdapterComp:
			public icomp::CComponentBase,
			private imod::CMultiModelDispatcherBase,
			virtual public iprm::IParamsManager,
			virtual public iprm::IOptionsList
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CParamsManagerAdapterComp);
		I_REGISTER_INTERFACE(iprm::IParamsManager);
		I_REGISTER_INTERFACE(iprm::IOptionsList);
		I_REGISTER_INTERFACE(iprm::ISelectionParam);
		I_ASSIGN(m_collectionCompPtr, "ParamsCollection", "Collection containing parameter sets", false, "ParamsCollection");
		I_ASSIGN_TO(m_collectionModelCompPtr, m_collectionCompPtr, false);
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "FilterByTypeIds", "List of type-IDs used for filtering the collection items", false);
	I_END_COMPONENT;

	// reimplemented (iprm::IParamsManager)
	virtual int GetIndexOperationFlags(int index = -1) const override;
	virtual bool SetIndexOperationFlags(int index, int flags) override;
	virtual int GetParamsSetsCount() const override;
	virtual const IOptionsList* GetParamsTypeConstraints() const override;
	virtual int InsertParamsSet(int typeIndex = -1, int index = -1) override;
	virtual bool RemoveParamsSet(int index) override;
	virtual bool SwapParamsSet(int index1, int index2) override;
	virtual iprm::IParamsSet* GetParamsSet(int index) const override;
	virtual iprm::IParamsSet* CreateParameterSet(int typeIndex = -1, int index = -1) const override;
	virtual QString GetParamsSetName(int index) const override;
	virtual bool SetParamsSetName(int index, const QString& name) override;
	virtual QString GetParamsSetDescription(int index) const override;
	virtual void SetParamsSetDescription(int index, const QString& description) override;

	// reimplemented (iprm::ISelectionParam)
	virtual const iprm::IOptionsList* GetSelectionConstraints() const override;
	virtual int GetSelectedOptionIndex() const override;
	virtual bool SetSelectedOptionIndex(int index) override;
	virtual ISelectionParam* GetSubselection(int index) const override;

	// reimplemented (iprm::IOptionsList)
	virtual int GetOptionsFlags() const override;
	virtual int GetOptionsCount() const override;
	virtual QString GetOptionName(int index) const override;
	virtual QString GetOptionDescription(int index) const override;
	virtual QByteArray GetOptionId(int index) const override;
	virtual bool IsOptionEnabled(int index) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet & changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

private:
	void UpdateList();
	QByteArray FindOptionFromIndex(int index) const;

private:
	I_REF(IObjectCollection, m_collectionCompPtr);
	I_REF(imod::IModel, m_collectionModelCompPtr);
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);

	iprm::COptionsManager m_options;
	QByteArray m_selectedId;

	typedef QMap<QByteArray, imtbase::IObjectCollection::DataPtr> ObjectMap;
	mutable ObjectMap m_objectCache;
};


} // namespace imtbase


