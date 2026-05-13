// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imthype/IJobQueueManager.h>


namespace imthype
{


/**
	Interface for managing job execution status and progress.
*/
class IJobStatus: virtual public istd::IChangeable
{
public:
	/**
		Get current processing status of the job.
	*/
	virtual IJobQueueManager::ProcessingStatus GetProcessingStatus() const = 0;
	
	/**
		Set processing status of the job.
		\param status New processing status
	*/
	virtual void SetProcessingStatus(IJobQueueManager::ProcessingStatus status) = 0;

	/**
		Get current progress of the job (0.0 to 1.0).
	*/
	virtual double GetProgress() const = 0;
	
	/**
		Set progress of the job.
		\param progress Progress value between 0.0 and 1.0
	*/
	virtual void SetProgress(double progress) = 0;
};


typedef istd::TSharedInterfacePtr<IJobStatus> IJobStatusSharedPtr;


} // namespace imthype


