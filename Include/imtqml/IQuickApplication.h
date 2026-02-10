// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplication.h>
#include <imtqml/IQuickObject.h>


namespace imtqml
{


/**
	Interface for Qt Quick based application with GUI.
*/
class IQuickApplication: virtual public ibase::IApplication
{
public:
	/**
		Get the main UI object of the application.
	*/
	virtual const imtqml::IQuickObject* GetApplicationItem() const = 0;
};


} // namespace imtqml


