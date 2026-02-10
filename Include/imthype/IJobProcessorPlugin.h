// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IPluginInfo.h>

// Acula includes
#include <imthype/IJobProcessor.h>


namespace imthype
{


/**
	Interface for a job processor Plug-In.
*/
class IJobProcessorPlugin: virtual public imtbase::IPluginInfo
{
public:
	/**
		Get access to the job processor factory.
	*/
	virtual const IJobProcessorFactory* GetJobProcessorFactory() const = 0;
};


} // namespace imthype


