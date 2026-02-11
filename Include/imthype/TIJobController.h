// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imthype/IJobController.h>


namespace imthype
{


template<typename JobParams, typename JobResult>
class TIJobController : virtual public IJobController
{
public:
	using JobResultPtr = std::shared_ptr<JobResult>;

	/**
		Begin new job with given params
	*/
	virtual RequestStatus BeginJob(const QByteArray& jobId, const JobParams& jobParams) = 0;

	/**
		Get the result of the job execution with given ID. Non-null if the job was successfully completed
	*/
	virtual JobResultPtr GetJobResult(const QByteArray& jobId) const = 0;
};


} // namespace imthype


