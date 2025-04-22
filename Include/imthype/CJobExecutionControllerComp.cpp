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


