// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


/**
	Interface for getting information about a Plug-In.
*/
class IPluginInfo: virtual public istd::IPolymorphic
{
public:
	/**
		Get the name of the plug-in.
	*/
	virtual QString GetPluginName() const = 0;

	/**
		Get the type-ID of the plug-in.
	*/
	virtual QByteArray GetPluginTypeId() const = 0;
};


} // namespace imtbase


