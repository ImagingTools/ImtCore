// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IPluginInfo.h>
#include <imtservice/IConnectionCollection.h>


namespace imtservice
{


/**
	Interface describing a task Plug-In.
*/
class IConnectionCollectionPlugin: virtual public imtbase::IPluginInfo
{
public:
	typedef istd::TIFactory<IConnectionCollection> IConnectionCollectionFactory;

	/**
		Get access to the ConnectionCollection factory.
	*/
	virtual const IConnectionCollectionFactory* GetConnectionCollectionFactory() const = 0;
};


} // namespace imtservice


