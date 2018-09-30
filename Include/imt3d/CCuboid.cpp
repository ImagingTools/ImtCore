#include <imt3d/CCuboid.h>


namespace imt3d
{


// static atributes

imt3d::CCuboid CCuboid::s_empty(0, 0, 0, 0, 0, 0);
imt3d::CCuboid CCuboid::s_invalid(0, -1, 0, -1, 0, -1);


// public methods

CCuboid::CCuboid(double left, double right, double bottom, double top, double near, double far)
	:m_horizontalRange(left, right),
	m_verticalRange(bottom, top),
	m_depthRange(far, near)
{
}


// static public methods

CCuboid CCuboid::FromCloudPoints(const CloudPoints& cloudPoints)
{
	if (!cloudPoints.isEmpty()){
		double left = cloudPoints.first().GetX();
		double right = cloudPoints.first().GetX();
		double bottom = cloudPoints.first().GetY();
		double top = cloudPoints.first().GetY();
		double near = cloudPoints.first().GetZ();
		double far = cloudPoints.first().GetZ();

		for (CloudPoints::const_iterator pointIter = cloudPoints.constBegin(); pointIter != cloudPoints.constEnd(); pointIter++){
			if ((pointIter->GetX() > right)){
				right = pointIter->GetX();
			} else if ((pointIter->GetX() < left)){
				left = pointIter->GetX();
			}
			if ((pointIter->GetY() > top)){
				top = pointIter->GetY();
			} else if ((pointIter->GetY() < bottom)){
				bottom = pointIter->GetY();
			}
			if ((pointIter->GetZ() > near)){
				near = pointIter->GetZ();
			} else if ((pointIter->GetZ() < far)){
				far = pointIter->GetZ();
			}
		}

		return CCuboid(left, right, bottom, top, near, far);
	}
	else{
		return s_invalid;
	}

	return s_invalid;
}


} // namespace imt3d


