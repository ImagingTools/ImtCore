#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ifile/IFileNameParam.h>
#include <iprm/CComposedParamsSetComp.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>


namespace imtbase
{


class CDelegatedParamsSetComp: public iprm::CComposedParamsSetComp

{
public:
	typedef iprm::CComposedParamsSetComp BaseClass;

	I_BEGIN_COMPONENT(CDelegatedParamsSetComp);
		I_REGISTER_INTERFACE(istd::IHierarchical);
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


