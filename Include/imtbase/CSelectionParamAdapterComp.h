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


class CSelectionParamAdapterComp:
			public icomp::CComponentBase,
			public CSelectionAdapterBase,
			virtual public iprm::ISelectionParam
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSelectionParamAdapterComp);
	I_REGISTER_SUBELEMENT(Constraints);
	I_REGISTER_SUBELEMENT_INTERFACE(Constraints, iprm::IOptionsList, ExtractConstraints);
	I_REGISTER_SUBELEMENT_INTERFACE(Constraints, imod::IModel, ExtractConstraints);
	I_REGISTER_SUBELEMENT_INTERFACE(Constraints, istd::IChangeable, ExtractConstraints);
	I_REGISTER_INTERFACE(iprm::ISelectionParam);
	I_REGISTER_INTERFACE(iser::ISerializable);
	I_ASSIGN(m_selectionCompPtr, "Selection", "Source selection", false, "");
	I_END_COMPONENT;

	CSelectionParamAdapterComp();

protected:
	// reimplemented (iprm::ISelectionParam)
	virtual const iprm::IOptionsList* GetSelectionConstraints() const override;
	virtual int GetSelectedOptionIndex() const override;
	virtual bool SetSelectedOptionIndex(int index) override;
	virtual ISelectionParam* GetSubselection(int index) const override;

	// reimplemented (CSelectionAdapterBase)
	virtual void OnConstraintsChanged() override;
	virtual void OnSelectionChanged() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	class Constraints: virtual public iprm::IOptionsList
	{
	public:
		Constraints();
		void SetParent(CSelectionParamAdapterComp* parentPtr);

		// reimplemented (iprm::IOptionsList)
		virtual int GetOptionsFlags() const override;
		virtual int GetOptionsCount() const override;
		virtual QString GetOptionName(int index) const override;
		virtual QString GetOptionDescription(int index) const override;
		virtual QByteArray GetOptionId(int index) const override;
		virtual bool IsOptionEnabled(int index) const override;

	private:
		CSelectionParamAdapterComp* m_parentPtr;
	};

	template< class InterfaceType>
	static InterfaceType* ExtractConstraints(CSelectionParamAdapterComp& component)
	{
		return &component.m_constraints;
	}

private:
	I_REF(ISelection, m_selectionCompPtr);

	ISelection* m_selectionPtr;
	const ICollectionInfo* m_selectionConstraintsPtr;

	imod::TModelWrap<Constraints> m_constraints;
};


} // namespace imtbase


