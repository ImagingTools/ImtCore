#pragma once

// Acula includes
#include <imthype/CJobQueueManagerCompBase.h>


namespace imthype
{


class CJobQueueManagerComp:
			public CJobQueueManagerCompBase,
			virtual public IJobQueueManager
{
	Q_OBJECT
public:
	typedef CJobQueueManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CJobQueueManagerComp);
		I_REGISTER_INTERFACE(IJobQueueManager);
		I_ASSIGN(m_taskInfoListCompPtr, "TaskInfoList", "List of informations about registered job tasks", true, "TaskInfoList");
		I_ASSIGN_MULTI_0(m_taskParamsFactoriesCompPtr, "TaskParamsFactories", "List of factories used for parameter creation related to the registered tasks", false);
	I_END_COMPONENT;

	CJobQueueManagerComp();

	// reimplemented (IJobQueueManager)
	virtual const iprm::IOptionsList* GetSupportedTasks() const override;
	virtual iprm::IParamsSetUniquePtr CreateJobParameters(
				const QByteArray& contextId,
				const QByteArray& taskTypeId,
				const iprm::IParamsSet* defaultParamPtr) const override;
protected:
	int FindTaskById(const QByteArray& taskTypeId) const;

protected:
	I_REF(iprm::IOptionsList, m_taskInfoListCompPtr);
	I_MULTIFACT(iprm::IParamsSet, m_taskParamsFactoriesCompPtr);
};


} // namespace imthype


