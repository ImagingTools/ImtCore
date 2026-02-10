// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CModelUpdateBridge.h>
#include <iprm/IEnableableParam.h>
#include <iprm/COptionsManager.h>

// Acula includes
#include <imthype/CTaskCollectionCompBase.h>


namespace imthype
{


/**
	Standard implementation of the \c ITaskCollection interface.
*/
class CTaskCollectionComp: public CTaskCollectionCompBase
{
public:
	typedef CTaskCollectionCompBase BaseClass;

	I_BEGIN_COMPONENT(CTaskCollectionComp);
		I_ASSIGN_MULTI_0(m_taskFactCompPtr, "TaskFactories", "List of task factories", false);
		I_ASSIGN_MULTI_0(m_taskTypeIdsAttrPtr, "TaskTypeIds", "List of type IDs for corresponding task", false);
		I_ASSIGN_MULTI_0(m_taskTypeNamesAttrPtr, "TaskTypeNames", "List of names for corresponding task type", false);
		I_ASSIGN_MULTI_0(m_taskEnablersCompPtr, "TaskTypeEnablers", "List of enablers for the task type", false);
		I_END_COMPONENT;

	// reimplemented (CTaskCollectionCompBase)
	virtual QString GetTaskTypeName(const QByteArray& taskTypeId) const override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;

protected:
	// reimplemented (CTaskCollectionCompBase)
	virtual iinsp::ISupplierUniquePtr CreateTaskInstance(const QByteArray& taskTypeId) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	imod::TModelWrap<iprm::COptionsManager> m_taskFactories;

private:
	I_MULTIFACT(iinsp::ISupplier, m_taskFactCompPtr);
	I_MULTITEXTATTR(m_taskTypeNamesAttrPtr);
	I_MULTIATTR(QByteArray, m_taskTypeIdsAttrPtr);
	I_MULTIREF(iprm::IEnableableParam, m_taskEnablersCompPtr);
};


} // namespace imthype


