// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IInformationProvider.h>
#include <ilog/IMessageContainer.h>


namespace imthype
{


/**
	This interface describes processing information for a job.
	The \c istd::IInformationProvder interface represents the status of the entire job.
	Additionaly you can get access to the procesing information using \c GetProcesinginfo method
	or get the processing log.
*/
class IProcessingInfo: virtual public istd::IInformationProvider, virtual public iser::ISerializable
{
public:
	/**
		Information about the job process.
	*/
	struct ProcessingInfo
	{
		ProcessingInfo()
			:processingTime(0.0)
		{
		}

		/**
			Consumed time for job processing.
		*/
		double processingTime;

		/**
			Start time of the job.
		*/
		QDateTime startedAt;
	};

	/**
		Get information about the processing.
	*/
	virtual ProcessingInfo GetProcessingInfo() const = 0;

	/**
		Get the processing log.
	*/
	virtual const ilog::IMessageContainer& GetProcessingLog() const = 0;
};


} // namespace imthype


