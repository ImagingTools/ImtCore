// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CJobExecutionControllerComp.h>


namespace imthype
{


// protected methods

// reimplemented (CJobExecutionControllerCompBase)

const IJobProcessor* CJobExecutionControllerComp::GetTaskProcessor(const QByteArray& workerTypeId) const
{
	for (int i = 0; i < m_jobTasksCompPtr.GetCount(); ++i){
		IJobProcessor* processorPtr = m_jobTasksCompPtr[i];
		if (processorPtr != nullptr){
			if (processorPtr->GetTaskTypeId() == workerTypeId){
				return processorPtr;
			}
		}
	}

	return nullptr;
}


} // namespace imthype


