// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iinsp/ISupplier.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>

// Acula includes
#include <imthype/ITaskResultCollection.h>


namespace imthype
{


/**
	Common interface for a task collection.
	Please note that this is just the container of the tasks without any business logic about how these tasks should be performed.
*/
class ITaskCollection: virtual public imtbase::IObjectCollection
{
public:
	/**
		Get the user-defined ID of the task.
	*/
	virtual QByteArray GetUserTaskId(const QByteArray& taskUuid) const = 0;

	/**
		Set the user-defined ID for a given task.
		The user-defined task-ID can be used related to some external controller system for the task execution.
		For example, an external system defines a task with the ID "FirstTask" and we want to connect it to a given task in the task collection.
		Then we use SetUserTaskId(\c TASK_UUID, "FirstTask") to create this binding.
		The task execution controller should find the task according to the user-defined task-ID and execute it.
	*/
	virtual bool SetUserTaskId(const QByteArray& taskUuid, const QByteArray& userTaskId) = 0;

	/**
		Get ID of the input data for a given task.
		The input-ID is an identifier for getting the data related to the task from some external storage or abstract data provider.
	*/
	virtual QByteArray GetTaskInputId(const QByteArray& taskUuid) const = 0;

	/**
		Set ID of the input data for a given task.
	*/
	virtual bool SetTaskInputId(const QByteArray& taskUuid, const QByteArray& inputId) = 0;

	/**
		Get access to the task instance.
		\param taskId	Unique ID of the task.
	*/
	virtual const iinsp::ISupplier* GetTask(const QByteArray& taskId) const = 0;

	/**
		Get the result data objects of the given task.
	*/
	virtual const ITaskResultCollection* GetTaskResults(const QByteArray& taskId) const = 0;

	/**
		Get available task inputs.
	*/
	virtual const imtbase::IObjectCollection* GetTaskInputs() const = 0;
};


} // namespace imthype


