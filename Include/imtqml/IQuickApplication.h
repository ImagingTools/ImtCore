/********************************************************************************
**
**	Copyright (C) 2007-2017 Witold Gantzke & Kirill Lepskiy
**
**	This file is part of the ACF Toolkit.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
** 	See http://www.ilena.org or write info@imagingtools.de for further
** 	information about the ACF.
**
********************************************************************************/


#ifndef imtqml_IQuickApplication_included
#define imtqml_IQuickApplication_included


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


#endif // !imtqml_IQuickApplication_included


