// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imthype
{


class ITaskCollectionContext: virtual public iser::ISerializable
{
public:
	/**
		Get UUID of the task in the collection.
	*/
	virtual QByteArray GetTaskId() const = 0;

	/**
		Set UUID of the task in the collection.
	*/
	virtual void SetTaskId(const QByteArray& taskId) = 0;

	/**
		Get input-ID of the task in the collection.
	*/
	virtual QByteArray GetTaskInputId() const = 0;

	/**
		Set input-ID of the task in the collection.
	*/
	virtual void SetTaskInputId(const QByteArray& inputId) = 0;
};


} // namespace imthype


