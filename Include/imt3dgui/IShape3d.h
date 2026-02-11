// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


