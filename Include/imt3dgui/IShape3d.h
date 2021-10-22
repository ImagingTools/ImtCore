/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
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
**
********************************************************************************/


#pragma once


// ImtCore includes
#include <imt3dview/IScene3dItem.h>


namespace imt3dgui
{


/**
	Basic drawable shape interface
*/
class IShape3d: virtual public imt3dview::IScene3dItem
{
public:
	virtual bool IsVisible() const = 0;
	virtual void SetVisible(bool isVisible) = 0;

protected:
	virtual QVector3D GetColor() const = 0;
};


} // namespace imt3dgui


