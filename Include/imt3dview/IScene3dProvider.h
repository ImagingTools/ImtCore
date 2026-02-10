// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imt3dview
{


class IScene3d;



class IScene3dProvider: virtual public istd::IPolymorphic
{
public:
	virtual IScene3d* GetScene() const = 0;
};


} // namespace imt3dview


