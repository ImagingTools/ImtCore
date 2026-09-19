// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imthype
{


class IProcessingAgent;


/**
	Interface of a registry (pool) of processing agents.

	The registry keeps track of the available processing agents and is used by the
	distributed job execution controller to find an agent that is able and ready to
	execute a task of a given type. This decouples the job execution controller from
	the concrete set of agents and the strategy used to select one of them.
*/
class IProcessingAgentRegistry: virtual public istd::IPolymorphic
{
public:
	/**
		Get the number of registered agents.
	*/
	virtual int GetAgentsCount() const = 0;

	/**
		Get the agent at the given index.
		\param index	Index of the agent in range [0, GetAgentsCount()).
		\return Pointer to the agent or \c nullptr if the index is invalid.
	*/
	virtual IProcessingAgent* GetAgentAt(int index) const = 0;

	/**
		Find an agent that is currently able to accept and process a task of the given type.

		The implementation selects an agent which supports the given task type and is
		in a state that allows accepting new tasks (e.g. not offline or fully loaded).

		\param taskTypeId	Type-ID of the task to be performed.
		\return Pointer to a ready agent or \c nullptr if no suitable agent is available.
	*/
	virtual IProcessingAgent* FindAgentForTask(const QByteArray& taskTypeId) const = 0;
};


} // namespace imthype
