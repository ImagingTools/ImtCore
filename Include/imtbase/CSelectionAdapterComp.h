#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TModelWrap.h>
#include <iprm/IOptionsList.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtbase/CSelectionAdapterBase.h>
#include <imtbase/ICollectionInfo.h>
#include <imtbase/ISelection.h>


namespace imtbase
{


class CSelectionAdapterComp:
			public icomp::CComponentBase,
			public CSelectionAdapterBase,
			virtual public ISelection
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSelectionAdapterComp);
		I_REGISTER_SUBELEMENT(Constraints);
		I_REGISTER_SUBELEMENT_INTERFACE(Constraints, ICollectionInfo, ExtractConstraints);
		I_REGISTER_SUBELEMENT_INTERFACE(Constraints, imod::IModel, ExtractConstraints);
		I_REGISTER_SUBELEMENT_INTERFACE(Constraints, istd::IChangeable, ExtractConstraints);
		I_REGISTER_INTERFACE(ISelection);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_selectionCompPtr, "SelectionParam", "Source selection param", false, "");
	I_END_COMPONENT;

	CSelectionAdapterComp();

protected:
	// reimplemented (ISelection)
	virtual const ICollectionInfo* GetSelectionConstraints() const override;
	virtual SelectionMode GetSelectionMode() const override;
	virtual bool SetSelectionMode(SelectionMode mode) override;
	virtual Ids GetSelectedIds() const override;
	virtual bool SetSelectedIds(const Ids& selectedIds) override;

	// reimplemented (CSelectionAdapterBase)
	virtual void OnConstraintsChanged() override;
	virtual void OnSelectionChanged() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	int GetOptionIndex(const QByteArray& optionId) const;
	QByteArray GetOptionId(int index) const;

private:
	class Constraints: virtual public ICollectionInfo
	{
	public:
		Constraints();
		void SetParent(CSelectionAdapterComp* parentPtr);

		// reimplemented (ICollectionInfo)
		virtual int GetElementsCount(
					const iprm::IParamsSet* selectionParamPtr = nullptr,
					const Id& parentId = Id(),
					int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
		virtual Ids GetElementIds(
					int offset = 0,
					int count = -1,
					const iprm::IParamsSet* selectionParamsPtr = nullptr,
					const Id& parentId = Id(),
					int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
		virtual bool GetSubsetInfo(
					ICollectionInfo& subsetInfo,
					int offset = 0,
					int count = -1,
					const iprm::IParamsSet* selectionParamsPtr = nullptr,
					const Id& parentId = Id(),
					int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
		virtual Id GetParentId(const Id& elementId) const override;
		virtual Ids GetElementPath(const Id& elementId) const override;
		virtual bool IsBranch(const Id& elementId) const override;
		virtual QVariant GetElementInfo(const Id& elementId, int infoType) const override;
		virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId) const override;
		virtual bool SetElementName(const Id& elementId, const QString& name) override;
		virtual bool SetElementDescription(const Id& elementId, const QString& description) override;
		virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true) override;

	private:
		CSelectionAdapterComp* m_parentPtr;
	};

	template< class InterfaceType>
	static InterfaceType* ExtractConstraints(CSelectionAdapterComp& component)
	{
		return &component.m_constraints;
	}

private:
	I_REF(iprm::ISelectionParam, m_selectionCompPtr);

	iprm::ISelectionParam* m_selectionPtr;
	const iprm::IOptionsList* m_selectionConstraintsPtr;

	imod::TModelWrap<Constraints> m_constraints;
};


} // namespace imtbase


