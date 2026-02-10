// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imthype
{


class IJobController : virtual public istd::IPolymorphic
{
public:
	enum RequestStatus
	{
		RS_INVALID_JOB_ID,
		RS_SUCCESS,
		RS_FAILED
	};

	enum JobStatus
	{
		JS_INVALID_JOB_ID,
		JS_IN_PROGRESS,
		JS_CANCELLATION,
		JS_CANCELLED,
		JS_COMPLETED,
		JS_FAILED
	};

	/**
		Get status of the job with given ID
	*/
	virtual JobStatus GetJobStatus(const QByteArray& jobId) const = 0;
	
	/**
		Cancel the execution of the job with given ID
	*/
	virtual RequestStatus CancelJob(const QByteArray& jobId) = 0;

	/**
		Remove job with given ID
		Getting result of the job with given ID will be unavailable
	*/
	virtual RequestStatus RemoveJob(const QByteArray& jobId) = 0;
};


} // namespace imthype


