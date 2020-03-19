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


#include <imtgui/CLayoutManagerComp.h>


// Qt includes




namespace imtgui
{


// public methods

CLayoutManagerComp::CLayoutManagerComp() 
{
}


void CLayoutManagerComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CLayoutManagerComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// reimplemented (icomp::CComponentBase)

void CLayoutManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

}


// private methods

void CLayoutManagerComp::UpdateVisualStatus()
{
	istd::CChangeNotifier visualStatusNotifier(&m_visualStatus);

}



} // namespace ilayoutmanager


