// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imthype
{


/**
	Interface for defining the parameter for controlling the order of the job execution.
	Each job have:
	* A priority (given as a number between 0 and 1)
	* ID of the predecessor job. In case this ID is empty the order will be defined according to the job priority only.
*/
class IJobSchedulerParams: virtual public iser::ISerializable
{
public:
	/**
		Get processing priority [0...1].
	*/
	virtual double GetPriority() const = 0;

	/**
		Set processing priority [0...1].
	*/
	virtual void SetPriority(double prioority) = 0;

	/**
		Get the predecessor job-ID.
	*/
	virtual QByteArray GetPredecessorJobId() const = 0;

	/**
		Set the predecessor job-ID.
	*/
	virtual void SetPredecessorJobId(const QByteArray& predecessorJobId) = 0;
};


} // namespace imthype


