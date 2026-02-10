// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Acula includes
#include <imthype/CJobExecutionControllerCompBase.h>


namespace imthype
{


/**
	Implementation based on statically connected list of job processors.
*/
class CJobExecutionControllerComp: public CJobExecutionControllerCompBase
{
public:
	typedef CJobExecutionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CJobExecutionControllerComp);
		I_ASSIGN_MULTI_0(m_jobTasksCompPtr, "JobTasks", "List of processors for corresponding file types", true);
	I_END_COMPONENT;

protected:
	// reimplemented (CJobExecutionControllerCompBase)
	virtual const IJobProcessor* GetTaskProcessor(const QByteArray& workerTypeId) const override;

private:
	/**
		Reference to the list of job tasks for the corresponding file type.
	*/
	I_MULTIREF(IJobProcessor, m_jobTasksCompPtr);
};


} // namespace imthype


