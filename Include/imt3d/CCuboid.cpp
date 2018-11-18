#include <imt3d/CCuboid.h>

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
	m_depthRange(farValue, nearValue)
{
}


// static public methods

CCuboid CCuboid::FromCloudPoints(const CloudPoints& cloudPoints)
{
	if (!cloudPoints.isEmpty()){
		double left = std::numeric_limits<double>::max();
		double right = std::numeric_limits<double>::min();
		double bottom = std::numeric_limits<double>::max();
		double top = std::numeric_limits<double>::min();
		double near = std::numeric_limits<double>::min();
		double far = std::numeric_limits<double>::max();

		for (CloudPoints::const_iterator pointIter = cloudPoints.constBegin(); pointIter != cloudPoints.constEnd(); pointIter++){
			if (!imt3d::CPointCloud3d::IsPointValid(*pointIter)){
				continue;
			}

			if ((pointIter->GetX() > right)){
				right = pointIter->GetX();
			}
			
			if ((pointIter->GetX() < left)){
				left = pointIter->GetX();
			}
			if ((pointIter->GetY() > top)){
				top = pointIter->GetY();
			}
			
			if ((pointIter->GetY() < bottom)){
				bottom = pointIter->GetY();
			}
			if ((pointIter->GetZ() > near)){
				near = pointIter->GetZ();
			}
			
			if ((pointIter->GetZ() < far)){
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


