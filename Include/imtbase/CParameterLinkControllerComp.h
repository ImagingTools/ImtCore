#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TSingleModelObserverBase.h>
#include <iprm/IParamsManager.h>


namespace imtbase
{


class CParameterLinkControllerComp:
			public icomp::CComponentBase,
			public imod::TSingleModelObserverBase<iprm::IParamsManager>
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CParameterLinkControllerComp);
		I_ASSIGN(m_selectionParamIdAttrPtr, "ReferenceParamId", "ID of the reference parameter in the parameter set", true, "Index");
		I_ASSIGN(m_paramsManagerCompPtr, "SlaveManager", "Slave parameter set manager containing the references from the observer data model", true, "SlaveManager");
	I_END_COMPONENT;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	const iprm::ISelectionParam* FindReferenceSelection(int paramSetIndex) const;

private:
	I_ATTR(QByteArray, m_selectionParamIdAttrPtr);
	I_REF(iprm::IParamsManager, m_paramsManagerCompPtr);
};


} // namespace imtbase


