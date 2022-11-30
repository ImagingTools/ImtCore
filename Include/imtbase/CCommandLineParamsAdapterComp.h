#pragma once

// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TSingleModelObserverBase.h>
#include <iprm/CParamsSet.h>
#include <iprm/IOptionsList.h>
#include <iser/ISerializable.h>

namespace imtbase
{


class CCommandLineParamsAdapterComp:
			public icomp::CComponentBase,
			public iprm::CParamsSet,
			public imod::TSingleModelObserverBase<iprm::IOptionsList>
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCommandLineParamsAdapterComp);
		I_REGISTER_INTERFACE(iprm::IParamsSet);
		I_ASSIGN(m_prefixAttrPtr, "OptionNamePrefix", "Prefix preceding option name", true, "-");
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "OptionName", "Option name", false);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "ParameterId", "Parameter ID in paramset", false);
		I_ASSIGN_MULTI_0(m_typeAttrPtr, "OptionType", "Option value type\n0 - String\n1 - Integer\n2 - Double", false);
		I_ASSIGN(m_optionListCompPtr, "OptionList", "List of registered options to search on the command line", false, "");
		I_ASSIGN_TO(m_paramListModelCompPtr, m_optionListCompPtr, true);
	I_END_COMPONENT;

	enum ParamType
	{
		PT_STRING = 0,
		PT_INT,
		PT_DOUBLE
	};

protected:
	int GetParameterCount();

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_ATTR(QByteArray, m_prefixAttrPtr);
	I_MULTIATTR(QByteArray, m_nameAttrPtr);
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(int, m_typeAttrPtr);
	I_REF(iprm::IOptionsList, m_optionListCompPtr);
	I_REF(imod::IModel, m_paramListModelCompPtr);
};


} // namespace imtbase


