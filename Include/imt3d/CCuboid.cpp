// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3d/CCuboid.h>


// ImtCore includes
#include <imt3d/CPointCloud3d.h>


namespace imt3d
{


// static atributes

imt3d::CCuboid CCuboid::s_empty(0, 0, 0, 0, 0, 0);
imt3d::CCuboid CCuboid::s_invalid(0, -1, 0, -1, 0, -1);


// public methods

CCuboid::CCuboid(double left, double right, double bottom, double top, double nearValue, double farValue)
	:m_horizontalRange(left, right),
	m_verticalRange(bottom, top),
	m_depthRange(nearValue, farValue)
{
}


} // namespace imt3d


