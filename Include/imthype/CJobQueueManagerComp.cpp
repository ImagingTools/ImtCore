// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CJobQueueManagerComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CSystem.h>


namespace imthype
{


// public methods

CJobQueueManagerComp::CJobQueueManagerComp()
{
}


// reimplemented (IJobQueueManager)

const iprm::IOptionsList* CJobQueueManagerComp::GetSupportedTasks() const
{
	return nullptr;
}


iprm::IParamsSetUniquePtr CJobQueueManagerComp::CreateJobParameters(
			const QByteArray& /*contextId*/,
			const QByteArray& taskTypeId,
			const iprm::IParamsSet* defaultParamPtr) const
{
	if (m_taskInfoListCompPtr.IsValid() && m_taskParamsFactoriesCompPtr.IsValid()){
		int factoryIndex = iprm::FindOptionIndexById(taskTypeId, *m_taskInfoListCompPtr.GetPtr());
		if ((factoryIndex >= 0) && (factoryIndex < m_taskParamsFactoriesCompPtr.GetCount())){
			iprm::IParamsSetUniquePtr newParamsPtr = m_taskParamsFactoriesCompPtr.CreateInstance(factoryIndex);
			if (newParamsPtr.IsValid()){
				if (newParamsPtr->GetFactoryId() != taskTypeId){
					SendCriticalMessage(0, "Job parameter were created, but they have a wrong type. Please check the parameter registration");

					return nullptr;
				}

				if (defaultParamPtr != nullptr){
					if (!newParamsPtr->CopyFrom(*defaultParamPtr)){
						SendCriticalMessage(0, "Job parameter could not be initialized");

						return nullptr;
					}
				}

				return newParamsPtr;
			}

			SendCriticalMessage(0, "Job parameter could not be created");
		}
	}

	return nullptr;
}


int CJobQueueManagerComp::FindTaskById(const QByteArray& taskTypeId) const
{
	if (!m_taskInfoListCompPtr.IsValid()){
		return -1;
	}

	int typesCount = m_taskInfoListCompPtr->GetOptionsCount();
	for (int i = 0; i < typesCount; ++i){
		if (m_taskInfoListCompPtr->GetOptionId(i) == taskTypeId){
			return i;
		}
	}

	return -1;
}


} // namespace imthype


