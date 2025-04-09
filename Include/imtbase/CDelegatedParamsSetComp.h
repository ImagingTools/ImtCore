#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IParamsSet.h>


namespace imtbase
{


class CDelegatedParamsSetComp:
			public icomp::CComponentBase,
			virtual public iprm::IParamsSet

{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedParamsSetComp);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(IParamsSet);
		I_ASSIGN(m_paramsSetCompPtr, "DelegatedParamsSet", "Delegated params set", false, "DelegatedParamsSet");
	I_END_COMPONENT;

	// reimplemented (iprm::IParamsSet)
	virtual Ids GetParamIds(bool editableOnly = false) const override;
	virtual const iser::ISerializable* GetParameter(const QByteArray& id) const override;
	virtual iser::ISerializable* GetEditableParameter(const QByteArray& id) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	I_REF(iprm::IParamsSet, m_paramsSetCompPtr);
};


} // namespace imtbase


