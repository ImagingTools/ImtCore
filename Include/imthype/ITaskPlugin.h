// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TIFactory.h>
#include <iinsp/ISupplier.h>

// ImtCore includes
#include <imtbase/IPluginInfo.h>


namespace imthype
{


/**
	Interface describing a task Plug-In.
*/
class ITaskPlugin: virtual public imtbase::IPluginInfo
{
public:
	typedef istd::TIFactory<iinsp::ISupplier> ISupplierFactory;
	
	/**
		Get access to the task factory.
	*/
	virtual const ISupplierFactory* GetTaskFactory() const = 0;
};


} // namespace imthype


